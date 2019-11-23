#include <MultiButtons.h>

const int btnPin = 35;

// Declare voltage ranges for each button
int voltageRanges[][2] = {
  {3000,3260},
  {2200,2450},
  {1400,1620},
  {600,810}
};
int btnCount = sizeof(voltageRanges)/sizeof(voltageRanges[0]);
// Must declare buttonHandler callback function and used variables 
// before generate MultiButtons object
void buttonHandler (MultiButtons *mb, int btnIndex) {
  /*
   * NOTE: you may call method in MultiButtons object by 'mb->theMethod();'
   */
  int btnID = btnIndex + 1;
  Serial.print("Button pressed: ");
  Serial.println(btnID);
}
MultiButtons mb(btnPin, btnCount, voltageRanges, buttonHandler, 4095, BTN_TRIGGER_EDGE_PRESS);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println();
  
  mb.begin(); // Prepare reading button state
  
  Serial.println("Ready.");
}

void loop() {
  mb.loop(); // Read button state with debouncing
}
