#include <Streaming.h>
#include <Servo.h>

const int BASE_PIN = 2;
const int LOWER_PIN = 3;
const int UPPER_PIN = 4;

Servo base, lower, upper;

const int BASE_MIN = 0;
const int LOWER_MIN = 0;
const int UPPER_MIN = 0;

const int BASE_MAX = 180;
const int LOWER_MAX = 180;
const int UPPER_MAX = 180;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(1); // wait for at most 1ms for serial data

  base.attach(BASE_PIN);
  lower.attach(LOWER_PIN);
  upper.attach(UPPER_PIN);
}

void loop() {
  if (Serial.available()) {
    String type = Serial.readStringUntil(' ');
    type.trim();

    if (type == "?") {
      Serial  << "= "
              << BASE_MIN   << " "
              << LOWER_MIN  << " "
              << UPPER_MIN  << " "
              << BASE_MAX   << " "
              << LOWER_MAX  << " "
              << UPPER_MAX  << endl;
    }

    if (type == ">") {
      int base_value = Serial.parseInt();
      int lower_value = Serial.parseInt();
      int upper_value = Serial.parseInt();

      base.write(base_value);
      lower.write(lower_value);
      upper.write(upper_value);

      Serial << "<" << endl;
    }
  }
}
