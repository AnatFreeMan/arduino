//AM2302
#include "DHT.h" /

//BMP180
#include "Wire.h"
#include "BMP085.h"

//AM2302 
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

//BMP180
long Temperature = 0, Pressure = 0, Altitude = 0;
unsigned long time1=0;
 
DHT dht(DHTPIN, DHTTYPE);
BMP085 dps = BMP085();    
 
void setup() {
  //pinMode(fan, OUTPUT);
  Serial.begin(9600);
  Wire.begin(); 
  scanI2Cdevices(); 
  dht.begin();

  delay(3000);
  dps.init(MODE_STANDARD, 27000, true); 
}
 
void loop() {
  // Wait a few seconds between measurements.
  delay(2000);
 
  showDHTData ();
  showBMP180Data ();
  
  delay(1000);
}

void showDHTData (){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
 // if(h > maxHum || t > maxTemp) {
 //     digitalWrite(fan, HIGH);
 // } else {
 //    digitalWrite(fan, LOW); 
 // }
  
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.println(" %\t");
}

void   showBMP180Data(){
  return;
  // calculate temperature every 1 sec (dynamic measurement)
  // NOTE: in order to take full advantage of dynamic measurement set AUTO_UPDATE_TEMPERATURE to false in bmp085.h
  if (((millis() - time1)/1000.0) >= 1.0) {     
     dps.calcTrueTemperature();
     time1 = millis();      
  }
  dps.getTemperature(&Temperature); 
  dps.getPressure(&Pressure);
  dps.getAltitude(&Altitude);
  
  
//  Serial.print("  Alt(cm):");
//  Serial.print(Altitude);
//  Serial.print("  Pressure(Pa):");
//  Serial.println(Pressure);
  
  Serial.print("Temp(C):");
  Serial.print(Temperature*0.1,1);
  Serial.print("  Pressure(mm):");
  Serial.print(Pressure/133.3,1);
  Serial.print("  Alt(m):");
  Serial.println(Altitude*0.01);
}
void scanI2Cdevices () {
  byte error, n,
   nDevices = 1;
  // BMP180 
  byte address[1] = {0x77};
  
  Serial.println("i2c device scanning...");
 
  
  for(n = 0; n < nDevices; n++ )
     Serial.print("n =");
     Serial.println(n);

  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
     Serial.print("Scaning device 0x");
     if (address[n]<16)
     Serial.print("0");
     Serial.println(address[n],HEX);
     
    Wire.beginTransmission(address[n]);

    error = Wire.endTransmission();
     //Serial.println(address[n],HEX);
     //Serial.print("Error =");
     //Serial.println(error);
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address[n]<16)
      Serial.print("0");
      Serial.print(address[n],HEX);
      Serial.println("  !");
 
     // n++;
    }else if (error==2)
    {
      Serial.print("Device at address 0x");
      if (address[n]<16)
        Serial.print("0");
        Serial.print(address[n],HEX);
        Serial.println(" does not connected");
    }
    else if (error==4)
    {
      Serial.print("Unknow error at address 0x");
      if (address[n]<16)
        Serial.print("0");
      Serial.println(address[n],HEX);
    }    
  }
  if (n == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done...\n");
 
  delay(5000);           // wait 5 seconds for next scan
}  
  
