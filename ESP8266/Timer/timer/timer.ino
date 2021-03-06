#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Ticker.h>
Ticker flipper;

#define RED 15// GPIO15/TXD01
#define GREEN 12// GPIO12/TXD01
#define GMT 3

char ssid[] = "TP-LINK";         //  your network SSID (name)
char pass[] = "76533457";        // your network password

unsigned int localPort = 2390;      // local port to listen for UDP packets
byte hour, minute, second;
boolean point;

IPAddress ip(10,24,2,106);
IPAddress gateway(10,24,0,1);
IPAddress subnet(255,255,240,0); 

/* Don't hardwire the IP address or we won't get the benefits of the pool. *  Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
//IPAddress timeServer(216, 229, 0, 179); // time.nist.gov NTP server
//0.ua.pool.ntp.org
//1.ua.pool.ntp.org
//2.ua.pool.ntp.org
//3.ua.pool.ntp.org

//ntp.time.in.ua - основной сервер точного времени (stratum 1);
//ntp2.time.in.ua - резервный сервер точного времени (stratum 1);
//ntp3.time.in.ua - резервный сервер точного времени (stratum 2).


IPAddress timeServerIP; // time.nist.gov NTP server address
//const char* ntpServerName = "time.nist.gov";

char* ntpServerName[3] = {"time.nist.gov", "ntp.time.in.ua" , "0.ua.pool.ntp.org"};


const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
//
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  flipper.attach(1, flip);          /// прерывание 1 сек 
  
  Serial.println();
  Serial.println();
  
  Serial.printf("\n\nFree memory %d\n",ESP.getFreeHeap());
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.config(ip,gateway,subnet);
  WiFi.setAutoReconnect(true);
  //WiFi.begin(ssid,"76533457");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(RED, HIGH);
    delay(500);
    digitalWrite(RED, LOW);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  digitalWrite(GREEN, HIGH);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  oldloop();        /// синхронизируем время при включении 

}
void oldloop()
{

//  do {
//      delay(50);          // подождать, пока датчики стабилизируются
//      x = readSensors(); // проверить датчики
//  } while (x < 100);
  int cb;
  //get a random server from the pool
  for (int i=0; i <= 2; i++){
      Serial.print("Get time Server IP: ");
      WiFi.hostByName(ntpServerName[i], timeServerIP); 
      Serial.print(ntpServerName[i]);
      Serial.print(" => ");
      Serial.println(timeServerIP);    
      sendNTPpacket(timeServerIP); // send an NTP packet to a time server
      // wait to see if a reply is available
      delay(1000);
  
      cb = udp.parsePacket();
      if (!cb) {
        Serial.println("no packet yet");
      }
      else {
        break;
     }
  }
  //int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("Sync time failed");
    return;
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);
    
          /// корректировка часового пояса и синхронизация 
    epoch = epoch + GMT * 3600;     
    
    hour = (epoch  % 86400L) / 3600;
    minute = (epoch  % 3600) / 60;
    second = epoch % 60;
   

    // print the hour, minute and second:
    Serial.println('');
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
    Serial.println('');
  }
  // wait ten seconds before asking for the time again

}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void flip(){ 
  point = !point;
  second++; 
  if (second > 59){
    second = 0;
    minute++;      
  }
  if (minute > 59){
    minute = 0;
    hour++;  
  }
  if (hour > 23){
    hour = 0;
  } 

}

void loop(){
  if (second == 59){            // если насчитали 60 сек
    flipper.detach();           // выключаем прерывание
    delay(1500);                // ждем, чтобы повторно не запустить
    oldloop();                  // синхронизируем время
    flipper.attach(1, flip);    // запускаем прерывание
  }
  
   //int8_t TimeDisp[4];          // отправляем всё на экран 
   //tm1637.point(point);         // управление :, мигаем если запущено прерывание
   
   //TimeDisp[0] = hour / 10;
   //TimeDisp[1] = hour % 10;
   //TimeDisp[2] = minute  / 10;
   //TimeDisp[3] = minute  % 10;
   //Serial.println(TimeDisp); // print the second  
   //tm1637.display(TimeDisp);
}
