#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define BOTtoken "5814744392:AAECJ7G2NAhYMusaGJF44VJGl7SsLsx1YIA"
#define CHAT_ID "1148159071"

const char* ssid = "KUKIL BORGOHAIN";
const char* password = "9101124506";

X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 10;
bool ledState = HIGH;


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start"){
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "switch_on to turn GPIO ON \n";
      welcome += "switch_off to turn GPIO OFF \n";
      welcome += "/state to request current GPIO state \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if ((text == "switch on")||(text == "start preparing")) {
      bot.sendMessage(chat_id, "Power ON", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "switch off") {
      bot.sendMessage(chat_id, "Power OFF", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "state") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "Power is OFF", "");
      }
      else{
        bot.sendMessage(chat_id, "Power is ON", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
