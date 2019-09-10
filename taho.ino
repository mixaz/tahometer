/*
 * Tahometer pulse meter
 *
 * Copyright (C) 2019 Mikhail Zemlyanukha <gmixaz@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */
 

#define PIN_TAHO              PB8
#define PIN_LED1              PC13
#define PIN_LED2              PC14

static volatile uint32_t taho_counter;    // volatile since it is written from several threads
static bool led1_triggered;
static bool led2_triggered;

void taho_interrupt()
{
  taho_counter++;
}

void taho_timer()
{
    led1_triggered = taho_counter >= 60;
    led2_triggered = taho_counter >= 70;
    taho_counter = 0;
}

void start_taho() 
{
  taho_counter = 0;
  Timer3.resume();
  attachInterrupt(PIN_TAHO, taho_interrupt, RISING);
}

void stop_taho()
{
  Timer3.pause();
  detachInterrupt(PIN_TAHO);
}

void init_timer()
{
    Timer3.pause();
    Timer3.setChannel1Mode(TIMER_OUTPUTCOMPARE);
    Timer3.setPeriod(200000*5); // in microseconds
    Timer3.setCompare1(1);      
    Timer3.attachCompare1Interrupt(taho_timer);     
}

void setup() {
  pinMode(PIN_TAHO, INPUT_PULLUP);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);

  init_timer();

  start_taho();
  
}


void loop() {
    digitalWrite(PIN_LED1, led1_triggered);
    digitalWrite(PIN_LED2, led2_triggered);
    delay(50);
}

