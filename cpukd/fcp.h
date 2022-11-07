// ======================================================================== //
// Copyright 2018-2022 Ingo Wald                                            //
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

namespace cpukd {

  inline static int levelOf(int nodeID)
  {
#ifdef __CUDA_ARCH__
    int k = 63 - __clzll(nodeID+1);
#else
    int k = 63 - __builtin_clzll(nodeID+1);
#endif
    return k;
  }

  template<typename scalar_t> scalar_t sqrt(scalar_t v);
  template<> float sqrt(float v) { return ::sqrtf(v); }
  template<> double sqrt(double v) { return ::sqrt(v); }
  
  template<typename point_t, typename scalar_t, int numDims>
  inline scalar_t distance(const point_t &a, const point_t &b)
  {
    scalar_t dot = scalar_t(0);
    for (int i=0;i<numDims;i++) {
      scalar_t a_i = ((const scalar_t*)&a)[i];
      scalar_t b_i = ((const scalar_t*)&b)[i];
      dot += (b_i-a_i)*(b_i-a_i);
    }
    return sqrt<scalar_t>(dot);
  }
  
  template<typename point_t, typename scalar_t, int numDims>
  inline
  int fcp(point_t queryPoint,
          const point_t *d_nodes,
          int N)
  {
    int   closest_found_so_far = -1;
    float closest_dist_found_so_far = std::numeric_limits<float>::infinity();
    
    int prev = -1;
    int curr = 0;

    while (true) {
      const int parent = (curr+1)/2-1;
      if (curr >= N) {
        // in some (rare) cases it's possible that below traversal
        // logic will go to a "close child", but may actually only
        // have a far child. In that case it's easiest to fix this
        // right here, pretend we've done that (non-existent) close
        // child, and let parent pick up traversal as if it had been
        // done.
        prev = curr;
        curr = parent;
        
        continue;
      }
      const int  child = 2*curr+1;
      const bool from_child = (prev >= child);
      if (!from_child) {
        float dist = distance<point_t,scalar_t,numDims>(queryPoint,d_nodes[curr]);
        if (dist < closest_dist_found_so_far) {
          closest_dist_found_so_far = dist;
          closest_found_so_far      = curr;
        }
      }

      const auto &curr_node = d_nodes[curr];
      const int   curr_dim = levelOf(curr) % numDims;
      const float curr_dim_dist = ((scalar_t*)&queryPoint)[curr_dim] - ((scalar_t*)&curr_node)[curr_dim];
      const int   curr_side = curr_dim_dist > 0.f;
      const int   curr_close_child = 2*curr + 1 + curr_side;
      const int   curr_far_child   = 2*curr + 2 - curr_side;
      
      int next = -1;
      if (prev == curr_close_child)
        // if we came from the close child, we may still have to check
        // the far side - but only if this exists, and if far half of
        // current space if even within search radius.
        next
          = ((curr_far_child<N) && (fabsf(curr_dim_dist) < closest_dist_found_so_far))
          ? curr_far_child
          : parent;
      else if (prev == curr_far_child)
        // if we did come from the far child, then both children are
        // done, and we can only go up.
        next = parent;
      else
        // we didn't come from any child, so must be coming from a
        // parent... we've already been processed ourselves just now,
        // so next stop is to look at the children (unless there
        // aren't any). this still leaves the case that we might have
        // a child, but only a far child, and this far child may or
        // may not be in range ... we'll fix that by just going to
        // near child _even if_ only the far child exists, and have
        // that child do a dummy traversal of that missing child, then
        // pick up on the far-child logic when we return.
        next
          = (child<N)
          ? curr_close_child
          : parent;

      if (next == -1)
      // if (curr == 0 && from_child)
        // this can only (and will) happen if and only if we come from a
        // child, arrive at the root, and decide to go to the parent of
        // the root ... while means we're done.
        return closest_found_so_far;
    
      prev = curr;
      curr = next;
    }
  }
  
} // ::cpukd

