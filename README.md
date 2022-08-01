## RBST

This is built on my solution for a programming assignment, in which the task was to :

 > Implement in C++, a Red-Black (self- height balancing) Binary Search Tree to store unique integers, that supports these 5 operations in `O(lg N)` time complexity each :
 > - *Insert* a new (unique) `int` into the tree
 > - *Delete* an `int` if it is present
 > - Find the *Rank* (position 1..N in sorted order among all keys) of a given `int`, or determine that it is not present.
 > - *Get* the `int` key at a given rank in the same range
 > - Compute the *Range Sum* of any subset of ranks (integer sum of all keys with rank ≥ `i` and rank ≤ `j`, with `1≤i≤j≤N`)

Additionally, this tree also has the following functionality :
- *Print* the tree structure, with all branches and Red/Black nodes coloured using ANSI escape codes
- Query the *Size* (i.e. `N`, number of nodes) in the tree, in `O(1)` time
- *Traverse* the tree, reading all keys present in their ascending order, in `O(N)` time

Select and RangeSum throw `std::out_of_range` if invalid index/position parameters are passed.

You can run [test.cpp](./test.cpp) to interact with the tree in this implementation. (`class RBST`). The [shell script](./stress-test.sh) inserts/deletes thousands of elements at once, usually takes ~0.5 sec (affected by how fast your terminal console prints the output, not real timing)

-----
Each tree Node is also as space efficient as I could make it with the above constraints, using only as much memory as is required for :
- 3 `int`s, the actual key along with 2 others related to position/sum 
- 2 pointers to other `TreeNode`s (left/right children) only
- 1 `bool`, for the node colour

And null leaves (often `O(N/2)` in number !) are represented using `nullptr` instead of actual TreeNode objects somehow marked as empty, which saves a lot more space.

-----
The iterator `RBST::InOrderTraverser` currently has a lot more scope for improvement.
