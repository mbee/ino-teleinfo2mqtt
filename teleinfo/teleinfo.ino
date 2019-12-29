#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <ArduinoLowPower.h>
#include <ECCX08.h>

#include "arduino_secrets.h" 

#define startFrame 0x02
#define endFrame 0x03
#define PIN_KEEPAWAKE 0
#define DEEP_SLEEP_DURATION 10 * 60 *1000 


const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char mqtt_user[] = SECRET_MQTT_USER;
const char mqtt_pass[] = SECRET_MQTT_PASS;
const char broker[] = "192.168.1.6";
const int port = 1883;

const int MAX_LINE_SIZE = 30;
const int MAX_FRAME_SIZE = 30;

WiFiClient net;
MqttClient mqttClient(net);

char line[MAX_LINE_SIZE];
char frame[MAX_FRAME_SIZE][MAX_LINE_SIZE];
bool DEBUG = false;

void setup() {
  pinMode(PIN_KEEPAWAKE, INPUT_PULLDOWN);
  if (DEBUG) {
    Serial.begin(115200);
    while (!Serial);
  }
  Serial1.begin(1200, SERIAL_7E1);
}

// A line is valid when the checksum is ok
bool isValidLine(char *line, int length) {
  int expectedChecksum = line[length - 1];
  int checksum = 0;
  for (int i = 0; i < length - 2; i++) {
    checksum += line[i];
  }
  checksum = ((checksum % 256) & 63) + 32;
  return expectedChecksum == checksum;
}

// A frame is valid when it begins with ADCO
bool isValidFrame() {
  return !strncmp(frame[0], "ADCO", 4);
}

// The process will read the serial, build the frame and send it to the mqtt client.
void process() {
  int currentCharPos = 0;
  int currentLinePos = 0;
  char charIn = 0;
  while (charIn != startFrame) {
    charIn = Serial1.read() & 0x7F;
  }
  DEBUG && Serial.println("new frame");
  while (charIn != endFrame) {
    if (Serial1.available()) {
      charIn = Serial1.read() & 0x7F;
      if (currentCharPos >= MAX_LINE_SIZE || currentLinePos >= MAX_FRAME_SIZE) {
        continue;
      }
      if (charIn != endFrame) {
        line[currentCharPos++] = charIn;        
        if (charIn == '\n') {
          line[currentCharPos - 1] = 0;
          DEBUG && Serial.println(line);
          if (isValidLine(line, currentCharPos - 1)) {
            strncpy(frame[currentLinePos], line, currentCharPos - 3);
            frame[currentLinePos][currentCharPos - 3] = 0;
            currentLinePos++;
          }
          currentCharPos = 0;
        }
      }
    }
  }
  DEBUG && Serial.println("end frame");
  DEBUG && Serial.print("Nb lines for this frame = ");
  DEBUG && Serial.println(currentLinePos);
  if (currentLinePos > 0 && isValidFrame()) {
    String result = "{";
    for (int i = 0; i < currentLinePos; i++) {
      char *currentLine = frame[i];
      char *label = currentLine;
      char *value;
      int pos = 0;
      while (currentLine[pos] != 32) { pos++; }
      currentLine[pos] = 0;
      value = &currentLine[pos + 1];
      result += "\"";
      result += label;
      result += "\":\"";
      result += value;
      result += "\"";
      if (i < currentLinePos - 1) {
        result += ", ";
      }
    }
    result += "}";
    mqttClient.beginMessage("teleinfo/trame", result.length(), false, 0, false);
    mqttClient.print(result);
    mqttClient.endMessage();
    delay(1000); // to let the wifi send the data
    DEBUG && Serial.println(result);
  }
}

// A loop does:
// - connect to the wifi
// - connect to mqtt
// - send 4 times the teleinfo to mqtt
// - disconnect everything
// - deep sleep or delay depending on the level of pin "0"
void loop() {
  int status = WL_IDLE_STATUS;
  DEBUG && Serial.println("new loop");
  DEBUG && Serial.print("Attempting to connect to WPA SSID: ");
  DEBUG && Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, 0, pass);
    DEBUG && Serial.print(".");
    delay(100);
  }
  DEBUG && Serial.println("\nConnected to the network");
  DEBUG && Serial.println();

  DEBUG && Serial.print("Attempting to connect to the MQTT broker: ");
  DEBUG && Serial.println(broker);

  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);
  if (!mqttClient.connect(broker, port)) {
    DEBUG && Serial.print("MQTT connection failed! Error code = ");
    DEBUG && Serial.println(mqttClient.connectError());
  } else {
    DEBUG && Serial.println("Connected to the MQTT broker");
    DEBUG && Serial.println();
  
    // Let's empty the buffer filled while we were sleeping
    while(Serial1.available()) {
      Serial1.read();
    }
  
    // Some weird errors occured (incomplete frames)
    // So four times should be enough to get at least one complete frame
    for (int i = 0; i < 4; i++) {
      // let's send it four times in a row
      process();
    }
  }

  // let's rest for a while
  Serial.end();
  mqttClient.stop();
  WiFi.disconnect();
  WiFi.end();
  if (!digitalRead(PIN_KEEPAWAKE)) {
    ECCX08.begin();
    ECCX08.end();     // power down ECC508
    LowPower.deepSleep(DEEP_SLEEP_DURATION);
  } else {
    delay(10000);
  }

}
