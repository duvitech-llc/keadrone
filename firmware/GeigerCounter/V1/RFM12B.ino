#ifdef PCB_MOUNTED_WIRELESS_RFM12B
#include "RF12.h"
void RFM12B_setup(void)
{
  // overide SS pin to another CS pin
  rf12_set_cs(8); 
  rf12_initialize(1, RF12_433MHZ, 33);
  Serial.println("RF12 initialized\n");

  // restore SS pin to output for PWM usage
  pinMode(10, OUTPUT);
}

void RFM12B_loop(void)
{

  unsigned char *ptr = (unsigned char*) &sensor;

  Serial.print("\nRfm12Status0: ");
  unsigned int rfm12_status = rf12_control(0);
  Serial.print(rfm12_status);
  Serial.print(",");
  Serial.print("\n");

  if (rf12_recvDone() && rf12_crc == 0) {
  }
  // the first datasamples on boot are not always right. this discards the first tx message.
  static unsigned int settlePcb = 1;

  if (settlePcb == 0){
    // flash the led.
    digitalWrite(5,1);
    delay(20);
    digitalWrite(5,0);

/*
    sensor.messageIndex = 11;
    sensor.msgSize = (sizeof(sensor) - 2) /2;
    Serial.print("MsgSize: ");
    Serial.print(sensor.msgSize);
    Serial.print(", ");

    unsigned int dataSendTimes = 3;
    for(dataSendTimes = 3; dataSendTimes != 0; dataSendTimes--){

      if (rf12_canSend()) {
        Serial.print("DataSend");
        rf12_sendStart(0, ptr, sizeof(sensor));
        delay(100); // need otherwize shutdown happens before data was send out
        break;
      } 
      if (rf12_recvDone() && rf12_crc == 0) {
      }
      delay(100);
      if(dataSendTimes == 1){
        Serial.print("failed to use rm12b to tx data\n");
      }
    }

    // read status command, also cleares all interrupts registers
    rfm12_status = rf12_control(0);
    Serial.print("\nRfm12Status1: ");
    Serial.print(rfm12_status);
    Serial.print(",");
    Serial.print("\n");
*/

    //rf12_sleep(30); // 65.5sec*10
    rf12_sleep(1); // 1sec*10
  }
  else{
    settlePcb--;
    rf12_sleep(1); // 1sec*10
  }
}


#endif


















