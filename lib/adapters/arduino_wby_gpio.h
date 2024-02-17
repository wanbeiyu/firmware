#ifndef ARDUINO_WBY_GPIO_H_
#define ARDUINO_WBY_GPIO_H_

#include <Arduino.h>
#include <wanbeiyu.h>

typedef struct arduino_wby_gpio_t
{
    wby_gpio_t parent;
    pin_size_t pin;
} arduino_wby_gpio_t;

void arduino_wby_gpio_set_low(wby_gpio_t *parent)
{
    arduino_wby_gpio_t *gpio = (arduino_wby_gpio_t *)parent;

    pinMode(gpio->pin, OUTPUT);
    digitalWrite(gpio->pin, LOW);
}

void arduino_wby_gpio_set_hi_z(wby_gpio_t *parent)
{
    arduino_wby_gpio_t *gpio = (arduino_wby_gpio_t *)parent;

    pinMode(gpio->pin, INPUT);
}

inline void arduino_wby_gpio_init(arduino_wby_gpio_t *gpio, pin_size_t pin)
{
    if (gpio == NULL)
    {
        return;
    } 

    gpio->parent.set_low = arduino_wby_gpio_set_low;
    gpio->parent.set_hi_z = arduino_wby_gpio_set_hi_z;

    gpio->pin = pin;
}

#endif // ARDUINO_WBY_GPIO_H_