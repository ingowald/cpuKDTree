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
#include "cpukd/parallel_for.h"

namespace cpukd {

//   /*! helper functions for a generic, arbitrary-size binary tree -
//     mostly to compute level of a given node in that tree, and child
//     IDs, parent IDs, etc */
//   struct BinaryTree {
//     inline static int rootNode() { return 0; }
//     inline static int parentOf(int nodeID) { return (nodeID-1)/2; }
//     inline static int isLeftSibling(int nodeID) { return (nodeID & 1); }
//     inline static int leftChildOf (int nodeID) { return 2*nodeID+1; }
//     inline static int rightChildOf(int nodeID) { return 2*nodeID+2; }
//     inline static int firstNodeInLevel(int L) { return (1<<L)-1; }
  
  
//     inline static int numLevelsFor(int numPoints)
//     {
//       return levelOf(numPoints-1)+1;
//     }
  
//     inline int numSiblingsToLeftOf(int n)
//     {
//       int levelOf_n = BinaryTree::levelOf(n);
//       return n - BinaryTree::firstNodeInLevel(levelOf_n);
//     }
//   };

  // /*! helper class for all expressions operating on a full binary tree
  //     of a given number of levels */
  // struct FullBinaryTreeOf
  // {
  //   inline FullBinaryTreeOf(int numLevels) : numLevels(numLevels) {}
  
  //   // tested, works for any numLevels >= 0
  //   inline int numNodes() const { return (1<<numLevels)-1; }
  //   inline int numOnLastLevel() const { return (1<<(numLevels-1)); }
  
  //   const int numLevels;
  // };

  // /*! helper class for all kind of values revolving around a given
  //     subtree in full binary tree of a given number of levels. Allos
  //     us to compute the number of nodes in a given subtree, the first
  //     and last node of a given subtree, etc */
  // struct SubTreeInFullTreeOf
  // {
  //   inline
  //   SubTreeInFullTreeOf(int numLevelsTree, int subtreeRoot)
  //     : numLevelsTree(numLevelsTree),
  //       subtreeRoot(subtreeRoot),
  //       levelOfSubtree(BinaryTree::levelOf(subtreeRoot)),
  //       numLevelsSubtree(numLevelsTree - levelOfSubtree)
  //   {}
  //   inline
  //   int lastNodeOnLastLevel() const
  //   {
  //     // return ((subtreeRoot+2) << (numLevelsSubtree-1)) - 2;
  //     int first = (subtreeRoot+1)<<(numLevelsSubtree-1);
  //     int onLast = (1<<(numLevelsSubtree-1)) - 1;
  //     return first+onLast;
  //   }
  //   inline
  //   int numOnLastLevel() const { return FullBinaryTreeOf(numLevelsSubtree).numOnLastLevel(); }
  //   inline
  //   int numNodes()            const { return FullBinaryTreeOf(numLevelsSubtree).numNodes(); }
  
  //   const int numLevelsTree;
  //   const int subtreeRoot;
  //   const int levelOfSubtree;
  //   const int numLevelsSubtree;
  // };

  // inline int clamp(int val, int lo, int hi)
  // { return max(min(val,hi),lo); }

                                       
  // /*! helper functions for a binary tree of exactly N nodes. For this
  //     paper, all we need to be able to compute is the size of any
  //     given subtree in this tree */
  // struct ArbitraryBinaryTree {
  //   inline ArbitraryBinaryTree(int numNodes)
  //     : numNodes(numNodes) {}
  //   inline int numNodesInSubtree(int n)
  //   {
  //     auto fullSubtree
  //       = SubTreeInFullTreeOf(BinaryTree::numLevelsFor(numNodes),n);
  //     const int lastOnLastLevel
  //       = fullSubtree.lastNodeOnLastLevel();
  //     const int numMissingOnLastLevel
  //       = clamp(lastOnLastLevel - numNodes, 0, fullSubtree.numOnLastLevel());
  //     const int result = fullSubtree.numNodes() - numMissingOnLastLevel;
  //     return result;
  //   }
  
  //   const int numNodes;
  // };

}

