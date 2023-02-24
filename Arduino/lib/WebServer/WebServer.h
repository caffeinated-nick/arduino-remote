/*==========================================================================
 * Description:
 * The ESP-13 WiFi shield will connect to the Access Point and serve a web page
 * on port 80. Once connected via WiFi, navigate your browser to 10.0.0.245 to
 * access the remote.
 *
 * Acknowledgements:
 * Some of the code used within this sketch was inspired by or adapted from
other notable sources.
 *    Webserver code: adapted from
https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
 *    WiFi scanner code adapted from tablatronix :
https://gist.github.com/tablatronix/497f3b299e0f212fc171ac5662fa7b42
 *    HTTP/1.1 protocols:
https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
 *
 * -------------------------------------------------------------------------
 * LIBRARIES:
 * Libraries used: ESP8266WiFi.h  - installed with Boards Manager :
https://github.com/esp8266/Arduino
 * More information about ESP8266WiFi.h library :
 * https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
 * https://arduino-esp8266.readthedocs.io/en/2.4.2/libraries.html#wifi-esp8266wifi-library
============================================================================ */

#include <ESP8266WiFi.h>

class WebServer {
 private:
  WiFiServer server = server(80);

 public:
  bool isReady = false;

  WebServer(const char ssid[], const char password[]) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    //  Serial.printf("Preparing to connect... %d", connected);
    //  Serial.println();

    log("Connecting...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      // Serial.println(WiFi.status());
    }
    log("\n");

    if (WiFi.status() == WL_CONNECTED) {
      server.begin();
      log("");
      //    log(WiFi.localIP());
      log("Should be accessible at:");
      //    log(WiFi.localIP());
      log("");
      isReady = true;
    } else {
      log("FAILED to connect to Access Point");
      int n = WiFi.scanNetworks();
      for (int i = 0; i < n; i++) {
        // Serial.println(WiFi.SSID(i));
      }
      // noop, isReady already false.
    }
  };

  void listen() {
    WiFiClient client = server.available();
    if (client) {
      log("\n A Client just connected to the server");
      while (client.connected()) {
        if (client.available()) {
          // req code here...
          String clientMessage = client.readStringUntil('\r');
          log("Client message:");
          // Serial.println(clientMessage);

          if (clientMessage.startsWith("GET /li")) {
            log("Toggling both lights...");
            // updateBothLights();
          } else if (clientMessage.startsWith("GET /fa")) {
            log("Setting both fans...");
            transmitter.incrementTVFan();
            transmitter.incrementLoungeFan();
          } else if (clientMessage.startsWith("GET /off")) {
            log("Shutting both switches completely off...");
            // killSwitches();
          } else if (clientMessage.startsWith("GET /tLi")) {
            log("Toggling TV light...");
            transmitter.toggleTVLight();
          } else if (clientMessage.startsWith("GET /tFa")) {
            log("Setting TV fan...");
            transmitter.incrementTVFan();
          } else if (clientMessage.startsWith("GET /lLi")) {
            log("Toggling Lounge light...");
            transmitter.toggleLoungeLight();
          } else if (clientMessage.startsWith("GET /lFa")) {
            log("Toggling Lounge fan...");
            transmitter.incrementLoungeFan();
          } else if (clientMessage.startsWith("GET /reset")) {
            // resend();
          }

          if (clientMessage.startsWith("GET / ")) {
            log("Returning homepage");
            client.println(constructHomepage());
          } else if (clientMessage.startsWith("GET /")) {
            log("Returning AJAX response");
            client.println(constructAjaxResponse());
          }

          if (clientMessage.length() == 1 && clientMessage[0] == '\n') {
            while (client.read() == '\n') {
              log("Reading empty lines from client...");
            }
            break;
          }
        }
      }
      client.stop();
    };
  };
};