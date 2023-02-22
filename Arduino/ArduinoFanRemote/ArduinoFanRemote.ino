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
 *
 * -------------------------------------------------------------------------
 * LIBRARIES:
 * Libraries used: ESP8266WiFi.h  - installed with Boards Manager : https://github.com/esp8266/Arduino
 * More information about ESP8266WiFi.h library :
 * https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
 * https://arduino-esp8266.readthedocs.io/en/2.4.2/libraries.html#wifi-esp8266wifi-library
 *
 *
 * GLOBAL VARIABLES:
 * The ssid and password WiFi server which will listen for Clients on Port 80 (eg. Web Browsers).
 * The possible states for the switches to be in (ranging from light off fan off -> light off fan on -> light on fan on etc.)
 ============================================================================ */
#include <ESP8266WiFi.h>
#include "secrets.h"
#include <FansRadioTransmitter.h>

bool debug = false;

FansRadioTransmitter transmitter = FansRadioTransmitter(2, debug);

WiFiServer server(80);

// Do we expect the next request to be a superfluous Chrome connection?
bool chromeHoldingRequest = 0;

/* ==========================================================================
 * SETUP:
 * Begin Serial Communication using a Baud Rate of 115200
 * Configure the ESP-13 WiFi shield into STA mode.
 * Connect to the Access Point.
 * Start the server if successfully connected to the AP.
 ============================================================================ */

void log(char message[])
{
  if (debug)
  {
    Serial.println(message);
  }
}

void setup()
{

  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  //  Serial.printf("Preparing to connect... %d", connected);
  //  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  log("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    // Serial.println(WiFi.status());
  }
  log("\n");

  if (WiFi.status() == WL_CONNECTED)
  {
    server.begin();
    log("");
//    log(WiFi.localIP());
    log("Should be accessible at:");
//    log(WiFi.localIP());
    log("");
  }
  else
  {
    log("FAILED to connect to Access Point");
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++)
    {
      // Serial.println(WiFi.SSID(i));
    }
    while (1)
    {
    }
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
void loop()
{
  WiFiClient client = server.available();
  if (client)
  {
    log("\n A Client just connected to the server");
    while (client.connected())
    {
      if (client.available())
      {
        String clientMessage = client.readStringUntil('\r');
        log("Client message:");
        // Serial.println(clientMessage);

        if (clientMessage.startsWith("GET /li"))
        {
          log("Toggling both lights...");
          // updateBothLights();
        }
        else if (clientMessage.startsWith("GET /fa"))
        {
          log("Setting both fans...");
          transmitter.incrementTVFan();
          transmitter.incrementLoungeFan();
        }
        else if (clientMessage.startsWith("GET /off"))
        {
          log("Shutting both switches completely off...");
          // killSwitches();
        }
        else if (clientMessage.startsWith("GET /tLi"))
        {
          log("Toggling TV light...");
          transmitter.toggleTVLight();
        }
        else if (clientMessage.startsWith("GET /tFa"))
        {
          log("Setting TV fan...");
          transmitter.incrementTVFan();
        }
        else if (clientMessage.startsWith("GET /lLi"))
        {
          log("Toggling Lounge light...");
          transmitter.toggleLoungeLight();
        }
        else if (clientMessage.startsWith("GET /lFa"))
        {
          log("Toggling Lounge fan...");
          transmitter.incrementLoungeFan();
        }
        else if (clientMessage.startsWith("GET /reset"))
        {
          // resend();
        }

        if (clientMessage.startsWith("GET / "))
        {
          log("Returning homepage");
          client.println(constructHomepage());
        }
        else if (clientMessage.startsWith("GET /"))
        {
          log("Returning AJAX response");
          client.println(constructAjaxResponse());
        }

        if (clientMessage.length() == 1 && clientMessage[0] == '\n')
        {
          while (client.read() == '\n')
          {
            log("Reading empty lines from client...");
          }
          break;
        }
      }
    }
    client.stop();
    log("\n The server has disconnected the Client");
    // Serial.printf("TV switch is now %d, Lounge switch is now %d", tvSwitchState, loungeSwitchState);
    log("");
  }
}

/* ==========================================================================
 * constructHTMLpage:
 * Construct the necessary HTTP response and HTML to display the buttons and
 * the current state of each switch.
 ============================================================================ */
String constructHomepage()
{
  String HTMLpage = String("HTTP/1.1 200 OK\r\n") +
                    "Content-Type: text/html\r\n" +
                    "Connection: close\r\n" +
                    "\r\n" +
                    "<!DOCTYPE HTML>";

  HTMLpage = HTMLpage + String("<html>") +
             "<head>\n" +
             "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>\n" +
             "<link rel=\"icon\" href=\"get-fucked-there's-no-favicon\" type=\"image/x-icon\" />\n" +
             "<style>\n" +
             "html {\n" +
             "height: 100%;\n" +
             "} \n" +
             " " +
             "body {\n" +
             "height: 90%;\n" +
             "margin-top: auto;\n" +
             "margin-bottom: auto;\n" +
             "display: flex;\n" +
             "flex-direction: column;\n" +
             "}\n" +
             " \n" +
             "div {\n" +
             "width: 90%;\n" +
             "margin-left: auto;\n" +
             "margin-right: auto;\n" +
             "display: flex;\n" +
             "justify-content: space-evenly;\n" +
             "flex-grow: 1;\n" +
             "}\n" +
             " \n" +
             "button {\n" +
             "padding: 50px;\n" +
             "font-size: 30px;\n" +
             "margin: 5px; \n" +
             "width: 30%;\n" +
             "border-radius: 15px;\n" +
             "}\n" +
             " \n" +
             ".error {\n" +
             "margin: auto auto;\n" +
             "font-size: 30px;\n" +
             "}\n" +
             " \n" +
             ".inactive {\n" +
             "background-color: #B0BEC5;\n" +
             "}\n" +
             " " +
             ".active {\n" +
             "background-color: #FDD835\n" +
             "}\n" +
             " " +
             ".oneSpeed {\n" +
             "background-color: #FDD835\n" +
             "}\n" +
             " \n" +
             ".twoSpeed {\n" +
             "background-color: #FB8C00\n" +
             "}\n" +
             " \n" +
             ".threeSpeed {\n" +
             "background-color: #E53935\n" +
             "}\n" +
             " " +
             ".kill {\n" +
             "background-color: #263238;\n" +
             "color: white;\n" +
             "border-style: solid;\n" +
             "}\n" +
             ".loading {\n" +
             "animation-duration: 2.5s;\n" +
             "animation-name: spin;\n" +
             "animation-iteration-count: infinite;\n" +
             "animation-fill-mode: forwards;\n" +
             "}\n" +
             "\n" +
             "@keyframes spin {\n" +
             "from {\n" +
             "" +
             "}\n" +

             "10% {\n" +
             "transform: rotate(-5deg);\n" +
             "}\n" +

             "75% {\n" +
             "transform: rotate(1445deg);\n" +
             "}\n" +

             "85% {\n" +
             "transform: rotate(1440deg);\n" +
             "}\n" +

             "100% {\n" +
             "transform: rotate(1440deg);\n" +
             "}\n" +
             "}\n" +
             "</style>\n" +
             "</head>\n" +
             " " +
             "<body>\n" +
             "<script>\n" +
             "baseUrl = \"http://192.168.1.85\";\n" +
             " " +
             "makeRequest = async (endpoint) => {\n" +
             "return $.ajax(baseUrl + \"/\" + endpoint)\n" +
             "}\n" +
             " " +
             "throwError = () => {\n" +
             "document.body.innerHTML = \"<span class='error'>Something went wrong &#x1F62D;<br/><a href='http://192.168.1.85''>Back</a></span>\"\n" +
             "}\n" +
             " \n" +
             "sendMessage = (element) => {\n" +
             "element.className = element.className + \" loading\"\n" +
             "elementText = element.innerHTML;\n" +
             "element.innerHTML = \"Loading...\";\n" +
             "this.makeRequest(element.id).then((response) => {\n" +
             "for(key in response) {\n" +
             "  document.getElementById(key).className = response[key];\n" +
             "}\n" +
             "document.getElementById(\"off\").className = \"kill\";\n" +
             "element.innerHTML = elementText;\n" +
             "}).catch((err) => {\n" +
             "console.log(err)\n" +
             "throwError()\n" +
             "});\n" +
             "}\n" +
             "</script>\n" +
             "<br/><br/><div> \n" +
             "<button id=\"li\" onclick=\"sendMessage(this)\" class=\"" + getCombinedLightsBtnClassByState() + "\">Lights</button>\n" +
             "<button id=\"tLi\" onclick=\"sendMessage(this)\" class=\"" + getLightButtonClassByState(false) + "\">TV Light</button>\n" +
             "<button id=\"lLi\" onclick=\"sendMessage(this)\" class=\"" + getLightButtonClassByState(true) + "\">Lounge Light</button>\n" +
             "</div><br/><br/><div>\n" +
             "<button id=\"fa\" onclick=\"sendMessage(this)\" class=\"" + getCombinedFansBtnClassByState() + "\">Fans</button>\n" +
             "<button id=\"tFa\" onclick=\"sendMessage(this)\" class=\"" + getFanButtonClassByState(false) + "\">TV Fan</button>\n" +
             "<button id=\"lFa\" onclick=\"sendMessage(this)\" class=\"" + getFanButtonClassByState(true) + "\">Lounge Fan</button>\n" +
             "</div><br/><br/><div>\n" +
             "<button id=\"off\" onclick=\"sendMessage(this)\" class=\"kill\">OFF</button>\n" +
             "</div>\n" +
             "</body> \n" +
             "</html>"
             "\r\n";

  return HTMLpage;
}

String constructAjaxResponse()
{
  return String("HTTP/1.1 200 OK\r\n") +
         "Content-Type: application/json\r\n" +
         "Connection: close\r\n" +
         "\r\n" +
         "{" +
         "\"li\": \"" + getCombinedLightsBtnClassByState() + "\"," +
         "\"tLi\": \"" + getLightButtonClassByState(false) + "\"," +
         "\"lLi\": \"" + getLightButtonClassByState(true) + "\"," +
         "\"fa\": \"" + getCombinedFansBtnClassByState() + "\"," +
         "\"tFa\": \"" + getFanButtonClassByState(false) + "\"," +
         "\"lFa\": \"" + getFanButtonClassByState(true) + "\"" +
         "}";
}

String getCombinedLightsBtnClassByState()
{
  const int loungeSwitchState = transmitter.getLoungeState();
  const int tvSwitchState = transmitter.getTVState();
  if (loungeSwitchState > 3 && tvSwitchState > 3)
  {
    return "active";
    // For the sake of all that is holy write a damn helper function to determine if a given light is on or off
  }
  else if (loungeSwitchState > 3 && tvSwitchState < 4 || loungeSwitchState < 4 && tvSwitchState > 3)
  {
    return "inactive";
  }
  return "inactive";
}

String getCombinedFansBtnClassByState()
{
  const int loungeSwitchState = transmitter.getLoungeState();
  const int tvSwitchState = transmitter.getTVState();

  if (loungeSwitchState == tvSwitchState || loungeSwitchState == tvSwitchState - 4 || loungeSwitchState == tvSwitchState + 4)
  {
    if (loungeSwitchState == 0 || loungeSwitchState == 4)
    {
      return "inactive";
    }
    return getFanButtonClassByState(0);
  }
  return "inactive";
}

/**
 *  Determine the CSS class for a light button based on the state of the associated switch.
 */
String getLightButtonClassByState(bool isLoungeSwitch)
{
  const int loungeSwitchState = transmitter.getLoungeState();
  const int tvSwitchState = transmitter.getTVState();
  if (isLoungeSwitch)
  {
    if (loungeSwitchState > 3)
    {
      return "active";
    }
  }
  else
  {
    if (tvSwitchState > 3)
    {
      return "active";
    }
  }
  return "inactive";
}

/**
 *  Determine the CSS class for a fan button based on the state of the associated switch.
 */
String getFanButtonClassByState(bool isLoungeSwitch)
{
  const int loungeSwitchState = transmitter.getLoungeState();
  const int tvSwitchState = transmitter.getTVState();
  if (isLoungeSwitch)
  {
    return classFromFanState(loungeSwitchState);
  }
  else
  {
    return classFromFanState(tvSwitchState);
  }
  return "inactive";
}

/**
 *  Determine the state of the fan switch (could be high or low depending on the state of the light)
 */
String classFromFanState(int state)
{
  switch (state)
  {
  case (0):
  case (4):
    return "inactive";
    break;
  case (1):
  case (5):
    return "oneSpeed";
    break;
  case (2):
  case (6):
    return "twoSpeed";
    break;
  case (3):
  case (7):
    return "threeSpeed";
    break;
  }
  return "inactive";
}
