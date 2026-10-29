# EpomakerQMK – Galaxy100 (new revision) fork

Fork of [SRGBmods/EpomakerQMK](https://github.com/SRGBmods/EpomakerQMK) that adds
QMK + VIA support for the **Epomaker Galaxy100 new revision** (sold from October
2025, USB `36B0:30D4`, ES32 FS026) and fixes bugs in the shared wireless library
[`lib/rdmctmzt_common`](lib/rdmctmzt_common/README.md).

> **Tested on a single keyboard only:** one Galaxy100 new revision (bought after
> October 2025) running this QMK build with VIA, in wired USB and 2.4 GHz modes.
> Bluetooth has not been verified. **Not** for the older WB32 Galaxy100
> (`342D:E489`). Keep a way back to the stock firmware before flashing.

## Changes compared to upstream

### New keyboard: [`epomaker/galaxy100_r`](keyboards/epomaker/galaxy100_r/readme.md)

- Port of `epomaker/galaxy100_lite`: same matrix, pins, layout and LED map; own
  name, PID `0x30D4`, BLE names `Galaxy100-1..3` and the stock BLE ID `0x10B4`
  (read from the stock firmware, so existing pairings are kept).
- VIA design file: [`VIA_Mapping_Galaxy100_R.json`](keyboards/epomaker/galaxy100_r/VIA_Mapping_Galaxy100_R.json).

### Bug fixes in `lib/rdmctmzt_common` (all boards using the library)

- **Stuck / repeating keys on wireless:** a full RF queue dropped the newest HID
  report, so a lost key release left the key held on the host. The newest pending
  frame is now overwritten instead, queue writes are interrupt-safe, and reports
  with an unknown ID are no longer sent (their command byte meant "switch to 2.4G").
- **Wrong or duplicate key after USB wake:** the wake path tapped a truncated
  keycode (e.g. `QK_GESC` became `S`), or Esc when no key woke the board.
- **Wireless input dead until power cycle:** if the RF module missed an SPI
  handshake the bus stayed locked; it is now released after 500 ms.
- **Connection-mode indicator:** only shown for `MO(2)`/`MO(3)`; now shown while
  any Fn layer is active (`LT`, `TT`, `OSL`, Fn remapped in VIA).

### Missing features implemented

- **RF sleep timer** (stock `TIME_ST`, protocol reverse-engineered from the stock
  firmware): opt-in `RF_SLEEP_TIMER_ENABLE`, `SLP_TM` keycode, VIA
  Wireless > Power > Sleep after (1/3/10/30 min). Enabled on `galaxy100_r`.
- **Num Lock indicator:** opt-in `LED_NUM_INDEX`; `galaxy100_r` lights the Num key.

The board port, fixes and features above (without the personal keymap below) are
on branch `pr/galaxy100-new-rev` for an upstream pull request.

### Personal additions (not proposed upstream)

- `galaxy100_r` keymap `fn_highlight` (my layout): while an Fn layer is held,
  keys that have their own assignment on it are recoloured on top of the running
  effect. `FN_HL` (Fn+H) toggles it, `FN_HLC` (Fn+J, Shift for backwards) cycles
  the colour; both also in VIA under Lighting > Fn highlight. Knob hold is Fn
  (`LT(2, KC_MUTE)`), `SLP_TM` is on Fn+Y.

### Build and flash

    make epomaker/galaxy100_r:default
    make epomaker/galaxy100_r:fn_highlight

Hold Esc while plugging the keyboard into a motherboard USB port (not a hub), then
copy the `.bin` onto the drive that appears. Use VIA over the USB cable; the
2.4 GHz dongle does not expose VIA.

---

# Quantum Mechanical Keyboard Firmware

[![Current Version](https://img.shields.io/github/tag/qmk/qmk_firmware.svg)](https://github.com/qmk/qmk_firmware/tags)
[![Discord](https://img.shields.io/discord/440868230475677696.svg)](https://discord.gg/qmk)
[![Docs Status](https://img.shields.io/badge/docs-ready-orange.svg)](https://docs.qmk.fm)
[![GitHub contributors](https://img.shields.io/github/contributors/qmk/qmk_firmware.svg)](https://github.com/qmk/qmk_firmware/pulse/monthly)
[![GitHub forks](https://img.shields.io/github/forks/qmk/qmk_firmware.svg?style=social&label=Fork)](https://github.com/qmk/qmk_firmware/)

This is a keyboard firmware based on the [tmk_keyboard firmware](https://github.com/tmk/tmk_keyboard) with some useful features for Atmel AVR and ARM controllers, and more specifically, the [OLKB product line](https://olkb.com), the [ErgoDox EZ](https://ergodox-ez.com) keyboard, and the Clueboard product line.

## 🚨 Forked firmware remarks

**This fork holds keyboards not officially supported by the official QMK tree and have been incorporated by myself from other sources and/or adjusted to compile. Not all keyboards here have been tested and work 100% as the original firmware. Always know what you are doing and have a backup plan in case it doesn't work (know how to but your keyboard in BOOTLOADER mode and have the official firmware to revert).**

Also there might be different revisions of the same keyboard so I can't assure the firmware works for all keyboards of a specific model.

They keyboards that have been incorporated are:

### Tested Keyboards

- Epomaker TH40
- Epomaker Luma40
- Akko Monsgeek M1 V5
- Evoworks EVO80
- Epomaker Galaxy68
- Chosfox Geonix R2
- Chosfox Geonix R2.5 40/41 keys
- Cidoo QK61 v2
- Chosfox Vero75
- Cerakey Zen75
- Epomaker TH85
- Epomaker Split 65
- Epomaker Split 70
- Epomaker Galaxy 100 Lite
- Epomaker Galaxy100 new revision (sold from Oct 2025, USB `36B0:30D4`, `epomaker/galaxy100_r`)

These are keyboards I own or have been tested and reported back to work. There might be differences to the official firmware in keymaps, sleep times, battery consumption and other factors but the main features should work as the original (wireless, LEDs, etc).

### Untested Keyboards

- Epomaker Tide Alice66
- Epomaker Tide65
- Epomaker Tide75
- ~Epomaker Galaxy100 (Removed on baf8d85bb9 due to issues)~
- Epomaker Galaxy100 Lite
- Epomaker Luma84
- Epomaker Tide49

These untested keyboards compile but haven't been tested for all it's functionality so wireless, backlights, RGB or other functions might not work as expected or as the official firmware.

There are factory firmware files for some of these keyboards at <https://github.com/carlosedp/qmk_firmware/releases/tag/0.31.2>. Use them in case you need to revert to the original.

I welcome testers and PRs to have a broad open-source support of them.

## Documentation

- [See the official documentation on docs.qmk.fm](https://docs.qmk.fm)

The docs are powered by [VitePress](https://vitepress.dev/). They are also viewable offline; see [Previewing the Documentation](https://docs.qmk.fm/#/contributing?id=previewing-the-documentation) for more details.

You can request changes by making a fork and opening a [pull request](https://github.com/qmk/qmk_firmware/pulls).

## Supported Keyboards

- [Planck](/keyboards/planck/)
- [Preonic](/keyboards/preonic/)
- [ErgoDox EZ](/keyboards/ergodox_ez/)
- [Clueboard](/keyboards/clueboard/)
- [Cluepad](/keyboards/clueboard/17/)
- [Atreus](/keyboards/atreus/)

The project also includes community support for [lots of other keyboards](/keyboards/).

## Maintainers

QMK is developed and maintained by Jack Humbert of OLKB with contributions from the community, and of course, [Hasu](https://github.com/tmk). The OLKB product firmwares are maintained by [Jack Humbert](https://github.com/jackhumbert), the Ergodox EZ by [ZSA Technology Labs](https://github.com/zsa), the Clueboard by [Zach White](https://github.com/skullydazed), and the Atreus by [Phil Hagelberg](https://github.com/technomancy).

## Official Website

[qmk.fm](https://qmk.fm) is the official website of QMK, where you can find links to this page, the documentation, and the keyboards supported by QMK.
