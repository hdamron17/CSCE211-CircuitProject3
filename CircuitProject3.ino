#include <dht11.h>

// Output pins.
static int output[] = {0, 1, 2, 3};

// ten_second and second digits
static int reading = 0;
static int BITS = 4;
static int BASE = 2; //pins 2 through 9 for both 7-segment displays

static int LEDS = 0;
static int LED_BASE = BASE + 2 * BITS; //LED pins disabled

static int TEMP_PIN = LED_BASE + LEDS; //pin 13 for temperature input

static int BUTTON_PIN = TEMP_PIN + 1; //pin 10 for button input
static int buttonState = 0;
static bool buttonPressed = false;
static long lastDebounceTime = 0;
static long DEBOUNCE_DELAY = 50;

static long lastTime = 0;
static long DELAY_AMOUNT = 2000;

// DHT11 Temperature and Humidity sensor
dht11 DHT11;
static int fahrenheit, celcius, humidity;

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
  show(base, lower);
  show(base + BITS, upper);
}

void color_led(int base, int num) {
  for (int i = base; i < base + 3; i++) {
    digitalWrite(i, LOW);
  }
  if (num >= 70 && num <= 80) {
    digitalWrite(base + 2, HIGH);
  } else if (num >= 65 && num <= 85) {
    digitalWrite(base + 1, HIGH);
  } else {
    digitalWrite(base, HIGH);
  }
}

void debounce() {
  int reading = digitalRead(BUTTON_PIN);
  Serial.println(reading); //TODO remove
  if (reading == LOW) {
    lastDebounceTime = millis();
    buttonPressed = false;
  }
  
  if (!buttonPressed && (millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    buttonState += 1;
    buttonState %= 3; // add 1 mod 3
    buttonPressed = true;
  }
}

// the setup routine runs once when you press reset:
void setup() {
  DHT11.attach(TEMP_PIN);
  Serial.begin(9600);

  for (int i = BASE; i < BASE + 2 * BITS; ++i) {
    Serial.print("Setting up ");
    Serial.println(i);
    pinMode(i, OUTPUT);
  }

  for (int i = LED_BASE; i < LED_BASE + 3; ++i) {
    pinMode(i, OUTPUT);
  }

  pinMode(BUTTON_PIN, INPUT);

  delay(1000);
}

// the loop routine runs over and over again forever:
void loop() {
  debounce();
  if (millis() - lastTime > DELAY_AMOUNT) {
    int chk = DHT11.read();
    lastTime = millis();
    fahrenheit = DHT11.fahrenheit();
    celcius = DHT11.celcius();
    humidity = DHT11.celcius();
  
    Serial.println("Reading");
    Serial.println(chk);
  }
  
    switch(buttonState) {
      case 0:
        reading = fahrenheit;
        break;
      case 1:
        reading = celcius;
        break;
      case 2:
        reading = humidity;
        break;
    }
    
    show2digit(BASE, reading);
  
    color_led(LED_BASE, reading);
    Serial.println(reading);
    
    Serial.println(buttonState);
    Serial.println(buttonPressed);
}
