/*******************************************************************************
 Rue's "is it alive" program for avr processors
  also makes a good skel to build your programs form.


relay 1 on port B2
relay 2 on port B4

start button is C1
stop button is  C0

*******************************************************************************/


#include <avr/io.h>
#include "avrcommon.h"


// misc

#define OUTPUT             1
#define INPUT              0

#define StartUp()  IsHigh(1, PINC)
#define StopUp()   IsLow(0, PINC)

#define RelaysOn()  PORTB |=  ((1 << 2) | (1 << 4)) 
#define RelaysOff() PORTB &= ~((1 << 2) | (1 << 4))
#define Relay1Off() ClearBit(2, PORTB)
#define Relay2Off() ClearBit(4, PORTB)


void Delay(unsigned int delay);
void LongDelay(unsigned int delay) ;

int main (void) {

  // set up directions 
  DDRB = (INPUT << PB0 | INPUT << PB1 |OUTPUT << PB2 |INPUT << PB3 |OUTPUT << PB4 |INPUT << PB5 |INPUT << PB6 |INPUT << PB7);
  DDRC = (INPUT << PC0 | INPUT << PC1 |INPUT << PC2 |INPUT << PC3 |INPUT << PC4 |INPUT << PC5 |INPUT << PC6 );
  DDRD = (INPUT << PD0 | INPUT << PD1 |INPUT << PD2 |INPUT << PD3 |INPUT << PD4 |INPUT << PD5 |INPUT << PD6 |INPUT << PD7);        

  PORTC = 0xFF; // pullups on

  while(1) {

    while(StartUp()); // wait for start button
    PORTB = 0xFF;
    
    while(StopUp()); // wait for stop button
    PORTB = 0x00;


  
  }

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


