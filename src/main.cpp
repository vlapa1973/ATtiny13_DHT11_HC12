/************************************************************
 *   DHT11 sensor + HC12
 *   = vlapa =
 *   20230222 - 20230313
 *   v.010
 *************************************************************/
/*
                      ______
    (RES)   PB5 _ 1 -|      |- 8 _ VCC   (+)
    (TX)    PB3 _ 2 -|      |- 7 _ PB2   (ADC_in)
    (Gate)  PB4 _ 3 -|      |- 6 _ PB1   ()
    (GND)   GND _ 4 -|      |- 5 _ PB0   (DHT_in)
                      ------
*/

#define F_CPU 9600000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart_intr.h"
#include "dht11.h"
#include "myLib.h"

#define PAUSE_TRANZ 50       //  задержка перед вкл/выкл
#define DATA_OSCCAL 0x50;    //  коррекция частоты внутр генер

UART uart;

const uint8_t SENSOR_NUM = 21;
const uint8_t COUNT = 1;
uint8_t countSleep = COUNT;

uint8_t _hum, _temp0, _temp1;

//==========================================
ISR(WDT_vect)
{
  countSleep--;
}

//==========================================
void dataOut(void)
{
  uart.print(SENSOR_NUM);

  uart.print("%");
  uart.print(_hum);

  uart.print("*");
  uart.print(_temp0);
  uart.print(".");
  uart.print(_temp1);

  uart.print("^");
  uart.print(median(adc_read()) * 16);
  uart.print(';');
  uart.println();
}

//==========================================
int main(void)
{
  OSCCAL = DATA_OSCCAL;        //  корректировка частоты кварца
  DDRB |= (1 << PB4);   //  управление транзистором
  PORTB |= (1 << PB4); //

  uart.begin();

  // for (uint8_t i = 0; i < 3; ++i)
  // {
  //   median(adc_read());
  // }

  MCUCR |= (1 << SE);  //  разрешить сон
  MCUCR |= (1 << SM1); //  power down

  WDTCR |= (1 << WDCE) | (1 << WDE);//  разр изм вачдога (действ 4 такта)
  WDTCR = (1 << WDTIE) | (1 << WDP3) | (1 << WDP0);// | (1 << WDP0);
  //  делитель WDT:
  //  8 - WDP3_WDP0
  //  4 - WDP3
  //  2 - WDP2_WDP1_WDP0
  //  1 - WDP2_WDP1
  sei();

  //==========================================
  while (1)
  {
    if (countSleep)
    {
      asm("sleep");
    }
    else
    {
      PORTB |= (1 << PB4);
      _delay_ms(PAUSE_TRANZ);

      countSleep = COUNT;
      adc_ini();
      dht_read(&_hum, &_temp0, &_temp1);
      dataOut();
      ADCSRA = 0;
      _delay_ms(PAUSE_TRANZ);
      PORTB &= ~(1 << PB4);
      _delay_ms(20);
      asm("sleep");
    }
  }
}

//==========================================