#pragma once

#include <furi.h>
#include <furi_hal_bt.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>

#include <extra_profiles/hid_profile.h>

#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>
#include <storage/storage.h>

#include "views/hid_macropad.h"

#define HID_BT_KEYS_STORAGE_NAME ".bt_hid.keys"

typedef struct Macropad Macropad;

struct Macropad {
    FuriHalBleProfileBase* ble_hid_profile;
    Bt* bt;
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    HidMacropad* hid_macropad;
};

void bt_hid_remove_pairing(Macropad* app);

void hid_hal_keyboard_press(Macropad* instance, uint16_t event);
void hid_hal_keyboard_release(Macropad* instance, uint16_t event);
void hid_hal_keyboard_release_all(Macropad* instance);

void hid_hal_consumer_key_press(Macropad* instance, uint16_t event);
void hid_hal_consumer_key_release(Macropad* instance, uint16_t event);
void hid_hal_consumer_key_release_all(Macropad* instance);

void hid_hal_mouse_move(Macropad* instance, int8_t dx, int8_t dy);
void hid_hal_mouse_scroll(Macropad* instance, int8_t delta);
void hid_hal_mouse_press(Macropad* instance, uint16_t event);
void hid_hal_mouse_release(Macropad* instance, uint16_t event);
void hid_hal_mouse_release_all(Macropad* instance);
