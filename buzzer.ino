/*
 NodeMCU-Sending Slack Message after Button press.
 Button press does a reset of the device
 After sending the Slack the Arduino goes in deep sleep
 */
#include <ESP8266WiFi.h>

#define LED 2 //GPIO2 Interne Led auf dem NodeMCU Board
#define BUZZER 5 //GPIO5 as buzzer input

#define SSID "SSID"
#define PASSWORD "WIFIpass"

// for using postMessageToSlack()
const String slack_hook_url = "https://hooks.slack.com/services/xxx/xxx/xxxxxx";
const String slack_icon_url = ":heart_eyes_cat:";
const String slack_message = "Die Katzen haben Futter bekommen.";
const String slack_username = "Katzefudderer";

void setup()
{
  // Serial.begin(115200);
  // Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  // Serial.print("Connecting to wifi...");

  while (WiFi.status() != WL_CONNECTED) {
    // Serial.print(".");
    delay(250);
  }

  // Serial.println();
  // Serial.println(WiFi.localIP());

  pinMode(LED, OUTPUT); // Port aus Ausgang schalten
  pinMode(BUZZER, INPUT_PULLUP); // Taster einlesen

  digitalWrite(LED, LOW); //Led port einschalten
  while (!postMessageToSlack(slack_message))
  {}

  delay(2000);

  digitalWrite(LED, HIGH); //Led port ausschalten

  // go to deepsleep
  ESP.deepSleep(0);
}

void loop()
{
}


bool postMessageToSlack(String msg)
{
  const char* host = "hooks.slack.com";
  // Serial.print("Connecting to ");
  // Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  const int httpsPort = 443;
  if (!client.connect(host, httpsPort)) {
    // Serial.println("Connection failed :-(");
    return false;
  }

  // We now create a URI for the request

  // Serial.print("Posting to URL: ");
  // Serial.println(slack_hook_url);

  String postData="payload={\"link_names\": 1, \"icon_url\": \"" + slack_icon_url + "\", \"username\": \"" + slack_username + "\", \"text\": \"" + msg + "\"}";

  // This will send the request to the server
  client.print(String("POST ") + slack_hook_url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Connection: close" + "\r\n" +
               "Content-Length:" + postData.length() + "\r\n" +
               "\r\n" + postData);
  // Serial.println("Request sent");
  String line = client.readStringUntil('\n');
  // Serial.printf("Response code was: ");
  // Serial.println(line);
  if (line.startsWith("HTTP/1.1 200 OK"))
  {
    return true;
  } else {
    return false;
  }
}
