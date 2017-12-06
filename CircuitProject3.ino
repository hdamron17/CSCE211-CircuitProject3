#include <dht11.h>

//Must modify IP/network settings depending on computer
#include <SPI.h>
#include <Ethernet2.h>

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
static long DEBOUNCE_wait = 50;

static long lastTime = 0;
static long wait_AMOUNT = 2000;


// Ethernet settings
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xE5, 0xF2 };

IPAddress ip( 10, 90, 5, 61 );
//IPAddress ip( 129, 252, 11, 16 );

//IPAddress gateway( 10, 90, 5, 1 );
//IPAddress subnet( 255, 255, 255, 192 );

// Email settings
IPAddress server( 66, 228, 43, 14 ); //smtp2go
int port = 2525; //smtp2go port
EthernetClient client;
char fromemail[] = "csce211_group@cse.sc.edu";
char toemail[] = "hdamron@email.sc.edu";

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

bool debounce() {
  int reading = digitalRead(BUTTON_PIN);
//  Serial.println(reading); //TODO remove
  if (reading == LOW) {
    lastDebounceTime = millis();
    buttonPressed = false;
  }
  
  if (!buttonPressed && (millis() - lastDebounceTime) > DEBOUNCE_wait) {
    buttonState += 1;
    buttonState %= 3; // add 1 mod 3
    buttonPressed = true;
    return true;
  }
  return false;
}

// the setup routine runs once when you press reset:
void setup() {
  DHT11.attach(TEMP_PIN);
  Serial.begin(9600);
  
  Serial.println("Setting up ");
 

  for (int i = BASE; i < BASE + 2 * BITS; ++i) {
    pinMode(i, OUTPUT);
  }

  for (int i = LED_BASE; i < LED_BASE + 3; ++i) {
    pinMode(i, OUTPUT);
  }

  pinMode(BUTTON_PIN, INPUT);

  Ethernet.begin(mac, ip); //, gateway, subnet);
  Serial.print("IP = ");
  Serial.println(Ethernet.localIP());

  wait(1000);
}

// the loop routine runs over and over again forever:
void loop() {
  bool button = debounce();
  if (millis() - lastTime > wait_AMOUNT) {
    int chk = DHT11.read();
    lastTime = millis();
    fahrenheit = DHT11.fahrenheit();
    celcius = DHT11.celcius();
    humidity = DHT11.humidity;
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

  if (button) {
    Serial.println("Sending email");
    sendEmail(emailText(fahrenheit, celcius, humidity));
  }
}

String emailText(int fahrenheit, int celcius, int humidity) {
  return String("Arduino DHT Reading")
    + "\r\n* Temp (Fahrenheit): " + String(fahrenheit)
    + "\r\n* Temp (Celcius): " + String(celcius)
    + "\r\n* Humidity (%): " + String(humidity);
}
 
byte sendEmail(String text) {
  byte thisByte = 0;
  byte respCode;

  wait(1);
  byte statusNum = client.connect(server,port);
  if(statusNum == 1) {
    Serial.println("connected");
  } else {
    Serial.print("connection failed with status: ");
    Serial.println(statusNum);
    return 0;
  }
 
  if(!eRcv()) return 0;
  Serial.println("Sending helo");
 
// change to your public ip
  client.print("helo ");
  client.print(ip);
  client.print("\r\n");
 
  if(!eRcv()) return 0;
  Serial.println("Sending From");
 
// change to your email address (sender)
  client.print("MAIL From: <");
  client.print(fromemail);
  client.print(">\r\n");
 
  if(!eRcv()) return 0;
 
// change to recipient address
  Serial.println("Sending To");
  client.print("RCPT To: <");
  client.print(toemail);
  client.print(">\r\n");
 
  if(!eRcv()) return 0;
 
  Serial.println("Sending DATA");
  client.print("DATA\r\n");
 
  if(!eRcv()) return 0;
 
  Serial.println("Sending email");
 
// change to recipient address
  client.print("To: You <");
  client.print(toemail);
  client.print(">\r\n");
 
// change to your address
  client.print("From: Circuit Project Group <");
  client.print(fromemail);
  client.print(">\r\n");
 
  client.print("Subject: Arduino Email\r\n");
 
  client.print(text);
 
  client.print("\r\n.\r\n");
 
  if(!eRcv()) return 0;
 
  Serial.println("Sending QUIT");
  client.print("QUIT\r\n");
 
  if(!eRcv()) return 0;

  wait(1);
  client.flush();
  client.stop();
  wait(1);
 
  Serial.println("disconnected");
 
  return 1;
}
 
byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;
 
  while(!client.available()) {
    wait(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println("\r\nTimeout");
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
 
  client.println("QUIT");
 
  while(!client.available()) {
    wait(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println("\r\nTimeout");
      return;
    }
  }
 
  while(client.available())
  {
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  client.stop();
 
  Serial.println("disconnected");
}

void wait(long mil) {
  long start = millis();
  while(millis() - start < mil);
}

