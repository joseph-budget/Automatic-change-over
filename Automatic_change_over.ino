#include <SoftwareSerial.h>
#include <ZMPT101B.h>
#include <LiquidCrystal_I2C.h>
#define SENSITIVITY 500.0f
SoftwareSerial sim(2, 3);  //Rx , Tx for arduino


// ZMPT101B sensor output connected to analog pin A0
// and the voltage source frequency is 50 Hz.
ZMPT101B voltageSensor(A0, 50.0);

LiquidCrystal_I2C lcd(0x27, 20, 4);
boolean solar_State = false;
boolean lowsms = false;
boolean highsms = false;
boolean gensms = false;
boolean solarsms = false;
boolean eneosms = false;

boolean Autosms = false;
boolean manualsms = false;

int main_contactor = 8;
int solar_contactor = 9;
int Gen_contactor = 10;
int Gen_starter = 11;
int i = 0;
float dc_vout = 0.0;
float dc_vin = 0.0;
float R1 = 31000;
float R2 = 7500;
int value = 0;

int Select= 4;
int Eneo_Select=5;
int Solar_Select=6;
int Gen_Select=7;
int AutomaticLed= A2;
int ManualLed= A3;
boolean Auto = false;
boolean manual = false;

byte Solar[] = {
  B00111,
  B01111,
  B11111,
  B11110,
  B11110,
  B00110,
  B00110,
  B00000
};

byte battery[] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111
};


byte batteryf[] = {
  B01110,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte gen[] = {
  B11111,
  B11111,
  B11000,
  B11000,
  B11011,
  B11001,
  B11111,
  B11111
};

byte gen2[] = {
  B11111,
  B11111,
  B11000,
  B11111,
  B11111,
  B11000,
  B11111,
  B11111
};

byte gen3[] = {
  B10011,
  B11011,
  B11011,
  B11111,
  B11111,
  B11011,
  B11001,
  B11000
};

byte face[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};

byte harth[] = {
  B00000,
  B00000,
  B01010,
  B10101,
  B10001,
  B01010,
  B00100,
  B00000
};


void Solar_check() {


  if (dc_vin >= 0 && dc_vin <= 11) {
    lcd.setCursor(12, 2);
    lcd.write(1);
    lcd.setCursor(0, 2);
    lcd.print("Solar: OFF");
    lcd.setCursor(16, 3);
    lcd.write(3);
    lcd.setCursor(17, 3);
    lcd.write(4);
    lcd.setCursor(18, 3);
    lcd.write(5);
    lcd.setCursor(0, 3);
    lcd.print("GENERATOR: ON");
    SendMessage3();
    if (solar_State == false) {

      Serial.print("INPUT V=");
      Serial.println(dc_vin, 3);
      GenContactor();
      
    }
   
    solar_State = true;
    solarsms = false;
    delay(200);
  }


  if (dc_vin > 11) {
    lcd.setCursor(12, 2);
    lcd.write(2);
    lcd.setCursor(0, 2);
    lcd.print("Solar: ON");
    lcd.setCursor(16, 3);
    lcd.write(3);
    lcd.setCursor(17, 3);
    lcd.write(4);
    lcd.setCursor(18, 3);
    lcd.write(5);
    lcd.setCursor(0, 3);
    lcd.print("GENERATOR: OFF");

    SolarContactor();
    SendMessage4();
    solar_State = false;
    gensms = false;
    delay(200);
  }
}


void SendMessage1() {
  float voltage = voltageSensor.getRmsVoltage();
  Serial.println(voltage);
  if (lowsms == false) {
    Serial.println("Sending Text...");
    sim.print("AT+CMGF=1\r");  // Set the shield to SMS mode
    delay(100);

    sim.print("AT+CMGS=\"+237691419771\"\r");
    delay(200);
    sim.print("Eneo Line has under-voltage ");
    sim.println(" V = ");
    sim.println(voltage);
    sim.println("Switching....");
    sim.print("\r");  //the content of the message
    delay(500);
    sim.print((char)26);  //the ASCII code of the ctrl+z is 26 (required according to the datasheet)
    delay(100);
    sim.println();
    Serial.println("Text Sent.");
    delay(500);
  }

  lowsms = true;
}

void SendMessage2() {
  float voltage = voltageSensor.getRmsVoltage();
  Serial.println(voltage);
  if (highsms == false) {
    Serial.println("Sending Text...");
    sim.print("AT+CMGF=1\r");  // Set the shield to SMS mode
    delay(100);

    sim.print("AT+CMGS=\"+237691419771\"\r");
    delay(200);
    sim.print("Eneo line has over-voltage ");
    sim.println(" V = ");
    sim.println(voltage);
    //sim.print(tempCavg);
    sim.println(" Switching...");
    sim.print("\r");  //the content of the message
    delay(500);
    sim.print((char)26);  //the ASCII code of the ctrl+z is 26 (required according to the datasheet)
    delay(100);
    sim.println();
    Serial.println("Text Sent.");
    delay(500);
  }

  highsms = true;
}


void SendMessage3() {
  if (gensms == false) {
    Serial.println("Sending Text...");
    sim.print("AT+CMGF=1\r");  // Set the shield to SMS mode
    delay(100);

    sim.print("AT+CMGS=\"+237691419771\"\r");
    delay(200);
    sim.print("Back-up system      ");
    sim.println("Generator on");
    sim.print("\r");  //the content of the message
    delay(500);
    sim.print((char)26);  //the ASCII code of the ctrl+z is 26 (required according to the datasheet)
    delay(100);
    sim.println();
    Serial.println("Text Sent.");
    delay(500);
  }

  gensms = true;
}

void SendMessage4() {
  if (solarsms == false) {
    Serial.println("Sending Text...");
    sim.print("AT+CMGF=1\r");  // Set the shield to SMS mode
    delay(100);

    sim.print("AT+CMGS=\"+237691419771\"\r");
    delay(200);
    sim.print("Back-up system    ");
    //sim.print(tempCavg);
    sim.println("Solar System On");
    sim.print("\r");  //the content of the message
    delay(500);
    sim.print((char)26);  //the ASCII code of the ctrl+z is 26 (required according to the datasheet)
    delay(100);
    sim.println();
    Serial.println("Text Sent.");
    delay(500);
  }

  solarsms = true;
}

void SendMessage5() {
  if (eneosms == false) {
    Serial.println("Sending Text...");
    sim.print("AT+CMGF=1\r");  // Set the shield to SMS mode
    delay(100);

    sim.print("AT+CMGS=\"+237691419771\"\r");
    delay(200);
    sim.print("Eneo voltage is Normal ");
    //sim.print(tempCavg);
    //sim.println("Solar System On");
    sim.print("\r");  //the content of the message
    delay(500);
    sim.print((char)26);  //the ASCII code of the ctrl+z is 26 (required according to the datasheet)
    delay(100);
    sim.println();
    Serial.println("Text Sent.");
    delay(500);
  }

  eneosms = true;
}
void SendMessage6()
{
if (Autosms == false)
{
      sim.println("AT+CMGF=1"); // Set SMS mode to text
      delay(1000);
      sim.println("AT+CMGS=\"+237691419771\""); // Replace with recipient phone number
      delay(1000);
      sim.print("Automatic Mode activated"); // SMS message
      sim.write(26); // End SMS
}
Autosms= true;
}
void SendMessage7()
{
  if (manualsms == false)
  {
      sim.println("AT+CMGF=1"); // Set SMS mode to text
      delay(1000);
      sim.println("AT+CMGS=\"+237691419771\""); // Replace with recipient phone number
      delay(1000);
      sim.print("Manual Mode activated"); // SMS message
      sim.write(26); // End SMS
  }
manualsms = true;
}



void setup() {
  sim.begin(9600);

  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  // Change the sensitivity value based on value you got from the calibrate
  // example.
  voltageSensor.setSensitivity(SENSITIVITY);


  lcd.createChar(0, Solar);
  lcd.createChar(1, battery);
  lcd.createChar(2, batteryf);
  lcd.createChar(3, gen);
  lcd.createChar(4, gen2);
  lcd.createChar(5, gen3);
  lcd.createChar(6, face);
  lcd.createChar(7, harth);

  pinMode(main_contactor, OUTPUT);
  pinMode(solar_contactor, OUTPUT);
  pinMode(Gen_contactor, OUTPUT);
  pinMode(Gen_starter, OUTPUT);

  pinMode(Select, INPUT_PULLUP);
  pinMode(Eneo_Select, INPUT_PULLUP);
  pinMode(Solar_Select, INPUT_PULLUP);
  pinMode(Gen_Select, INPUT_PULLUP);

  pinMode(ManualLed, OUTPUT);
  pinMode(AutomaticLed, OUTPUT); 

  digitalWrite(Gen_starter, HIGH);
  digitalWrite(solar_contactor, HIGH);
  digitalWrite(main_contactor, HIGH);
  digitalWrite(Gen_contactor, HIGH);

  sim.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  sim.println("AT+CNMI=2,2,0,0,0"); // Set SMS notification mode

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.setCursor(1, 1);
  lcd.write(0);
  lcd.setCursor(18, 1);
  lcd.write(0);
  lcd.setCursor(19, 1);
  lcd.write(0);
  lcd.setCursor(1, 0);
  lcd.write(6);
  lcd.setCursor(17, 0);
  lcd.write(6);

  lcd.setCursor(0, 0);
  lcd.write(7);
  lcd.setCursor(18, 0);
  lcd.write(7);
  lcd.setCursor(4, 0);
  lcd.print("WELCOME TO..");
  lcd.setCursor(3, 1);
  lcd.print("BACK-UP V 1.0");
  lcd.setCursor(1, 2);
  lcd.print("loading.......");
  lcd.setCursor(1, 3);
  lcd.print("System");
  delay(10000);
}

void loop() {

  if (sim.available()) {
    String sms = sim.readStringUntil('\n');
    if (sms.indexOf("Manual") != -1)
    {
      if (manual== true)
      {
         manual= true;
      }
      else if (manual==false)
      {
        manual= true;
      }
    }
    if (digitalRead(Select)==HIGH)
    {
      manual = true;
    }
  if (manual == true)
  {
    analogWrite(ManualLed, 200);
    analogWrite(AutomaticLed, 0);
    Manual();
    SendMessage6();
      Auto= false;
  }

    if (sms.indexOf("Auto") != -1)
    {
      if (Auto == true)
      {
        Auto= true;
      }
      else if (Auto==false)
      {
        Auto= true;
      }
    }
    if (digitalRead(Select)==LOW)
    {
      Auto = true;
    }

  if (Auto== true)
  {
    analogWrite(ManualLed, 0);
    analogWrite(AutomaticLed, 200);
    Automatic();
    SendMessage7();
      manual = false;
  }
delay(50);

  }
}

void Automatic()
{
  value = analogRead(A1);
  dc_vout = (value * 5.0) / 1024.0;
  dc_vin = dc_vout / (R2 / (R1 + R2));
  float voltage = voltageSensor.getRmsVoltage();
  Serial.println(voltage);
 if ((voltage >= 0) && (voltage < 180)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(0);
    lcd.setCursor(1, 0);
    lcd.write(0);
    lcd.setCursor(18, 0);
    lcd.write(0);
    lcd.setCursor(19, 0);
    lcd.write(0);

    lcd.setCursor(2, 0);
    lcd.print("BACK-UP V1 Auto");
    lcd.setCursor(0, 1);
    lcd.print("Eneo: Under Voltage");
    SendMessage1();
    Solar_check();
    eneosms= false;
  }

  if ((voltage >= 180) && (voltage <= 230)) {

   EneoContactor();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(0);
    lcd.setCursor(1, 0);
    lcd.write(0);
    lcd.setCursor(18, 0);
    lcd.write(0);
    lcd.setCursor(19, 0);
    lcd.write(0);

    lcd.setCursor(16, 3);
    lcd.write(3);
    lcd.setCursor(17, 3);
    lcd.write(4);
    lcd.setCursor(18, 3);
    lcd.write(5);

    lcd.setCursor(2, 0);
    lcd.print("BACK-UP V1 Auto");
    lcd.setCursor(0, 1);
    lcd.print("Eneo: Normal Voltage");
    lcd.setCursor(0, 2);
    lcd.print("Solar: OFF");
    lcd.setCursor(0, 3);
    lcd.print("GENERATOR: OFF");
    delay(200);
    SendMessage5();
    lowsms = false;
    highsms = false;
    gensms = false;
    solarsms = false;
  }

  if (voltage > 230) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(0);
    lcd.setCursor(1, 0);
    lcd.write(0);
    lcd.setCursor(18, 0);
    lcd.write(0);
    lcd.setCursor(19, 0);
    lcd.write(0);

    lcd.setCursor(2, 0);
    lcd.print("BACK-UP V1 Auto");
    lcd.setCursor(0, 1);
    lcd.print("Eneo: Over Voltage");

    SendMessage2();
    Solar_check();
    eneosms= false;
  }

  delay(1000);
}

void EneoContactor()
{
    digitalWrite(Gen_starter, HIGH);
    digitalWrite(solar_contactor, HIGH);
    digitalWrite(main_contactor, LOW);
    digitalWrite(Gen_contactor, HIGH);
}
void SolarContactor()
{
    digitalWrite(Gen_starter, HIGH);
    digitalWrite(solar_contactor, LOW);
    digitalWrite(main_contactor, HIGH);
    digitalWrite(Gen_contactor, HIGH);
}

void GenContactor()
{
      digitalWrite(Gen_starter, LOW);
      digitalWrite(solar_contactor, HIGH);
      digitalWrite(main_contactor, HIGH);
      digitalWrite(Gen_contactor, HIGH);
      delay(4000);
      digitalWrite(Gen_starter, HIGH);
      digitalWrite(solar_contactor, HIGH);
      digitalWrite(main_contactor, HIGH);
      digitalWrite(Gen_contactor, LOW);
}

void Manual()
{
  if (digitalRead(Eneo_Select)==HIGH)
    {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(0);
    lcd.setCursor(1, 0);
    lcd.write(0);
    lcd.setCursor(18, 0);
    lcd.write(0);
    lcd.setCursor(19, 0);
    lcd.write(0);

    lcd.setCursor(16, 3);
    lcd.write(3);
    lcd.setCursor(17, 3);
    lcd.write(4);
    lcd.setCursor(18, 3);
    lcd.write(5);

    lcd.setCursor(2, 0);
    lcd.print("BACK-UP V1 Manl");
    lcd.setCursor(0, 1);
    lcd.print("Eneo: ON");
    lcd.setCursor(0, 2);
    lcd.print("Solar: OFF");
    lcd.setCursor(0, 3);
    lcd.print("GENERATOR: OFF");
    delay(200);
      EneoContactor();
    }
    if (digitalRead(Solar_Select)==HIGH)
    {
          lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(0);
    lcd.setCursor(1, 0);
    lcd.write(0);
    lcd.setCursor(18, 0);
    lcd.write(0);
    lcd.setCursor(19, 0);
    lcd.write(0);

    lcd.setCursor(16, 3);
    lcd.write(3);
    lcd.setCursor(17, 3);
    lcd.write(4);
    lcd.setCursor(18, 3);
    lcd.write(5);

    lcd.setCursor(2, 0);
    lcd.print("BACK-UP V1 Manl");
    lcd.setCursor(0, 1);
    lcd.print("Eneo: OFF");
    lcd.setCursor(0, 2);
    lcd.print("Solar: ON");
    lcd.setCursor(0, 3);
    lcd.print("GENERATOR: OFF");
    delay(200);

      SolarContactor();
    }

     if (digitalRead(Gen_Select)==HIGH)
    {
      
        lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(0);
    lcd.setCursor(1, 0);
    lcd.write(0);
    lcd.setCursor(18, 0);
    lcd.write(0);
    lcd.setCursor(19, 0);
    lcd.write(0);

    lcd.setCursor(16, 3);
    lcd.write(3);
    lcd.setCursor(17, 3);
    lcd.write(4);
    lcd.setCursor(18, 3);
    lcd.write(5);

    lcd.setCursor(2, 0);
    lcd.print("BACK-UP V1 Manl");
    lcd.setCursor(0, 1);
    lcd.print("Eneo: OFF");
    lcd.setCursor(0, 2);
    lcd.print("Solar: OFF");
    lcd.setCursor(0, 3);
    lcd.print("GENERATOR: ON");
    delay(200);
    
      GenContactor();
    }
}