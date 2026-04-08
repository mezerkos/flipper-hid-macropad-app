#include "hid_macropad.h"
#include <gui/elements.h>
#include "../hid.h"

#include "hid_icons.h"

#define TAG "HidMacropad"

struct HidMacropad {
    View* view;
    Hid* hid;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool back_pressed;
    bool connected;
} HidMacropadModel;

static void hid_macropad_draw_arrow(Canvas* canvas, uint8_t x, uint8_t y, CanvasDirection dir) {
    canvas_draw_triangle(canvas, x, y, 5, 3, dir);
    if(dir == CanvasDirectionBottomToTop) {
        canvas_draw_line(canvas, x, y + 6, x, y - 1);
    } else if(dir == CanvasDirectionTopToBottom) {
        canvas_draw_line(canvas, x, y - 6, x, y + 1);
    } else if(dir == CanvasDirectionRightToLeft) {
        canvas_draw_line(canvas, x + 6, y, x - 1, y);
    } else if(dir == CanvasDirectionLeftToRight) {
        canvas_draw_line(canvas, x - 6, y, x + 1, y);
    }
}

static void hid_macropad_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidMacropadModel* model = context;

    // Header
#ifdef HID_TRANSPORT_BLE
    if(model->connected) {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
    } else {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
    }
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 20, 3, AlignLeft, AlignTop, "mPad");
#else
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 12, 3, AlignLeft, AlignTop, "mPad");
#endif

    canvas_draw_icon(canvas, 2, 18, &I_Pin_back_arrow_10x8);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 15, 19, AlignLeft, AlignTop, "Hold to exit");

    const uint8_t x_2 = 23;
    const uint8_t x_1 = 2;
    const uint8_t x_3 = 44;

    const uint8_t y_1 = 44;
    const uint8_t y_2 = 65;

    // Up
    canvas_draw_icon(canvas, x_2, y_1, &I_Button_18x18);
    if(model->up_pressed) {
        elements_slightly_rounded_box(canvas, x_2 + 3, y_1 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_macropad_draw_arrow(canvas, x_2 + 9, y_1 + 6, CanvasDirectionBottomToTop);
    canvas_set_color(canvas, ColorBlack);

    // Down
    canvas_draw_icon(canvas, x_2, y_2, &I_Button_18x18);
    if(model->down_pressed) {
        elements_slightly_rounded_box(canvas, x_2 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_macropad_draw_arrow(canvas, x_2 + 9, y_2 + 10, CanvasDirectionTopToBottom);
    canvas_set_color(canvas, ColorBlack);

    // Left
    canvas_draw_icon(canvas, x_1, y_2, &I_Button_18x18);
    if(model->left_pressed) {
        elements_slightly_rounded_box(canvas, x_1 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_macropad_draw_arrow(canvas, x_1 + 7, y_2 + 8, CanvasDirectionRightToLeft);
    canvas_set_color(canvas, ColorBlack);

    // Right
    canvas_draw_icon(canvas, x_3, y_2, &I_Button_18x18);
    if(model->right_pressed) {
        elements_slightly_rounded_box(canvas, x_3 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_macropad_draw_arrow(canvas, x_3 + 11, y_2 + 8, CanvasDirectionLeftToRight);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    canvas_draw_icon(canvas, 2, 86, &I_Space_60x18);
    if(model->ok_pressed) {
        elements_slightly_rounded_box(canvas, 5, 88, 55, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 11, 90, &I_Ok_btn_9x9);
    elements_multiline_text_aligned(canvas, 26, 98, AlignLeft, AlignBottom, "F16");
    canvas_set_color(canvas, ColorBlack);

    // Back
    canvas_draw_icon(canvas, 2, 107, &I_Space_60x18);
    if(model->back_pressed) {
        elements_slightly_rounded_box(canvas, 5, 109, 55, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 11, 111, &I_Pin_back_arrow_10x8);
    elements_multiline_text_aligned(canvas, 26, 119, AlignLeft, AlignBottom, "Back");
}

static void hid_macropad_process(HidMacropad* hid_macropad, InputEvent* event) {
    with_view_model(
        hid_macropad->view,
        HidMacropadModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = true;
                    hid_hal_keyboard_press(hid_macropad->hid, KEY_MOD_LEFT_SHIFT);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    hid_hal_keyboard_press(hid_macropad->hid, HID_KEYBOARD_F18);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    hid_hal_keyboard_press(hid_macropad->hid, HID_KEYBOARD_F16);
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    hid_hal_keyboard_press(hid_macropad->hid, HID_KEYBOARD_F17);
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                    hid_hal_keyboard_press(hid_macropad->hid, HID_KEYBOARD_RETURN);
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = true;
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = false;
                    hid_hal_keyboard_release(hid_macropad->hid, KEY_MOD_LEFT_SHIFT);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = false;
                    hid_hal_keyboard_release(hid_macropad->hid, HID_KEYBOARD_F18);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                    hid_hal_keyboard_release(hid_macropad->hid, HID_KEYBOARD_F16);
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;
                    hid_hal_keyboard_release(hid_macropad->hid, HID_KEYBOARD_F17);
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = false;
                    hid_hal_keyboard_release(hid_macropad->hid, HID_KEYBOARD_RETURN);
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = false;
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack) {
                    hid_hal_keyboard_press(hid_macropad->hid, HID_KEYBOARD_ESCAPE);
                    hid_hal_keyboard_release(hid_macropad->hid, HID_KEYBOARD_ESCAPE);
                }
            }
        },
        true);
}

static bool hid_macropad_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidMacropad* hid_macropad = context;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        hid_hal_keyboard_release_all(hid_macropad->hid);
    } else {
        hid_macropad_process(hid_macropad, event);
        consumed = true;
    }

    return consumed;
}

HidMacropad* hid_macropad_alloc(Hid* hid) {
    HidMacropad* hid_macropad = malloc(sizeof(HidMacropad));
    hid_macropad->view = view_alloc();
    hid_macropad->hid = hid;
    view_set_context(hid_macropad->view, hid_macropad);
    view_allocate_model(hid_macropad->view, ViewModelTypeLocking, sizeof(HidMacropadModel));
    view_set_draw_callback(hid_macropad->view, hid_macropad_draw_callback);
    view_set_input_callback(hid_macropad->view, hid_macropad_input_callback);
    view_set_orientation(hid_macropad->view, ViewOrientationVerticalFlip);
    return hid_macropad;
}

void hid_macropad_free(HidMacropad* hid_macropad) {
    furi_assert(hid_macropad);
    view_free(hid_macropad->view);
    free(hid_macropad);
}

View* hid_macropad_get_view(HidMacropad* hid_macropad) {
    furi_assert(hid_macropad);
    return hid_macropad->view;
}

void hid_macropad_set_connected_status(HidMacropad* hid_macropad, bool connected) {
    furi_assert(hid_macropad);
    with_view_model(
        hid_macropad->view, HidMacropadModel * model, { model->connected = connected; }, true);
}
