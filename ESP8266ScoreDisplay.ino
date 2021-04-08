#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
//#include <FS.h>
#include <arduino-timer.h>

#define PIN 2
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
                            NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB            + NEO_KHZ800);
const char *ssid = "WiFiFaOpenFiber";
const char *password = "+viacampolongo23*";
String color = "00FFFF";
String jscolor = "";
int r = 0;
int g = 255;
int b = 255;
int brightness = 10;
uint8_t intSpeed = 30;

//bool brightnessChanged = false;

ESP8266WebServer server ( 80 );

//const int led = 13;
String message = "";
char ScoreMsg[10];
int Score[2];
bool blnScoreChanged = true;
const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(255, 255, 0),matrix.Color(0, 0, 255), matrix.Color(255, 0, 255), matrix.Color(0, 255, 255), matrix.Color(255, 255, 255)};

enum mode {
  IDLE,
  SCROLL,
  STATIC
};

int intMode=SCROLL;
int x = matrix.width();
int pass = 0;

auto tmrFlashing = timer_create_default();
uint8_t intFlashSpeed=4000;
uint8_t intFlashDuration=12;
uint8_t intFlashCounter[2];
boolean blnFlash[2];
boolean blnFlashToggle[2];
int intScoreX[2]={0,21};

void getData() {
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "Score1") {
      if(Score[0]!=server.arg(i).toInt()) {
        Score[0] = server.arg(i).toInt();
        intFlashCounter[0]=intFlashDuration;
        blnFlash[0]=true;
        intMode=STATIC;
      }
    } else if (server.argName ( i ) == "Score2") {
      if(Score[1]!=server.arg(i).toInt()) {
        Score[1] = server.arg(i).toInt();
        intFlashCounter[1]=intFlashDuration;
        blnFlash[1]=true;
        intMode=STATIC;
      }
    } else if (server.argName ( i ) == "color") {
      color = server.arg ( i );
      Serial.println(color);
      if (color.charAt(0) == '#') {
        color = color.substring(1);
      }
      Serial.println(color);
      Serial.println(color.length());
      if (color.length() != 6) continue;
      long number = (long) strtol(color.c_str(), NULL, 16);
      r = number >> 16;
      g = number >> 8 & 0xFF;
      b = number & 0xFF;
    } else if(server.argName(i)=="brightness") {
      int br = server.arg(i).toInt();
      if (br >= 0 && br <= 255) brightness = br;
      matrix.setBrightness(brightness);
      //brightnessChanged = true;
    } else if(server.argName(i)=="speed") {
      int sp = server.arg(i).toInt();
      if (sp >= 1 && sp <= 255) intSpeed = sp;
    }
  }
/*  if(blnScoreChanged) {
    sprintf(ScoreMsg,"%i-%i",Score1,Score2);
    intMode=STATIC;
  }*/
  //Serial.println(message);
  handleRoot();
}
void handleRoot() {
  /*String messageSafe;
  messageSafe.reserve(message.length());
  for (size_t pos = 0; pos != message.length(); ++pos) {
    switch (message[pos]) {
      case '&':  messageSafe += "&amp;";       break;
      case '\"': messageSafe += "&quot;";      break;
      case '\'': messageSafe += "&apos;";      break;
      case '<':  messageSafe += "&lt;";        break;
      case '>':  messageSafe += "&gt;";        break;
      default:   messageSafe += message[pos];  break;
    }
  }*/
//  String toSend = "<html><head><script src=\"jscolor.min.js\"></script><title>Score Display</title><style>body{background-color:#ccc;font-family:Arial,Helvetica,Sans-Serif;Color:#008}#picker{width:200px;height:200px;display:inline-block;}#slide{width:30px;height:200px;display:inline-block;}</style></head><body><h1>Hello from ESP8266!</h1><form action=\"\"method=\"post\"><label for=\"name\">Message:</label><input id=\"name\" name=\"name\" value=\"" + messageSafe + "\"><br><label for=\"color\">Color:</label><input id=\"color\" name=\"color\" class=\"jscolor\"  value=\"" + color + "\"><div id=\"picker\"></div><div id=\"slide\"></div><script type=\"text/javascript\">ColorPicker(document.getElementById('slide'),document.getElementById('picker'),function(hex){document.getElementById('color').value=hex;document.getElementById('color').style.backgroundColor=hex}).setHex('#'+document.getElementById('color').value);document.getElementById('color').style.backgroundColor='#'+document.getElementById('color').value;</script><br><label for=\"brightness\">Brightness:</label><input id=\"brightness\" name=\"brightness\" value=\"" + brightness + "\" min=\"0\" max=\"255\" type=\"number\"><br><input type=\"submit\" value=\"Save\"></form></body></html>";
  String toSend = "<html><head><title>Score Display</title><style>body{background-color:#ccc;font-family:Arial,Helvetica,Sans-Serif;Color:#008}</style>"
  "</head><body><h1>Score Display</h1><form id=\"theForm\" action=\"\"method=\"post\">" 
  "<label for=\"Score1\">Score1:</label><input id=\"Score1\" name=\"Score1\" value=\"" + String(Score[0]) + "\" min=\"0\" max=\"99\" type=\"number\" onclick=\"document.getElementById('theForm').submit();\"><br>"
  "<label for=\"Score2\">Score2:</label><input id=\"Score2\" name=\"Score2\" value=\"" + String(Score[1]) + "\" min=\"0\" max=\"99\" type=\"number\"><br>"
  "<label for=\"color\">Color:</label><input id=\"color\" name=\"color\" value=\"" + color + "\"><br>"
  "<label for=\"brightness\">Brightness:</label><input id=\"brightness\" name=\"brightness\" value=\"" + String(brightness) + "\" min=\"0\" max=\"255\" type=\"number\"><br>"
  "<label for=\"speed\">Speed:</label><input id=\"speed\" name=\"speed\" value=\"" + String(intSpeed) + "\" min=\"1\" max=\"255\" type=\"number\"><br>"
  "<input type=\"submit\" value=\"Save\"></form></body></html>";
  server.send ( 200, "text/html", toSend );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void setup ( void ) {
  Serial.begin ( 115200 );
  WiFi.begin ( ssid, password );

  Serial.print("Waiting for connection");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  //WiFi.softAP("LED Matrix", "12345678");
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );
  message = "IP Address: ";
  message += WiFi.localIP().toString();

  if(MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  SPIFFS.begin();
  //File file = SPIFFS.open("/jscolor.min.js", "r");
  //jscolor = file.readString();
  //file.close();
  server.on ( "/", HTTP_GET, handleRoot );
  server.on ( "/", HTTP_POST, getData );
  /*server.on ( "/jscolor.min.js", HTTP_GET, []() {
    server.send ( 200, "application/javascript", jscolor );
  });*/
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println("HTTP server started");
  randomSeed(analogRead(0));
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(brightness);
  matrix.setTextColor(colors[0]);
  Serial.println("matrix started");
  tmrFlashing.every(intFlashSpeed, doFlash);
}

void ShowScore(uint8_t intScoreIdx) {
  sprintf(ScoreMsg,"%2i",Score[intScoreIdx]);
  matrix.setCursor(intScoreX[intScoreIdx], 0);
  matrix.print(ScoreMsg);
}

void loop ( void ) {
  switch(intMode) {

    case IDLE:
      break;

    case SCROLL:
      matrix.fillScreen(0);
      matrix.setCursor(x, 0);
    //  matrix.setTextColor(matrix.Color(r, g, b));
      matrix.print(message.c_str());
      if (--x < ((int)message.length()) * -6) {
        x = matrix.width();
        if(++pass >= 7) pass = 0;
        matrix.setTextColor(colors[pass]);
      }
      matrix.show();
      break;

    case STATIC:
      matrix.fillScreen(0);
      matrix.setTextColor(colors[0]);
      ShowScore(0);
      matrix.setTextColor(colors[1]);
      ShowScore(1);

      matrix.show();
      intMode=IDLE;
      break;
  }

  server.handleClient();
  if (blnScoreChanged) {
    x = matrix.width();
    blnScoreChanged = false;
    //Serial.print("New message: ");
    //Serial.println(message);
  }

  tmrFlashing.tick();

  yield();
  //if (brightnessChanged) matrix.setBrightness(brightness);

  delay(intSpeed);
}

boolean blnSecondTick;
bool doFlash(void *argument)
{
  for(uint8_t i=0;i<2;i++) {
    if(blnFlash[i]) {
      if(--intFlashCounter[i]==0) {
        blnFlash[i]=false;
        blnFlashToggle[i]=false;
      } else {
        blnFlashToggle[i]=!blnFlashToggle[i];
        if(blnFlashToggle[i]) {
          matrix.setTextColor(colors[i]);
        } else {
          matrix.setTextColor(0);
        }
        ShowScore(i);
      }
    }
  }
  if(intMode==IDLE) {
    blnSecondTick=!blnSecondTick;
    matrix.setCursor(14, 0);
    if(blnSecondTick) {
      matrix.setTextColor(colors[3]);
    } else {
      matrix.setTextColor(0);
    }
    matrix.print(":");
  }
  matrix.show();
  return(true);
}
