#include <U8g2lib.h>

bool matrix1[2];
bool matrix2[2];
char keys1[] = {'B','C'};
char keys2[] = {'A','D'};
int latchtime = 100;
bool latch=false;
unsigned long lastlatch = 0;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0);

void setup(){
  Serial.begin(9600);
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  oled.begin();
  oled.setCursor(0,16);


}

void loop(){
  
    oled.setDrawColor(1);
    oled.setFont(u8g2_font_helvB10_tr);

scankey(matrix1 , matrix2);
printbuffer();
Serial.println(lastlatch);
Serial.println(latchtime);
  oled.sendBuffer();
}

void scankey(bool *m1,bool *m2){

digitalWrite(A0,LOW);
digitalWrite(A1,HIGH);
m1[0]=digitalRead(8);
m1[1]=digitalRead(9);
digitalWrite(A0,HIGH);
digitalWrite(A1,LOW);
m2[0]=digitalRead(8);
m2[1]=digitalRead(9);
}

void printbuffer(){
if(latch == false){
    for (int i=0; i<2; i++)  
    {
    if(matrix1[i]==0){
      Serial.println(keys1[i]);
      oled.print(keys1[i]);
      latch=true;
      lastlatch = millis();
    }
    }
    
    for (int i=0; i<2; i++)  
    {
    if(matrix2[i]==0){
      Serial.println(keys2[i]);
      oled.print(keys2[i]);
      latch=true;
      lastlatch = millis();
    }
    }
}
if(latch == true && (millis() >= lastlatch + latchtime ) ){
    latch = false;
}
}
