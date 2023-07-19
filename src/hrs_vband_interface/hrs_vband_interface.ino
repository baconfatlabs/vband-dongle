//***********************************************************************************************************************
// Author: Eric Wolf, N5EBW <eric.n5ebw@gmail.com>
// Date: 07/19/2023
// Original inspiration by OZ1JHM 05/02/2021
// Description: This program is for use with Ham Radio Solutions VBand Interface.  It interprets open/close signals from a paddle,
//    straight key, or keyer input into Left Control/Right Control key presses and releases that VBand recognizes.

// Prerequisites: In Arduino, search for "keyboard" in the library manager and install.

// Instructions: 
//    Paddle: Don't connect MODE_PIN to GND.
//    Keyer Circuit: Connect MODE_PIN to GND.
//    Straight Key: Connect MODE_PIN to GND.
// *** This is really not even necessary if you are connecting to VBand unless you have something miswired I don't think.
//     If you put VBand in straight key mode and leave this as is, as long as you wire your output to ring, this will still
//     work without modification in configuration.

// Notes: Remember to set the settings on https://hamradio.solutions/vband

#include <Keyboard.h>

// Define pins on the Arduino that map to the device this is installed on.
# define DIH_PIN 3
# define DAH_PIN 4
# define MODE_PIN 2

// Track the historic state of both pins
bool DIH_PRESSED;
bool DAH_PRESSED;

int reaction_delay;
  
void setup() {
  // Set the pin modes for all three inputs.
  pinMode(DIH_PIN, INPUT_PULLUP);
  pinMode(DAH_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);

  // Initially set the working state of both paddle contacts to unpressed.  The loop will determine and set these based on state changes.
  DIH_PRESSED = false;
  DAH_PRESSED = false;

  // Reaction delay in between scans for crude debouncing.
  // The fastest human reaction time is about 120 milliseconds, this should be plenty fast enough to scan without interfering.
  reaction_delay = 20;

  // Start the keyboard interface where keys will be sent.
  Keyboard.begin();

}

void loop() {
  if(digitalRead(MODE_PIN) == HIGH){
    Mode_keyer();
  }else{
    Mode_key();
  }
}

// Compare the state of last know pin to the current state.  If there is a change, press a key or release it and set the
// new global state for that pin for the next loop run. 
void Mode_keyer(){
  
  if(digitalRead(DIH_PIN) == LOW && !DIH_PRESSED){
    // User has closed the DIH paddle side.  Press the left control key to simulate a dih, set the dih state to pressed.
    Keyboard.press(KEY_LEFT_CTRL);
    DIH_PRESSED = true;
  }

  if(digitalRead(DIH_PIN) == HIGH && DIH_PRESSED){
    // User has opened the DIH paddle side.  Release the left control key, set the dih state to unpressed.
    Keyboard.release(KEY_LEFT_CTRL);
    DIH_PRESSED = false;
  } 

  if(digitalRead(DAH_PIN) == LOW && !DAH_PRESSED){
    // User has closed the DAH paddle side.  Press the right control key to simulate a dah, set the dah state to pressed.
    Keyboard.press(KEY_RIGHT_CTRL);
    DAH_PRESSED = true;
  }

  if(digitalRead(DAH_PIN) == HIGH && DAH_PRESSED){
    // User has opened the DAH paddle side.  Release the right control key, set the dah state to unpressed.
    Keyboard.release(KEY_RIGHT_CTRL);
    DAH_PRESSED = false;
  }
  delay(reaction_delay);
}

// Do the same thing we did for keyer mode but only look at the right ctrl key operation.
// Key can be wired on TIP or RING for this to work as long as one of these pins are pulling low,
// and both are high upon release.
// VBand does not seem as stable with the left control key in this mode.
void Mode_key(){
  if(digitalRead(DIH_PIN) == LOW && !DAH_PRESSED){
    // User has closed the key.  Press the right control key, set the dah state to pressed.
    Keyboard.press(KEY_RIGHT_CTRL);
    DAH_PRESSED = true;
  }
  if(digitalRead(DAH_PIN) == LOW && !DAH_PRESSED){
    // User has closed the key.  Press the right control key, set the dah state to pressed.
    Keyboard.press(KEY_RIGHT_CTRL);
    DAH_PRESSED = true;
  }
  if((digitalRead(DAH_PIN) == HIGH && digitalRead(DIH_PIN) == HIGH) && DAH_PRESSED){
    // User has opened the key.  Release the right control key, set the dah state to unpressed.
    Keyboard.release(KEY_RIGHT_CTRL);
    DAH_PRESSED = false;
  }
  delay(reaction_delay);
}
