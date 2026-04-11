# Macropad

A standalone Flipper Zero HID macropad app. Boots directly into a single vertical-layout view with 6 programmable buttons.

Extracted from [fidian/flipper-hid-app](https://github.com/fidian/flipper-hid-app) (Momentum firmware, GPL).

## Button Mappings

| Button | Key | Icon |
|--------|-----|------|
| Up | F18 | Hamburger menu |
| Down | Left Shift | Shift |
| Left | Backspace | Backspace |
| Right | Enter | Return |
| Center | F17 | Mic |
| Back (short) | ESC | Back arrow + "ESC" |
| Back (long) | Exit app | — |

## Build & Deploy

Requires [ufbt](https://pypi.org/project/ufbt/).

```bash
# Build
ufbt fap_macropad_ble    # Bluetooth variant
ufbt fap_macropad_usb    # USB variant

# Build + deploy + run on connected Flipper
ufbt launch APPID=macropad_ble
ufbt launch APPID=macropad_usb
```

## License

GPL — see [LICENSE](LICENSE).
