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
#include <avr/interrupt.h>
#include "avrcommon.h"
#include "binary.h"
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
void inline printDigit(unsigned char n);
void sendTime(unsigned int n);
void startButtonPushed( void );
void stopButtonPushed( void ) ;
void lane1Finished( void );
void lane2Finished( void );



unsigned int masterTime;
unsigned int lane1Time;
unsigned int lane2Time;


#define flag_start  0
#define flag_stop   1
#define flag_lane1  2
#define flag_lane2  3

volatile uint8_t flags = 0;


int main (void) {
 
  initHardware();

  while(1) {

    if(0) {
    } else if (IsHigh(flag_start, flags)) {  // start event
       USART_printstring((unsigned char*)"START\r\n");
       ClearBit(flag_start, flags);
      
    } else if (IsHigh(flag_stop, flags)) {  // stop event
       USART_printstring((unsigned char*)"STOP\r\n");
       ClearBit(flag_stop, flags);
    
    } else if (IsHigh(flag_lane1, flags)) {  // lane 1 time
          // send lane 1 time  "1 - ss.th"
       USART_printstring((unsigned char*)"1 - ");
       sendTime(lane1Time);
       USART_printstring((unsigned char*)"\r\n");
       ClearBit(flag_lane1, flags);
     
    } else if (IsHigh(flag_lane2, flags)) {  // lane 2 time
         // send lane 1 time  "2 - ss.th"
       USART_printstring((unsigned char*)"2 - ");    
       sendTime(lane2Time);
       USART_printstring((unsigned char*)"\r\n");
       ClearBit(flag_lane2, flags);
    
    } else {
      ;
    }
    
  }

}

void inline printDigit(unsigned char n) {
  USART_Transmit( (n & 0x0F) | 0x30 );
}


void sendTime(unsigned int n) {
  unsigned int d;
  
  d = n/10000;
  printDigit(d);
  n -= d * 10000;
  
  d = n/1000;
  printDigit(d);
  n -= d * 1000;
  
  d = n/100;
  printDigit(d);
  n -= d * 100;
  
  USART_Transmit('.');
  
  d = n/10;
  printDigit(d);
  n -= d * 10;
  
  printDigit(n);



}


// { page 74 }
ISR (PCINT1_vect) {

  if (0){
  } else if (IsHigh(0, PINC)) {  // stop
      stopButtonPushed();
  } else if (IsLow(1, PINC)) {  // start
      startButtonPushed();
  } else if (IsLow(2, PINC)) {  // lane 1
      lane1Finished();
  } else if (IsLow(3, PINC)) {  // lane 2
      lane2Finished();
  }

}


//   100Hz timer/counter
ISR( TIMER1_COMPA_vect ) {  
  masterTime++;  
}



void initHardware(void) {

  // set up directions 
  DDRB = (INPUT << PB0 | INPUT << PB1 |OUTPUT << PB2 |INPUT << PB3 |OUTPUT << PB4 |INPUT << PB5 |INPUT << PB6 |INPUT << PB7);
  DDRC = (INPUT << PC0 | INPUT << PC1 |INPUT << PC2 |INPUT << PC3 |INPUT << PC4 |INPUT << PC5 |INPUT << PC6 );
  DDRD = (INPUT << PD0 | INPUT << PD1 |INPUT << PD2 |INPUT << PD3 |INPUT << PD4 |INPUT << PD5 |INPUT << PD6 |INPUT << PD7);        

  PORTC = 0xFF; // pullups on

  // init serial
  USART_Init( 103 ); // 9600 baud


  // init pin change interrupts
  SetBit(PCIE1,   PCICR);    //    PCICR |= _BV(PCIE2); // set PCIE2 to enable PCMSK2 scan
  
  SetBit(PCINT8,  PCMSK1);   //   PCMSK2 |= _BV(PCINT16); // set PCINT16 to trigger an interrupt on state change
  SetBit(PCINT9,  PCMSK1);
  SetBit(PCINT10, PCMSK1);
  SetBit(PCINT11, PCMSK1);
    
  // counter 1 is used to time out the pwm pulse
  // counter 1 to CTC, no output control,  interrupt enabled, clock source off.
  
  TCCR1A = b00000000; // [ COM1A1 | COM1A0 | COM1B1 | COM1B0 | FOC1A | FOC1B  | WGM11   | WGM10  ]
  TCCR1B = b00011001; // [ ICNC1  | ICES1  |        | WGM13  | WGM12 | CS12   | CS11    | CS10   ]
  
  TIMSK1  = b00000010;
  
  OCR1A = 8000;
    
  sei(); // turn on interrupts


}

void startButtonPushed( void ) { 
  // close both relays 
  RelaysOn();
  
  // reset timer 
  masterTime = 0;
  
  // send 'START'
  SetBit(flag_start, flags);
  
}

void stopButtonPushed( void ) {
  // open both relays
  RelaysOff();
  
  // send 'STOP'
  SetBit(flag_stop, flags);
}

void lane1Finished( void ) {
  // open lane 1 relay
  Relay1Off();
  lane1Time = masterTime;

  // send lane 1 time  "1 - ss.th"
  SetBit(flag_lane1, flags);
}

void lane2Finished( void ) {
  // open line 2 relay
  Relay2Off();
  lane2Time = masterTime;

  // send lane 2 time "2 - ss.th"
  SetBit(flag_lane2, flags);
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


