#pragma once
#include <vector>
#include <map>
struct Node {
        int value;
        bool visited;
        struct Node *left;
        struct Node *right;
        std::vector<struct Node *> preds;

        void make_left_child (struct Node *child);
        void make_right_child (struct Node *child);
        std::vector<struct Node *> postorder_traversal ();
};
std::map<int, struct Node *> build_graph (char *graph_file);
void node_init (struct Node *n);