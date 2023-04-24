/*
Created by Bill Lattimer bill.lattimer@outlook.com

Life Simulator based on the work of John Horton Conway in the 70's
 
https://conwaylife.com/wiki/
https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

The Four Rules of Life;
  Any live cell with fewer than two live neighbors dies, as if caused by underpopulation.
  Any live cell with two or three live neighbors lives on to the next generation.
  Any live cell with more than three live neighbors dies, as if by overpopulation.
  Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
  
 Uses an 8x8 NeoPixel Matrix for display using the Adafruit NeoPixel library.
  A representation of the matrix is stored in three two-dimensional arrrays
  One that contains the NeoPixel address
  One that contains the current state of the simulation
  One that contains the next generation of the simulation
 
The general flow is: 

  Randomly generate the starting positions by using a random number of cells to be lit using 1/2 of the available pixels

  Apply the rules to determine who lives and dies
 
  Check to see if the simulation is extinct (no pixels lit) or static (no changes between rounds) and if so generate new start.
 
  Display the current generation

Future work
  Determine if an array is static with repeating patterns
  Consider wrapping display 
*/

#include <Adafruit_NeoPixel.h>

// Number of LEDs in attached NeoPixel
#define LED_COUNT 64

// Arduino pin connected to NeoPixel
#define LED_PIN 6

// BRIGHTNESS level 
#define STRIP_BRIGHT 3    // set low to stay within the power ability of ~800mA of the Nano to power directly. 

/*
Declare NeoPixel strip object:
  Argument 1 = Number of pixels in NeoPixel strip
  Argument 2 = Arduino pin number (most are valid)
  Argument 3 = Pixel type flags, add together as needed:
  NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
  NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
  NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
  NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
  NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
*/
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN);

// Set the array dimensions based on matrix size
#define LED_ROW 8
#define LED_COL 8

// Define pixel # in a 2-dimensional array - NOTE that this array is specific to a particular matrix and must be tested. 
int pixel_reference[LED_ROW][LED_COL] = {{63,62,61,60,59,58,57,56},{48,49,50,51,52,53,54,55},{47,46,45,44,43,42,41,40},{32,33,34,35,36,37,38,39},{31,30,29,28,27,26,25,24},{16,17,18,19,20,21,22,23},{15,14,13,12,11,10,9,8},{0,1,2,3,4,5,6,7}};

// Number of attributes
#define CELL_ATTRIB 2

// Cell alive or dead attrbute
#define CELL_STATUS 0

// Number of generations cell has been alive attribute
#define CELL_GEN 1

// Array for the current generation
int current_state[LED_ROW][LED_COL][CELL_ATTRIB];

// Array for the next generation
int future_state[LED_ROW][LED_COL][CELL_ATTRIB];

// Generation counter
int generations = 0;

void setup() {
  // Start the Serial Monitor for debugging
  // Serial.begin(9600);
  
  strip.begin();                      // Setup the strip, all off
  strip.show();                       // Display the strip
  strip.setBrightness(STRIP_BRIGHT);  // Set low brightness to stay within Arduino power output of ~800mA
  
  random_start(); 
}

void loop() {
  // Perform tests on all current cells for life or death, and set the future state

  // Copy the current matrix into the future matrix  
  for (int row = 0; row < LED_ROW; row++) {
    for (int col = 0; col < LED_COL; col++) {
      future_state[row][col][CELL_STATUS] = current_state[row][col][CELL_STATUS];
      future_state[row][col][CELL_GEN] = current_state[row][col][CELL_GEN];
    }
  }

  // Apply the rules to see which cells live or die. 
  for (int row = 0; row < LED_ROW; row++) {
    for (int col = 0; col < LED_COL; col++) {
      int neighbors = 0;

      // check the 8 adjoining cells; colony does not wrap, while Conway would indicate that it should. 
      // check row above
      if ((current_state[row-1][col][CELL_STATUS]  == 1) and (row - 1 > 0)) (neighbors++);
      if ((current_state[row-1][col-1][CELL_STATUS]  == 1) and ((row-1 > 0) and (col-1 > 0))) (neighbors++);
      if ((current_state[row-1][col+1][CELL_STATUS]  == 1) and ((row-1 > 0) and (col+1 < LED_COL))) (neighbors++);

      // check row below
      if ((current_state[row+1][col][CELL_STATUS]  == 1) and ((row+1 < LED_ROW) and (col + 1 < LED_COL))) (neighbors++);
      if ((current_state[row+1][col-1][CELL_STATUS]  == 1) and ((row+1 < LED_ROW) and (col - 1 > 0))) (neighbors++);
      if ((current_state[row+1][col+1][CELL_STATUS]  == 1) and ((row + 1 < LED_ROW) and (col + 1 < LED_COL))) (neighbors++);
      
      // check right and left in current row
      if ((current_state[row][col-1][CELL_STATUS]  == 1) and (col - 1 > 0)) (neighbors++);
      if ((current_state[row][col+1][CELL_STATUS]  == 1) and (col + 1 < LED_COL)) (neighbors++);

      // Who lives, who dies
      // If cell is alive and has <2 or >3 neighbors, it dies
      if ((current_state[row][col][CELL_STATUS]  == 1) and ((neighbors < 2) or (neighbors > 3))) { 
        future_state[row][col][CELL_STATUS]  = 0;
        future_state[row][col][CELL_GEN] = 0;
      }
      // If cell is dead and has exactly 3 neighbors, regenerates
      else if ((current_state[row][col][CELL_STATUS]  == 0) and (neighbors == 3)) {     
        future_state[row][col][CELL_STATUS]  = 1;
        future_state[row][col][CELL_GEN] = 1;
      }
      // if neither is true, then the cell survives to the next generation
      else  {
        future_state[row][col][CELL_GEN]++;                                                        
      }
    }
  }

  // If life is extinct or static (no dynamis) randomize and restart
  if (life_extinct() || life_static())  {
    random_start();
  }
  else  {
    // Copy the future matrix to the current matrix
    for (int row = 0; row < LED_ROW; row++) {
      for (int col = 0; col < LED_COL; col++) {
        current_state[row][col][CELL_STATUS] = future_state[row][col][CELL_STATUS];
        current_state[row][col][CELL_GEN] = future_state[row][col][CELL_GEN];
      }
    }
  }
    
  // Show the current matrix, then go back, Jack, and do it again
  matrix_show();
  delay(1000);

  // Reset after 100 generations; currently to address lack of trap for repeating patterns
  generations++;
  if (generations > 100) {
    generations = 0;
    random_start();
  }
}

// Iterate through matrix and see if all cells are dead
bool life_extinct()  {
  bool le = true;

  for (int row = 0; row < LED_ROW; row++) {
    for (int col = 0; col < LED_COL; col++) {
      // if any single cell is alive, then the colony is not extinct
      if (future_state[row][col][CELL_STATUS] == 1) {
        le = false;
      }
    }
  }

  return(le);
}

// Iterate through matrix and see if the colony is static
// NOTE: does not detect repeating patterns such as propeller, etc.
bool life_static()  {
  bool stat = true;

  for (int row = 0; row < LED_ROW; row++) {
    for (int col = 0; col < LED_COL; col++) {
      // in any case where the future state doesn't equal the current state, then the colony is not static
      if (future_state[row][col][CELL_STATUS] != current_state[row][col][CELL_STATUS]) {
        stat = false;
      }
    }
  }

  return(stat);
}

// Creates a random starting position
void random_start() {
  // Clear the matrix
  for (int row = 0; row < LED_ROW; row++) {
    for (int col = 0; col < LED_COL; col++) {
      current_state[row][col][CELL_STATUS] = 0;
      current_state[row][col][CELL_GEN] = 0;
    }
  }
   
  // Randomly mark approximately 1/2 of the pixels as alive
  for (int rnd = 0; rnd <= (LED_COUNT / 2); rnd++) {
    int row = random(0,LED_ROW);
    int col = random(0,LED_COL);
    
    current_state[row][col][CELL_STATUS] = 1;
    current_state[row][col][CELL_GEN] = 1;
  }
  
  // Slowly bring up brightness
  for (int brt = 0; brt <= STRIP_BRIGHT; brt++) {
    strip.setBrightness(brt); 
    matrix_show();
    delay(100);
    }
    
delay(1000);
}

// Show the current matrix
void matrix_show()  {
  strip.clear();  // clear the display
  
  // turn on LEDs based on the matrix
  for (int row = 0; row < LED_ROW; row++) {
    for (int col = 0; col < LED_COL; col++) {
      // get the state for each LED and turn on/off
      // if the cell is dead, turn off
      if  (current_state[row][col][CELL_STATUS] == 0)  {
        strip.setPixelColor(pixel_reference[row][col],0,0,0);
      }
      // if it's first generation, make green
      else if (current_state[row][col][CELL_GEN] == 1)  {
        strip.setPixelColor(pixel_reference[row][col],0,255,0);
      }
      //else it's more than 1 generation, turn red
      else {
        strip.setPixelColor(pixel_reference[row][col],255,0,0);
      }
    }
  }
  strip.show();
}