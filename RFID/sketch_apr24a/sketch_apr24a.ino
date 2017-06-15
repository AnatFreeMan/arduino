// sketch Arduino ANTENNA NFC MFRC522 МТС TEST.
#include <SPI.h>
#include <MFRC522.h> // Библиотека "rfid". https://github.com/miguelbalboa/rfid
#define SS_PIN 10 // !(SS - он же - SDA).
#define RST_PIN 9 // Два Pin (SS и RST) допускают произвольное подключение, и конфигурируются в коде.
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Объект MFRC522C / reate MFRC522 instance.
void setup() {
  Serial.begin(9600);
  SPI.begin(); // Инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init(); // Инициализация MFRC522 / Init MFRC522 card.
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Увеличение коэффициента усиления антенны Increase antenna gain per.
  pinMode(14, OUTPUT);
}
void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) { // Поиск новой карточки / Look for new cards.
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { // Выбор карточки / Select one of the cards.
    return;
  }
  unsigned long uidDec, uidDecTemp;  // Для отображения номера карточки, в десятичном формате.
  for (byte i = 0; i < mfrc522.uid.size; i++) { // Выдача серийного номера карточки "UID".
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec * 256 + uidDecTemp;
  }
  Serial.println(uidDec); // Номер "UID", NFC-карты.
  digitalWrite(14, HIGH);
  delay(50);
  digitalWrite(14, LOW);
}
