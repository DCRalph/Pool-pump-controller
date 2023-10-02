//===Libarys==========================================
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
//===Pins=============================================
#define DS18S20_Pin 12
#define up A0
#define down A1
#define pump 8
#define back_light 9
#define hanging_light 10
#define back_light_switch 7
#define hanging_light_switch 6
#define OLED_RESET 4
//===Temperature Sensor===============================
OneWire ds(DS18S20_Pin);
//===OLED=============================================
Adafruit_SSD1306 Display1(OLED_RESET);
Adafruit_SSD1306 Display2(OLED_RESET);
//====================================================

long timeout = 0;
int  temp = 30;
bool outside_light = 0;
bool hanging_lightval = 0;
int  btemp = 5;
unsigned long  lastmillis;
unsigned long  lastmillis2;
bool oled = true;

int  wait = 0;
int  wait2 = 0;
bool once11 = 0;
bool once12 = 0;
bool once21 = 0;
bool once22 = 0;
bool once31 = 0;
bool once32 = 0;
float temperature = 0;
bool debug = 0;
//===Temperature======================================
float getTemp() {
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr)) {
    ds.reset_search();
    return -1000;
  }
  if ( OneWire::crc8( addr, 7) != addr[7]) {
    return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    return -1000;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);
  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  for (int i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
  ds.reset_search();
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB);
  float TemperatureSum = tempRead / 16;
  return TemperatureSum;
}
//===Set Up===========================================
void setup() {
  Serial.begin(9600);

  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(hanging_light, OUTPUT);
  pinMode(back_light, OUTPUT);
  pinMode(pump, OUTPUT); //pump
  pinMode(back_light_switch, INPUT);
  pinMode(hanging_light_switch, INPUT);

  digitalWrite(pump, LOW);
  digitalWrite(hanging_light, LOW);
  digitalWrite(back_light, LOW);

  Display1.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  Display1.clearDisplay();

  Display2.begin(SSD1306_SWITCHCAPVCC, 0x3d);
  Display2.clearDisplay();

  Display1.display();
  Display2.display();

  Display1.setTextSize(1);
  Display1.setTextColor(WHITE, BLACK);
  Display1.setCursor(0, 0 );
  Display1.println("  MANUFACTURRD BY");
  Display1.setTextSize(2);
  Display1.setCursor(0, 10 );
  Display1.println("WILLIAM'S ");

  Display2.setTextSize(1);
  Display2.setTextColor(WHITE, BLACK);
  Display2.setCursor(0, 0 );
  Display2.println("WILLIAM    [TRADMARK]");
  Display2.setTextSize(2);
  Display2.setCursor(0, 10 );
  Display2.println("POOL PUMP");

  Display1.display();
  Display2.display();

  delay(5000);
}
//===Loop=============================================
void loop() {
  temperature = getTemp();
  //Serial.println(temperature);
  //Serial.println(timer);
  Serial.print(digitalRead(up));
  Serial.print(" - ");
  Serial.print(digitalRead(down));
  Serial.print(" - ");
  Serial.print(debug);
  Serial.print(" - ");
  Serial.print(digitalRead(back_light_switch));
  Serial.print("-");
  Serial.print(digitalRead(back_light));
  Serial.print(" - ");
  Serial.print(digitalRead(hanging_light_switch));
  Serial.print("-");
  Serial.print(digitalRead(hanging_light));
  Serial.print(" end");
  Serial.println();

  if (Serial.available() > 0) {
    char letter = Serial.read();
    if (letter == '1') {
      debug = !debug;
      Serial.println("debug");
      letter = 0;
    }
    if (letter == '2') {
      letter = 0;
      if (digitalRead(pump)) {
        digitalWrite(pump, 0);
        Serial.println("pump off");
      } else {
        digitalWrite(pump, 1);
        Serial.println("pump on");
      }
    }
    if (letter == '3') {
      letter = 0;
      if (digitalRead(back_light)) {
        outside_light = 0;
        Serial.println("back off");
      } else {
        outside_light = 1;
        Serial.println("back on");
      }
    }
    if (letter == '4') {
      letter = 0;
      if (digitalRead(hanging_light)) {
        hanging_lightval = 0;
        Serial.println("hanging off");
      } else {
        hanging_lightval = 1;
        Serial.println("hanging on");
      }
    }
  }

  if (oled) {
    if (false) {

    } else {

      Display1.setTextSize(1);
      Display1.setTextColor(WHITE, BLACK);
      Display1.setCursor(0, 0 );
      Display1.println("  WIlliam'S POOL PUMP");
      Display1.setTextSize(2);
      Display1.setCursor(0, 10 );
      Display1.println("Temp");
      Display1.setCursor(55, 10 );
      Display1.println(temperature);
      Display1.display();
      Display1.clearDisplay();

      Display2.setTextSize(1);
      Display2.setTextColor(WHITE, BLACK);
      Display2.setCursor(0, 0 );
      Display2.println("WILLIAM'S POOL PUMP");
      Display2.setCursor(0, 10 );
      Display2.setTextSize(2);
      Display2.println("SetTemp");
      Display2.setCursor(90, 10 );
      Display2.print(temp);
      Display2.display();
      Display2.clearDisplay();
    }
  } else {
    Display1.clearDisplay();
    Display2.clearDisplay();

    Display1.display();
    Display2.display();
  }

  if (temperature > temp) {
    wait ++;
  }
  else {
    wait = 0;
  }
  if (temperature < temp - btemp) {
    wait2 ++;
  }
  else {
    wait2 = 0;
  }
  if (wait == 50 ) {
    digitalWrite(pump, HIGH);
  }
  if (wait2 == 300) {
    digitalWrite(pump, LOW);
  }


  run_outside_light();
  run_hanging_light();



  temp = constrain(temp, 10, 80);

  if (true && debug == 0) {
    oledon();
  }
  if (false && debug == 0) {
    oledoff();
  }


  if (debug == 1) {
    hiden();
  }
}
//===========================================
void oledon() {

  if (temperature > temp) {
    wait ++;
  }
  else {
    wait = 0;
  }
  if (temperature < temp - btemp) {
    wait2 ++;
  }
  else {
    wait2 = 0;
  }
  if (wait == 50 ) {
    digitalWrite(pump, HIGH);
  }
  if (wait2 == 300) {
    digitalWrite(pump, LOW);
  }
  //===========================================
  if (digitalRead(up) == LOW) {
    temp += 1;
    delay(200);
  }
  if (digitalRead(down) == LOW) {
    temp -= 1;
    delay(200);
  }
  if (digitalRead(up) == 0 && digitalRead(down) == 0) {
    lastmillis2 = millis();
  }

  if (digitalRead(up) == 0 && digitalRead(down) == 0) {
    lastmillis2 = millis();
  }
  while ((millis() - lastmillis2) < 5000) {
    if (digitalRead(up) == LOW) {
      btemp += 1;
      delay(200);
    }
    if (digitalRead(down) == LOW) {
      btemp -= 1;
      delay(200);
    }
    btemp = constrain(btemp, 0, 10);
    Display1.setTextSize(1);
    Display1.setTextColor(WHITE, BLACK);
    Display1.setCursor(30, 0 );
    Display1.println("TEMP DIFFRENCE");
    Display1.setTextSize(3);
    Display1.setCursor(60, 10 );
    Display1.println(btemp);
    Display1.display();
    Display1.clearDisplay();

    Display2.setTextSize(1);
    Display2.setTextColor(WHITE, BLACK);
    Display2.setCursor(30, 0 );
    Display2.println("TIME LEFT");
    Display2.setTextSize(3);
    Display2.setCursor(30, 10 );
    int x = (millis() - lastmillis2) / 1000;
    x = map(x, 0, 5, 5, 0);
    Display2.println(x);
    Display2.display();
    Display2.clearDisplay();
  }
}

void oledoff() {
  if (temperature > temp) {
    wait ++;
  }
  else {
    wait = 0;
  }
  if (temperature < temp - btemp) {
    wait2 ++;
  }
  else {
    wait2 = 0;
  }
  if (wait == 50 ) {
    digitalWrite(pump, HIGH);
  }
  if (wait2 == 300) {
    digitalWrite(pump, LOW);
  }

}

//===Lights===========================================
void run_outside_light() {
  if (digitalRead(back_light_switch) == LOW) {
    if (once12 == true) {
      outside_light = false;
    }
    once11 = true;
    once12 = false;
  }
  if (digitalRead(back_light_switch) == HIGH) {
    if (once11 == true) {
      outside_light = true;
    }
    once12 = true;
    once11 = false;
  }
  digitalWrite(back_light, outside_light);
}

void run_hanging_light() {
  if (digitalRead(hanging_light_switch) == LOW) {
    if (once22 == true) {
      hanging_lightval = false;
    }
    once21 = true;
    once22 = false;
  }
  if (digitalRead(hanging_light_switch) == HIGH) {
    if (once21 == true) {
      hanging_lightval = true;
    }
    once22 = true;
    once21 = false;
  }
  digitalWrite(hanging_light, hanging_lightval);
}

//===I2C==============================================

void hiden() {
  Display1.clearDisplay();
  Display2.clearDisplay();

  Display1.setTextSize(1);
  Display1.setTextColor(WHITE, BLACK);
  Display1.setCursor(0, 0 );
  Display1.println("Debug");
  Display1.setCursor(40, 0 );
  Display1.println("Time Sec:");
  Display1.setCursor(95, 0 );
  Display1.println(millis() / 1000);
  Display1.setCursor(0, 10 );
  Display1.println("Outside Light:");
  Display1.setCursor(95, 10 );
  switch (outside_light) {
    case false:
      Display1.println("OFF");
      break;
    case true:
      Display1.println("ON");
      break;
  }
  Display1.println("Hanging Light:");
  Display1.setCursor(95, 20 );
  switch (hanging_lightval) {
    case false:
      Display1.println("OFF");
      break;
    case true:
      Display1.println("ON");
      break;
  }
  Display2.setTextSize(1);
  Display2.setTextColor(WHITE, BLACK);
  Display2.setCursor(0, 0 );
  Display2.println("Debug");

  Display1.display();
  Display2.display();
}
