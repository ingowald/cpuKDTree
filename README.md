# cpuKDTree - A Library for Building and Querying Left-Balanced (point-)kd-Trees in CPU Code

This is the "CPU Equivalent" of my github `cudaKDTree` project
(https://github.com/ingowald/cudaKDTree); for those that for some
reason cannot or do not want to use a GPU (the GPU version will be
MUCH faster, though).


Left-balanced kd-trees can be used to store and query k-dimensional
data points; their main advantage over other data structures is that
they can be stored without any pointers or other admin data - which
makes them very useful for large data sets, and/or where you want to
be able to predict how much memory you are going to use.

This repo contains regular C++-11 code two kinds of operations:
*building* such trees, and *querying* them.

## Building Left-balanced KD-Trees

The main builder provide dy this repo is one for those that are
left-balanced, and where the split dimension in each level of the tree
is chosen in a round-robin manner; i.e., for a builder over float3
data, the root would split in x coordinate, the next level in y, then
z, then the fourth level is back to x, etc. I also have a builder
where the split dimension gets chosen based on the widest extent of
the given subtree, but that one isn't included yet - let me know if
you need it.

The builder is templated over the type of data points; to use it, for
example, on float3 data, use thefollwing

    cukd::buildTree<float3,float,3>(points[],numPoints);
	
To do the me on float4 data, use 

    cukd::buildTree<float4,float,4>(points[],numPoints);
	
More interestingly, if you want to use a data type where you have
three float coordinates per point, and one extra 32-bit value as
"payload", you can, for example, use a `float4` for that point type,
store each point's payload value in its `float4::w` member, and then
build as follows:
	
    cukd::buildTree<float4,float,3>(points[],numPoints);
	
In this case, the biulder known that the structs provided by the user
are `float4`, but that the actual *points* are only *three* floats.

The builder included in this repo makes use of thrust for sorting; it
runs entirely on the GPU, with complexity O(N log^2 N) (and parallel
complexity O(N/k log^2 N), where K is the number of processors/cores);
it also needs only one int per data point in temp data storage during
build (plus however much thrust::sort is using, which is out of my
control).

## Stack-Free Traversal and Querying

This repo also contains a stack-free traversal code for doing
"shrinking-radius range-queries" (i.e., radius range queries where the
radius can shrink during traversal). This traversal code is used in
two examples: *fcp* (for find-closst-point) and *knn* (for k-nearest
neighbors).

<needs documenting>

	
