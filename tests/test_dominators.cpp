#include "../dominators/dominators.h"
#include "../utils/node.h"
#include <cstdio>
#include <map>
int main (int argc, char **argv)
{
        std::map<int, struct Node *> map = build_graph (argv[1]);

        struct Node *node = map[1];

        std::map<struct Node *, struct Node *> idoms = compute_dominator_tree (node);

        for (auto map_item : idoms) {
                printf ("%d immediately dominates %d\n", map_item.second->value, map_item.first->value);
        }
}
