#pragma once

void create_graph_new (char *graph_file, void (*make_child) (int parent, int child));