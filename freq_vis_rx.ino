// This is the receive module for the frequency visualizer.
// Brute force iteration:   display on 64x32, use mega.  Use  Serial1 (pins 18 and 19) for com with tx side.
// Go to 21 bins for the cool rectangle effect.


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
#define FREQ_BINS 21

int freq[FREQ_BINS];
int freq_hist[FREQ_BINS]={0};

#define START_CHAR 's'

// Color pallete for spectrum...cooler than just single green.
uint16_t spectrum_colors[] = 
{
  matrix.Color444(7,0,0),   // index 0
  matrix.Color444(6,1,0),   // index 1
  matrix.Color444(5,2,0),   // index 2
  matrix.Color444(4,3,0),   // index 3
  matrix.Color444(3,4,0),   // index 4
  matrix.Color444(2,5,0),   // index 5
  matrix.Color444(1,6,0),   // index 6
  matrix.Color444(0,7,0),   // index 7 
  matrix.Color444(0,6,1),   // index 8
  matrix.Color444(0,5,2),   // index 9
  matrix.Color444(0,4,3),   // index 10
  matrix.Color444(0,3,4),   // index 11
  matrix.Color444(0,2,5),   // index 12 
  matrix.Color444(0,1,6),   // index 13
  matrix.Color444(0,0,7),   // index 14
  matrix.Color444(1,0,6),   // index 15
  matrix.Color444(2,0,5),   // index 16
  matrix.Color444(3,0,4),   // index 17
  matrix.Color444(4,0,3),   // index 18
  matrix.Color444(5,0,2),   // index 19
  matrix.Color444(6,0,1),   // index 20
  matrix.Color444(7,0,0),   // index 21
};

void setup() 
{
  matrix.begin();
  
  Serial.begin(9600);
  Serial1.begin(38400);

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

void display_freq_raw( void )
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
     
    x = i*3;
    
    matrix.drawRect(x,32,3,0-mag, spectrum_colors[i]);
  }

  matrix.swapBuffers(true);
 
}


void display_freq_decay( void )
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
        
    // check if current magnitude is smaller than our recent history.   
    if (mag < freq_hist[i])
    {
      // decay by 1...but only if we're not going negative
      if (freq_hist[i]) 
      {
        mag = freq_hist[i] - 1;
      }
    }

    // store new value...this will either be the new max or the new "decayed" value.
    freq_hist[i] = mag;
     
    x = i*3;
    
    matrix.drawRect(x,32,3,0-mag, spectrum_colors[i]);
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
            display_freq_raw();
            current_state = WAIT_FOR_BUFFER;
         }
      break;
        
    }  // end switch on state
  }  // end while xbee available.

  
} // end of loop
