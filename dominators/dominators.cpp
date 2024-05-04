/**
    Computing Immediate Dominators

    Cooper, Harvey & Kennedy Algorithm for computing the dominator tree of a CFG

    Paper: https://www.cs.tufts.edu/comp/150FP/archive/keith-cooper/dom14.pdf

    Great visualization of the algorithm:

    Source: https://sbaziotis.com/compilers/visualizing-dominators.html

*/

#include <bitset>
#include <iterator>
#include <map>
#include <vector>

struct Node {
        int value;
        bool visited;
        struct Node *left;
        struct Node *right;
        std::vector<struct Node *> preds;

        void make_left_child (struct Node *child)
        {
                this->left = child;

                child->preds.push_back (this);
        }

        void make_right_child (struct Node *child)
        {
                this->right = child;

                child->preds.push_back (this);
        }

        std::vector<struct Node *> postorder_traversal ()
        {
                this->visited = true;

                std::vector<struct Node *> left;

                if (this->left && !this->left->visited)

                        left = this->left->postorder_traversal ();

                if (this->right && !this->right->visited) {
                        std::vector<struct Node *> right = this->right->postorder_traversal ();

                        left.insert (left.end (), right.begin (), right.end ());
                }

                left.push_back (this);

                return left;
        }
};

struct Node *intersect (struct Node *a,
                        struct Node *b,
                        std::map<struct Node *, int> visit_order,
                        std::map<struct Node *, struct Node *> idom)
{
        while (a != b) {
                if (visit_order[a] > visit_order[b]) {
                        a = idom[a];
                } else {
                        b = idom[b];
                }
        }

        return b;
}

std::map<struct Node *, struct Node *> compute_dominator_tree (struct Node *root)
{
        std::vector<struct Node *> postorder = root->postorder_traversal ();

        std::map<struct Node *, int> visit_order;

        int count = 0;

        for (auto n = postorder.rbegin (); n != postorder.rend (); n++) {
                visit_order[*n] = count++;
        }

        std::map<struct Node *, struct Node *> idoms;

        idoms[root] = root;
        bool has_changes;

        /**
                Loops may occur in the CFG, thus we need an outer most loop to
                track changes.
        */
        do {
                has_changes = false;
                for (auto bb = postorder.rbegin (); bb != postorder.rend (); bb++) {
                        struct Node *block = *bb;
                        struct Node *dom = block->preds[0];

                        for (auto pred : block->preds) {
                                // if there is a loop this condition will be false
                                if (idoms.find (block) != idoms.end ()) {
                                        dom = intersect (dom, pred, visit_order, idoms);
                                }
                        }

                        if (idoms[block] != dom) {
                                idoms[block] = dom;
                                has_changes = true;
                        }
                }
        } while (has_changes);

        return idoms;
}

std::map<struct Node *, struct Node *> compute_dominator_DFA(struct Node *root) {
        
}

int main (int argc, char **argv)
{
        struct Node p;

        return 0;
}