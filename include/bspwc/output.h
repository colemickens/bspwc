#ifndef OUTPUT_H
#define OUTPUT_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <wayland-server.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_matrix.h>
#include <wlr/types/wlr_output.h>

#include "bspwc/desktop.h"
#include "bspwc/server.h"
#include "bspwc/window.h"

struct output
{
	struct server *server;
	struct desktop *desktop;

	struct wlr_output *wlr_output;
	struct timespec last_frame;

	struct wl_listener destroy;
	struct wl_listener frame;

	struct wl_list link; // backend::outputs
};

void output_destroy_notify(struct wl_listener* listener, void* data);
void output_frame_notify(struct wl_listener* listener, void* data);
void new_output_notify(struct wl_listener* listener, void* data);

#endif // OUTPUT_H
