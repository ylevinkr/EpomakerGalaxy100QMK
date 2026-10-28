# EPOMAKER Galaxy100 (new revision)

Port of `epomaker/galaxy100_lite` for the Galaxy100 sold from Oct 2025 on
(USB `36B0:30D4`, FS026). Matrix, pins, layout and LED map are identical to the
Lite; only the name, PID, BLE names and BLE ID differ. **Not** for the older
WB32 Galaxy100 (`342D:E489`).

The wireless sleep time (1, 3, 10 or 30 minutes, default 3) is set with the
`SLP_TM` keycode or in VIA under Wireless > Power > Sleep after.

The original Lite notes follow.

A customizable 96% (1800) keyboard with 100 keys and a rotary knob, built around
the same ES32 FS026 microcontroller as the other Epomaker wireless boards in this
tree (TH40, Luma40, Galaxy68, TH85). It uses the open-source
[`lib/rdmctmzt_common`](../../../lib/rdmctmzt_common/README.md) library instead of
the closed-source `rdr_lib` shipped by the vendor.

* Keyboard Maintainer: [CarlosEDP](https://github.com/carlosedp)
* Hardware Supported: EPOMAKER Galaxy100 Lite PCB with es32fs026 microcontroller

Make example for this keyboard (after setting up your build environment):

    make epomaker/galaxy100_r:default

Flashing example for this keyboard:

1. Hold ESC while plugging it in wired mode to enter flash mode
2. Create the binary with make
3. Copy the binary on the mounted flash drive
4. The flash drive should disappear and the keyboard restarts with the new firmware

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## VIA

VIA is enabled in the default keymap. Load
[`VIA_Mapping_Galaxy100_R.json`](VIA_Mapping_Galaxy100_R.json) as a design
file in VIA (Settings -> Show Design tab -> Design -> Load). It carries the
custom keycode list and a brightness range that matches the cap built into the
firmware. Epomaker's stock VIA file for this board only matches the stock
firmware (different effect IDs and custom keycodes).

The effect dropdown is generated from the effect order the firmware actually
compiles (the enabled entries of `rgb_matrix.animations` in `keyboard.json`, in
`quantum/rgb_matrix/animations/rgb_matrix_effects.inc` order). If you change that
animation list, the dropdown has to be regenerated or VIA will select the wrong
effect.

## Layout notes

`LAYOUT_1800_ansi` has 101 positions: 100 keys plus the knob press on `(5, 3)`.
To the right of the F-row there are only three keys — Home, End and PrtSc — and
then the knob.

The vendor's `info.json` also lists `(5, 8)`, stacked at the same coordinate as
the knob. There is no switch there; the slot only exists because LED 17 is wired
to it. It is left out of the layout, as it is in the vendor's own VIA file.

## Macro storage

The stock firmware left 194 bytes for VIA macros. This build gets that to **570
bytes** by keeping the shared 2048-byte EEPROM budget and spending the extra room
on macros rather than layers: the 7x16 matrix costs 224 bytes per VIA layer, so
this board is configured for 6 dynamic keymap layers instead of the 8 the other
FS026 boards use.

Enlarging `EEPROM_SIZE` instead would buy more macro space, but it lengthens the
wear-levelling page transfer in `user_eeprom.c`, which runs with the CPU tied up
rewriting flash — that stalls USB reports and matrix scanning, so it is not a
free trade.

## Bootloader

Enter the bootloader in 2 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (Esc key) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB
