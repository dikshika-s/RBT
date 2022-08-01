#include <stack>
#include <cassert>
#include <sstream>
#include <vector>
#include <stdexcept>



/* 
- In a BST, insert & delete operations take O(depth) time
- In order to constrain these, a colour [bool] is additionally stored on
each node to make depth = O(lg n) by RBT properties.
- Since nodes containing inserted values may be internal or leaf nodes, 
implementing rank or select in O(lg n) time isn't feasible without knowing
the size of any subtree. So this is appended to each node and used along with
BST properties to be able to perform these operations without traversing
multiple branches. 
- RangeSum could be found ay adding values of the nodes accessed through
in-order traversal of the next (j-i) nodes from the node at rank i
However traversal takes Theta(n) time (> O(lg n)). So the sum of values of 
its subtree is also augmented in each node, and used along with size & BST
properties to get this faster.
*/


class TreeNode {

    protected :
        int val;
        bool red = false;
        int sum;
        int size = 1;

        TreeNode* lc = nullptr;
        TreeNode* rc = nullptr;

    friend class RBST;

    public :
        TreeNode() {}
        TreeNode(int x, bool r=false) : val(x), red(r), sum(x) {}

};

// Is the Tree node a null leaf ? 
// (or meant to be one, during intermediate deletion procedures)
#define isNull(t) (t == nullptr || (t == doubleblack && t->red))

typedef std::vector<TreeNode*>::iterator vi;




class RBST {

    TreeNode* root = nullptr;

    void leftRotate(TreeNode*, TreeNode*);
    void rightRotate(TreeNode*, TreeNode*);
    void printSubtree(std::ostringstream&, const std::string&,
                      const TreeNode*, bool);
    void maintainRBT_ins(std::vector<TreeNode*>&);
    void maintainRBT_del(std::vector<TreeNode*>&);
    int prefixSumSubtree(TreeNode*, int);

    // There is atmost just 1 temporary doubleblack node at any time
    TreeNode* doubleblack = nullptr;
    // Node returned by the traversal iterator after the end
    TreeNode* endnode = new TreeNode;

    public :
        bool insert(int);
        bool remove(int);
        int rank(int);
        int select(int);
        int rangeSum(int, int);

        RBST() {}
        ~RBST();
        std::string print();
        int size() {return (root != nullptr)? root->size : 0;}

        class InOrderTraverser;
        InOrderTraverser begin() const;
        InOrderTraverser end() const;
};






RBST::~RBST() {
    // Destructor
    // Traverse the tree to free all memory allocated with `new`
    if (root != nullptr) {
        std::stack<TreeNode*> iot;  
        TreeNode *curr = root, *temp;
        // In-order traversal so that node's children dont need to be
        // accessed after it is deleted (a node is popped only once)
        while (!iot.empty()) {
            if (curr != nullptr) {
                iot.push(curr);
                curr = curr->lc;
            } else {
                curr = iot.top(); iot.pop();
                temp = curr->rc; delete curr; curr = temp;
            }
        }
    }
    delete endnode;
}


void RBST::leftRotate(TreeNode* node, TreeNode* parent) {
    /* 
            node                            rc
           /    \            ==>          /    \
          lc     rc                   node     ...
                /   \               /      \
           rc->lc   ...            lc    rc->lc
     */
    assert(node != nullptr); 
    assert(node->rc != nullptr);       // For debugging
    if (parent == nullptr) assert(node==root);
    else assert(parent->rc == node || parent->lc == node);
    // Adjust the augmented info (sum, size) of nodes being rotated, O(1)
    int tempsum1 = node->rc->val;
    int tempsum2 = 0, tempsize2 = 0;
    if (! isNull(node->rc->rc)) {
        tempsum2 = node->rc->rc->sum; tempsize2 =node->rc->rc->size;
    }
    node->rc->sum += node->val ;
    node->rc->size += 1 ;
    if (! isNull(node->lc)) {
        node->rc->sum += node->lc->sum; 
        node->rc->size += node->lc->size;
    }
    node->sum -= tempsum1 + tempsum2;
    node->size -= 1       + tempsize2;
    // Now perform the rotation, also O(1) time
    if (parent != nullptr) {
        if (parent->lc == node) parent->lc = node->rc;
        else parent->rc = node->rc;
    } else
        root = node->rc;
    TreeNode* temp = node->rc->lc;
    node->rc->lc = node;
    node->rc = temp;
}

void RBST::rightRotate(TreeNode* node, TreeNode* parent) {
    /* 
            node                            lc
           /    \            ==>           /   \
         lc     rc                       ...    node
        /  \                                  /      \
      ...   lc->rc                        lc->rc      rc
     */
    assert(node != nullptr);
    assert(node->lc != nullptr);       // For debugging
    if (parent == nullptr) assert(node==root);
    else assert(parent->rc == node || parent->lc == node);
    // Adjust the augmented info (sum, size) of nodes being rotated, O(1)
    int tempsum1 = node->lc->val;
    int tempsum2 = 0, tempsize2 = 0;
    if (! isNull(node->lc->lc)) {
        tempsum2 = node->lc->lc->sum; tempsize2 = node->lc->lc->size;
    }
    node->lc->sum += node->val ;
    node->lc->size += 1 ;
    if (! isNull(node->rc)) {
        // If node->rc is not null (take care of intermediate deletion stages)
        node->lc->sum += node->rc->sum;
        node->lc->size += node->rc->size;
    }
    node->sum -= tempsum1 + tempsum2;
    node->size -= 1       + tempsize2;
    // Now perform the rotation, also O(1) time
    if (parent != nullptr) {
        if (parent->lc == node) parent->lc = node->lc;
        else parent->rc = node->lc;
    } else
        root = node->lc;
    TreeNode* temp = node->lc->rc;
    node->lc->rc = node;
    node->lc = temp;
}


void RBST::printSubtree(std::ostringstream& out, 
        const std::string& pref, const TreeNode* node, bool l) {
    // This code block is a Modified version of https://stackoverflow.com/a/51730733

    out << pref << (l ? "\u251c\u2500\u2500" : "\u2514\u2500\u2500" );
    if( node != nullptr ) {
        out << ((node->red)? "\u001b[91m[" : "[") << node->val  << 
            "] " << node->size << ", " << node->sum  << 
            ((node->red)? "\u001b[0m\n" : "\n");

        printSubtree(out, pref+(l? "\u2502   ":"    "), node->lc, true);
        printSubtree(out, pref+(l? "\u2502   ":"    "), node->rc, false);
    } else {
        out << "\u257a\n"; // Null leaves printed as "dots"
    }
}

std::string RBST::print() {
    // For debugging purposes 
    std::ostringstream output;
    printSubtree(output, "", root, false);
    return output.str();
}


bool RBST::insert(int x) {
    if (root==nullptr) {
        root = new TreeNode(x, false);
        return true;
    }
    TreeNode* t = root;
    // Temporary O(height) auxiliary space during insertion
    // saves us from having to use O(n) space by permanently storing
    // the parent in each node, while keeping O(lg n) time
    std::vector<TreeNode*> ancestry;
    while (t != nullptr) {
        if (t->val == x)
            return false;
        ancestry.push_back(t);
        if (x < t->val)
            t = t->lc;
        else
            t = t->rc;
    }

    t = ancestry.back();
    TreeNode* n = new TreeNode(x, true);
    if (x < t->val) t->lc = n; 
    else            t->rc = n;

    // Adjust augmented sum/size info in nodes above it
    for (vi it=ancestry.begin(); it<ancestry.end(); ++it) {
        (*it)->size ++;
        (*it)->sum += x;
    }
    ancestry.push_back(n);
    
    // Check that the tree remains a valid RBT, and finish
    maintainRBT_ins(ancestry);
    root->red = false;
    return true;
}


void RBST::maintainRBT_ins(std::vector<TreeNode*>& ancestry) {
    // Note : ancestry contains all nodes from root till newly 
    // inserted leaf along its branch in sequence
    if (ancestry.size() < 2)
        return; // New node does not have grandparent, no possible conflicts
    
    TreeNode *c = ancestry.back(), *u,
        *p = ancestry.end()[-2], *g = ancestry.end()[-3];
    if (! (c->red && p->red))
        return; // Child & parent aren't both red, no possible conflicts
    
    bool side; // 1 if c is in right subtree, 0 if left
    if (p == g->rc) {
        u = g->lc; side = true;
    } else {
        u = g->rc; side = false;
    }

    if (u != nullptr && u->red) {
        // child, parent, uncle all red
        g->red = true; u->red = false; p->red = false;
        ancestry.pop_back(); ancestry.pop_back();
        maintainRBT_ins(ancestry);
        // Continue checking upwards from grandparent
    
    } else { // child & parent red, uncle black

        if (side && p->lc == c) {// Convert triangle case to line case
            rightRotate(p, g);
            std::swap(p, c);
        } else if (!side && p->rc == c) {
            leftRotate(p, g);
            std::swap(p, c);
        }
                                // Line case
        if (side) 
            leftRotate(g, (g==root) ? nullptr : ancestry.end()[-4]);
        else
            rightRotate(g, (g==root) ? nullptr : ancestry.end()[-4]);
        p->red = false;
        g->red = true;

        // Continue checking upwards from parent (now at location of grandp)
        ancestry.pop_back(); ancestry.pop_back(); ancestry.pop_back();
        ancestry.push_back(p);
        maintainRBT_ins(ancestry);
    }
}


bool RBST::remove(int x) {
    TreeNode* t = root;
    std::vector<TreeNode*> ancestry;
    // Temporary O(height) auxiliary space, used similarly as in insertion
    while (t != nullptr) {
        ancestry.push_back(t);  // Search for the node
        if (x == t->val)
            break;
        else if (x < t->val)
            t = t->lc;
        else
            t = t->rc;
    }
    if (t==nullptr)
        return false; // Node is not present


    // In case of 2 non-null children, use in-order successor
    // (will have 1 non-null child at most)
    if (t->lc != nullptr && t->rc != nullptr) {
        TreeNode* ios = t->rc;
        ancestry.push_back(ios); 
        while (ios->lc != nullptr) {
            ios = ios->lc;
            ancestry.push_back(ios);
        }
        int diff = ios->val - t->val;
        t->val = ios->val; t->sum += diff; // Replace value; 
        // Now adjust augmented info, propagate upwards
        for (vi it=ancestry.begin(); *it != t; ++it)
            (*it)->sum += diff;
        t = ios; //Change node to delete to the successor
    }


    bool simple = false;
    TreeNode* g = ancestry.end()[-2];
    // Simple cases (non-recursive)
    if (t==root && t->lc == nullptr && t->rc == nullptr) {
        root = nullptr;    // Delete root when it is the only node
        simple = true;
    } else if (t->red) {  // Node to delete is red
        TreeNode* temp = (t->rc != nullptr)? t->rc : t->lc;
        if (g->rc == t) g->rc = temp;
        else g->lc = temp;
        simple = true;
    } else if (t->lc != nullptr && t->rc == nullptr) {
        // Black node, with 1 left child only (will be red)
        assert(t->lc->red); simple = true; 
        t->lc->red = false; // Change red to black
        if (t != root) {
            if (g->rc == t) g->rc = t->lc;
            else  g->lc = t->lc;
        } else
            root = t->lc;
    } else if (t->lc == nullptr && t->rc != nullptr) {
        // Black node, with 1 right child only (will be red)
        assert(t->rc->red); simple = true;
        t->rc->red = false; // Change red to black
        if (t != root) {
            if (g->rc == t) g->rc = t->rc;
            else  g->lc = t->rc;
        } else
            root = t->rc;
    }


    // Propagate up, changing augmented sum & size values till root
    for (vi it=ancestry.begin(); *it != t; ++it) {
        (*it)->size --;
        (*it)->sum -= t->val;
    }

    if (simple) {
        delete t;
    } else {
        // The (only) complex case - black leaf node
        assert(t->lc==nullptr && t->rc==nullptr);
        doubleblack = t;
        t->red = true;
        /* NOTE : Since we are using `nullptr` to denote NULL leaves, 
                It isn't possible to store additonal info like "double black"
                on those. => Do not delete immediately, but mark it
                and use the now redundant colour bit (TreeNode.red) to
                store whether it should be null (1) or not (0) */
        maintainRBT_del(ancestry);
    }
    return true;
}


void RBST::maintainRBT_del(std::vector<TreeNode*>& ancestry) {
    // The last node is always double black, when called
    TreeNode *u = ancestry.back();
    assert(u == doubleblack);
    if (u==root) {  // Case 3
        doubleblack = nullptr;
        return;
    }
    assert(ancestry.size() > 1);
    TreeNode *g = ancestry.end()[-2], *p, *c, *d;
    bool side = (g->lc == u);
    if (side) {
        p = g->rc; assert(p != nullptr); // will have a sibling
        c = p->lc; d = p->rc;
    } else {
        p = g->lc; assert(p != nullptr); // will have a sibling
        c = p->rc; d = p->lc;
    }

    if (p->red) { // Case 1, sibling red
        TreeNode* a = (ancestry.size() > 2)? ancestry.end()[-3] : nullptr;
        if (side)
            leftRotate(g, a);
        else 
            rightRotate(g, a);
        p->red = false;
        g->red = true;
        ancestry.pop_back(); ancestry.pop_back();
        ancestry.push_back(p); ancestry.push_back(g); ancestry.push_back(u);
        maintainRBT_del(ancestry);
    }
    else if (!p->red && (c==nullptr || !c->red) && (d==nullptr || !d->red)) {
        // sibling & both its children black
        if (g->red) {   // Case 2.1.2
            p->red = true; g->red = false;
        } else {        // Case 2.1.1
            p->red = true;
            doubleblack = g;
            ancestry.pop_back();
            maintainRBT_del(ancestry);
        }
        if (! u->red)
            doubleblack = nullptr;
        else {
            if (side) g->lc = nullptr;
            else g->rc = nullptr;
            delete u;
        }
    } 
    else if (!p->red && (d==nullptr || !d->red) && (c!=nullptr && c->red)) {
        c->red = false; p->red = true;      // Case 2.2
        if (side)
            rightRotate(p, g);
        else 
            leftRotate(p, g);
        maintainRBT_del(ancestry);
    } 
    else if (!p->red && (d!=nullptr && d->red)) {   // Case 2.3
        p->red = g->red;
        g->red = false; d->red = false;
        TreeNode* a = (ancestry.size() > 2)? ancestry.end()[-3] : nullptr;
        if (side)
            leftRotate(g, a);
        else 
            rightRotate(g, a);
        if (! u->red)
            doubleblack = nullptr;
        else {
            if (side) g->lc = nullptr;
            else g->rc = nullptr;
            delete u;
        }
    }
}


int RBST::rank(int x) {
    // Return 0 if not found, else a rank from 1..(tree.size)
    int r = 0;
    TreeNode* n = root;
    while (n != nullptr) {
        if (x < n->val) {
            n = n->lc;
        } else {
            r += (n->lc != nullptr) ? n->lc->size + 1 : 1;
            if (x == n->val)
                break;
            else
                n = n->rc;
        }
    }
    if (n==nullptr)
        return 0;
    else
        return r;
}


int RBST::select(int r) {
    // Given rank must be in range
    if (r < 1 || r > root->size) {
        throw std::out_of_range("Invalid index " + std::to_string(r) + 
        ". Extent is 1.." + std::to_string(root->size));
    }
    assert(root != nullptr);
    int cr = (root->lc != nullptr)? root->lc->size + 1 : 1;
    TreeNode* n = root; // Start at top
    while (cr != r) {
        if (cr < r) {   // Move right
            n = n->rc; 
            cr += 1 + ((n->lc != nullptr)? n->lc->size : 0);
        } else if (cr > r) { // Move left
            n = n->lc; 
            cr -= 1 + ((n->rc != nullptr)? n->rc->size : 0);
        }
    }
    return n->val;
}


int RBST::prefixSumSubtree(TreeNode* n, int j) {
    assert(j >= 1 && j <= n->size);
    if (n==nullptr)
        return 0;
    if (n->lc == nullptr && n->rc == nullptr)
        return n->val;
    int lsize = (n->lc != nullptr) ? n->lc->size : 0;
    int lsum  = (n->lc != nullptr) ? n->lc->sum : 0;
    // In each case, subtree height reduces 1 step
    if (lsize > j-1)   // Move left
        return prefixSumSubtree(n->lc, j);
    else if (lsize + 1 == j)
        return n->val + lsum;
    else            // Move right
        return n->val + lsum +  prefixSumSubtree(n->rc, j-1-lsize);
}

int RBST::rangeSum(int i, int j) {
    if (i < 1 || i > root->size) {
        throw std::out_of_range("Invalid start index " + std::to_string(i) + 
        ". Extent is 1.." + std::to_string(root->size));
    } else if (j < 1 || j > root->size) {
        throw std::out_of_range("Invalid end index " + std::to_string(j) + 
        ". Extent is 1.." + std::to_string(root->size));
    }
    // Prefixsum takes O(lg n) time, this is the same
    if (i==1) {
        return prefixSumSubtree(root, j);
    } else {
        return prefixSumSubtree(root, j) - prefixSumSubtree(root, i-1);
    }
}



/* 
Similar to an Input iterator, allows reading the values of all nodes
in the tree sequentially in ascending order, with approxiamtely O(N)
complexity overall. The iterator maintains a stack (auxiliary O(lg N) space)
to do this internally. Otherwise, alternative would be to call
RBST::select(1), RBST::select(2), ... RBST::select(N) where each individual
call takes O(lg N) complexity because they are stateless/independent
Modifying the tree (insertion/deletion) in-between traversal will likely break 
any/all of these iterators that are being used meanwhile
 */
class RBST::InOrderTraverser {
            
    TreeNode* n;
    std::stack<TreeNode*> stk;
    TreeNode* en;
    void send_to_end() {n = en;}

    friend bool operator==(const InOrderTraverser& a,
                            const InOrderTraverser& b);
    friend bool operator!=(const InOrderTraverser& a,
                            const InOrderTraverser& b);
    friend class RBST;
    public :
        InOrderTraverser(const RBST& tree);
        int& operator*() const {return n->val;}
        InOrderTraverser& operator++();
        InOrderTraverser  operator++(int) {
            InOrderTraverser copy(*this); ++(*this); return copy;
        };
};


RBST::InOrderTraverser::InOrderTraverser(const RBST& tree) {
    en = tree.endnode;
    if (tree.root == nullptr) {
        n = tree.endnode;
    } else {
        n = tree.root; 
        stk.push(n);
        while (n->lc != nullptr) {
            n = n->lc;
            stk.push(n);
        }
    }
}

RBST::InOrderTraverser& RBST::InOrderTraverser::operator++() {
    n = n->rc;
    if (n != nullptr) {
        stk.pop();
        stk.push(n);
    }
    while (!stk.empty()) {
        if (n != nullptr) {
            n = n->lc; stk.push(n); 
        } else {
            stk.pop(); 
            if (!stk.empty())
                n = stk.top();
            else 
                n = en;
            return *this; 
        }
    }
    return *this;
}

inline bool operator==(const RBST::InOrderTraverser& a, 
                       const RBST::InOrderTraverser& b) {
    return a.n == b.n;
}

inline bool operator!=(const RBST::InOrderTraverser& a, 
                       const RBST::InOrderTraverser& b) {
    return a.n != b.n;
}



RBST::InOrderTraverser RBST::begin() const {
    InOrderTraverser iot(*this);
    return iot;
}

RBST::InOrderTraverser RBST::end() const {
    InOrderTraverser iot(*this);
    iot.send_to_end();
    return iot;
}