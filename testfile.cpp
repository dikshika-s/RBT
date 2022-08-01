#include <iostream>
#include <exception>

#include "rbt.hpp"

#define MINIMAL_OUTPUT
/* By defining this flag, it is easier to automate operations
without having to parse all the user-friendly messages. Example -
```
2 10
2 5
2 4
2 2
2 3
2 -1
2 15
2 -13
2 133
1
0
```
to insert those elements, print the tree and exit
 */


using namespace std;


int main() {

    RBST tree;

#ifndef MINIMAL_OUTPUT
    std::cout << "Before\n" << tree.print();
    tree.insert(10);
    std::cout << "Inserted 10\n" << tree.print();
    tree.insert(5);
    std::cout << "Inserted 5\n" << tree.print();
    tree.insert(5);
    std::cout << "Inserted 5\n" << tree.print();
    tree.insert(4);
    std::cout << "Inserted 4\n" << tree.print();
    tree.insert(2);
    std::cout << "Inserted 2\n" << tree.print();
    tree.insert(3);
    std::cout << "Inserted 3\n" << tree.print();
    tree.insert(15);
    std::cout << "Inserted 15\n" << tree.print();
    tree.insert(12);
    std::cout << "Inserted 12\n" << tree.print();
    tree.insert(13);
    std::cout << "Inserted 13\n" << tree.print();
    tree.remove(10);
    std::cout << "\nDeleted 10\n" << tree.print();
    tree.remove(5);
    std::cout << "Deleted 5\n" << tree.print();
    tree.remove(5);
    std::cout << "Deleted 5\n" << tree.print();
    tree.remove(15);
    std::cout << "Deleted 15\n" << tree.print();
    tree.remove(3);
    std::cout << "Deleted 3\n" << tree.print();
    tree.remove(13);
    std::cout << "Deleted 13\n" << tree.print();
    tree.remove(2);
    std::cout << "Deleted 2\n" << tree.print();
#endif

    // Begin menu-driven loop
    int opt=1, x, y;
    do {
        #ifndef MINIMAL_OUTPUT
            std::cout << "Operations :\n[0] Exit\t[1] Print\t[2] Insert\t[3] Delete\t"  << 
            "[4] Rank\t[5] Select\t[6] RangeSum\t[7] Get Size\t[8] Traverse\nSelect choice - ";
        #endif
        std::cin >> opt;
        switch (opt) {
            case 1:
                std::cout << tree.print(); break;
            case 2:
                #ifndef MINIMAL_OUTPUT
                    std::cout << "Element to insert> ";
                #endif
                std::cin >> x;
                #ifndef MINIMAL_OUTPUT
                    std::cout << (tree.insert(x)?"Ok":"Duplicate") << '\n' << tree.print();
                #else
                    tree.insert(x);
                #endif
                break;
            case 3:
                #ifndef MINIMAL_OUTPUT
                    std::cout << "Element to remove> "; 
                #endif
                std::cin >> x;
                #ifndef MINIMAL_OUTPUT
                    std::cout << (tree.remove(x)?"Ok":"Not found") << '\n' << tree.print();
                #else
                    tree.remove(x);
                #endif
                break;
            case 4:
                #ifndef MINIMAL_OUTPUT
                    std::cout << "Element to search> ";
                #endif
                std::cin >> x;
                std::cout << tree.rank(x) << '\n';
                break;
            case 5:
                #ifndef MINIMAL_OUTPUT
                    std::cout << "Rank to get> ";
                #endif
                std::cin >> x;
                try {
                    std::cout << tree.select(x) << '\n';
                } catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
                break;
            case 6:
                #ifndef MINIMAL_OUTPUT
                    std::cout << "Lower & upper bounds> ";
                #endif
                std::cin >> x >> y;
                try {
                    std::cout << tree.rangeSum(x, y) << '\n';
                } catch (const std::exception& e) { 
                    std::cerr << e.what() << std::endl;
                }
                break;
            case 7:
                std::cout << tree.size() << '\n';
                break;
            case 8:
                RBST::InOrderTraverser en = tree.end();
                for (RBST::InOrderTraverser it = tree.begin(); it != en; ++it) {
                    std::cout << *it << '\n';
                }
                break;
        }
    } while (opt != 0);

    return 0; 
}