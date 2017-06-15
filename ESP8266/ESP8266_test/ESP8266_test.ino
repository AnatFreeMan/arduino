#include <ESP8266WiFi.h> 
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "TP-LINK"; //Подключается к точке доступа OpenWrt 
char strok[30];
char buf[30];
long sec;
int ss;
int leftWheel = D5;
int rightWheel = D6;
int leftWheelDirection = D7;
int rightWheelDirection = D8;
int wifiStatusPin = D0;
int cmdStopTime = 0;

IPAddress ip(10,24,2,102);
IPAddress gateway(10,24,0,1);
IPAddress subnet(255,255,240,0); 
ESP8266WebServer server(80);

void processWheelsCmd() {
  if (cmdStopTime < millis()) {
    analogWrite(leftWheel,0);
    analogWrite(rightWheel,0);
    digitalWrite(leftWheelDirection,HIGH);
    digitalWrite(rightWheelDirection,HIGH);
  }
}

void handleCmd() {
  String message = "Number of args received:";  
  message += server.args();            //Get number of parameters
  message += "\n";       
  for (int i = 0; i < server.args(); i++) {
    message += "Arg #" + (String)i + " –> ";
    message += server.argName(i) + ": ";
    message += server.arg(i) + "\n";
    
    if (server.argName(i)=="left") {      
      int ctrl = map(server.arg(i).toInt(), 0,100 , 0, 1024);
      message += "left power: " + String(ctrl) + " dir:" + (ctrl>0?"fwd":"back") + "\n";
      if (ctrl < 0) {
        digitalWrite(leftWheelDirection,LOW);
      } else {
        digitalWrite(leftWheelDirection,HIGH);
      }
      analogWrite(leftWheel,abs(ctrl));      
    }
    if (server.argName(i)=="right") {
      int ctrl = map(server.arg(i).toInt(), 0,100 , 0, 1024);
      message += "right power: " + String(ctrl) + " dir:" + (ctrl>0?"fwd":"back") + "\n";
      if (ctrl < 0) {
        digitalWrite(rightWheelDirection,LOW);
      } else {
        digitalWrite(rightWheelDirection,HIGH);
      }
      analogWrite(rightWheel,abs(ctrl));
    }
    if (server.argName(i)=="duration") {
      message += "duration: " + String(server.arg(i)) + "\n";
      cmdStopTime = millis()+server.arg(i).toInt();
    } else {
      cmdStopTime = millis()+1000;
    }    
  }
  server.send(200, "text/plain", message);
}

void handleHelp() {
  server.send(200, "text/html","<h1>This is web-server on Wifi-esp-12.<br>He controls IT-robot.<br>Command:/help /cmd?left=100&right=100&duration=1000</h1>"); 
}

void setup(void){
ESP.eraseConfig();
analogWrite(leftWheel,0);
analogWrite(rightWheel,0);
digitalWrite(leftWheelDirection,HIGH);
digitalWrite(rightWheelDirection,HIGH);
pinMode(leftWheel, OUTPUT);
pinMode(rightWheel, OUTPUT);
pinMode(leftWheelDirection, OUTPUT);
pinMode(rightWheelDirection, OUTPUT);
pinMode(wifiStatusPin, OUTPUT);
  
  WiFi.config(ip,gateway,subnet);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid,"76533457");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.on("/cmd", handleCmd);
  server.on("/help", handleHelp);
  server.begin();
// Serial.println("HTTP server started");
}
 
void loop(void){ 
  digitalWrite(wifiStatusPin,WiFi.isConnected()?HIGH:LOW);
  sec=millis();
  server.handleClient();
  processWheelsCmd();
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(wifiStatusPin, HIGH);
  } else {
    digitalWrite(wifiStatusPin, LOW);
  }
  
}
