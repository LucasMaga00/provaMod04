#include <WiFi.h>
#include <HTTPClient.h>

// Pin definitions
#define GREEN_LED 2     // Green LED
#define RED_LED 40      // Red LED
#define YELLOW_LED 9    // Yellow LED
#define BUTTON 18       // Push button
#define LDR 4          // Light sensor

// Constants
const int LIGHT_THRESHOLD = 600;  // Threshold for night mode
const int DEBOUNCE_DELAY = 50;    // Debounce time in ms
const int GREEN_TIME = 3000;      // 3 seconds
const int YELLOW_TIME = 2000;     // 2 seconds
const int RED_TIME = 5000;        // 5 seconds

// Global variables
unsigned long lastDebounceTime = 0;
unsigned long lastYellowLedTime = 0;
int lastButtonState = HIGH;
int buttonPressCount = 0;
unsigned long lastButtonTime = 0;

void setup() {
  // Pin configuration
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LDR, INPUT);

  // Initialize all LEDs off
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);

  Serial.begin(9600);

  // WiFi connection
  Serial.print("Connecting to WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void loop() {
  int lightReading = digitalRead(LDR);

  if (lightReading <= LIGHT_THRESHOLD) {
    nightMode();
  } else {
    dayMode();
  }
}

void nightMode() {
  // Turn off other LEDs
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Blink yellow LED every second
  unsigned long currentTime = millis();
  if (currentTime - lastYellowLedTime >= 1000) {
    lastYellowLedTime = currentTime;
    digitalWrite(YELLOW_LED, !digitalRead(YELLOW_LED));
  }
}

void dayMode() {
  static int state = 0;
  static unsigned long previousTime = 0;
  unsigned long currentTime = millis();

  // Read button with debounce
  checkButton();

  switch (state) {
    case 0: // Green
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(RED_LED, LOW);
      if (currentTime - previousTime >= GREEN_TIME) {
        state = 1;
        previousTime = currentTime;
      }
      break;

    case 1: // Yellow
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(YELLOW_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      if (currentTime - previousTime >= YELLOW_TIME) {
        state = 2;
        previousTime = currentTime;
      }
      break;

    case 2: // Red
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      if (currentTime - previousTime >= RED_TIME) {
        if (buttonPressCount >= 3) {
          sendAlert();
          buttonPressCount = 0;
        }
        state = 0;
        previousTime = currentTime;
      }
      break;
  }
}

void checkButton() {
  static int buttonState;
  int reading = digitalRead(BUTTON);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        if (digitalRead(RED_LED)) {
          buttonPressCount++;
          if (buttonPressCount == 1) {
            delay(1000); // Wait 1 second after first press
            dayMode();   // Force change to green
          }
        }
      }
    }
  }

  lastButtonState = reading;
}

void sendAlert() {
  HTTPClient http;
  http.begin("http://www.google.com.br/");
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}