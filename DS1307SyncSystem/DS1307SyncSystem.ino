#include <OneWire.h>

// Author: alxarduino@gmail.com
// Sample Clock App for http://alxarduino.blogspot.com
 
// Библиотеки необходимые для работы модуля часов
#include "OneWire.h"
#include "Wire.h"
#include "DS1307new.h"
#include "LiquidCry

#include "dallastemperature.h"
// Провід  даних підключений до контакту 2 на Arduino:
#define ONE_WIRE_BUS 2

// Налаштування 1-Wire для спілкуватися з будь-якими 1-Wire-пристроями (не тільки з мікросхемою датчика температури Maxim/Dallas)
OneWire oneWire(ONE_WIRE_BUS);
// Передаємо наше посилання 1-Wire до датчика температури
DallasTemperature sensors(&oneWire);

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2); // запускаем библиотеку
  sensors.begin();
  isRTCPresent();
  delayMicroseconds(2000);  // this command takes a long time!
  lcd.clear();
  delayMicroseconds(2000);
}
 
void loop(){
  RTC.getTime();// получить время от модуля
  
  printTimeToSerial(); // выводим полученное время
  printTimeToLCD(); // выводи время на экран
  printTempToSerial();
  //doSerialCommands(); // слушаем и выполняем команды из Serial
}
//////////////////////////////// temperature /////////////////////////////
printTempToSerial(){
   sensors.requestTemperatures(); // Послати команду, щоб отримати температуру
   Serial.print(sensors.getTempCByIndex(0));  // Чому "byIndex"? Ви можете мати більше однієї мікросхеми на одній шині. 0 відноситься до першого приєднаного пристрою
};
printTimeToLCD(){
   // Викликаємо sensors.requestTemperatures (), щоб отримати глобальну температуру
   // Запит для всіх пристроїв на шині
    sensors.requestTemperatures(); // Послати команду, щоб отримати температуру
    lcd.setCursor(0, 1);
    lcd.print(sensors.getTempCByIndex(0));  // Чому "byIndex"? Ви можете мати більше однієї мікросхеми на одній шині. 0 відноситься до першого приєднаного пристрою
    lcd.print((char)223);
    lcd.print("C ");
    delay(500);
}
/////////////////////////////////////////////////       clock   //////////////////////////////////
//проверяем часы
void isRTCPresent(){
    lcd.setCursor(0,0); // курсов в левый верхний угол экрана
    if(RTC.isPresent()){ // обнаружен ли модуль?
      Serial.println("RTC Ready"); // все хорошо
      lcd.print("RTC Ready");
    } else {
      Serial.println("Error!!!! RTC Module not found"); // сообщаем о проблеме
      lcd.print("RTC Module not found");
      while(1); // и останавливаем скетч
    }
  }
 
// Выводит текущие время в Serial
void printTimeToSerial(){
 
  byte static prevSecond=0; // тут будем хранить, сколько секунд было при прошлом отчете
   
  if(RTC.second!=prevSecond){ // что-то делаем только если секунды поменялись
    Serial.print(RTC.hour); // часы
   
    Serial.print(":"); // разделитель
    Serial.print(RTC.minute);
   
    Serial.print(":"); 
    Serial.println(RTC.second);
     
    prevSecond=RTC.second; // запомнили когда мы "отчитались"
  }
}
 
// устанавливает часы модуля на какое-то заранее определенное время
void setSomeTime(){
  RTC.stopClock();// останавливаем часы
  RTC.fillByHMS(0,22,0); // "подкручиваем стрелки на 19:15:00
  RTC.setTime();// отправляем "подкрученное время" самому модулю
  RTC.startClock(); // и опять запускаем часы
}
 
// слушает из Serial команды и выполняет их. Каждая команда - один символ.
// доступны команды:
//  s - установить время указанное в функции setSomeTime()
void doSerialCommands(){
  if(Serial.available()){ // что-нибудь пришло?
    char ch=Serial.read(); // читаем что пришло
     
    switch(ch){
      case 's': // команда установки времени
           setSomeTime(); // устанавливаем
           break;
            
       // тут, в будущем, мы можем добавлять дополнительные команды
      default:;
           // на неизвестную команду - ничего не делаем
    };
  }
}

// Выводит текущие время на LCD
 
void printTimeToLCD(){
    byte static prevSecond=0; // тут будем хранить, сколько секунд было при выводе
     
    if(RTC.second!=prevSecond){ // что-то делаем только если секунды поменялись
       lcd.setCursor(0,0); // устанавливаем позицию курсора
        
      if(RTC.hour<10)lcd.print(" ");
      lcd.print(RTC.hour); // часы
     
      lcd.print( (RTC.second % 2)?" ":":"); // разделитель моргает
       
      if(RTC.minute<10)lcd.print(0); // лидирующий ноль, если нужен
      lcd.print(RTC.minute);
      
      lcd.print( (RTC.second % 2)?" ":":"); // разделитель моргает
       
      if(RTC.second<10)lcd.print(0); // лидирующий ноль, если нужен
      lcd.print(RTC.second);
     
    prevSecond=RTC.second; // запомнили когда мы "отчитались"
  }
}
/////////////////////////////////////////////////       clock    end    //////////////////////////////////
