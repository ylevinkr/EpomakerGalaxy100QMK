/* Copyright 2025 Carlos Eduardo de Paula <carlosedp@gmail.com>
 * Copyright 2025 EPOMAKER <https://github.com/Epomaker>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "rdmctmzt_common.h"
#include "via.h"

// Layer 0: Windows base      Layer 2: Windows Fn (hold the knob: LT(2, KC_MUTE))
// Layer 1: macOS base        Layer 3: macOS Fn   (held with MO(3))

_Static_assert(MATRIX_COLS <= 16, "highlight bitmap uses one uint16_t per row");

enum keymap_keycodes {
    FN_HL = QMK_KB_SAFE_RANGE, // VIA customKeycodes: entry after SLP_TM
};
_Static_assert((uint16_t)FN_HL == (uint16_t)QK_KB_17, "FN_HL must match its customKeycodes position in VIA_Mapping_Galaxy100_R.json");

// Layers that never trigger the overlay (Windows base, macOS base)
#ifndef FN_HL_BASE_LAYERS
#    define FN_HL_BASE_LAYERS ((layer_state_t)0x03)
#endif
#ifndef FN_HL_COLOR
#    define FN_HL_COLOR HSV_ORANGE
#endif

#define FN_HL_EECONFIG_OFF_BIT 0x01 // stored inverted so a cleared EEPROM means "on"

static bool          fn_hl_enabled = true;
static bool          fn_hl_dirty   = true;
static layer_state_t fn_hl_state;
static uint16_t      fn_hl_rows[MATRIX_ROWS];

static uint16_t fn_hl_keycode_below(uint8_t top, layer_state_t state, keypos_t pos) {
    for (int8_t layer = top - 1; layer >= 0; layer--) {
        if (state & ((layer_state_t)1 << layer)) {
            uint16_t keycode = keymap_key_to_keycode(layer, pos);
            if (keycode != KC_TRNS) {
                return keycode;
            }
        }
    }
    return KC_NO;
}

static void fn_hl_rebuild(uint8_t top, layer_state_t state) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        uint16_t bits = 0;
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            keypos_t pos     = {.col = col, .row = row};
            uint16_t keycode = keymap_key_to_keycode(top, pos);
            if (keycode != KC_TRNS && keycode != KC_NO && keycode != fn_hl_keycode_below(top, state, pos)) {
                bits |= (uint16_t)1 << col;
            }
        }
        fn_hl_rows[row] = bits;
    }
    fn_hl_state = state;
    fn_hl_dirty = false;
}

void keyboard_post_init_user(void) {
    uint8_t flags;
    eeconfig_read_user_datablock(&flags, 0, sizeof(flags));
    fn_hl_enabled = !(flags & FN_HL_EECONFIG_OFF_BIT);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == FN_HL) {
        if (record->event.pressed) {
            fn_hl_enabled = !fn_hl_enabled;
            uint8_t flags = fn_hl_enabled ? 0 : FN_HL_EECONFIG_OFF_BIT;
            eeconfig_update_user_datablock(&flags, 0, sizeof(flags));
        }
        return false;
    }
    return true;
}

bool via_command_kb(uint8_t *data, uint8_t length) {
    switch (data[0]) {
        case id_dynamic_keymap_set_keycode:
        case id_dynamic_keymap_set_buffer:
        case id_dynamic_keymap_reset:
            fn_hl_dirty = true;
            break;
        default:
            break;
    }
    return false;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (!fn_hl_enabled) {
        return true;
    }

    layer_state_t state = layer_state | default_layer_state;
    uint8_t       top   = get_highest_layer(state);
    if (FN_HL_BASE_LAYERS & ((layer_state_t)1 << top)) {
        return true;
    }
    if (fn_hl_dirty || state != fn_hl_state) {
        fn_hl_rebuild(top, state);
    }

    hsv_t hsv = {FN_HL_COLOR};
    hsv.v     = rgb_matrix_get_val();
    rgb_t rgb = hsv_to_rgb(hsv);

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t index = g_led_config.matrix_co[row][col];
            if (index == NO_LED || index < led_min || index >= led_max) {
                continue;
            }
            if (fn_hl_rows[row] & ((uint16_t)1 << col)) {
                rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
            }
        }
    }
    // true so the library's indicators (caps, connection mode, battery) still draw on top
    return true;
}

// clang-format off
// Converted from E:\QMK\epomaker_galaxy100.layout.json (stock-firmware VIA export)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_1800_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_INS,  KC_PGUP, KC_PGDN, KC_PSCR, LT(2, KC_MUTE),
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,  KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_HOME, KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_END,  KC_P4,   KC_P5,   KC_P6,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                    KC_RALT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT, KC_P0,   KC_PDOT
    ),
    [1] = LAYOUT_1800_ansi(
        QK_GESC, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_F5,   KC_F6,   KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_INS,  KC_PGUP, KC_PGDN, KC_PSCR, KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_HOME, KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_END,  KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_DEL,  KC_P4,   KC_P5,   KC_P6,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_LCTL, KC_LALT, KC_LGUI,                   KC_SPC,                    KC_RGUI, MO(3),   KC_LEFT, KC_DOWN, KC_RGHT, KC_P0,   KC_PDOT
    ),
    [2] = LAYOUT_1800_ansi(
        KC_ESC,  KC_MYCM, KC_WHOM, KC_CALC, KC_MAIL, KC_MSEL, KC_MSTP, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_DEL,  KC_HOME, KC_END,  KC_PSCR, KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  EE_CLR,  KC_INS,  KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,  MD_BLE1, MD_BLE2, MD_BLE3, MD_24G,  RGB_RTOG, SLP_TM, KC_U,    KC_I,    KC_O,    KC_P,    RM_SATD, RM_SATU, RM_HUED, KC_PGUP, KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_CAPS, TO(0),   TO(1),   KC_D,    KC_F,    KC_G,    FN_HL,   KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, RM_NEXT, KC_PGDN, KC_P4,   KC_P5,   KC_P6,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, RM_VALU, KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_LCTL, QK_WLO,  KC_LALT,                   QK_BAT,                    KC_RALT, KC_NO,   RM_SPDD, RM_VALD, RM_SPDU, KC_P0,   KC_PDOT
    ),
    [3] = LAYOUT_1800_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_HOME, KC_END,  KC_PSCR, KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  EE_CLR,  KC_INS,  KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,  MD_BLE1, MD_BLE2, MD_BLE3, MD_24G,  RGB_RTOG, SLP_TM, KC_U,    KC_I,    KC_O,    KC_P,    RM_SATD, RM_SATU, RM_HUED, KC_PGUP, KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_CAPS, TO(0),   TO(1),   KC_D,    KC_F,    KC_G,    FN_HL,   KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, RM_NEXT, KC_PGDN, KC_P4,   KC_P5,   KC_P6,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, RM_VALU, KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_LCTL, KC_LALT, KC_LGUI,                   QK_BAT,                    KC_RGUI, KC_NO,   RM_SPDD, RM_VALD, RM_SPDU, KC_P0,   KC_PDOT
    )
};
// clang-format on

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [1] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [2] = {ENCODER_CCW_CW(RM_SATD, RM_SATU)},
    [3] = {ENCODER_CCW_CW(RM_SPDD, RM_SPDU)},
};
#endif
