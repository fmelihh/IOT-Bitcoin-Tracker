#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

#define LED 2


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

int alertPIN = 2;
const float BTC_THRESHOLD = 88395.44;
String apiUrl = "https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT";
String n8nUrl = "<Your N8N WEBHOOK URL>";

void setup() {
  Serial.begin(9600);
  pinMode(alertPIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 bulunamadı!"));
    for(;;); // Ekran yoksa dur
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();

  Serial.print("Connecting to WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}


void loop() {
  digitalWrite(alertPIN, LOW);
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(apiUrl);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);

      float currency = parseJSON(payload);
      if (currency == -1) {
          delay(10000);
          return;
      }
      displayCurrency(currency);
      if (currency >= BTC_THRESHOLD) {
          Serial.print("Threshold value is exceeded! The threshold value is ");
          Serial.println(BTC_THRESHOLD);

          blinkLED(3, 1000);
          Serial.println("Triggering the n8n flow...");
          triggerN8NFlow();

        } else {
          digitalWrite(alertPIN, LOW);
      }
      Serial.print("Parsed currency record is: ");
      Serial.println(currency);
    } else {
      Serial.println("Error on HTTP Request");
    }
    http.end();
  }
  delay(100000);
}

void triggerN8NFlow() {
    HTTPClient http;
    http.begin(n8nUrl);
    http.GET();
    http.end();
}

void blinkLED(int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(alertPIN, HIGH);
    delay(duration);
    digitalWrite(alertPIN, LOW);
    delay(duration);
  }
}

void displayCurrency(float currency) {
    display.clearDisplay();
    
    display.fillRect(0, 0, 128, 14, WHITE); 
    display.setTextColor(BLACK);            
    display.setFont(NULL);
    display.setCursor(35, 3);
    display.print("BTC/USDT");

    display.setTextColor(WHITE);           
    display.setFont(&FreeMono9pt7b);
    display.setCursor(5, 45);
    display.print("$");
    display.print(currency, 2);

    display.setFont(NULL);
    display.setCursor(0, 56);
    display.print("Live: Binance API");
    
    display.display();
}

float parseJSON(String payload) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (!error) {
    float currency = doc["price"];
    return currency;
  } else {
    Serial.print("JSON Deserialize error!: ");
    Serial.println(error.f_str());
    return -1;
  }
}

