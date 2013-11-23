#include <SD.h>
const int chipSelect = 10;
File dataFile;

static int initialized = false;

void sdInit(void)
{

  //sd
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SS, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    delay(1500);  
  }
  else
  {
    Serial.println("card initialized."); 

    // Open up the file we're going to log to!
    dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (! dataFile) {
      Serial.println("error opening datalog.txt");
      delay(1500);  
    }
    Serial.println("file opened\n");

    initialized = true;
  }
  // sd end
}

void sdPrintData(String *value)
{
  if(initialized == false)
  {
    static unsigned long timerX = 0;
    // retry initializing the SD card.
    if (millis() - timerX > (10000)) {
      Serial.println("SD init retry...\n");
      timerX = millis();
      sdInit();
    }

    return; 
  }
  
  dataFile.print(*value);
  Serial.print("*sdWrites:");
  Serial.print(*value);
  
}

void sdPrintDataDone(void)
{
  dataFile.flush();
}




