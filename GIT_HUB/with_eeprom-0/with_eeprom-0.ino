      
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"
#include<EEPROM.h>

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;
int sensor=4;
unsigned long a,b;
unsigned long flag,val;
int bulb = 3;
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8,9);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

// Used to control whether this node is sending or receiving
bool role = 0;

void setup() {
  
  pinMode(bulb,OUTPUT);
  pinMode(sensor,INPUT);
  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
  
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();
  int state=EEPROM.read(0);
  if (state==1)
     digitalWrite(bulb,HIGH);
    else if (state=0)
     digitalWrite(bulb,LOW);
    else
     digitalWrite(bulb,LOW);
}

void loop() {
  
  
/****************** Ping Out Role ***************************/  
if (role == 1)  {
    
    radio.stopListening();                                    // First, stop listening so we can talk.
    
    
    Serial.println(F("Now sending"));

    unsigned long time = micros();                             // Take the time, and send it.  This will block until complete
     if (!radio.write( &time, sizeof(unsigned long) )){
       Serial.println(F("failed"));
     }
        
    radio.startListening();                                    // Now, continue listening
    
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while ( ! radio.available() ){                             // While nothing is received
      if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
      }      
    }
        
    if ( timeout ){                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
    }else{
        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
        radio.read( &got_time, sizeof(unsigned long) );
        unsigned long time = micros();
        
        // Spew it
        Serial.print(F("Sent "));
        Serial.print(time);
        Serial.print(F(", Got response "));
        Serial.print(got_time);
        Serial.print(F(", Round-trip delay "));
        Serial.print(time-got_time);
        Serial.println(F(" microseconds"));
    }

    // Try again 1s later
    delay(1000);
  }



/****************** Pong Back Role ***************************/

  if ( role == 0 )
  {
    unsigned long got_time;

    
     
    if( radio.available()){
                                                                    // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
      }
      //Serial.print(got_time);
      a=got_time%10;
      b=got_time/10;
      
      if (a==0) {
      
      radio.stopListening();                                        // First, stop listening so we can talk   
       if (b==1)
        digitalWrite(bulb,HIGH);
       else if (b==0)
       digitalWrite(bulb,LOW);

        if (b==0||b==1)  {
        
       EEPROM.update(0,b);
      delay(100);
        }
        if(b==0||b==1||b==2)
        {
      sensor=digitalRead(4);
      if (sensor==HIGH)
       flag=1;
      else
       flag=0; 

       if (a==0&&flag==0)   
       val=0;
       if (a==0&&flag==1)   
       val=1;
       if (a==1&&flag==0)   
       val=2;
       if (a==1&&flag==1)   
       val=3;
       
      radio.write(&flag,  sizeof(unsigned long) );              // Send the final one back.      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F("Sent response "));
      Serial.println(got_time);  
      }

      }      
   }
 }




/****************** Change Roles via Serial Commands ***************************/

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == 0 ){      
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      role = 1;                  // Become the primary transmitter (ping out)
    
   }else
    if ( c == 'R' && role == 1 ){
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));      
       role = 0;                // Become the primary receiver (pong back)
       radio.startListening();
       
    }
  }


} // Loop

