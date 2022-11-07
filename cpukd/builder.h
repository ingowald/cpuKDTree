// ======================================================================== //
// Copyright 2019-2021 Ingo Wald                                            //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "cpukd/common.h"
#include <vector>
#include <algorithm>

namespace cpukd {
  
  // ==================================================================
  // INTERFACE SECTION
  // ==================================================================

  /*! builds a regular, "round-robin" style k-d tree over the given
    (device-side) array of points. Each point can have an arbitrary
    struct type 'point_t', but this point_t must store its point
    coordinates at offset 0; i.e., the first 'numDims' members of each
    point-t must be of type scalar_t, and those are the coordinates of
    this point

    Example 1: To build a 2D k-dtree over a int2 type (no other
    payload than the two coordinates):

    buildKDTree<int2,int>(....);

    Example 2: to build a 1D kd-tree over a data type of float4, where
    the first coordinate of each point is the dimension we want to
    build the kd-tree over, and the other three coordinate are
    arbitrary other payload data:

    buildKDTree<float4,float,1>(...);
  */
  template<typename point_t,
           typename scalar_t,
           int      numDims=sizeof(point_t)/sizeof(scalar_t)>
  void buildTree(point_t *d_points, int numPoints);

  // ==================================================================
  // IMPLEMENTATION SECTION
  // ==================================================================

  inline int lChild(int n) { return 2*n+1; }
  inline int rChild(int n) { return 2*n+2; }
  inline int subtreeSize(int n, int N)
  {
    int ss = 0;
    int width = 1;
    while (n < N) {
      int begin = n;
      ss += std::min(width,N-begin);
      n = lChild(n);
      width += width;
    }
    return ss;
  }

  template<typename point_t,
           typename scalar_t,
           int      numDims>
    struct DimCompare {
      DimCompare(point_t *points, int dim) : points(points), dim(dim) {};
      inline bool operator()(const point_t &a, const point_t &b) const
      {
        scalar_t a_dim = ((scalar_t *)&a)[dim];
        scalar_t b_dim = ((scalar_t *)&b)[dim];
        return a_dim < b_dim;
      }
      point_t *const points;
      const int dim;
    };

  template<typename point_t,
           typename scalar_t,
           int      numDims>
  void buildTree_rec(int tgt, int level,
                 int begin, int end,
                 point_t *d_points,
                 point_t *d_array,
                 int numPoints)
  {
    if (tgt >= numPoints) return;
    
    if (end - begin == 1) {
      d_points[tgt] = d_array[begin];
      return;
    }

    int dim = level % numDims;
    std::sort(d_array+begin,d_array+end,
              DimCompare<point_t,scalar_t,numDims>(d_array,dim));
    int pivot = begin+subtreeSize(lChild(tgt),numPoints);
    d_points[tgt] = d_array[pivot];
    buildTree_rec<point_t,scalar_t,numDims>
      (lChild(tgt),level+1,begin,pivot,d_points,d_array,numPoints);
    buildTree_rec<point_t,scalar_t,numDims>
      (rChild(tgt),level+1,pivot+1,end,d_points,d_array,numPoints);
  }

  template<typename point_t,
           typename scalar_t,
           int      numDims>
  void buildTree(point_t *d_points,
                 int numPoints)
  {
    std::vector<point_t> tmpArray(numPoints);
    std::copy(d_points,d_points+numPoints,tmpArray.data());
    buildTree_rec<point_t,scalar_t,numDims>
      (/* target node: */0, /* level */ 0,
       /* range */0,numPoints,
       d_points,tmpArray.data(),numPoints);
  }
}

