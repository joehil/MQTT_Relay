#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "<ssid>";
const char* password = "<password>";
const char* mqtt_server = "<server>";

const char* clientId = "Relay02";
const int relay_pin1 = 12;
const int relay_pin2 = 13;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char timestring[50];
char msg[50];
int status1 = 2;
int status2 = 2;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  if (payload[0] == '1') {
    Serial.println("Action triggered 1");
    digitalWrite(relay_pin1, LOW);
    status1 = 1;
  }
  if (payload[0] == '2') {
    Serial.println("Action triggered 2");
    digitalWrite(relay_pin1, HIGH);
    status1 = 2;
  }
  if (payload[0] == '3') {
    Serial.println("Action triggered 3");
    if (status1 == 1) {
      digitalWrite(relay_pin1, HIGH);
      status1 = 2;
    }
    else {
      digitalWrite(relay_pin1, LOW);
      status1 = 1;
    }
  }
  if (payload[0] == '4') {
    Serial.println("Action triggered 4");
    digitalWrite(relay_pin1, LOW);
    delay(1000);
    digitalWrite(relay_pin1, HIGH);
    status1 = 2;
  }
  if (payload[0] == '5') {
    Serial.println("Action triggered 5");
    digitalWrite(relay_pin2, LOW);
    status2 = 1;
  }
  if (payload[0] == '6') {
    Serial.println("Action triggered 6");
    digitalWrite(relay_pin2, HIGH);
    status2 = 2;
  }
  if (payload[0] == '7') {
    Serial.println("Action triggered 7");
    if (status2 == 1) {
      digitalWrite(relay_pin2, HIGH);
      status2 = 2;
    }
    else {
      digitalWrite(relay_pin2, LOW);
      status2 = 1;
    }
  }
  if (payload[0] == '8') {
    Serial.println("Action triggered 8");
    digitalWrite(relay_pin2, LOW);
    delay(1000);
    digitalWrite(relay_pin2, HIGH);
    status2 = 2;
  }
  if (payload[0] == 'A') {
    Serial.println("Action triggered A");
    digitalWrite(relay_pin1, LOW);
    status1 = 1;
    digitalWrite(relay_pin2, LOW);
    status2 = 1;
  }
  if (payload[0] == 'B') {
    Serial.println("Action triggered B");
    digitalWrite(relay_pin1, HIGH);
    status1 = 2;
    digitalWrite(relay_pin2, HIGH);
    status2 = 2;
  }
  if (payload[0] == 'C') {
    Serial.println("Action triggered C");
    if (status1 == 1) {
      digitalWrite(relay_pin1, HIGH);
      status1 = 2;
      digitalWrite(relay_pin2, HIGH);
      status2 = 2;
    }
    else {
      digitalWrite(relay_pin1, LOW);
      status1 = 1;
      digitalWrite(relay_pin2, LOW);
      status2 = 1;
    }
  }
  if (payload[0] == 'D') {
    Serial.println("Action triggered D");
    digitalWrite(relay_pin1, LOW);
    digitalWrite(relay_pin2, LOW);
    delay(1000);
    digitalWrite(relay_pin1, HIGH);
    digitalWrite(relay_pin2, HIGH);
    status2 = 2;
  }
}

char* TSystemUptime() {
  long millisecs = millis();
  int systemUpTimeMn = int((millisecs / (1000 * 60)) % 60);
  int systemUpTimeHr = int((millisecs / (1000 * 60 * 60)) % 24);
  int systemUpTimeDy = int((millisecs / (1000 * 60 * 60 * 24)) % 365);
  sprintf(timestring,"%d days %02d:%02d:%02d", systemUpTimeDy, systemUpTimeHr, systemUpTimeMn, 0);
  return timestring;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      strcpy(msg,clientId);
      strcat(msg,"/outTopic/uptime");
      client.publish(msg, TSystemUptime());
      strcpy(msg,clientId);
      strcat(msg,"/outTopic/IP");
      client.publish(msg, WiFi.localIP().toString().c_str());
      // ... and resubscribe
      strcpy(msg,clientId);
      strcat(msg,"/inTopic");
      client.subscribe(msg);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(relay_pin1, OUTPUT);     // Initialize the relay_pin pin as an output
  pinMode(relay_pin2, OUTPUT);     // Initialize the relay_pin pin as an output
  digitalWrite(relay_pin1, HIGH);
  digitalWrite(relay_pin2, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 300000) {
    if (WiFi.status() != WL_CONNECTED){
      ESP.restart();
    }
    lastMsg = now;
    Serial.print("Publish message: ");
    Serial.println(TSystemUptime());
    strcpy(msg,clientId);
    strcat(msg,"/outTopic/uptime");
    client.publish(msg, TSystemUptime());
  }
}
