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

#include "cpukd/builder.h"
#include "parallel_for.h"
// fcp = "find closest point" query
#include "cpukd/fcp.h"

using namespace cpukd;

  struct float3 { float x, y, z; };
  struct float4 { float x, y, z, w; };
  inline float3 make_float3(float x, float y, float z) { return {x,y,z}; }
  inline float4 make_float4(float x, float y, float z, float w) { return {x,y,z,w}; }

  template<typename T> struct point_traits;


template<> struct point_traits<float3> { enum { numDims = 3 }; };
  template<> struct point_traits<float4> { enum { numDims = 4 }; };
  
  inline 
  float dot(float4 a, float4 b) { return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w; }
  
  inline 
  float4 sub(float4 a, float4 b) { return make_float4(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w); }
  
  inline 
  float sqr_distance(float4 a, float4 b)
  {
    return dot(sub(a,b),sub(a,b)); 
  }

  inline 
  float distance(float4 a, float4 b)
  {
    return sqrtf(sqr_distance(a,b));
  }
  
  inline 
  float dot(float3 a, float3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
  
  inline 
  float3 sub(float3 a, float3 b) { return make_float3(a.x-b.x,a.y-b.y,a.z-b.z); }
  
  inline 
  float sqr_distance(float3 a, float3 b)
  {
    return dot(sub(a,b),sub(a,b)); 
  }

  inline 
  float distance(float3 a, float3 b)
  {
    return sqrtf(sqr_distance(a,b));
  }

float4 *generatePoints(int N)
{
  std::cout << "generating " << N <<  " points" << std::endl;
  float4 *d_points = new float4[N];
  for (int i=0;i<N;i++) {
    d_points[i].x = (float)drand48();
    d_points[i].y = (float)drand48();
    d_points[i].z = (float)drand48();
    d_points[i].w = (float)drand48();
  }
  return d_points;
}

void fcp(int *d_results,
         float4 *d_queries,
         int numQueries,
         float4 *d_nodes,
         int numNodes)
{
  cpukd::common::parallel_for_blocked
    (0,numQueries,1024,
     [&](int begin, int end) {
       for (int i=begin;i<end;i++)
         d_results[i] = cpukd::fcp<float4,float,4>(d_queries[i],d_nodes,numNodes);
     });
}

bool noneBelow(float4 *d_points, int N, int curr, int dim, float value)
{
  if (curr >= N) return true;
  return
    ((&d_points[curr].x)[dim] >= value)
    && noneBelow(d_points,N,2*curr+1,dim,value)
    && noneBelow(d_points,N,2*curr+2,dim,value);
}

bool noneAbove(float4 *d_points, int N, int curr, int dim, float value)
{
  if (curr >= N) return true;
  return
    ((&d_points[curr].x)[dim] <= value)
    && noneAbove(d_points,N,2*curr+1,dim,value)
    && noneAbove(d_points,N,2*curr+2,dim,value);
}

bool checkTree(float4 *d_points, int N, int curr=0)
{
  if (curr >= N) return true;

  int dim = cpukd::levelOf(curr)%4;
  float value = (&d_points[curr].x)[dim];
  
  if (!noneAbove(d_points,N,2*curr+1,dim,value))
    return false;
  if (!noneBelow(d_points,N,2*curr+2,dim,value))
    return false;
  
  return
    checkTree(d_points,N,2*curr+1)
    &&
    checkTree(d_points,N,2*curr+2);
}

int main(int ac, const char **av)
{
  using namespace cpukd::common;
  
  int nPoints = 173;
  bool verify = false;
  int nRepeats = 1;
  for (int i=1;i<ac;i++) {
    std::string arg = av[i];
    if (arg[0] != '-')
      nPoints = std::stoi(arg);
    else if (arg == "-v")
      verify = true;
    else if (arg == "-nr")
      nRepeats = atoi(av[++i]);
    else
      throw std::runtime_error("known cmdline arg "+arg);
  }
  
  float4 *d_points = generatePoints(nPoints);

  {
    double t0 = getCurrentTime();
    std::cout << "calling builder..." << std::endl;
    cpukd::buildTree<float4,float>(d_points,nPoints);
    double t1 = getCurrentTime();
    std::cout << "done building tree, took " << prettyDouble(t1-t0) << "s" << std::endl;
  }

  if (verify) {
    std::cout << "checking tree..." << std::endl;
    if (!checkTree(d_points,nPoints))
      throw std::runtime_error("not a valid kd-tree!?");
    else
      std::cout << "... passed" << std::endl;
  }

  size_t nQueries = 10000000;
  float4 *d_queries = generatePoints(nQueries);
  int    *d_results = new int[nQueries];
  {
    double t0 = getCurrentTime();
    for (int i=0;i<nRepeats;i++) {
      fcp(d_results,d_queries,nQueries,d_points,nPoints);
    }
    double t1 = getCurrentTime();
    std::cout << "done " << nRepeats << " iterations of 10M fcp queries, took " << prettyDouble(t1-t0) << "s" << std::endl;
    std::cout << "that is " << prettyDouble(nQueries*nRepeats/(t1-t0)) << " queries/s" << std::endl;
  }
  
  if (verify) {
    std::cout << "verifying ..." << std::endl;
    for (int i=0;i<nQueries;i++) {
      if (d_results[i] == -1) continue;
      
      float4 qp = d_queries[i];
      float reportedDist = distance(qp,d_points[d_results[i]]);
      for (int j=0;j<nPoints;j++) {
        float dist_j = distance(qp,d_points[j]);
        if (dist_j < reportedDist) {
          printf("for query %i: found offending point %i (%f %f %f %f) with dist %f (vs %f)\n",
                 i,
                 j,
                 d_points[j].x,
                 d_points[j].y,
                 d_points[j].z,
                 d_points[j].w,
                 dist_j,
                 reportedDist);
          
          throw std::runtime_error("verification failed ...");
        }
      }
    }
    std::cout << "verification succeeded... done." << std::endl;
  }
}
