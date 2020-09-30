#include "scrc_wifi.h"

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <WiFiUdp.h>


const char* ssid = MAIN_SSID;                      // your network SSID (name)
const char* password = MAIN_PASS;                      // your network password

static bool nw_connected = false;

void nw_setup()
{
  //WiFi.mode(WIFI_OFF);                           //Prevents reconnection issue (taking too long to connect)
  //WiFi.mode(WIFI_STA);                           //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
}

// timeout in milliseconds
bool nw_start(int timeout, int retry_delay)
{
  static long int last_ms = millis();
  
  Serial.print("last_ms: " +  last_ms);
  Serial.print("timeout: " +  timeout);
  Serial.print("retry_delay: " +  retry_delay);

  Serial.println("nw_start...");
  if (nw_connected) {
    //If already connected show IP address in serial monitor
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return nw_connected;
  }

  Serial.println("Establishing connection to WiFi...");
  // WiFi.mode(WIFI_OFF);                           //Prevents reconnection issue (taking too long to connect)
  WiFi.mode(WIFI_STA);                           //Only Station No AP, This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password);                        //Connect to your WiFi router


  int attempt = 0;
  bool error = false;

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {   
    if ((millis() - last_ms) > timeout){
      Serial.print("last_ms: ");
      Serial.println(last_ms);
      Serial.print(", current_ms: ");
      Serial.println(millis());
      Serial.println(", timeout: " + timeout);
      
      Serial.println("timed out!");
      error = true;
      break;
    }
    Serial.print(".");
    delay(retry_delay);
  }

  if (error)
    nw_connected = false;
  else
    nw_connected = true;

  if (nw_connected) {
    //If connection successful show IP address in serial monitor
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.print("Unable to connect!!!");
  }

  return nw_connected;
}

void nw_stop() {
  if (nw_connected){
    WiFi.disconnect();
    nw_connected = false;
  }
}
