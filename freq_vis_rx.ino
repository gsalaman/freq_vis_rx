// This is the receive module for the frequency visualizer.
// Right now, we're just printing the values we receive.

#include "SoftwareSerial.h"
SoftwareSerial XBee(2,3);

// Must match the TX side!
#define FREQ_BINS 32

int freq[FREQ_BINS];

#define START_CHAR 's'

void setup() 
{
  Serial.begin(9600);
  XBee.begin(9600);

  Serial.println("Freq RX initialized");
}

typedef enum
{
  WAIT_FOR_BUFFER,
  PROCESS_BUFFER
} state_type;

state_type current_state=WAIT_FOR_BUFFER;

void print_freq_results( void )
{
  int i;

  for (i = 0; i < FREQ_BINS; i++)
  {
    Serial.print( freq[i] );
    Serial.print(" ");
  }

  Serial.println();
  Serial.println("====================");
}

void loop() 
{
  char c;
  int buff_index;
  
  while (XBee.available())
  {
    c = XBee.read();
    
    switch (current_state)
    {
      case WAIT_FOR_BUFFER:
        if (c == START_CHAR)
        {
          buff_index = 0;
          current_state = PROCESS_BUFFER;
        }
      break;

      case PROCESS_BUFFER:
         freq[buff_index] = c;
         buff_index++;
         if (buff_index == FREQ_BINS)
         {
            print_freq_results();
            current_state = WAIT_FOR_BUFFER;
         }
      break;
        
    }  // end switch on state
  }  // end while xbee available.
} // end of loop
