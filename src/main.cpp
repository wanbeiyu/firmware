#include <Arduino.h>

#include <nthaka.h>
#include <nthaka/nxmc2.h>
#include <nthaka/pokecon.h>

#include <wanbeiyu.h>

static nxmc2_format_handler_t nxmc2;
static pokecon_format_handler_t pokecon;
static nthaka_format_handler_t *fmts[] = {(nthaka_format_handler_t *)&nxmc2, //
                                          (nthaka_format_handler_t *)&pokecon};
static nthaka_multi_format_handler_t fmt;
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

static wby_button_t btn_up;
static wby_button_t btn_right;
static wby_button_t btn_down;
static wby_button_t btn_left;
static wby_hat_t hat;

static wby_slidepad_t sp;

static wby_touchscreen_t ts;

void update(nthaka_gamepad_state_t *state)
{
}

void setup()
{
    nxmc2_format_handler_init(&nxmc2);
    pokecon_format_handler_init(&pokecon);
    nthaka_multi_format_handler_init(&fmt, fmts, 2);
    nthaka_buffer_init(&buf, (nthaka_format_handler_t *)&fmt);
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

    update(&out);

    nthaka_buffer_clear(&buf);
}
