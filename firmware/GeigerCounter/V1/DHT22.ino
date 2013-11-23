#ifdef PCB_MOUNTED_HUMSENS_DHT22

#include <DHT22.h>
#define DHT22_PIN 6

DHT22 myDHT22(DHT22_PIN);

static float dhtTemp = 0;
static float dhtHum = 0;

void dht22_Loop(void){
  //if (!timer2.poll(2000)){
  //  return;
  //}

  DHT22_ERROR_t errorCode;

  //Serial.print("Requesting data...");
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
  case DHT_ERROR_NONE:
    //Serial.print("Got Data ");
    dhtTemp = myDHT22.getTemperatureC();
    //Serial.print(dhtTemp);
    //Serial.print("C ");
    dhtHum = myDHT22.getHumidity();
    //Serial.print(dhtHum);
    //Serial.print("%");
    sensor.humidity = myDHT22.getHumidity()*100.0;
    // discard digits, by cast to integer, after decimal point  
    sensor.temperature = myDHT22.getTemperatureC() * 100;
    /*
      // Alternately, with integer formatting which is clumsier but more compact to store and
     	  // can be compared reliably for equality:
     	  //	  
     char buf[128];
     
     sprintf(buf, "Integer-only reading: Temperature %hi.%01hi C, Humidity %i.%01i %% RH",
     myDHT22.getTemperatureCInt()/10, abs(myDHT22.getTemperatureCInt()%10),
     myDHT22.getHumidityInt()/10, myDHT22.getHumidityInt()%10);
     Serial.println(buf);
     */
    break;
  case DHT_ERROR_CHECKSUM:
    Serial.print("check sum error ");
    Serial.print(myDHT22.getTemperatureC());
    Serial.print("C ");
    Serial.print(myDHT22.getHumidity());
    Serial.println("%");
    break;
  case DHT_BUS_HUNG:
    Serial.println("BUS Hung ");
    break;
  case DHT_ERROR_NOT_PRESENT:
    Serial.println("Not Present ");
    break;
  case DHT_ERROR_ACK_TOO_LONG:
    Serial.println("ACK time out ");
    break;
  case DHT_ERROR_SYNC_TIMEOUT:
    Serial.println("Sync Timeout ");
    break;
  case DHT_ERROR_DATA_TIMEOUT:
    Serial.println("Data Timeout ");
    break;
  case DHT_ERROR_TOOQUICK:
    Serial.println("Polled to quick ");
    break;
  }
}

float dhtGetTemp(void)
{
  return dhtTemp;
}

float dhtGetHum(void)
{ 
  return dhtHum;
}


#endif


