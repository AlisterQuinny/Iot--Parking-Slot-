#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

Servo entryServo;
Servo exitServo;

// ---------------- PINS ----------------
#define ENTRY_SERVO 9
#define EXIT_SERVO 11

#define IR_ENTRY 2
#define IR_EXIT 3

#define S1 4
#define S2 5
#define S3 6
#define S4 7
#define S5 8
#define S6 10

// ------------- SETTINGS ---------------
#define ENTRY_OPEN_ANGLE 120
#define ENTRY_CLOSE_ANGLE 0

#define EXIT_OPEN_ANGLE 0     // reversed direction
#define EXIT_CLOSE_ANGLE 120

// ------------- STATES -----------------
bool entryOpen = false;
bool exitOpen  = false;

// -------- SCROLL TEXT -----------------
String scrollText = "   Welcome to VisionSlot   ";
int scrollIndex = 0;
unsigned long lastScroll = 0;

// =====================================
void setup()
{
  lcd.init();
  lcd.backlight();

  entryServo.attach(ENTRY_SERVO);
  exitServo.attach(EXIT_SERVO);

  entryServo.write(ENTRY_CLOSE_ANGLE);
  exitServo.write(EXIT_CLOSE_ANGLE);

  // Stable IR signals
  pinMode(IR_ENTRY, INPUT_PULLUP);
  pinMode(IR_EXIT, INPUT_PULLUP);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);
  pinMode(S6, INPUT);

  lcd.clear();
}

// =====================================
void loop()
{
  // ---------- READ SLOTS ----------
  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);
  int s4 = digitalRead(S4);
  int s5 = digitalRead(S5);
  int s6 = digitalRead(S6);

  int available = 0;

  if(s1) available++;
  if(s2) available++;
  if(s3) available++;
  if(s4) available++;
  if(s5) available++;
  if(s6) available++;

  // ---------- ENTRY GATE ----------
  if(digitalRead(IR_ENTRY) == LOW && available > 0)
  {
    entryServo.write(ENTRY_OPEN_ANGLE);
    entryOpen = true;
  }
  else if(digitalRead(IR_ENTRY) == HIGH)
  {
    entryServo.write(ENTRY_CLOSE_ANGLE);
    entryOpen = false;
  }

  // ---------- EXIT GATE ----------
  if(digitalRead(IR_EXIT) == LOW)
  {
    exitServo.write(EXIT_OPEN_ANGLE);
    exitOpen = true;
  }
  else if(digitalRead(IR_EXIT) == HIGH)
  {
    exitServo.write(EXIT_CLOSE_ANGLE);
    exitOpen = false;
  }

  // ---------- LCD DISPLAY ----------
  updateScroll();
  updateLCD(available, s1,s2,s3,s4,s5,s6);

  delay(100);
}

// =====================================
// SCROLLING HEADER
// =====================================
void updateScroll()
{
  if(millis() - lastScroll > 300)
  {
    lcd.setCursor(0,0);

    String part = scrollText.substring(scrollIndex) +
                  scrollText.substring(0, scrollIndex);

    lcd.print(part.substring(0,20));

    scrollIndex++;
    if(scrollIndex >= scrollText.length())
      scrollIndex = 0;

    lastScroll = millis();
  }
}

// =====================================
// LCD BODY
// =====================================
void updateLCD(int available,
               int s1,int s2,int s3,
               int s4,int s5,int s6)
{
  // Row 2
  lcd.setCursor(0,1);
  lcd.print("Slots Available:   ");
  lcd.setCursor(17,1);
  lcd.print(available);

  if(available == 0)
  {
    lcd.setCursor(0,2);
    lcd.print("PARKING FULL   ");

    lcd.setCursor(0,3);
    lcd.print("Entry Blocked       ");
  }
  else
  {
    // Row 3
    lcd.setCursor(0,2);
    lcd.print("P1:"); lcd.print(s1?"E ":"F ");
    lcd.print("P2:"); lcd.print(s2?"E ":"F ");
    lcd.print("P3:"); lcd.print(s3?"E ":"F ");

    // Row 4
    lcd.setCursor(0,3);
    lcd.print("P4:"); lcd.print(s4?"E ":"F ");
    lcd.print("P5:"); lcd.print(s5?"E ":"F ");
    lcd.print("P6:"); lcd.print(s6?"E ":"F ");
  }
}
