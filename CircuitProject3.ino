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

#include <dht11.h>

// Output pins.
static int output[] = {0, 1, 2, 3};

// ten_second and second digits
static int reading = 0;
static int BITS = 4;
static int BASE = 0;

static int LED_BASE = BASE + 2*BITS;

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
  DHT11.attach(13);
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

/*
   Email client sketch for IDE v1.0.1 and w5100/w5200
   Posted December 2012 by SurferTim
   Must modify IP/network settings depending on computer
*/
 
#include <SPI.h>
#include <Ethernet.h>
 
// this must be unique
byte mac[] = { 0xAA, 0xBB, 0xCC, 0x00, 0x20, 0x17 };  
// change network settings to yours
IPAddress ip(  100, 64, 14, 19 );    
IPAddress gateway( 100, 4, 0, 1 );
IPAddress subnet( 255, 255, 240, 0 );
 
// change server to your email server ip or domain
// IPAddress server( 1, 2, 3, 4 );
char server[] = "smtp-mail.outlook.com";
 
EthernetClient client;
 
void setup()
{
  Serial.begin(9600);
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  delay(2000);
  Serial.println(F("Ready. Press 'e' to send."));
}
 
void loop()
{
  byte inChar;
 
  inChar = Serial.read();
 
  if(inChar == 'e')
  {
      if(sendEmail()) Serial.println(F("Email sent"));
      else Serial.println(F("Email failed"));
  }
}
 
byte sendEmail()
{
  byte thisByte = 0;
  byte respCode;
 
  if(client.connect(server,25) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
 
  if(!eRcv()) return 0;
  Serial.println(F("Sending helo"));
 
// change to your public ip
  client.println(F("helo 162.242.50.250"));
 
  if(!eRcv()) return 0;
  Serial.println(F("Sending From"));
 
// change to your email address (sender)
  client.println(F("MAIL From: <mstjohn@email.sc.edu>"));
 
  if(!eRcv()) return 0;
 
// change to recipient address
  Serial.println(F("Sending To"));
  client.println(F("RCPT To: <michellegstjohn@gmail.com>"));
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending DATA"));
  client.println(F("DATA"));
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending email"));
 
// change to recipient address
  client.println(F("To: You <michellegstjohn@gmail.com>"));
 
// change to your address
  client.println(F("From: Me <mstjohn@email.sc.edu>"));
 
  client.println(F("Subject: Arduino email test\r\n"));
 
  client.println(F("This is from my Arduino!"));
 
  client.println(F("."));
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending QUIT"));
  client.println(F("QUIT"));
 
  if(!eRcv()) return 0;
 
  client.stop();
 
  Serial.println(F("disconnected"));
 
  return 1;
}
 
byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
 
  respCode = client.peek();
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  if(respCode >= '4')
  {
    efail();
    return 0;  
  }
 
  return 1;
}
 
 
void efail()
{
  byte thisByte = 0;
  int loopCount = 0;
 
  client.println(F("QUIT"));
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  client.stop();
 
  Serial.println(F("disconnected"));
}
