#include <Wire.h>

#define SLAVE_ADDR 8

#define TO_CONST_HEX(X, Y, Z) 0x ## X ## Y ## Z
#define TO_VAR_HEX(X, Y, Z) (Z << 8 | Y << 4 | X)

#define MASTER 0
#define SLAVE  1

void arduino_pin_pair_for_coordinates(int x, int y, int z, int * arduino, int * pin) {
  if (!arduino || !pin || x < 0 || y < 0 || z < 0 || x > 2 || y > 2 || z > 2) {
    return;
  }

#define ASSOCIATE(X, Y, Z, ARDUINO, PIN) case TO_CONST_HEX(X, Y, Z): *arduino = ARDUINO; *pin = PIN; break;
  switch (TO_VAR_HEX(x, y, z)) {
      // 0, 0, 0
      ASSOCIATE(0, 0, 0, MASTER, 4);
      ASSOCIATE(0, 1, 0, SLAVE, 11);
      ASSOCIATE(0, 2, 0, SLAVE, 8);
      ASSOCIATE(1, 0, 0, MASTER, 7);
      ASSOCIATE(1, 1, 0, SLAVE, 10);
      ASSOCIATE(1, 2, 0, SLAVE, 5);
      ASSOCIATE(2, 0, 0, MASTER, 10);
      ASSOCIATE(2, 1, 0, SLAVE, 9);
      ASSOCIATE(2, 2, 0, SLAVE, 2);

      // 0, 0, 1
      ASSOCIATE(0, 0, 1, MASTER, 5);
      // Unhandled case
      ASSOCIATE(0, 1, 1, -1, -1);
      ASSOCIATE(0, 2, 1, SLAVE, 7);
      ASSOCIATE(1, 0, 1, MASTER, 8);
      ASSOCIATE(1, 1, 1, SLAVE, 13);
      ASSOCIATE(1, 2, 1, SLAVE, 4);
      ASSOCIATE(2, 0, 1, MASTER, 11);
      ASSOCIATE(2, 1, 1, SLAVE, 12);
      ASSOCIATE(2, 2, 1, SLAVE, 15);

      // 0, 0, 2
      ASSOCIATE(0, 0, 2, MASTER, 6);
      ASSOCIATE(0, 1, 2, SLAVE, 3);
      ASSOCIATE(0, 2, 2, SLAVE, 6);
      ASSOCIATE(1, 0, 2, MASTER, 9);
      ASSOCIATE(1, 1, 2, SLAVE, 2);
      ASSOCIATE(1, 2, 2, SLAVE, 3);
      ASSOCIATE(2, 0, 2, MASTER, 12);
      ASSOCIATE(2, 1, 2, MASTER, 15);
      ASSOCIATE(2, 2, 2, SLAVE, 14);
  }

#undef ASSOCIATE
}

void print_arduino_pin_pair(int x, int y, int z, int arduino, int pin) {
  Serial.print("("); Serial.print(x); Serial.print(", ");
  Serial.print(y); Serial.print(", "); Serial.print(z);
  Serial.print("): ");
  const char* device_name = "slave";
  if (arduino == MASTER) {
    device_name = "master";
  }
  Serial.print(device_name); Serial.print(" pin "); Serial.println(pin);
}

void setup() {
  Wire.begin(SLAVE_ADDR);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  for (int i = 0; i < 15; i++) {
    pinMode(i, OUTPUT);
  }
  Serial.println("Slave setup done");
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  Serial.println("receiveEvent");
  byte cmd = Wire.read();
  int on = cmd & (1 << 7);
  int led = cmd & ~(1 << 7);
  Serial.print(cmd);
  Serial.print(", "); Serial.print(on); Serial.print(", ");
  Serial.print(led);
  Serial.print(" SLAVE led ");
  Serial.print(led);
  const char* stat = " off";
  if (on) {
    stat = " on";
  }
  Serial.println(stat);

  if (on) {
    digitalWrite(led, HIGH);
  }
  else {
    digitalWrite(led, LOW);
  }
}
