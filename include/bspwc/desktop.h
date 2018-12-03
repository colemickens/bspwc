#ifndef DESKTOP_H
#define DESKTOP_H

#include <wayland-server.h>

#include "bspwc/output.h"
#include "bspwc/tree.h"
#include "bspwc/window.h"

#define DEFAULT_DESKTOP_NAME "desktop"
#define DESKTOP_NAME_SIZE 8

struct desktop
{
	struct output *output;

	char name[DESKTOP_NAME_SIZE];

	struct desktop *next;
	struct desktop *previous;

	struct node* root;
	struct wl_list nodes; // node::link
};

struct desktop *create_desktop(struct output *output);
void destroy_desktop(struct desktop *desktop);

#endif // DESKTOP_H
