#include <U8g2lib.h>
#include <U8x8lib.h>
#include <MUIU8g2.h>
#include <Arduino.h>
#include <SPI.h>

#define NUM_INPUT_PINS 7
#define ENABLE_PIN 8

volatile bool dataReady = false;
volatile uint8_t dataBuffer;
volatile int bufferIndex = 0;
volatile bool enabled = false;

U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 13, /* dc=*/ 12, /* reset=*/ 11);
    
int left_Pot = 0;
int right_Pot = 0;

int left_potMapped = 0;
int right_potMapped = 0;

int left_cpuPos = 0;
int right_cpuPos = 0;

int left_cpuSpeed = 0;
int right_cpuSpeed = 0;

int left_servoPos = 0;
int left_rightPos = 0;

bool left_up_limit = 0;
bool right_up_limit = 0;

bool left_down_limit = 0;
bool right_down_limit = 0;

int printTimer =0 ;

void setup() {

  for (int i = 2; i < 10; i++) {
    pinMode(i, INPUT_PULLUP);
  }  
  // put your setup code here, to run once:
  u8g2.begin();
  Serial.begin(115200);
  enabled=true;
  attachInterrupt(digitalPinToInterrupt(3), dataInterrupt, CHANGE);
}


void renderDisplayPage() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(10,7,"G-Loc Motion Controller");

    char cstr[10];
    sprintf(cstr,"L Speed %02d", left_cpuSpeed);
    u8g2.drawStr(3,48, cstr);
    sprintf(cstr,"R Speed %02d", right_cpuSpeed);
    u8g2.drawStr(3,58, cstr);

    u8g2.setFont(u8g2_font_8x13_t_symbols);

    // RIGHT
    right_up_limit ? u8g2.drawGlyph(115, 22, 0x25b2) : u8g2.drawGlyph(115, 22, 0x25b3);
    right_down_limit ? u8g2.drawGlyph(2, 22, 0x25bc) : u8g2.drawGlyph(2, 22, 0x25bd);


    // Right
    left_up_limit ? u8g2.drawGlyph(115, 36, 0x25b2) : u8g2.drawGlyph(115, 36, 0x25b3);
    left_down_limit ? u8g2.drawGlyph(2, 36, 0x25bc) : u8g2.drawGlyph(2, 36, 0x25bd);

    // Gages
    // RIGHT
    u8g2.setFont(u8g2_font_unifont_t_75);
    // gadges
    u8g2.drawGlyph(12 + (right_cpuPos * 3), 19, 0x25be);
    u8g2.drawGlyph(12 + (right_potMapped * 3), 27, 0x25b5);
    // position marks x30
    for (int i = 0; i < 32; i++) {
      u8g2.drawVLine(16 + (i*3), 17, 3);
    }
    // LEFT
    u8g2.setFont(u8g2_font_unifont_t_75);
    // gadges
    u8g2.drawGlyph(12 + (left_cpuPos * 3), 32, 0x25be);
    u8g2.drawGlyph(12 + (left_potMapped * 3), 40, 0x25b5);
    // position marks x30
    for (int i = 0; i < 32; i++) {
      u8g2.drawVLine(16 + (i*3), 30, 3);
    }



  } while ( u8g2.nextPage() );
}


void enableInterrupt() {
  // Check if enable pin is HIGH
  if (digitalRead(ENABLE_PIN) == HIGH) {
    // Enable data reception
    enabled = true;
  } else {
    // Disable data reception
    enabled = false;
  }
}

void dataInterrupt() {
  //Serial.println("INT");
  if (enabled) {
    // Read data from input pins and store in buffer
    uint8_t newData = 0;
    for (int i = 0; i < 7; i++) {
      newData |= !digitalRead(8-i) << i;
    }
    dataBuffer = newData;
    dataReady = true;
  }
}

void loop() {
  if (dataReady) {
    // Process the data in the buffer
    if (dataBuffer < 32) {
      right_cpuPos = dataBuffer;
    } else if (dataBuffer > 31 && dataBuffer < 40) {
      right_cpuSpeed = dataBuffer - 32;
    } else if (dataBuffer > 63 && dataBuffer < 96) {
      left_cpuPos = dataBuffer - 64;
    } else if (dataBuffer > 95 && dataBuffer < 104) {
      left_cpuSpeed = dataBuffer - 96;
    }      
    /*
    printTimer++;
    if (printTimer > 5) {
          Serial.print("RIGHT: position [");
          Serial.print(right_cpuPos, HEX);  
          Serial.print("]   speed [");
          Serial.print(right_cpuSpeed, HEX);  
          Serial.println("]");
          Serial.println("");


          Serial.println("");
          Serial.print("LEFT : position [");
          Serial.print(left_cpuPos, HEX);  
          Serial.print("]   speed [");
          Serial.print(left_cpuSpeed, HEX);  
          Serial.println("]");


          printTimer=0;
    }
    */
    // Reset dataReady flag
    dataReady = false;
  }


  // put your main code here, to run repeatedly:
  renderDisplayPage();
  delay(1);
}
