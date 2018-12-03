#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "bspwc/desktop.h"
#include "bspwc/config.h"
#include "bspwc/server.h"
#include "bspwc/window.h"

struct desktop;
struct server;
struct backend;


struct node
{
	struct window *window;

	struct node *parent;
	struct node *left;
	struct node *right;

	struct wl_list link; // desktop::nodes
};

struct node *create_node(struct desktop *desktop);
void destroy_node(struct node *node);

bool insert_node(const struct server *server, struct node **root,
		struct node* child);
void render_tree(const struct node *root);

#endif // TREE_H
