/*******************************************************************************

relay 1 on port B2
relay 2 on port B4

// PCMSK 1

start button is C1  PCINT9
stop button is  C0  PCINT8

finish lane 1 is C2 PCINT10
finish lane 2 is C3 PCINT11

reset button is B1 PCINT1

clock test pin is C4



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
  
all events are poured into a LUT based FSM which minds itself and outputs 
  flags for procedural operations.
  
  Rue Mohr 2018  


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
void eventStart( void );
void eventStop( void ) ;
void eventLane1Finished( void );
void eventLane2Finished( void );
void eventSetup( void );
void stateMachine(uint8_t eventCode);

unsigned int masterTime;
unsigned int lane1Time;
unsigned int lane2Time;


#define flag_start  0
#define flag_stop   1
#define flag_lane1  2
#define flag_lane2  3
#define flag_setup  4

volatile uint8_t flags = 0; // triggers from state machine




int main (void) {
 
  initHardware();
  
  USART_printstring((unsigned char*)"IzBOOTED\r\n"); // this is to debug power problems.


  while(1) {  // this is basically just a asynchronous comm loup
  
    
    if (IsHigh(flag_setup, flags)) {  // setup event
         // send lane 1 time  "SETUP"
       USART_printstring((unsigned char*)"SETUP\r\n");    
       ClearBit(flag_setup, flags);
    } 
    
    if (IsHigh(flag_start, flags)) {  // start event
         USART_printstring((unsigned char*)"START\r\n");
         ClearBit(flag_start, flags);	       
    } 
    
    if (IsHigh(flag_stop, flags)) {  // stop event
         USART_printstring((unsigned char*)"STOP\r\n");
         ClearBit(flag_stop, flags);
    } 
    
    if (IsHigh(flag_lane1, flags)) {  // lane 1 time
          // send lane 1 time  "1 - ss.th"
       USART_printstring((unsigned char*)"1 - ");
       sendTime(lane1Time);
       USART_printstring((unsigned char*)"\r\n");
       ClearBit(flag_lane1, flags);     
    } 
    
    if (IsHigh(flag_lane2, flags)) {  // lane 2 time
         // send lane 1 time  "2 - ss.th"
       USART_printstring((unsigned char*)"2 - ");    
       sendTime(lane2Time);
       USART_printstring((unsigned char*)"\r\n");
       ClearBit(flag_lane2, flags);           
    }
        
    
  }

}





//------------------------ INTERRUPT HANDLERS ------------------------------

// { page 74 }
ISR (PCINT1_vect) {


  if (IsHigh(0, PINC)) {  // stop
     stateMachine(0);     
  } 
  
  if (IsLow(1, PINC)) {  // start
     stateMachine(1);     
  } 
  
  if (IsLow(2, PINC)) {  // lane 1
     stateMachine(2);     
  }
  
  if (IsLow(3, PINC)) {  // lane 2
     stateMachine(3);     
  }  
  

}

ISR (PCINT0_vect) {

  if (IsLow(1, PINB)) {  // setup
     //eventSetup();
     stateMachine(4); 
  }

}



//   100Hz timer/counter
ISR( TIMER1_OVF_vect ) {  
  masterTime++;  
  PORTC ^= (1<<4); //SetBit(4, PINC);
}


// ---------------------- OPERATIONAL STATE MACHINE ----------------------------------

/********** junk bin ***************

 //uint8_t table[] = { 0, 1, 0, 0, 0, 1, 3, 2, 0, 2, 0, 2, 0, 3, 3, 0 };  
 // uint8_t table[] = {0,5,0,0,8,1,0x0F, 0x12, 8, 2, 0x0C, 2, 8, 3, 3, 0x10 };
  //idx = ((state<<2) | eventCode);


*************************************/

void stateMachine(uint8_t eventCode) {
 static uint8_t state = 0;
  
 uint8_t idx;
 
 // address compressed LUT, see the excel spreadsheet.
  uint8_t table[] = { 0, 0, 0, 0, 41, 0, 10, 1, 1, 1, 16, 2, 28, 35, 2, 16, 3, 24, 3, 3, 16, 4, 4, 32, 4};
 
 idx = state*5+eventCode; // thats address compression for ya.
 state = table[ idx ] & 0x07;

 // trigger things from the state machine output
 switch ( table[idx] >> 3) {
          
   case 1:  // start
      eventStart(); 
   break;
         
   case 2:  // abort
     eventStop();
   break;
         
   case 3:  // lane 1 finish
     eventLane1Finished();
   break;
         
   case 4:  //  lane 2 finish
     eventLane2Finished();
   break;
   
   case 5:  //  setup 
     eventSetup();
   break;
 
 }

}


//-------------------------------- UTILITY --------------------------------

void inline printDigit(unsigned char n) {
  USART_Transmit( (n & 0x0F) | 0x30 );
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

void initHardware(void) {

  // set up directions 
  DDRB = (INPUT << PB0 | INPUT << PB1 |OUTPUT << PB2 |INPUT << PB3 |OUTPUT << PB4 |INPUT << PB5 |INPUT << PB6 |INPUT << PB7);
  DDRC = (INPUT << PC0 | INPUT << PC1 |INPUT << PC2 |INPUT << PC3 |OUTPUT << PC4 |INPUT << PC5 |INPUT << PC6 );
  DDRD = (INPUT << PD0 | INPUT << PD1 |INPUT << PD2 |INPUT << PD3 |INPUT << PD4 |INPUT << PD5 |INPUT << PD6 |INPUT << PD7);        

  PORTC = 0x13; // pullups on
  PORTB = 0x02; // pullup on

  // ----------- init serial --------
  USART_Init( 103 ); // 9600 baud


  // ---------- init pin change interrupts ------
  SetBit(PCIE1,   PCICR);    //    PCICR |= _BV(PCIE2); // set PCIE2 to enable PCMSK2 scan
  SetBit(PCIE0,   PCICR);
  
  SetBit(PCINT1,  PCMSK0);
  
  SetBit(PCINT8,  PCMSK1);   //   PCMSK2 |= _BV(PCINT16); // set PCINT16 to trigger an interrupt on state change
  SetBit(PCINT9,  PCMSK1);
  SetBit(PCINT10, PCMSK1);
  SetBit(PCINT11, PCMSK1);
    
  
  // ----------- init timer 100Hz ------  
  // TCCR1 is 16 bits

  // set prescaler to 8Mhz / 64 base rate 62500 Hz
  // CTC mode so that we can define top.
  TCCR1A = (1<<WGM10)|(1<<WGM11);
  TCCR1B = (1<<CS10)|(1<<CS11)|(1<<WGM13);
  
//  OCR1A = 1250-6; // cap the counter at 1250+tuning, which leaves 100 rollover per second
    OCR1A = 1250; // cap the counter at 1250+tuning, which leaves 100 rollover per second
  
  // set interrupt mask register 
  TIMSK1|=(1<<TOIE1);  
    
    
  sei(); // turn on interrupts


}

// -------------------- STATE MACHINE TRIGGERED ------------------------------

void eventSetup( void ){

 // send 'SETUP'
  SetBit(flag_setup, flags);

}


void eventStart( void ) { 
  // close both relays 
  RelaysOn();
  
  // reset timer 
  masterTime = 0;
  
  // send 'START'
  SetBit(flag_start, flags);
  
}

void eventStop( void ) {
  // open both relays
  RelaysOff();
  
  // send 'STOP'
  SetBit(flag_stop, flags);
}

void eventLane1Finished( void ) {
  // open lane 1 relay
  Relay1Off();
  lane1Time = masterTime;

  // send lane 1 time  "1 - ss.th"
  SetBit(flag_lane1, flags);
}

void eventLane2Finished( void ) {
  // open line 2 relay
  Relay2Off();
  lane2Time = masterTime;

  // send lane 2 time "2 - ss.th"
  SetBit(flag_lane2, flags);
}


