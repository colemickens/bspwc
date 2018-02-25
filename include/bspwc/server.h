#ifndef BSPWC_H
#define BSPWC_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/render.h>
#include <wlr/render/gles2.h>
#include <wlr/xwayland.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_gamma_control.h>
#include <wlr/types/wlr_idle.h>
#include <wlr/types/wlr_primary_selection.h>
#include <wlr/types/wlr_screenshooter.h>
#include <wlr/types/wlr_server_decoration.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/util/log.h>

#include "bspwc/common.h"
#include "bspwc/handles.h"
#include "bspwc/monitor.h"

struct server
{
	struct wl_display* display;
	struct wlr_backend* backend;
    struct wlr_renderer* renderer;
    
    struct wl_listener new_monitor;
    struct wl_list monitors;

    struct wlr_output_layout* layout;
    struct wl_listener layout_change;

    struct wlr_xdg_shell_v6* xdg_shell_v6;
    struct wl_listener xdg_shell_v6_surface;
    
    struct wlr_wl_shell* wl_shell;
    struct wl_listener wl_shell_surface;
    
    struct wlr_xwayland* xwayland;
    struct wl_listener xwayland_surface;
    
    struct wlr_xcursor_theme* xcursor_theme;
    struct wlr_xcursor_manager* xcursor_manager;

    struct wl_listener decoration_new;

    struct wl_event_loop* event_loop;
    struct wl_event_source* input_event;
    struct wlr_compositor* compositor;
    struct wlr_cursor* cursor;

    // BSPWM related stuff
    int socket;
    char* socket_name;
};

bool init_server(struct server* s);
bool setup_bspwc(struct server* s);
bool start_server(struct server* s);
bool terminate_server(struct server* s);

#endif // BSPWC_H
