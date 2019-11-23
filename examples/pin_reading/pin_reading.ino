#include <MultiButtons.h>

int buffer[5];
int reading, sum, avg;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println();
}

void loop() {
  static int i = 0;
  reading = MultiButtons::printReading(35);
  if (reading > 0) {
    buffer[i] = reading;
    if (i == 4) {
      for (int j = 0; j < 5; j++) {
        sum += buffer[j];
      }
      avg = sum / 5;
      Serial.print("Avarage Reading: ");
      Serial.println(avg);
    }
    i++;
    if (i > 4) {
      i = 0;
      sum = 0;
    }
  }
  delay(200);
}
