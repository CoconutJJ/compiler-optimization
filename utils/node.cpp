
#include "node.h"
#include "../cfg-builder/builder.h"
#include <cstdio>
#include <map>
#include <vector>

static std::map<int, struct Node *> node_map;

void Node::make_left_child (struct Node *child)
{
        this->left = child;

        child->preds.push_back (this);
}

void Node::make_right_child (struct Node *child)
{
        this->right = child;

        child->preds.push_back (this);
}

std::vector<struct Node *> Node::postorder_traversal ()
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

void node_init (struct Node *n)
{
        n->left = NULL;
        n->right = NULL;
        n->value = -1;
        n->visited = false;
}

void make_child (int parent, int child)
{
        struct Node *parent_node, *child_node;
        if (!node_map.count (parent)) {
                parent_node = (struct Node *)malloc (sizeof (struct Node));
                node_init (parent_node);
                parent_node->value = parent;

                node_map[parent] = parent_node;
        } else {
                parent_node = node_map[parent];
        }

        if (!node_map.count (child)) {
                child_node = (struct Node *)malloc (sizeof (struct Node));
                node_init (child_node);

                child_node->value = child;
                node_map[child] = child_node;
        } else {
                child_node = node_map[child];
        }

        if (!parent_node->left) {
                parent_node->make_left_child (child_node);
        } else if (!parent_node->right) {
                parent_node->make_right_child (child_node);
        } else {
                fprintf (
                        stderr,
                        "WARNING: ignoring attempt to assign node (%d) to parent (%d) that already has 2 child nodes\n",
                        child,
                        parent);
        }
}

std::map<int, struct Node *> build_graph (char *graph_file)
{
        node_map.clear ();

        create_graph (graph_file, &make_child);

        return node_map;
}