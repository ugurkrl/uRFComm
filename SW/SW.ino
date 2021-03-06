#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <U8g2lib.h>
#include <HDQ.h>

bool line1[10];
bool line2[10];
bool line3[10];
const char shiftkeys[30] PROGMEM = {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ' ', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ' ', ' ', ' '};
const char keys[30] PROGMEM = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ' ', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ' ', ' ', ' '};

byte packetbuffer[50];
int bufferindex;
int latchtime = 100;
bool latch = false;
bool alt, shift, enter, del = false;
unsigned long lastlatch = 0;
int MSB, LSB;
int soc, tte;
float volt;
int displine = 1;
U8G2_SSD1306_128X64_NONAME_1_HW_I2C oled(U8G2_R0);
HDQ gg(PIN_PD6);
void setup() {
  Serial.begin(115200);
  Serial.println("sup");
  pinMode(PIN_PD3, OUTPUT);
  pinMode(PIN_PD4, OUTPUT);
  pinMode(PIN_PE0, OUTPUT);
  pinMode(PIN_PE1, OUTPUT);
  pinMode(PIN_PD5, OUTPUT);
  pinMode(PIN_PD7, INPUT_PULLUP);
  pinMode(PIN_PB0, INPUT_PULLUP);
  pinMode(PIN_PB1, INPUT_PULLUP);
  pinMode(PIN_PC2, INPUT_PULLUP);
  pinMode(PIN_PC1, INPUT_PULLUP);
  pinMode(PIN_PC0, INPUT_PULLUP);
  oled.begin();
  // attachInterrupt(1, sendbuffer, RISING);
  cc1101set();
  delay(50);


}

void loop() {
  scankey(line1 , line2, line3);
  printbuffer();

  LSB = gg.read(0x2c); //SoC oku
  MSB = gg.read(0x2d);
  soc = word(MSB, LSB);

  LSB = gg.read(0x16); //TTE oku
  MSB = gg.read(0x17);
  tte = word(MSB, LSB);

  LSB = gg.read(8); //Read voltage
  MSB = gg.read(9);
  int volttemp = word(MSB, LSB);
  volt = volttemp * 0.001;
  int lineht = oled.getMaxCharHeight();

  if (ELECHOUSE_cc1101.CheckRxFifo(500)) {
    if (ELECHOUSE_cc1101.CheckCRC()) {
      for (int i = 0; i < 50; i++)
      {
        packetbuffer[i] = 0;
      }
      Serial.print("Rssi: ");
      Serial.println(ELECHOUSE_cc1101.getRssi());

      //Link Quality Indicator
      Serial.print("LQI: ");
      Serial.println(ELECHOUSE_cc1101.getLqi());

      //Get received Data and calculate length
      int len = ELECHOUSE_cc1101.ReceiveData(packetbuffer);

      //Print received in char format.
      Serial.println((char *) packetbuffer);

      Serial.println();
    }
  }

  oled.firstPage();
  do {


    oled.setFont(u8g2_font_helvB10_tr);
    oled.setCursor(2, lineht);//pil y??zdesi
    oled.print("%");
    oled.print(soc);
    oled.setCursor(80, lineht);//biti??e kalan zaman
    oled.print(tte);
    oled.print("min"); //Saat format??na ??evirilecek


    oled.drawFrame(0, 16, 128, 48);
    displine = 0;     //Buffer?? yazd??r
    oled.setCursor(2, 30);
    for (int i = 0; i < 50; i++) {
      oled.print((char)packetbuffer[i]);
      if (oled.tx > 110) {
        displine = displine + 1;
        oled.setCursor(2, 30 + (lineht * displine));
        oled.tx = 2;
      }
    }
  }
  while ( oled.nextPage() );

}

void scankey(bool * m1, bool * m2, bool * m3) {

  digitalWrite(PIN_PD3, LOW); //KBA
  digitalWrite(PIN_PD4, HIGH); //KBB
  digitalWrite(PIN_PE0, HIGH); //KBC
  digitalWrite(PIN_PE1, HIGH); //KBD
  digitalWrite(PIN_PD5, HIGH); //KBE
  m1[0] = digitalRead(PIN_PD7); //KB0
  m1[1] = digitalRead(PIN_PB0); //KB1
  m1[2] = digitalRead(PIN_PB1); //KB2
  m1[3] = digitalRead(PIN_PC2); //KB3
  m1[4] = digitalRead(PIN_PC1); //KB4
  m1[5] = digitalRead(PIN_PC0); //KB5

  digitalWrite(PIN_PD3, HIGH); //KBA
  digitalWrite(PIN_PD4, LOW); //KBB
  digitalWrite(PIN_PE0, HIGH); //KBC
  digitalWrite(PIN_PE1, HIGH); //KBD
  digitalWrite(PIN_PD5, HIGH); //KBE
  m2[0] = digitalRead(PIN_PD7); //KB0
  m2[1] = digitalRead(PIN_PB0); //KB1
  m2[2] = digitalRead(PIN_PB1); //KB2
  m2[3] = digitalRead(PIN_PC2); //KB3
  m2[4] = digitalRead(PIN_PC1); //KB4
  m2[5] = digitalRead(PIN_PC0); //KB5

  digitalWrite(PIN_PD3, HIGH); //KBA
  digitalWrite(PIN_PD4, HIGH); //KBB
  digitalWrite(PIN_PE0, LOW); //KBC
  digitalWrite(PIN_PE1, HIGH); //KBD
  digitalWrite(PIN_PD5, HIGH); //KBE
  m3[0] = digitalRead(PIN_PD7); //KB0
  m3[1] = digitalRead(PIN_PB0); //KB1
  m3[2] = digitalRead(PIN_PB1); //KB2
  m3[3] = digitalRead(PIN_PC2); //KB3
  m3[4] = digitalRead(PIN_PC1); //KB4
  m3[5] = digitalRead(PIN_PC0); //KB5

  digitalWrite(PIN_PD3, HIGH); //KBA
  digitalWrite(PIN_PD4, HIGH); //KBB
  digitalWrite(PIN_PE0, HIGH); //KBC
  digitalWrite(PIN_PE1, LOW); //KBD
  digitalWrite(PIN_PD5, HIGH); //KBE
  m1[6] = digitalRead(PIN_PD7); //KB0
  m2[6] = digitalRead(PIN_PB0); //KB1
  m3[6] = digitalRead(PIN_PB1); //KB2
  m3[7] = digitalRead(PIN_PC2); //KB3
  m1[7] = digitalRead(PIN_PC1); //KB4
  m2[7] = digitalRead(PIN_PC0); //KB5

  digitalWrite(PIN_PD3, HIGH); //KBA
  digitalWrite(PIN_PD4, HIGH); //KBB
  digitalWrite(PIN_PE0, HIGH); //KBC
  digitalWrite(PIN_PE1, HIGH); //KBD
  digitalWrite(PIN_PD5, LOW); //KBE
  m1[8] = digitalRead(PIN_PD7); //KB0
  m1[9] = digitalRead(PIN_PB0); //KB1
  m2[9] = digitalRead(PIN_PB1); //KB2
  m2[8] = digitalRead(PIN_PC2); //KB3
  m3[9] = digitalRead(PIN_PC1); //KB4
  m3[8] = digitalRead(PIN_PC0); //KB5


}

void printbuffer() {
  int j;
  if (latch == false) {
    if (line2[9] == 0) { //Del
      bufferindex = bufferindex - 1;
      packetbuffer[bufferindex] = 0;
      lastlatch = millis();
      latch = true;
    }
    if (line3[7] == 0) { //Shift
      shift = !shift;
      lastlatch = millis();
      latch = true;
    }
    if (line3[8] == 0) { //Alt
      alt = !alt;
      lastlatch = millis();
      latch = true;
    }
    if (line3[9] == 0) { //Enter
      sendbuffer();
      lastlatch = millis();
      latch = true;
    }

    if (line3[4] == 0  & line3[5] == 0) { //Space
      packetbuffer[bufferindex] = ' ';
      bufferindex = bufferindex + 1;
      lastlatch = millis();
      latch = true;
      return;
    }

    for (int i = 0; i < 10; i++)
    {
      if (line1[i] == 0) {
        if (shift == true) {
          packetbuffer[bufferindex] = pgm_read_byte_near(shiftkeys + i);

        }
        else {
          packetbuffer[bufferindex] = pgm_read_byte_near(keys + i);
        }
        bufferindex = bufferindex + 1;
        if (bufferindex >= 50) {
          bufferindex = 50;
        }

        latch = true;
        lastlatch = millis();
      }

      if (line2[i] == 0 & i != 9) {
        if (shift == true) {
          packetbuffer[bufferindex] = pgm_read_byte_near(shiftkeys + i + 10);

        }
        else {
          packetbuffer[bufferindex] = pgm_read_byte_near(keys + i + 10);
        }
        bufferindex = bufferindex + 1;
        if (bufferindex >= 50) {
          bufferindex = 50;
        }

        latch = true;
        lastlatch = millis();
      }

      if (line3[i] == 0 & i != 7 & i != 8 & i != 9) {
        if (shift == true) {
          packetbuffer[bufferindex] = pgm_read_byte_near(shiftkeys + i + 20);

        }
        else {
          packetbuffer[bufferindex] = pgm_read_byte_near(keys + i + 20);
        }
        bufferindex = bufferindex + 1;
        if (bufferindex >= 50) {
          bufferindex = 50;
        }



        latch = true;
        lastlatch = millis();
      }
    }
  }
  if (latch == true && (millis() >= lastlatch + latchtime ) ) {
    latch = false;
  }
}

void sendbuffer() {  //butona bas??ld??????nda buffer aktar
  for (int i = 0; i < 50; i++)
  {
    Serial.print((char)packetbuffer[i]);
  }
  packetbuffer[bufferindex] = '\0';
  ELECHOUSE_cc1101.SendData(packetbuffer, bufferindex, 50);
  for (int i = 0; i < 50; i++)
  {
    packetbuffer[i] = 0;
  }
  bufferindex = 0;
  Serial.println();


}



void cc1101set() {
  ELECHOUSE_cc1101.Init();                // must be set to initialize the cc1101!
  ELECHOUSE_cc1101.setCCMode(1);          // set config for internal transmission mode.
  ELECHOUSE_cc1101.setModulation(1);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
  ELECHOUSE_cc1101.setMHZ(433.92);        // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
  ELECHOUSE_cc1101.setDeviation(47.60);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
  ELECHOUSE_cc1101.setChannel(0);         // Set the Channelnumber from 0 to 255. Default is cahnnel 0.
  ELECHOUSE_cc1101.setChsp(199.95);       // The channel spacing is multiplied by the channel number CHAN and added to the base frequency in kHz. Value from 25.39 to 405.45. Default is 199.95 kHz.
  ELECHOUSE_cc1101.setRxBW(812.50);       // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  ELECHOUSE_cc1101.setDRate(99.97);       // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
  ELECHOUSE_cc1101.setPA(12);             // Set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
  ELECHOUSE_cc1101.setSyncMode(2);        // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
  ELECHOUSE_cc1101.setSyncWord(211, 145); // Set sync word. Must be the same for the transmitter and receiver. (Syncword high, Syncword low)
  ELECHOUSE_cc1101.setAdrChk(0);          // Controls address check configuration of received packages. 0 = No address check. 1 = Address check, no broadcast. 2 = Address check and 0 (0x00) broadcast. 3 = Address check and 0 (0x00) and 255 (0xFF) broadcast.
  ELECHOUSE_cc1101.setAddr(0);            // Address used for packet filtration. Optional broadcast addresses are 0 (0x00) and 255 (0xFF).
  ELECHOUSE_cc1101.setWhiteData(1);       // Turn data whitening on / off. 0 = Whitening off. 1 = Whitening on.
  ELECHOUSE_cc1101.setPktFormat(0);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
  ELECHOUSE_cc1101.setLengthConfig(1);    // 0 = Fixed packet length mode. 1 = Variable packet length mode. 2 = Infinite packet length mode. 3 = Reserved
  ELECHOUSE_cc1101.setPacketLength(0);    // Indicates the packet length when fixed packet length mode is enabled. If variable packet length mode is used, this value indicates the maximum packet length allowed.
  ELECHOUSE_cc1101.setCrc(1);             // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
  ELECHOUSE_cc1101.setCRC_AF(0);          // Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.
  ELECHOUSE_cc1101.setDcFilterOff(0);     // Disable digital DC blocking filter before demodulator. Only for data rates ??? 250 kBaud The recommended IF frequency changes when the DC blocking is disabled. 1 = Disable (current optimized). 0 = Enable (better sensitivity).
  ELECHOUSE_cc1101.setManchester(0);      // Enables Manchester encoding/decoding. 0 = Disable. 1 = Enable.
  ELECHOUSE_cc1101.setFEC(0);             // Enable Forward Error Correction (FEC) with interleaving for packet payload (Only supported for fixed packet length mode. 0 = Disable. 1 = Enable.
  ELECHOUSE_cc1101.setPRE(7);             // Sets the minimum number of preamble bytes to be transmitted. Values: 0 : 2, 1 : 3, 2 : 4, 3 : 6, 4 : 8, 5 : 12, 6 : 16, 7 : 24
  ELECHOUSE_cc1101.setPQT(0);             // Preamble quality estimator threshold. The preamble quality estimator increases an internal counter by one each time a bit is received that is different from the previous bit, and decreases the counter by 8 each time a bit is received that is the same as the last bit. A threshold of 4???PQT for this counter is used to gate sync word detection. When PQT=0 a sync word is always accepted.
  ELECHOUSE_cc1101.setAppendStatus(0);    // When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.
  if (ELECHOUSE_cc1101.getCC1101()) {     // Check the CC1101 Spi connection.
    Serial.println("Connection OK");
  } else {
    Serial.println("Connection Error");
  }
}
