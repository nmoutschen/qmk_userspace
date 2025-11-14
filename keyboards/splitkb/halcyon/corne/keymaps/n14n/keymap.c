#include QMK_KEYBOARD_H
#if __has_include("keymap.h")
#    include "keymap.h"
#endif
#include "hlc_tft_display/hlc_tft_display.h"
#include "logo_lg.qgf.h"
#include "atkinson.qff.h"
#include "atkinson_bold.qff.h"

#define LAYER_WIN 1
#define LAYER_LO 2
#define LAYER_HI 3
#define LAYER_FN 4
#define HSV_N14N 0xF1, 0xDC, 0xD9

static bool scrolling_mode = false;

// Layers
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case LAYER_FN:
        case LAYER_HI:
        case LAYER_LO:
            scrolling_mode = true;
            break;
        default:
            if (scrolling_mode) {
                scrolling_mode = false;
            }
            break;
    }
    return state;
}

hsv_t get_layer_color(void) {
    switch(get_highest_layer(layer_state|default_layer_state)) {
        case LAYER_FN:
            return (hsv_t){HSV_RED};
        case LAYER_HI:
            return (hsv_t){HSV_AZURE};
        case LAYER_LO:
            return (hsv_t){HSV_YELLOW};
        default:
            return (hsv_t){HSV_OFF};
    }
}

// Keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_corne_hlc(
        KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_BSPC,
        KC_LSFT, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT,
        KC_LGUI, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_ESC ,
                                   KC_LCTL, MO(2)  , KC_SPC , KC_ENT , MO(3)  , KC_RALT,
                 KC_MUTE, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_MUTE, KC_NO  , KC_NO  , KC_NO  , KC_NO
    ),
    [LAYER_WIN] = LAYOUT_corne_hlc(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_LCTL, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                   KC_LGUI, _______, _______, _______, _______, _______,
                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
    [LAYER_LO] = LAYOUT_corne_hlc(
        _______, KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_DEL ,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_UP  , KC_RIGHT,XXXXXXX, XXXXXXX,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                   _______, _______, _______, _______, MO(4)  , _______,
                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
    [LAYER_HI] = LAYOUT_corne_hlc(
        _______, KC_EXLM, KC_AT  , KC_HASH, KC_DLR , KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_MINS, KC_EQL , KC_LBRC, KC_RBRC, KC_BSLS, KC_GRV ,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
                                   _______, MO(4)  , _______, _______, _______, _______,
                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
    [LAYER_FN] = LAYOUT_corne_hlc(
        QK_BOOT, KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , XXXXXXX,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_F11 , KC_F12 , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        TG(1)  , XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                   _______, _______, _______, _______, _______, _______,
                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    )
};

// RGB Matrix

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(HSV_OFF);
}

bool rgb_matrix_indicators_user(void) {
    hsv_t color = get_layer_color();

    if (color.v > 64) {
        color.v = 64;
    }
    rgb_matrix_sethsv_noeeprom(color.h, color.s, color.v);
    return false;
}

// Display
#ifdef HLC_TFT_DISPLAY
painter_device_t lcd;
painter_device_t lcd_surface;
static painter_image_handle_t logo;
static painter_font_handle_t font;
static painter_font_handle_t font_bold;

bool module_post_init_user(void) {
    font = qp_load_font_mem(font_atkinson);
    font_bold = qp_load_font_mem(font_atkinson_bold);
    logo = qp_load_image_mem(gfx_logo_lg);
    qp_drawimage(lcd_surface, 9, 9, logo);
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    return false;
}

void display_layer(int16_t pos) {
hsv_t color = get_layer_color();
    int16_t height = 240-9-pos*font->line_height;
    int16_t left = 135-9-qp_textwidth(font, "   ");
    qp_drawtext_recolor(lcd_surface, 9, height, font_bold, "Layer", HSV_N14N, HSV_BLACK);
    switch(get_highest_layer(layer_state|default_layer_state)) {
        case LAYER_FN:
            qp_drawtext_recolor(lcd_surface, left, height, font, " Fn", color.h, color.s, color.v, HSV_BLACK);
            break;
        case LAYER_HI:
            qp_drawtext_recolor(lcd_surface, left, height, font, " Hi", color.h, color.s, color.v, HSV_BLACK);
            break;
        case LAYER_LO:
            qp_drawtext_recolor(lcd_surface, left, height, font, " Lo", color.h, color.s, color.v, HSV_BLACK);
            break;
        default:
            qp_drawtext_recolor(lcd_surface, left, height, font, "Std", HSV_WHITE, HSV_BLACK);
            break;
    }
}

void display_mode(int16_t pos) {
    int16_t height = 240-9-pos*font->line_height;
    int16_t left = 135-9-qp_textwidth(font, "   ");
    qp_drawtext_recolor(lcd_surface, 9, height, font_bold, "Mode", HSV_N14N, HSV_BLACK);
    if (layer_state_is(LAYER_WIN)) {
        qp_drawtext_recolor(lcd_surface, left, height, font, "Win", HSV_WHITE, HSV_BLACK);
    } else {
        qp_drawtext_recolor(lcd_surface, left, height, font, "Mac", HSV_WHITE, HSV_BLACK);
    }
}

bool display_module_housekeeping_task_user(bool second_display) {
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    display_layer(2);
    display_mode(1);
    return false;
}
#endif

// Pointing device
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (scrolling_mode) {
        mouse_report.h = mouse_report.x;
        mouse_report.v = mouse_report.y;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

#ifdef OTHER_KEYMAP_C
#    include OTHER_KEYMAP_C
#endif // OTHER_KEYMAP_C

