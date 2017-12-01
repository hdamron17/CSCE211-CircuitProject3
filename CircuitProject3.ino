#include <dht11.h>

// Output pins.
static int output[] = {0, 1, 2, 3};

// ten_second and second digits
static int reading = 0;
static int BITS = 4;
static int BASE = 2; //pins 2 through 9 for both 7-segment displays

static int LEDS = 3;
static int LED_BASE = BASE + 2*BITS; //pins 10 through 12 for led displays

static int TEMP_PIN = LED_BASE + LEDS; //pin 13 for temperature input

static int BUTTON_PIN = TEMP_PIN + 1; //TODO is 14 a real pin //pin 14 for button input

// DHT11 Temperature and Humidity sensor
dht11 DHT11;

// 8421 code lookup table.
static int segs_encoding[][4] =
{ {0, 0, 0, 0},
  {0, 0, 0, 1},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 1, 0, 0},
  {0, 1, 0, 1},
  {0, 1, 1, 0},
  {0, 1, 1, 1},
  {1, 0, 0, 0},
  {1, 0, 0, 1}
};

// Outputs an 8421 decimal digit on the output pins from
// base to base + 3 in big endian order.
void show(int base, int num) {
  for (int i = 0; i < BITS; ++i) {
    if (segs_encoding[num][i] == 1) {
      digitalWrite(base + 3 - i , HIGH);
    } else {
      digitalWrite(base + 3 - i , LOW);
    }
  }
}

void show2digit(int base, int num) {
  int below99 = num % 100;
  int upper = below99 / 10;
  int lower = below99 % 10;
  Serial.println("show2digit");
  Serial.println(below99);
  Serial.print(lower);
  Serial.print(" on pin ");
  Serial.println(base);
  Serial.print(upper);
  Serial.print(" on pin ");
  Serial.println(base+BITS);
  show(base, lower);
  show(base+BITS, upper);
}

void color_led(int base, int num) {
  for (int i = base; i < base + 3; i++) {
    digitalWrite(i, LOW);
  }
  if (num >= 70 && num <= 80) {
    digitalWrite(base+2, HIGH);
  } else if (num >= 65 && num <= 85) {
    digitalWrite(base+1, HIGH);
  } else {
    digitalWrite(base, HIGH);
  }
}

// the setup routine runs once when you press reset:
void setup() {
  DHT11.attach(TEMP_PIN);
  Serial.begin(9600);
  
  for (int i = BASE; i < BASE+2*BITS; ++i) {
    Serial.print("Setting up ");
    Serial.println(i);
    pinMode(i, OUTPUT);
  }
  
  for (int i = LED_BASE; i < LED_BASE+3; ++i) {
    pinMode(i, OUTPUT);
  }
  
  delay(1000);
}

// the loop routine runs over and over again forever:
void loop() {
  int chk = DHT11.read();

  reading = DHT11.fahrenheit();
  show2digit(BASE, reading);

  color_led(LED_BASE, reading);
  
  Serial.println("Reading");
  Serial.println(chk);
  Serial.println(reading);
  delay(2000);
}
