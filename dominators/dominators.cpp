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
#include <set>
#include "../utils/node.h"

#define MAX_NODES 1000

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

/**
        Cooper, Harvey & Kennedy Algorithm
*/
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

                        if (block->preds.size() == 0) {
                                continue;
                        }

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

std::map<struct Node *, std::bitset<MAX_NODES> > compute_dominator_DFA (struct Node *root)
{
        std::vector<struct Node *> postorder = root->postorder_traversal ();

        std::map<struct Node *, std::bitset<MAX_NODES> > doms;
        for (auto n = postorder.rbegin (); n != postorder.rend (); n++) {
                doms[*n].set ((*n)->value);
        }

        bool has_changes;

        do {
                has_changes = false;

                for (auto n = postorder.rbegin (); n != postorder.rend (); n++) {
                        struct Node *bb = *n;

                        if (bb->preds.size () == 0)
                                continue;

                        std::bitset<MAX_NODES> curr_doms = doms[bb->preds[0]];

                        for (auto pred: bb->preds) {
                                curr_doms = curr_doms & doms[pred];
                        }

                        curr_doms.set (bb->value);

                        if (doms[bb] != curr_doms) {
                                doms[bb] = curr_doms;
                                has_changes = true;
                        }
                }

        } while (has_changes);

        return doms;
}
