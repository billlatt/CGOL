/*
Life Simulator Matrix Test
  
Walks through each cell in the matrix, lighting up in RED and then each neighbor in turn in GREEN, without wrapping

*/

#include <Adafruit_NeoPixel.h>

// Number of LEDs in attached NeoPixel
#define LED_COUNT 64

// Arduino pin connected to NeoPixel
#define LED_PIN 6

/*
 * Declare NeoPixel strip object:
 *  Argument 1 = Number of pixels in NeoPixel strip
 *  Argument 2 = Arduino pin number (most are valid)
 *  Argument 3 = Pixel type flags, add together as needed:
 *  NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
 *  NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
 *  NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
 *  NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
 *  NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
*/
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN);

// Set the array dimensions based on matrix size
#define LED_ROW 8
#define LED_COL 8

// Define pixel # in a 2-dimensional array; NOTE that this is very specific to individual matrixes
int pixel_reference[LED_ROW][LED_COL] = {{63,62,61,60,59,58,57,56},{48,49,50,51,52,53,54,55},{47,46,45,44,43,42,41,40},{32,33,34,35,36,37,38,39},{31,30,29,28,27,26,25,24},{16,17,18,19,20,21,22,23},{15,14,13,12,11,10,9,8},{0,1,2,3,4,5,6,7}};

void setup() {
  // Start the Serial Monitor for debugging
  // Serial.begin(9600);
  
  strip.begin();          // Setup the strip, all off
  strip.show();           // Display the strip
  strip.setBrightness(3); // Set low brightness (max 255)
}

void loop() {
  // Walk through each cell (matrix LED) and light up neighbors
  
  for (int row = 0; row < LED_ROW; row++) {
    for (int col = 0; col < LED_COL; col++) {
            
      //Set cell to RED
      strip.setPixelColor(pixel_reference[row][col],255,0,0); 
      strip.show(); 
      delay(500);
    
      // Set cell to left GREEN
      if(col-1>=0){
        strip.setPixelColor(pixel_reference[row][col-1],0,255,0); 
        strip.show(); 
        delay(500);         
      }
      
      // Set cell to right GREEN
      if(col+1<LED_COL){
        strip.setPixelColor(pixel_reference[row][col+1],0,255,0); 
        strip.show(); 
        delay(500);         
      }
      
      //Set cells ABOVE current cell to GREEN
      if(row-1>=0){
        
        // Set cell directly above to GREEN
        strip.setPixelColor(pixel_reference[row-1][col],0,255,0); 
        strip.show(); 
        delay(500);
      
        //Set cell above and the the left GREEN
        if(col-1>=0){
          strip.setPixelColor(pixel_reference[row-1][col-1],0,255,0); 
          strip.show(); 
          delay(500);
        }
        
       // Set cell above and to the right GREEN
       if(col+1<LED_COL){        
          strip.setPixelColor(pixel_reference[row-1][col+1],0,255,0); 
          strip.show(); 
          delay(500);        
       }
      }
      
      //Set cells BELOW current cell to GREEN
      if(row+1<LED_ROW){
        
        //Set cell directely BELOW to GREEN
        strip.setPixelColor(pixel_reference[row+1][col],0,255,0); 
        strip.show(); 
        delay(500);
      
        //Set cell below and to the left GREEN
        if(col-1>=0){
          strip.setPixelColor(pixel_reference[row+1][col-1],0,255,0); 
          strip.show(); 
          delay(500);
        }
        
        //Set cell below and to the right GREEN
       if(col+1<LED_COL){        
          strip.setPixelColor(pixel_reference[row+1][col+1],0,255,0); 
          strip.show(); 
          delay(500);        
        }
      }
      
      //Clear the strip and show; start again
      strip.clear();          // Setup the strip, all off
      strip.show();           // Display the strip
      delay(500);
      }
    }
  }