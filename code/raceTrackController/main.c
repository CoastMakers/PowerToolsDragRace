/*******************************************************************************
 Rue's "is it alive" program for avr processors
  also makes a good skel to build your programs form.


relay 1 on port B2
relay 2 on port B4

// PCMSK 1

start button is C1  PCINT9
stop button is  C0  PCINT8

finish lane 1 is C2 PCINT10
finish lane 2 is C3 PCINT11



were looking at 99.99 second limit

thats 9999 100'ths of a second. which will fit in a 12 bit integer, THIS IS WHY WE SHOULD STILL HAVE 4 BIT COMPUTERS!
so, our code will have capacity of up to 655.35 seconds, cause I'm using signed, thats right, if you finish 
before you started, we got it covered.

0 means were not running. when we start, we bump it to 0.01 seconds


interrupts:

  - timer 1/100th of a second, increment counters if they were not 0
  
  - start button, do stuff
  
  - stop button, do stuff
  
  - lane 1 finish, do stuff
  
  - lane 2 finish, do stuff
  
  - apocaplyse. stop doing stuff, save work.
  
  


*******************************************************************************/
 

#include <avr/io.h>
#include "avrcommon.h"
#include "usart.h"

// misc

#define OUTPUT             1
#define INPUT              0

#define StartUp()  IsHigh(1, PINC)
#define StopUp()   IsLow(0, PINC)

#define RelaysOn()  PORTB |=  ((1 << 2) | (1 << 4)) 
#define RelaysOff() PORTB &= ~((1 << 2) | (1 << 4))
#define Relay1Off() ClearBit(2, PORTB)
#define Relay2Off() ClearBit(4, PORTB)


void initHardware(void) ;
void Delay(unsigned int delay);
void LongDelay(unsigned int delay) ;


int main (void) {
 
  uint8_t flag = 0;

  initHardware();

  while(1) {

    while(StartUp()); // wait for start button
    PORTB = 0xFF;
    
    while(StopUp()); // wait for stop button
    PORTB = 0x00;

    if(0) {
    } else if (flag) {
      USART_printstring((unsigned char*)"Hello terminal.\n");
    } else {
      ;
    }
    
  }

}


// { page 74 }
ISR (PCINT1_vect) {
  if (IsHigh(3, PIND))  SetBit(5, PORTB);
  else                ClearBit(5, PORTB);
}




void initHardware(void) {

  // set up directions 
  DDRB = (INPUT << PB0 | INPUT << PB1 |OUTPUT << PB2 |INPUT << PB3 |OUTPUT << PB4 |INPUT << PB5 |INPUT << PB6 |INPUT << PB7);
  DDRC = (INPUT << PC0 | INPUT << PC1 |INPUT << PC2 |INPUT << PC3 |INPUT << PC4 |INPUT << PC5 |INPUT << PC6 );
  DDRD = (INPUT << PD0 | INPUT << PD1 |INPUT << PD2 |INPUT << PD3 |INPUT << PD4 |INPUT << PD5 |INPUT << PD6 |INPUT << PD7);        

  PORTC = 0xFF; // pullups on

  // init pin change interrupts


  // init serial
  USART_Init( 103 ); // 9600 baud

  SetBit(PCIE1, PCICR);    //    PCICR |= _BV(PCIE2); // set PCIE2 to enable PCMSK2 scan
  SetBit(PCINT8, PCMSK1); //   PCMSK2 |= _BV(PCINT16); // set PCINT16 to trigger an interrupt on state change
  SetBit(PCINT9, PCMSK1);
  SetBit(PCINT10, PCMSK1);
  SetBit(PCINT11, PCMSK1);
    
  sei(); // turn on interrupts


}

void StartButtonPushed( void ) {
  // close both relays
  // reset timer
  // send 'START'
}

void StopButtonPushed( void ) {
  // open both relays
}

void lane1Finished( void ) {
  // open lane 1 relay
  // send lane 1 time  "1 - ss.t"
}

void lane2Finished( void ) {
  // open line 2 relay
  // send lane 2 time "2 - ss.t"
}





void Delay(unsigned int delay) {
  unsigned int x;
  for (x = delay; x != 0; x--) {
    asm volatile ("nop"::); 
  }
}

void LongDelay(unsigned int delay) {

  unsigned int x;
  for (x = delay; x != 0; x--) {
    Delay(65535);
  }

}


