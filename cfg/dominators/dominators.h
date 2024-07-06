#pragma once
#include <bitset>
#include <map>
#include <vector>
#define MAX_NODES 1000

std::map<struct Node *, std::bitset<MAX_NODES> > compute_dominator_DFA (struct Node *root);
std::map<struct Node *, struct Node *> compute_dominator_tree (struct Node *root);
