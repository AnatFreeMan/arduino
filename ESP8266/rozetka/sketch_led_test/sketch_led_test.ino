#define RED 15// GPIO15/TXD01
#define GREEN 12// GPIO12/TXD01
#define BLUE 13// GPIO13/TXD01

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void loop() {
  digitalWrite(RED, HIGH);
  delay(1000);
  digitalWrite(RED, LOW);
  delay(1000);
  digitalWrite(GREEN, HIGH);
  delay(1000);
  digitalWrite(GREEN, LOW);
  delay(1000);
  digitalWrite(BLUE, HIGH);
  delay(1000);
  digitalWrite(BLUE, LOW);
  delay(1000);
}
