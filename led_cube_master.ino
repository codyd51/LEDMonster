#include <Wire.h>

#define TO_CONST_HEX(X, Y, Z) 0x ## Z ## Y ## X
#define TO_VAR_HEX(X, Y, Z) (Z << 8 | Y << 4 | X)

#define MASTER 0
#define SLAVE  1

#define LED_ON  1
#define LED_OFF 0

#define SLAVE_ADDR 8
#define MASTER_ADDR 4

#define STEP_DELAY 20

void arduino_pin_pair_for_coordinates(int x, int y, int z, int * arduino, int * pin) {
  if (!arduino || !pin || x < 0 || y < 0 || z < 0 || x > 2 || y > 2 || z > 2) {
    return;
  }

#define ASSOCIATE(X, Y, Z, ARDUINO, PIN) case TO_CONST_HEX(X, Y, Z): *arduino = ARDUINO; *pin = PIN; break;
  switch (TO_VAR_HEX(x, y, z)) {
      ASSOCIATE(0, 0, 0, SLAVE, 10);
      ASSOCIATE(0, 0, 1, SLAVE, 7);
      ASSOCIATE(0, 0, 2, SLAVE, 4);
      ASSOCIATE(0, 1, 0, MASTER, 9);
      ASSOCIATE(0, 1, 1, MASTER, 10);
      ASSOCIATE(0, 1, 2, MASTER, 11);
      ASSOCIATE(0, 2, 0, MASTER, 2);
      ASSOCIATE(0, 2, 1, MASTER, 5);
      ASSOCIATE(0, 2, 2, MASTER, 8);
      ASSOCIATE(1, 0, 0, SLAVE, 11);
      ASSOCIATE(1, 0, 1, SLAVE, 8);
      ASSOCIATE(1, 0, 2, SLAVE, 5);
      ASSOCIATE(1, 1, 0, MASTER, 12);
      ASSOCIATE(1, 1, 1, MASTER, 13);
      ASSOCIATE(1, 2, 0, MASTER, 15);
      ASSOCIATE(1, 2, 1, MASTER, 4);
      ASSOCIATE(1, 2, 2, MASTER, 7);
      ASSOCIATE(2, 0, 0, SLAVE, 12);
      ASSOCIATE(2, 0, 1, SLAVE, 9);
      ASSOCIATE(2, 0, 2, SLAVE, 6);
      ASSOCIATE(2, 1, 0, SLAVE, 15);
      ASSOCIATE(2, 1, 1, SLAVE, 2);
      ASSOCIATE(2, 1, 2, SLAVE, 3);
      ASSOCIATE(2, 2, 0, MASTER, 14);
      ASSOCIATE(2, 2, 1, MASTER, 3);
      ASSOCIATE(2, 2, 2, MASTER, 6);
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
  Wire.begin(4);
  Serial.begin(9600);

  for (int i = 2; i < 16; i++) {
    pinMode(i, OUTPUT);
  }
}

void send_slave_cmd(int on, int pin) {
  byte cmd = (on << 7) | pin;
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(cmd);
  Wire.endTransmission();
}

void send_master_cmd(int on, int pin) {
  int state = LOW;
  if (on) state = HIGH;
  digitalWrite(pin, state);
}

void led_command(int x, int y, int z, int on) {
  int arduino, pin;
  arduino_pin_pair_for_coordinates(x, y, z, &arduino, &pin);
  if (arduino == SLAVE) {
    send_slave_cmd(on, pin);
  }
  else {
    send_master_cmd(on, pin);
  }
}

int real_led_map[3][3][3] = {0};
int led_map[3][3][3] = {0};

int commit_leds() {
  for (int x = 0; x <= 2; x++) {
    for (int y = 0; y <= 2; y++) {
      for (int z = 0; z <= 2; z++) {
        if (led_map[x][y][z]) {
          if (real_led_map[x][y][z]) {
            continue;
          }
          else {
            real_led_map[x][y][z] = 1;
            led_command(x, y, z, LED_ON);
          }
        }
        else {
          if (!real_led_map[x][y][z]) {
            continue;
          }
          else {
            real_led_map[x][y][z] = 0;
            led_command(x, y, z, LED_OFF);
          }
        }
      }
    }
  }
}

int mark_led(int x, int y, int z, int on) {
  if (on) {
    led_map[x][y][z] = 1;
  }
  else {
    led_map[x][y][z] = 0;
  }
}

void led_delay(int ms) {
  commit_leds();
  delay(ms);
}

void mark_and_wait(int x, int y, int z, int on) {
  mark_led(x, y, z, on);
  led_delay(STEP_DELAY);
}

void all_leds_off() {
  for (int x = 0; x <= 2; x++) {
    for (int y = 0; y <= 2; y++) {
      for (int z = 0; z <= 2; z++) {
        mark_led(x, y, z, LED_OFF);
      }
    }
  }
}

void all_leds_on() {
  for (int x = 0; x <= 2; x++) {
    for (int y = 0; y <= 2; y++) {
      for (int z = 0; z <= 2; z++) {
        mark_led(x, y, z, LED_ON);
      }
    }
  }
}

void animation1() {
  all_leds_off();
  led_delay(500);
  mark_led(0, 0, 0, LED_ON);
  led_delay(STEP_DELAY);
  for (int x = 0; x <= 1; x++) {
    for (int y = 0; y <= 1; y++) {
      for (int z = 0; z <= 1; z++) {
        mark_led(x, y, z, LED_ON);
      }
    }
  }
  led_delay(STEP_DELAY);
  all_leds_on();
  led_delay(STEP_DELAY);
  for (int x = 0; x <= 2; x++) {
    for (int y = 0; y <= 2; y++) {
      for (int z = 0; z <= 2; z++) {
        if (x != 2 && y != 2 && z != 2) {
          continue;
        }
        mark_led(x, y, z, LED_OFF);
      }
    }
  }
  led_delay(STEP_DELAY);
  for (int x = 0; x <= 2; x++) {
    for (int y = 0; y <= 2; y++) {
      for (int z = 0; z <= 2; z++) {
        if (x == 0 && y == 0 && z == 0) {
          continue;
        }
        mark_led(x, y, z, LED_OFF);
      }
    }
  }
  led_delay(STEP_DELAY);
}

void animation3() {
  for (int y = 0; y <= 2; y++) {
    mark_led(1, y, 0, LED_ON);
    mark_led(0, y, 1, LED_ON);
    mark_led(1, y, 2, LED_ON);
    mark_led(2, y, 1, LED_ON);
  }
  led_delay(STEP_DELAY*4);
  all_leds_off();
  led_delay(STEP_DELAY*4);
  for (int y = 0; y <= 2; y++) {
    mark_led(0, y, 0, LED_ON);
    mark_led(2, y, 0, LED_ON);
    mark_led(2, y, 2, LED_ON);
    mark_led(0, y, 2, LED_ON);
  }
  led_delay(STEP_DELAY*4);
  all_leds_off();
  led_delay(STEP_DELAY*4);
}

void animation2() {
  for (int i = 0; i < 2; i++) {
    all_leds_off();
    led_delay(STEP_DELAY);
    all_leds_on();
    led_delay(STEP_DELAY);
  }
}

void animation4() {
  for (int y = 0; y <= 2; y++) {
    for (int x = 0; x <= 2; x++) {
      for (int z = 0; z <= 2; z++) {
        mark_and_wait(x, y, z, LED_ON);
      }
    }
  }
  for (int y = 2; y >= 0; y--) {
    for (int x = 2; x >= 0; x--) {
      for (int z = 2; z >= 0; z--) {
        mark_and_wait(x, y, z, LED_OFF);
      }
    }
  }
}

void animation5() {
  all_leds_on();
  led_delay(1000);
  for (int y = 0; y <= 2; y++) {
    for (int x = 0; x <= 2; x++) {
      for (int z = 0; z <= 2; z++) {
        int tally = 0;
        if (x == 1) tally++;
        if (y == 1) tally++;
        if (z == 1) tally++;
        if (tally != 2) continue;
        mark_led(x, y, z, LED_OFF);
      }
    }
  }
  led_delay(1000);
  all_leds_off();
  led_delay(1000);
}


void animation6() {
  all_leds_on();
  led_delay(500);
  mark_and_wait(1, 1, 1, LED_OFF);
  for (int y = 0; y <= 2; y++) {
    for (int x = 0; x <= 2; x++) {
      for (int z = 0; z <= 2; z++) {
        int tally = 0;
        if (x != 1) tally++;
        if (y != 1) tally++;
        if (z != 1) tally++;
        if (tally == 2) continue;
        mark_led(x, y, z, LED_OFF);
      }
    }
  }
  led_delay(500);
  all_leds_off();
  led_delay(500);
}

void loop() {
  int animations[] = {animation4, animation2, animation3};
  //int animations[] = {animation6};
  for (int animation = 0; animation < sizeof(animations) / sizeof(animations[0]); animation++) {
    Serial.print("Animation ");Serial.println(animation);
    int (*func)(void) = animations[animation];
    for (int i = 0; i < 2; i++) {
      func();
    }
  }
  Serial.println("Rotation done");
}
