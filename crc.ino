

int h_s = 0;
int i = 0;

char main_buffer[256] = { 0x01, 0x02, 0x03, 0x04 };  // тут создаем болванку под максимально возможный размер плюс преамбула уже есть

byte addr_rcv = 0x01;
byte addr_snd = 0x01;
byte len = 0;
bool rstart = 0;


void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  Serial.write('1');
}

byte crc8(byte *buffer, byte size) {  //рассчет CRC содрано у гивера) хотя есть для ардуины полно библиотек и такого и табличного
  byte crc = 0;
  for (byte i = 0; i < size; i++) {
    byte data = buffer[i];
    for (int j = 8; j > 0; j--) {
      crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);  //тут полином под даллас считалось на передатчике по тому же алгоритму
      data >>= 1;
    }
  }
  return crc;
}



void loop() {
  if (Serial.available() > 0) {
    char rr = Serial.read();



    if (rr == 0x04 && h_s == 3) {
      h_s = 0;

      int start_block = Serial.readBytes((byte *)&main_buffer[4], 3);                          //тут неплохо убедиться что принято все три байта - но я не стал)
      if (main_buffer[5] == addr_snd && main_buffer[6] == addr_rcv) { len = main_buffer[4]; }  //убеждаемся что адреса наши тогда присваиваем длину сообщения
      

      if (len) {
        Serial.setTimeout(1000);



        int main_block = Serial.readBytes((byte *)&main_buffer[7], len);  //подразумеваем что передали длину только основной части + CRC

        


        byte crc = crc8((byte *)&main_buffer, len + 7);  // считаем crc посылки полностью

        if (crc == 0) {
          // данные верны
        } else {
          digitalWrite(13, HIGH);  // данные повреждены
        }
      }
    }
    if (rr == 0x03 && h_s == 2) { h_s = 3; }
    if (rr == 0x02 && h_s == 1) { h_s = 2; }
    if (rr == 0x01) { h_s = 1; }
  }
}
