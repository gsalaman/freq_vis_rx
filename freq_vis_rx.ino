// This is the receive module for the frequency visualizer.
// Next iteration:   display on 64x32, use mega.  XBee needs to go on Serial1 (pins 18 and 19)

#include "SoftwareSerial.h"
  
// These two defines are for the RGB Matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

// Pin defines for the 32x32 RGB matrix.
#define CLK 11  
#define LAT 10
#define OE  9
#define A   A0
#define B   A1
#define C   A2
#define D   A3

/* Other Pin Mappings...hidden in the RGB library:
 *  Sig   Uno  Mega
 *  R0    2    24
 *  G0    3    25
 *  B0    4    26
 *  R1    5    27
 *  G1    6    28
 *  B1    7    29
 */

// Note "false" for double-buffering to consume less memory, or "true" for double-buffered.
// Double-buffered makes updates look smoother.
RGBmatrixPanel matrix(A, B, C,  D,  CLK, LAT, OE, true, 64);

// Must match the TX side!
#define FREQ_BINS 32

int freq[FREQ_BINS];

#define START_CHAR 's'

void setup() 
{
  matrix.begin();
  
  Serial.begin(9600);
  Serial1.begin(9600);

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

void display_freq( void )
{
  int i;
  int mag;
  
  int x;    

  matrix.fillScreen(0);

  // we have 32 freq bins, but I want to each bin to be 3 wide.
  // This means I'm going from bins 1 to 21 (which gets us to 63)
  for (i = 0; i < FREQ_BINS; i++)
  {
    mag = freq[i];
     
    x = i*2;
    
    matrix.drawRect(x,32,2,0-mag, matrix.Color333(1,0,0));
  }

  matrix.swapBuffers(true);
 
}
void loop() 
{
  char c;
  int buff_index;
  
  while (Serial1.available())
  {
    c = Serial1.read();
    
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
            //print_freq_results();
            display_freq();
            current_state = WAIT_FOR_BUFFER;
         }
      break;
        
    }  // end switch on state
  }  // end while xbee available.

  
} // end of loop
