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
    delay(1500);  

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

void sdPrintData(String value)
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
  
  dataFile.println(value);
  Serial.print("sdWrites:");
  Serial.println(value);
  dataFile.flush();
  /*
    // make a string for assembling the data to log:
   String dataString = "";
   
   dataString += String(millis());
   dataString +=',';
   dataString += String(1);
   dataString +=',';
   
   dataFile.println(dataString);
   // print to the serial port too:
   Serial.println(dataString);
   
   // The following line will 'save' the file to the SD card after every
   // line of data - this will use more power and slow down how much data
   // you can read but it's safer! 
   // If you want to speed up the system, remove the call to flush() and it
   // will save the file only every 512 bytes - every time a sector on the 
   // SD card is filled with data.
   dataFile.flush();
   
   */

}




