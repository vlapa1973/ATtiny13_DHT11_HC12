#pragma once

#include <avr/io.h>

//===================================================
void adc_ini(void)
{
    ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX0);
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

//===================================================
uint8_t adc_read(void)
{
    _delay_ms(1);
    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADSC)))
        ;
    return ADCH;
}

//===================================================
// медиана на 3 значения со своим буфером
uint8_t median(uint8_t newVal)
{
    static uint8_t buf[3];
    static uint8_t count = 0;
    buf[count] = newVal;
    if (++count >= 3)
        count = 0;

    return (buf[0] < buf[1]) ? ((buf[1] < buf[2]) ? buf[1] : ((buf[2] < buf[0]) ? buf[0] : buf[2])) : ((buf[0] < buf[2]) ? buf[0] : ((buf[2] < buf[1]) ? buf[1] : buf[2]));
}

//===================================================