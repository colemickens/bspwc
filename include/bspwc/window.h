#ifndef WINDOW_H
#define WINDOW_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <time.h>

#include <wlr/types/wlr_xdg_shell_v6.h>

#include "bspwc/backend.h"
#include "bspwc/desktop.h"
#include "bspwc/xdg_surface_v6.h"

enum window_type
{
	NONE,
	WL_SHELL,
	XDG_SHELL,
	XDG_SHELL_V6,
	XWAYLAND,
};

struct window
{
	char *title;
	double x, y;
	uint32_t width, height;
	float alpha;

	struct desktop *desktop;

	enum window_type type;
	union
	{
		struct wlr_xdg_surface_v6 *wlr_xdg_surface_v6;
	};

	union
	{
		struct xdg_surface_v6 *xdg_surface_v6;
	};

	struct wlr_surface *wlr_surface;

	struct wl_listener new_subsurface;

	struct wl_signal event_unmap;
	struct wl_signal event_destroy;
};

struct window *create_window();
void destroy_window(struct window *window);

struct wlr_box get_window_box(const struct window *window);
void set_window_position(struct window *window, const double x, const double y);
void resize_window(struct window *window, uint32_t width, uint32_t height);
void render_window(const struct window *window);
void map_window(struct window *window, struct wlr_surface *wlr_surface);
void focus_window(struct window *window);

#endif // WINDOW_H
