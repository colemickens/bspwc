#include <wayland-server.h>
#include <wlr/types/wlr_input_device.h>
#include <xkbcommon/xkbcommon.h>

#include "bspwc/keyboard.h"
#include "bspwc/input.h"

bool handle_keybinding(uint32_t modifiers, const xkb_keysym_t *syms, int nsyms)
{
	// TODO: Make this a real thing by copying sxhkd's functionality
	char keysym_name[64];
	for (int i=0; i < nsyms; i++)
	{
		xkb_keysym_get_name(syms[i], keysym_name, sizeof(keysym_name));
		wlr_log(WLR_DEBUG, "%s", keysym_name);
	}
	return true;
}

void handle_keyboard_key(struct wl_listener *listener, void *data)
{
	struct keyboard *keyboard = wl_container_of(listener, keyboard, key);
	struct wlr_event_keyboard_key *event = data;

	struct xkb_state *state =
		xkb_state_new(keyboard->device->keyboard->keymap);
	if (state == NULL)
	{
		wlr_log(WLR_ERROR, "Failed to create xkb state");
	}

	// Have to add 8 to convert libinput keycode to libxkbcommon
	xkb_keycode_t keycode = event->keycode + 8;
	const xkb_keysym_t *syms;
	int nsyms = xkb_state_key_get_syms(
			keyboard->device->keyboard->xkb_state, keycode, &syms);

	uint32_t modifiers =
		wlr_keyboard_get_modifiers(keyboard->device->keyboard);
	bool handled = handle_keybinding(modifiers, syms, nsyms);

	if (!handled)
	{
		wlr_seat_set_keyboard(keyboard->input->seat, keyboard->device);
		wlr_seat_keyboard_notify_key(keyboard->input->seat,
				event->time_msec, event->keycode, event->state);
	}
}

void handle_keyboard_modifiers(struct wl_listener *listener, void *data)
{
	struct keyboard *keyboard = wl_container_of(listener, keyboard, modifiers);

	struct wlr_seat *seat = keyboard->input->seat;
	wlr_seat_set_keyboard(seat, keyboard->device);

	wlr_seat_keyboard_notify_modifiers(seat,
			&keyboard->device->keyboard->modifiers);
}

struct keyboard *create_keyboard(struct input *input,
		struct wlr_input_device *device)
{
	struct keyboard *keyboard = calloc(1, sizeof(struct keyboard));
	if (keyboard == NULL)
	{
		wlr_log(WLR_ERROR, "Failed to create keyboard");
		return NULL;
	}
	device->data = keyboard;
	keyboard->device = device;
	keyboard->input = input;

	struct xkb_rule_names rules = {
		.rules = getenv("XKB_DEFAULT_RULES"),
		.model = getenv("XKB_DEFAULT_MODEL"),
		.layout = getenv("XKB_DEFAULT_LAYOUT"),
		.variant = getenv("XKB_DEFAULT_VARIANT"),
		.options = getenv("XKB_DEFAULT_OPTIONS"),
	};

	struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	if (context == NULL)
	{
		wlr_log(WLR_ERROR, "Cannot create XKB context");
		destroy_keyboard(keyboard);
		return NULL;
	}

	struct xkb_keymap *keymap = xkb_map_new_from_names(context, &rules,
			XKB_KEYMAP_COMPILE_NO_FLAGS);
	if (keymap == NULL) {
		xkb_context_unref(context);
		wlr_log(WLR_ERROR, "Cannot create XKB keymap");
		return NULL;
	}

	wlr_keyboard_set_keymap(device->keyboard, keymap);
	
	// TODO: Make these configurable
	int repeat_rate = 25;
	int repeat_delay = 600;
	wlr_keyboard_set_repeat_info(device->keyboard, repeat_rate,
			repeat_delay);

	xkb_keymap_unref(keymap);
	xkb_context_unref(context);

	keyboard->key.notify = handle_keyboard_key;
	wl_signal_add(&keyboard->device->keyboard->events.key, &keyboard->key);
	keyboard->modifiers.notify = handle_keyboard_modifiers;
	wl_signal_add(&keyboard->device->keyboard->events.modifiers,
			&keyboard->modifiers);

	return keyboard;
}

void destroy_keyboard(struct keyboard *keyboard)
{
	free(keyboard);
}
