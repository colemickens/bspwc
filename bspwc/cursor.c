#include "bspwc/cursor.h"

void handle_cursor_motion(struct wl_listener *listener, void *data)
{
	struct cursor *cursor = wl_container_of(listener, cursor, motion);
	struct wlr_event_pointer_motion *event = data;

	wlr_cursor_move(cursor->wlr_cursor, event->device, event->delta_x,
			event->delta_y);

	cursor_motion(cursor, event->time_msec);
}

void handle_cursor_motion_absolute(struct wl_listener *listener, void *data)
{
	struct cursor *cursor = wl_container_of(listener, cursor, motion_absolute);
	struct wlr_event_pointer_motion_absolute *event = data;

	wlr_cursor_warp_absolute(cursor->wlr_cursor, event->device, event->x,
			event->y);

	cursor_motion(cursor, event->time_msec);
}

void handle_cursor_button(struct wl_listener *listener, void *data)
{
	struct cursor *cursor = wl_container_of(listener, cursor, button);
	struct wlr_seat *seat = cursor->input->seat;
	struct wlr_event_pointer_button *event = data;

	wlr_seat_pointer_notify_button(seat, event->time_msec, event->button,
			event->state);

	if (event->state == WLR_BUTTON_RELEASED)
	{
		wlr_log(WLR_DEBUG, "Cursor %p released", (void*)cursor);
		cursor->cursor_mode = CURSOR_MODE_PASSTHROUGH;
	}
	else // event->state == WLR_BUTTON_PRESSED
	{
		wlr_log(WLR_DEBUG, "Cursor %p pressed", (void*)cursor);
		wlr_log(WLR_INFO, "TODO: handle_cursor_button WLR_BUTTON_PRESSED");
	}
}

void handle_cursor_axis(struct wl_listener *listener, void *data)
{
	struct cursor *cursor = wl_container_of(listener, cursor, button);
	struct wlr_seat *seat = cursor->input->seat;
	struct wlr_event_pointer_axis *event = data;

	wlr_seat_pointer_notify_axis(seat, event->time_msec, event->orientation,
			event->delta, event->delta_discrete, event->source);

}

void cursor_motion(struct cursor *cursor, uint32_t time)
{
	const struct backend *backend = cursor->input->server->backend;
	struct wlr_seat *seat = cursor->input->seat;

	struct window *window = window_at(backend, cursor->wlr_cursor->x,
			cursor->wlr_cursor->y);

	if (window != NULL)
	{
		wlr_log(WLR_DEBUG, "Window under cursor is %p", (void*)window);
		struct wlr_surface *surface = window->wlr_surface;

		wlr_seat_pointer_notify_enter(seat, surface, cursor->wlr_cursor->x,
				cursor->wlr_cursor->y);
		if (seat->pointer_state.focused_surface != surface)
		{
			/* The enter event contains coordinates, so we only need to notify
			 * on motion if the focus did not change. */
			wlr_seat_pointer_notify_motion(seat, time, cursor->wlr_cursor->x,
				cursor->wlr_cursor->y);
		}
	}
	else
	{
		wlr_seat_pointer_clear_focus(seat);
	}
}

struct cursor *create_cursor(struct input *input,
		struct wlr_input_device *device)
{
	wlr_log(WLR_DEBUG, "Creating cursor");
	struct cursor *cursor = malloc(sizeof(struct cursor));
	if (cursor == NULL)
	{
		wlr_log(WLR_ERROR, "Failed to create cursor");
		return NULL;
	}

	cursor->input = input;

	cursor->wlr_cursor = wlr_cursor_create();
	if (cursor->wlr_cursor == NULL)
	{
		wlr_log(WLR_ERROR, "Failed to create cursor's wlr_cursor");
		free(cursor);
		return NULL;
	}
	wlr_cursor_attach_input_device(cursor->wlr_cursor, device);

	struct wlr_output_layout *output_layout = input->server->output_layout;
	wlr_cursor_attach_output_layout(cursor->wlr_cursor, output_layout);

	cursor->wlr_xcursor_manager = wlr_xcursor_manager_create("default", 24);
	if (cursor->wlr_xcursor_manager == NULL)
	{
		wlr_log(WLR_ERROR, "Failed to create wlr_xcursor_manager");
		free(cursor);
		return NULL;
	}

	wlr_xcursor_manager_load(cursor->wlr_xcursor_manager, 1);
	wlr_xcursor_manager_set_cursor_image(cursor->wlr_xcursor_manager,
			"left_ptr", cursor->wlr_cursor);

	struct wlr_box *layout_box = wlr_output_layout_get_box(output_layout, NULL);
	wlr_cursor_warp(cursor->wlr_cursor, NULL, layout_box->width / 2 ,
			layout_box->height / 2);

	cursor->cursor_mode = CURSOR_MODE_PASSTHROUGH;

	// Setup callbacks
	wl_signal_add(&cursor->wlr_cursor->events.motion, &cursor->motion);
	cursor->motion.notify = handle_cursor_motion;

	wl_signal_add(&cursor->wlr_cursor->events.motion_absolute,
			&cursor->motion_absolute);
	cursor->motion_absolute.notify = handle_cursor_motion_absolute;

	wl_signal_add(&cursor->wlr_cursor->events.button, &cursor->button);
	cursor->button.notify = handle_cursor_button;

	wl_signal_add(&cursor->wlr_cursor->events.axis, &cursor->axis);
	cursor->axis.notify = handle_cursor_axis;

	// Let wlr_seat know we have a pointer
	uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
	wlr_seat_set_capabilities(input->seat, caps);

	wlr_log(WLR_DEBUG, "Cursor %p created", (void*)cursor);
	return cursor;
}

void destroy_cursor(struct cursor *cursor)
{
	wlr_log(WLR_DEBUG, "Destroying cursor %p", (void*)cursor);

	wlr_xcursor_manager_destroy(cursor->wlr_xcursor_manager);
	wlr_cursor_destroy(cursor->wlr_cursor);

	free(cursor);
	wlr_log(WLR_DEBUG, "Cursor destroyed");
}
