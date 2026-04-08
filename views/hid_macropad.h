#pragma once

#include <gui/view.h>

typedef struct Hid Hid;
typedef struct HidMacropad HidMacropad;

HidMacropad* hid_macropad_alloc(Hid* hid);

void hid_macropad_free(HidMacropad* hid_macropad);

View* hid_macropad_get_view(HidMacropad* hid_macropad);

void hid_macropad_set_connected_status(HidMacropad* hid_macropad, bool connected);
