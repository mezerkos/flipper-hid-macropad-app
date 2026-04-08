#include "macropad.h"
#include <extra_profiles/hid_profile.h>
#include <notification/notification_messages.h>
#include <dolphin/dolphin.h>

#define TAG "MacropadApp"

#define MacropadViewMain 0

static bool macropad_back_event_callback(void* context) {
    UNUSED(context);
    return false;
}

static void bt_hid_connection_status_changed_callback(BtStatus status, void* context) {
    furi_assert(context);
    Macropad* app = context;
    const bool connected = (status == BtStatusConnected);
    notification_internal_message(
        app->notifications, connected ? &sequence_set_blue_255 : &sequence_reset_blue);
    hid_macropad_set_connected_status(app->hid_macropad, connected);
}

void bt_hid_remove_pairing(Macropad* app) {
    Bt* bt = app->bt;
    bt_disconnect(bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    furi_hal_bt_stop_advertising();

    bt_forget_bonded_devices(bt);

    furi_hal_bt_start_advertising();
}

static Macropad* macropad_alloc() {
    Macropad* app = malloc(sizeof(Macropad));

    // Gui
    app->gui = furi_record_open(RECORD_GUI);

    // Bt
    app->bt = furi_record_open(RECORD_BT);

    // Notifications
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, macropad_back_event_callback);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Macropad view
    app->hid_macropad = hid_macropad_alloc(app);
    view_dispatcher_add_view(
        app->view_dispatcher, MacropadViewMain, hid_macropad_get_view(app->hid_macropad));

    return app;
}

static void macropad_free(Macropad* app) {
    furi_assert(app);

    // Reset notification
#ifdef HID_TRANSPORT_BLE
    notification_internal_message(app->notifications, &sequence_reset_blue);
#endif

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, MacropadViewMain);
    hid_macropad_free(app->hid_macropad);
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;
    furi_record_close(RECORD_BT);
    app->bt = NULL;

    // Free rest
    free(app);
}

int32_t macropad_usb_app(void* p) {
    UNUSED(p);
    Macropad* app = macropad_alloc();

    FURI_LOG_D(TAG, "Starting as USB app");

    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);

    dolphin_deed(DolphinDeedPluginStart);

    view_dispatcher_switch_to_view(app->view_dispatcher, MacropadViewMain);
    view_dispatcher_run(app->view_dispatcher);

    furi_hal_usb_set_config(usb_mode_prev, NULL);

    macropad_free(app);

    return 0;
}

int32_t macropad_ble_app(void* p) {
    UNUSED(p);
    Macropad* app = macropad_alloc();

    FURI_LOG_D(TAG, "Starting as BLE app");

    bt_disconnect(app->bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    // Migrate data from old sd-card folder
    Storage* storage = furi_record_open(RECORD_STORAGE);

    storage_common_migrate(
        storage,
        EXT_PATH("apps/Tools/" HID_BT_KEYS_STORAGE_NAME),
        APP_DATA_PATH(HID_BT_KEYS_STORAGE_NAME));

    bt_keys_storage_set_storage_path(app->bt, APP_DATA_PATH(HID_BT_KEYS_STORAGE_NAME));

    furi_record_close(RECORD_STORAGE);

    app->ble_hid_profile = bt_profile_start(app->bt, ble_profile_hid, NULL);

    furi_check(app->ble_hid_profile);

    furi_hal_bt_start_advertising();
    bt_set_status_changed_callback(app->bt, bt_hid_connection_status_changed_callback, app);

    dolphin_deed(DolphinDeedPluginStart);

    view_dispatcher_switch_to_view(app->view_dispatcher, MacropadViewMain);
    view_dispatcher_run(app->view_dispatcher);

    bt_set_status_changed_callback(app->bt, NULL, NULL);

    bt_disconnect(app->bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_default_path(app->bt);

    furi_check(bt_profile_restore_default(app->bt));

    macropad_free(app);

    return 0;
}
