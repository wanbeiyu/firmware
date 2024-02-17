#include <Arduino.h>

#include <nthaka.h>
#include <nthaka/nxmc2.h>
#include <nthaka/pokecon.h>

#include <wanbeiyu.h>

#include <arduino_wby_gpio.h>

#include "config.h"

static nthaka_buffer_t buf;

static wby_button_t btn_y;
static wby_button_t btn_b;
static wby_button_t btn_a;
static wby_button_t btn_x;
static wby_button_t btn_l;
static wby_button_t btn_r;
static wby_button_t btn_select;
static wby_button_t btn_start;
static wby_button_t btn_home;
static wby_button_t btn_power;
static wby_button_t btn_wireless;
static wby_hat_t hat;
static wby_slidepad_t sp;
static wby_touchscreen_t ts;

#define update_button(btn, state) ((state) == NTHAKA_BUTTON_PRESSED ? wby_button_hold((btn)) \
                                                                    : wby_button_release((btn)))
#define update_hat(hat, state) (wby_hat_hold((hat), ((state) == NTHAKA_HAT_UP          ? WBY_HAT_UP        \
                                                     : (state) == NTHAKA_HAT_UPRIGHT   ? WBY_HAT_UPRIGHT   \
                                                     : (state) == NTHAKA_HAT_RIGHT     ? WBY_HAT_RIGHT     \
                                                     : (state) == NTHAKA_HAT_DOWNRIGHT ? WBY_HAT_DOWNRIGHT \
                                                     : (state) == NTHAKA_HAT_DOWN      ? WBY_HAT_DOWN      \
                                                     : (state) == NTHAKA_HAT_DOWNLEFT  ? WBY_HAT_DOWNLEFT  \
                                                     : (state) == NTHAKA_HAT_LEFT      ? WBY_HAT_LEFT      \
                                                     : (state) == NTHAKA_HAT_UPLEFT    ? WBY_HAT_UPLEFT    \
                                                                                       : WBY_HAT_NEUTRAL)))
#define update_slidepad(sp, state) (((state).x == NTHAKA_STICK_AXIS_NEUTRAL && \
                                     (state).y == NTHAKA_STICK_AXIS_NEUTRAL)   \
                                        ? wby_slidepad_release((sp))           \
                                        : wby_slidepad_hold((sp), (state).x, (state).y))
#define update_touchscreen(ts, state) (/* 0bhxxxxxxx'xxyyyyyy'yy000000 */                                                           \
                                       ((state)[0] >> 7) & 1 == 1                                                                   \
                                           ? wby_touchscreen_hold((ts),                                                             \
                                                                  (((state)[0] & 0x7F /* 0b0111'1111 */) << 2) | ((state)[1] >> 6), \
                                                                  (((state)[1] & 0x3F /* 0b0011'1111 */) << 2) | ((state)[2] >> 6)) \
                                           : wby_touchscreen_release((ts)))
#define update_3ds(state) (state != NULL                                          \
                               ? (update_button(&btn_y, (state)->y),              \
                                  update_button(&btn_b, (state)->b),              \
                                  update_button(&btn_a, (state)->a),              \
                                  update_button(&btn_x, (state)->x),              \
                                  update_button(&btn_l, (state)->l),              \
                                  update_button(&btn_r, (state)->r),              \
                                  update_button(&btn_select, (state)->minus),     \
                                  update_button(&btn_start, (state)->plus),       \
                                  update_button(&btn_home, (state)->home),        \
                                  update_button(&btn_power, (state)->l_click),    \
                                  update_button(&btn_wireless, (state)->r_click), \
                                  update_hat(&hat, (state)->hat),                 \
                                  update_slidepad(&sp, (state)->l_stick),         \
                                  update_touchscreen(&ts, (state)->ext))          \
                               : (void)0)

void setup()
{
    Serial.setTimeout(100);
    Serial.begin(9600);

    static nxmc2_format_handler_t nxmc2;
    static pokecon_format_handler_t pokecon;
    static nthaka_format_handler_t *fmts[] = {(nthaka_format_handler_t *)&nxmc2, //
                                              (nthaka_format_handler_t *)&pokecon};
    static nthaka_multi_format_handler_t fmt;
    nxmc2_format_handler_init(&nxmc2);
    pokecon_format_handler_init(&pokecon);
    nthaka_multi_format_handler_init(&fmt, fmts, 2);
    nthaka_buffer_init(&buf, (nthaka_format_handler_t *)&fmt);

    static arduino_wby_gpio_t gpio_y;
    static arduino_wby_gpio_t gpio_b;
    static arduino_wby_gpio_t gpio_a;
    static arduino_wby_gpio_t gpio_x;
    static arduino_wby_gpio_t gpio_l;
    static arduino_wby_gpio_t gpio_r;
    static arduino_wby_gpio_t gpio_select;
    static arduino_wby_gpio_t gpio_start;
    static arduino_wby_gpio_t gpio_home;
    static arduino_wby_gpio_t gpio_power;
    static arduino_wby_gpio_t gpio_wireless;
    arduino_wby_gpio_init(&gpio_y, PIN_Y);
    arduino_wby_gpio_init(&gpio_b, PIN_B);
    arduino_wby_gpio_init(&gpio_a, PIN_A);
    arduino_wby_gpio_init(&gpio_x, PIN_X);
    arduino_wby_gpio_init(&gpio_l, PIN_L);
    arduino_wby_gpio_init(&gpio_r, PIN_R);
    arduino_wby_gpio_init(&gpio_select, PIN_SELECT);
    arduino_wby_gpio_init(&gpio_start, PIN_START);
    arduino_wby_gpio_init(&gpio_home, PIN_HOME);
    arduino_wby_gpio_init(&gpio_power, PIN_POWER);
    arduino_wby_gpio_init(&gpio_wireless, PIN_WIRELESS);
    wby_button_init(&btn_y, (wby_gpio_t *)&gpio_y);
    wby_button_init(&btn_b, (wby_gpio_t *)&gpio_b);
    wby_button_init(&btn_a, (wby_gpio_t *)&gpio_a);
    wby_button_init(&btn_x, (wby_gpio_t *)&gpio_x);
    wby_button_init(&btn_l, (wby_gpio_t *)&gpio_l);
    wby_button_init(&btn_r, (wby_gpio_t *)&gpio_r);
    wby_button_init(&btn_select, (wby_gpio_t *)&gpio_select);
    wby_button_init(&btn_start, (wby_gpio_t *)&gpio_start);
    wby_button_init(&btn_home, (wby_gpio_t *)&gpio_home);
    wby_button_init(&btn_power, (wby_gpio_t *)&gpio_power);
    wby_button_init(&btn_wireless, (wby_gpio_t *)&gpio_wireless);

    static arduino_wby_gpio_t gpio_up;
    static arduino_wby_gpio_t gpio_right;
    static arduino_wby_gpio_t gpio_down;
    static arduino_wby_gpio_t gpio_left;
    arduino_wby_gpio_init(&gpio_up, PIN_UP);
    arduino_wby_gpio_init(&gpio_right, PIN_RIGHT);
    arduino_wby_gpio_init(&gpio_down, PIN_DOWN);
    arduino_wby_gpio_init(&gpio_left, PIN_LEFT);
    static wby_button_t btn_up;
    static wby_button_t btn_right;
    static wby_button_t btn_down;
    static wby_button_t btn_left;
    wby_button_init(&btn_up, (wby_gpio_t *)&gpio_up);
    wby_button_init(&btn_right, (wby_gpio_t *)&gpio_right);
    wby_button_init(&btn_down, (wby_gpio_t *)&gpio_down);
    wby_button_init(&btn_left, (wby_gpio_t *)&gpio_left);
    wby_hat_init(&hat, &btn_up, &btn_right, &btn_down, &btn_left);
}

void loop()
{
    static nthaka_gamepad_state_t out;

    uint8_t d;
    nthaka_buffer_state_t s;
    if (Serial.readBytes(&d, 1) != 1 ||
        (s = nthaka_buffer_append(&buf, d, &out)) == NTHAKA_BUFFER_REJECTED)
    {
        nthaka_buffer_clear(&buf);
        return;
    }
    else if (s == NTHAKA_BUFFER_PENDING)
    {
        return;
    }

    update_3ds(&out);

    nthaka_buffer_clear(&buf);
}
