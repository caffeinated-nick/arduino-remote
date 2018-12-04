/*==========================================================================
 * Description: 
 * The ESP-13 WiFi shield will connect to the Access Point and serve a web page
 * on port 80. Once connected via WiFi, navigate your browser to 10.0.0.245 to 
 * access the remote.
 * 
 * Acknowledgements:
 * Some of the code used within this sketch was inspired by or adapted from other notable sources.
 *    Webserver code: adapted from https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
 *    WiFi scanner code adapted from tablatronix : https://gist.github.com/tablatronix/497f3b299e0f212fc171ac5662fa7b42
 *    HTTP/1.1 protocols: https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
 *    This tutorial: https://arduinobasics.blogspot.com/2018/09/getting-started-with-keyes-esp-13-wifi.html 
 *    
 * -------------------------------------------------------------------------   
 * LIBRARIES:
 * Libraries used: ESP8266WiFi.h  - installed with Boards Manager : https://github.com/esp8266/Arduino
 * More information about ESP8266WiFi.h library : 
 * https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
 * https://arduino-esp8266.readthedocs.io/en/2.4.2/libraries.html#wifi-esp8266wifi-library
 * https://github.com/sui77/rc-switch
 *                                       
 *                                       
 * GLOBAL VARIABLES:
 * The ssid and password WiFi server which will listen for Clients on Port 80 (eg. Web Browsers).
 * The possible states for the switches to be in (ranging from light off fan off -> light off fan on -> light on fan on etc.)
 ============================================================================ */
#include <ESP8266WiFi.h>
#include <RCSwitch.h>
const char* ssid = "ssid";
const char* password = "password";
WiFiServer server(80);

/** TV Light States, Lounge Light States (see "Fan RF Signals.csv" for more info)
 * TV:
 *  Light off, fan 0-3: 229389, 229472, 229455, 229422 
 *  Light on, fan 0-3: 229505, 229604, 229571, 229538 
 * Lounge:
 *  Light off, fan 0-3: 245775, 245858, 245825, 245792
 *  Light on, fan 0-3: 245891, 245990, 245957, 245924
 *
 */
int states[2][8] = {{229389, 229472, 229455, 229422, 229505, 229604, 229571, 229538}, {245775, 245858, 245825, 245792, 245891, 245990, 245957, 245924}};

// Do we expect the next request to be a superfluous Chrome connection?
bool chromeHoldingRequest = 0;

// This int represents which index in states[][] is currently set.
// e.g. if tvSwitchState is 4 then we send signal[0][4], or 229505
int tvSwitchState = 0;
int loungeSwitchState = 0;

RCSwitch mySwitch = RCSwitch();

/* ==========================================================================
 * SETUP:
 * Begin Serial Communication using a Baud Rate of 115200
 * Configure the ESP-13 WiFi shield into STA mode.
 * Connect to the Access Point.
 * Start the server if successfully connected to the AP.
 ============================================================================ */
void setup() {

  // Transmitter is connected to Arduino Pin #2 
  mySwitch.enableTransmit(2);
  
  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch.setProtocol(6);
  
  // Optional set number of transmission repetitions.
  mySwitch.setRepeatTransmit(15);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  boolean r = WiFi.begin(ssid, password);

  if (r) {
    server.begin();
    Serial.println("");
    Serial.print("Conencted to Access Point: ");
    Serial.println(ssid);
    Serial.println("Should be accessible at 10.0.0.245. If not available double check the config on the router.");
    Serial.println("");
  } else {
    Serial.println("FAILED to connect to Access Point");
    while(1){}
  }
}

/* ==========================================================================
 * LOOP:
 * Start listening for Clients
 * Notify when a Client connects to the server using the Serial Monitor
 * Receive any switch requests.
 * Then write any request messages from the client to the Serial Monitor
 * Wait for the client to send through an empty line, then
 * Serve the constructed HTML page to the client
 * The delay gives the browser some time to receive the HTML page, before
 * the server dicsonnects from the client.
 * Notify when the client has been disconnected.
 ============================================================================ */
void loop() {
  WiFiClient client = server.available();
  if(client){
    Serial.println("\n A Client just connected to the server");
    if(!chromeHoldingRequest) {
      while(client.connected()) {
        if(client.available()){
          String clientMessage = client.readStringUntil('\r');
          Serial.println("Client message:");
          Serial.println(clientMessage);

          if(clientMessage.startsWith("GET /LIGHTS")) {
            Serial.println("Toggling both lights...");
            tvSwitchState = updateLightState(0, tvSwitchState);
            loungeSwitchState = updateLightState(1, loungeSwitchState);
          } else if (clientMessage.startsWith("GET /FANS")) {
            Serial.println("Setting both fans...");
            tvSwitchState = updateFanState(0, tvSwitchState);
            loungeSwitchState = updateFanState(1, loungeSwitchState);
          } else if (clientMessage.startsWith("GET /KILL")) {
            Serial.println("Shutting both switches completely off...");
            killSwitches();
          } else if (clientMessage.startsWith("GET /TVL")) {
            Serial.println("Toggling TV light...");
            tvSwitchState = updateLightState(0, tvSwitchState);
          } else if (clientMessage.startsWith("GET /TVF")) {
            Serial.println("Setting TV fan...");
            tvSwitchState = updateFanState(0, tvSwitchState);
          } else if (clientMessage.startsWith("GET /LOUNGEL")) {
            Serial.println("Toggling Lounge light...");
            loungeSwitchState = updateLightState(1, loungeSwitchState);
          } else if (clientMessage.startsWith("GET /LOUNGEF")) {
            Serial.println("Toggling Lounge fan...");
            loungeSwitchState = updateFanState(1, loungeSwitchState);
          } 
          
          if(clientMessage.length() == 1 && clientMessage[0] =='\n') {
            client.println(constructHTMLpage());
            break;
          }

          if(strstr(clientMessage.c_str(), "Chrome")){
            Serial.println("Request came from Chrome, expect to close next connection manually.");
            chromeHoldingRequest = 1;
          }
        } 
      }
    } else {
      Serial.println("Looks like Chrome maintaining a connection. Disconnecting...");
      chromeHoldingRequest = 0;
    }
    delay(1000);
    client.stop();
    Serial.println("\n The server has disconnected the Client");
  }
}

/* ==========================================================================
 * constructHTMLpage:
 * Construct the necessary HTTP response and HTML to display the buttons and 
 * the current state of each switch.
 ============================================================================ */
String constructHTMLpage(){
  String HTMLpage = String("HTTP/1.1 200 OK\r\n") +
                            "Content-Type: text/html\r\n" +
                            "Connection: close\r\n" +
                            "Refresh: 5\r\n" +
                            "\r\n" +
                            "<!DOCTYPE HTML>";
                            
  HTMLpage = HTMLpage + String("<html>");
  HTMLpage = HTMLpage + String("<head>") +
        "<style>" +
          "button {" +
            "padding: 50px;" +
            "font-size: 30px;" + 
          "}" +
        
          ".inactive {" +
            "background-color: red;" +
          "}" +
      
          ".active {" +
            "background-color: green" +
          "}" +

          ".oneSpeed {" +
            "background-color: orange" +
          "}" +

          ".twoSpeed {" +
            "background-color: yellow" +
          "}" +

          ".blank {" +
            "background-color: white;" +
            "border-style: solid;" +
          "}" +
        "</style>";
  HTMLpage = HTMLpage + String("</head>");
  
  HTMLpage = HTMLpage + String("<body>");
  
  HTMLpage = HTMLpage + String("<br/><br/>");

  HTMLpage = HTMLpage + String("<button onclick=\"window.location.href=\'/LIGHTS\'\" class=\"" + getCombinedLightsBtnClassByState() + "\">Lights</button>");
  HTMLpage = HTMLpage + String("<button onclick=\"window.location.href=\'/TVL\'\" class=\"" + getLightButtonClassByState(0) + "\">TV Light</button>");
  HTMLpage = HTMLpage + String("<button onclick=\"window.location.href=\'/LOUNGEL\'\" class=\"" + getLightButtonClassByState(1) + "\">Lounge Light</button>");
  HTMLpage = HTMLpage + String("<br/><br/>"); 
  HTMLpage = HTMLpage + String("<button onclick=\"window.location.href=\'/FANS\'\" class=\"" + getCombinedFansBtnClassByState() + "\">Fans</button>");
  HTMLpage = HTMLpage + String("<button onclick=\"window.location.href=\'/TVF\'\" class=\"" + getFanButtonClassByState(0) + "\">TV Fan</button>");
  HTMLpage = HTMLpage + String("<button onclick=\"window.location.href=\'/LOUNGEF\'\" class=\"" + getFanButtonClassByState(1) + "\">Lounge Fan</button>"); 
  HTMLpage = HTMLpage + String("<br/><br/>");
  HTMLpage = HTMLpage + String("<button onclick=\"window.location.href=\'/KILL\'\" class=\"kill\">Kill Switch</button>");

  HTMLpage = HTMLpage + String("</body></html>");
  
  HTMLpage = HTMLpage + String("\r\n");

  return HTMLpage;
}

String getCombinedLightsBtnClassByState() {
  if(loungeSwitchState > 3 && tvSwitchState > 3) {
    return "active";
    // For the sake of all that is holy write a damn helper function to determine if a given light is on or off
  } else if(loungeSwitchState > 3 && tvSwitchState < 4 || loungeSwitchState < 4 && tvSwitchState > 3) {
    return "blank"; 
  }  
  return "inactive";
}

String getCombinedFansBtnClassByState() {
  // For the love of god write a help function for if the fans are the same
  if(loungeSwitchState == tvSwitchState || loungeSwitchState == tvSwitchState - 4 || loungeSwitchState == tvSwitchState + 4) {
    if(loungeSwitchState == 0 || loungeSwitchState == 4) {
      return "inactive";
    } 
    return getFanButtonClassByState(0);
  }
  return "blank";
}

/**
 *  Determine the CSS class for a light button based on the state of the associated switch.
 */
String getLightButtonClassByState(bool isLoungeSwitch) {
  if(isLoungeSwitch) {
    if(loungeSwitchState > 3) {
      return "active"; 
    }
  } else {
    if(tvSwitchState > 3) {
      return "active";
    }
  }
  return "inactive"; 
}

/**
 *  Determine the CSS class for a fan button based on the state of the associated switch.
 */
String getFanButtonClassByState(bool isLoungeSwitch) {
  if(isLoungeSwitch) {
    return classFromFanState(loungeSwitchState);
  } else {
    return classFromFanState(tvSwitchState);
  }
  return "inactive";
}

/**
 *  Determine the state of the fan switch (could be high or low depending on the state of the light)
 */
String classFromFanState(int state){
  switch(state){
    case(0):
    case(4): 
      return "inactive";
      break;
    case(1):
    case(5):
      return "oneSpeed";
      break;
    case(2):
    case(6):
      return "twoSpeed";
      break;
    case(3):
    case(7):
      return "active";
      break;
  }
  return "inactive";
}

/**
 * Update the ligth state for a switch, return the new state.
 * If the state is high (4-7) then the light is on already, so turn it off.
 */
int updateLightState(int switchIndex, int switchState) {
  if(switchState < 4) {
    switchState += 4;
  } else {
    switchState -= 4;
  }
  Serial.println("");
  Serial.printf("Setting switch %d to %d...", switchIndex, switchState);
  Serial.println("");
  updateState(switchIndex, switchState);
  return switchState;
}

/**
 * Update the fan state for a switch, return the new state.
 * If the state is 3 or 7 then the fan is at max speed, so set back down to 0/off.
 */
int updateFanState(int switchIndex, int switchState) {
  if(switchState == 3 || switchState == 7){
    switchState -= 3;
  } else {
    switchState++;
  }
  Serial.println("");
  Serial.printf("Setting switch %d to %d", switchIndex, switchState);
  Serial.println("");
  updateState(switchIndex, switchState);
  return switchIndex;
}

/**
 *  Update the state of the FAN itself (i.e. send the updated state to the fan)
 */
void updateState(int index, int switchState){
  sendSignal(states[index][switchState]);
}

void killSwitches() {
  loungeSwitchState = 0;
  tvSwitchState = 0;
  updateState(0, tvSwitchState);
  updateState(1, loungeSwitchState);
}

/**
 *  Send a generic (decimal number) signal to the fan.
 */
void sendSignal(int signal) {
  Serial.printf("Sending signal %d...", signal);
  for(int i = 0; i < 3; i++){
    Serial.printf("Attempt %d...", i);
    mySwitch.send(signal, 21);  
    delay(10);
  }
  Serial.println("");
}