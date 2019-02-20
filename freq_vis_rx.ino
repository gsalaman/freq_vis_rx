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

// Color pallete for spectrum...cooler than just single green.
uint16_t spectrum_colors[] = 
{
  matrix.Color444(15,0,0),   // index 0
  matrix.Color444(13,1,0),   // index 1
  matrix.Color444(12,3,0),   // index 2
  matrix.Color444(10,4,0),   // index 3
  matrix.Color444(9,6,0),   // index 4
  matrix.Color444(7,7,0),   // index 5
  matrix.Color444(6,9,0),   // index 6
  matrix.Color444(4,10,0),   // index 7 
  matrix.Color444(3,12,0),   // index 8
  matrix.Color444(1,13,0),   // index 9
  matrix.Color444(0,15,0),   // index 10
  matrix.Color444(0,13,1),   // index 11
  matrix.Color444(0,12,3),   // index 12 
  matrix.Color444(0,10,4),   // index 13
  matrix.Color444(0,9,6),   // index 14
  matrix.Color444(0,7,7),   // index 15
  matrix.Color444(0,6,9),   // index 16
  matrix.Color444(0,4,10),   // index 17
  matrix.Color444(0,3,12),   // index 18
  matrix.Color444(0,1,13),   // index 19
  matrix.Color444(0,0,15),   // index 20
  matrix.Color444(1,0,13),   // index 21
  matrix.Color444(3,0,12),   // index 22
  matrix.Color444(4,0,10),   // index 23
  matrix.Color444(6,0,9),   // index 24
  matrix.Color444(7,0,7),   // index 25
  matrix.Color444(9,0,6),   // index 26
  matrix.Color444(10,0,4),   // index 27
  matrix.Color444(11,0,3),   // index 28
  matrix.Color444(12,0,2),   // index 29
  matrix.Color444(13,0,1),   // index 30
  matrix.Color444(15,0,0),   // index 31  
};

void setup() 
{
  matrix.begin();
  
  Serial.begin(9600);
  Serial1.begin(115200);

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
    
    matrix.drawRect(x,32,2,0-mag, spectrum_colors[i]);
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
