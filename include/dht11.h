#pragma once

#include <avr/io.h>
#include <util/delay.h>

#define DHT_DDR DDRB
#define DHT_PIN PINB
#define DHT_PORT PORTB
#define DHT PB0

uint8_t corr = 8; //  корректировка данных датчика температуры

uint8_t dht_start(void)
{
    DHT_DDR |= (1 << DHT);
    _delay_ms(20);
    DHT_DDR &= ~(1 << DHT);
    _delay_us(40);
    if (DHT_PIN & (1 << DHT))
        return 1;
    _delay_us(80);
    if (!(DHT_PIN & (1 << DHT)))
        return 2;
    while (DHT_PIN & (1 << DHT))
        ;
    return 0;
}

uint8_t dht_byte(void)
{
    uint8_t i = 8, byte = 0;
    while (i--)
    {
        while (!(DHT_PIN & (1 << DHT)))
            ;
        _delay_us(40);
        if (DHT_PIN & (1 << DHT))
        {
            byte |= (1 << i);
            while (DHT_PIN & (1 << DHT))
                ;
        }
    }
    return byte;
}

uint8_t dht_read(uint8_t *hum, uint8_t *temp0, uint8_t *temp1)
{
    uint8_t data[5];
    uint8_t error = dht_start();

    if (error)
        return error;

    for (uint8_t i = 0; i < 5; i++)
    {
        data[i] = dht_byte();
    }

    if (data[0] + data[1] + data[2] + data[3] != data[4])
        return 3;

    *hum = data[0];

    if ((data[3] + corr) < 10)
    {
        *temp1 = data[3] + corr;
        *temp0 = data[2];
    }
    else
    {
        *temp1 = (data[3] + corr) % 10;
        *temp0 = data[2] + (data[3] + corr) / 10;
    }

    return 0;
}
