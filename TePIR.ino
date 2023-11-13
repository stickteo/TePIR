// TePIR
// teod 2023-11-12

#include <TimerOne.h>
#include <TimerThree.h>

uint8_t ser_buf[1024];
uint16_t ser_sta;
uint16_t ser_end;

uint8_t decode_state;
uint16_t preamble[4];

uint16_t pairs;
uint16_t pair_cur;
uint16_t period;
uint16_t pulse[128], space[128];

uint32_t time_sta, time_cur;

uint32_t us_min_repeat, time_sta2;

uint16_t hex2int(uint16_t c) {
  if(c >= '0' && c <= '9') {
    return c-'0';
  }
  if(c >= 'a' && c <= 'f') {
    return c-'a'+10;
  }
  if(c >= 'A' && c <= 'F') {
    return c-'A'+10;
  }
  return 0;
}

uint8_t ser_get() {
  uint8_t out;
  out = ser_buf[ser_sta];
  ser_sta = (ser_sta+1)&1023;
  return out;
}

uint16_t readhex() {
  uint16_t a, b, c, d, out;
  a = ser_get();
  b = ser_get();
  c = ser_get();
  d = ser_get();

  out = (hex2int(a)<<12) + (hex2int(b)<<8) + (hex2int(c)<<4) + hex2int(d);
  return out;
}

uint16_t checkspace() {
  if(ser_get() != ' ') {
    Serial.println("Decode error, expecting space!");
    ser_sta = ser_end;
    decode_state = 0;
    return 1;
  }

  return 0;
}

uint16_t period2ms(uint16_t n) {
  return (n*31+64)/128;
}

/*
void timer3_done() {
  Timer3.detachInterrupt();
  decode_state = 1;
}

void timer3_space() {
  Timer1.disablePwm(4);
  Timer3.detachInterrupt();
  Timer3.attachInterrupt(timer3_done);
  Timer3.initialize(space);
}
*/

void setup() {
  Serial.begin(115200);
  ser_sta = 0;
  ser_end = 0;

  decode_state = 0;

  pinMode(4, OUTPUT);
  digitalWrite(4, 0);

  pinMode(11, OUTPUT);
  digitalWrite(11, 0);
  
  //Timer1.initialize(25);
  //Timer1.pwm(4, 512);

  //Timer1.disablePwm(4);

  us_min_repeat = 1000000; //1s
}

void loop() {
  while(Serial.available() && ser_end != ((ser_sta-16)&1023)) {
    ser_buf[ser_end] = Serial.read();
    ser_end = (ser_end+1)&1023;
  }

  /*
  if(ser_end - ser_sta) {
    if(ser_end > ser_sta) {
      Serial.write(ser_buf+ser_sta, ser_end-ser_sta);
    } else {
      Serial.write(ser_buf+ser_sta, 1024-ser_sta);
      Serial.write(ser_buf, ser_end);
    }
    ser_sta = ser_end;
  }
  */

  uint16_t len = (ser_end-ser_sta)&1023;
  if(decode_state == 0 && len>=19) {
    preamble[0] = readhex();
    if(checkspace()) return;
    preamble[1] = readhex();
    if(checkspace()) return;
    preamble[2] = readhex();
    if(checkspace()) return;
    preamble[3] = readhex();

    if(preamble[0] != 0) {
      Serial.printf("Bad preamble!");
    }

    pairs = preamble[2]+preamble[3];
    //period = (preamble[1]*31+64)/128;
    period = period2ms(preamble[1]);
    pair_cur = 0;
    
    Serial.print("Carrier Period (us): ");
    Serial.println(period,DEC);
    Serial.print("Non-repeating Pairs: ");
    Serial.println(preamble[2],DEC);
    Serial.print("Burst Pairs: ");
    Serial.println(preamble[3],DEC);


    decode_state = 1;

    Timer1.initialize(period);
    //Timer1.pwm(4, 512);
  }

  if(decode_state == 1 && len>=10 && pair_cur<pairs) {
    //digitalWrite(11, 1);
    //Timer1.pwm(4, 512);
    
    if(checkspace()) return;
    pulse[pair_cur] = readhex()*period;
    if(checkspace()) return;
    space[pair_cur] = readhex()*period;

    /*
    Serial.print(pulse, DEC);
    Serial.print(' ');
    Serial.println(space, DEC);
    */
    
    /*
    pairs--;
    if(pairs){
      decode_state = 2;
      
      Timer1.initialize(period);
      Timer1.pwm(4, 512);
      Timer3.initialize(pulse);
      Timer3.attachInterrupt(timer3_space);
    } else {
      decode_state = 0;
      Serial.println("Done send!");
    }
    */
    
    //digitalWrite(11, 1);
    //Timer1.initialize(period);
    //Timer1.pwm(4, 512);
    //time_sta = time_cur;
    

    //pairs--;
    //decode_state = 2;

    pair_cur++;
    if(pair_cur == pairs) {
      Timer1.initialize(period);
      Timer1.pwm(4,0);
      time_sta2 = micros();
      pair_cur = 0;
      decode_state = 2;
    }
  }

  time_cur = micros();

  if(decode_state == 2) {
    if(pair_cur == 0) {
      time_sta = time_cur;
    } else {
      time_sta = time_sta + space[pair_cur-1];
    }
    digitalWrite(11, 1);
    Timer1.setPwmDuty(4,512);

    decode_state = 3;
  }

  if(decode_state == 3 && (time_cur-time_sta)>=pulse[pair_cur]) {
    Timer1.setPwmDuty(4,0);
    digitalWrite(11, 0);
    //time_sta = time_cur;
    time_sta = time_sta + pulse[pair_cur];
    decode_state = 4;
  }

  if(decode_state == 4 && (time_cur-time_sta)>=space[pair_cur]) {
    if(pair_cur < pairs) {
      decode_state = 2;
      pair_cur++;
    } else if((time_cur-time_sta2) < us_min_repeat) {
      decode_state = 2;
      pair_cur = 0;
    } else {
      Serial.println("Done!");
      ser_sta = ser_end;
      decode_state = 0;
    }
  }

  /*
  if(decode_state == 2 && (time_cur-time_sta)>=pulse) {
    Timer1.disablePwm(4);
    digitalWrite(11, 0);
    time_sta = time_cur;
    decode_state = 3;
  }

  if(decode_state == 3 && (time_cur-time_sta)>=space) {
    if(pairs) {
      decode_state = 1;
    } else {
      Serial.println("Done!");
      ser_sta = ser_end;
      decode_state = 0;
    }
  }
  */

  
}
