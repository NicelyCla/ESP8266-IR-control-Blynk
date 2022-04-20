//irremote
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <time.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// Example of data captured by IRrecvDumpV2.ino
uint16_t rawData[89] = {4584, 4396,  652, 476,  650, 1596,  660, 470,  658, 1590,  654, 474,  
654, 1594,  650, 478,  650, 482,  4588, 4396,  654, 474,  652, 1596,  
660, 470,  658, 1590,  654, 474,  652, 1594,  650, 478,  648, 484,  
4586, 4394,  654, 476,  652, 1594,  650, 480,  658, 1588,  656, 472,  
654, 1594,  652, 476,  650, 482,  4578, 4404,  656, 472,  654, 1592,  
652, 476,  650, 1598,  658, 472,  656, 1592,  654, 474,  652, 480,  
4580, 4402,  656, 474,  654, 1592,  652, 478,  650, 1598,  658, 472,  
656, 1592,  652, 476,  652};  // UNKNOWN 52A3E788

String days[] = {"Domenica", "Lunedì", "Martedì", "Mercoledì", "Giovedì" ,"Venerdì", "Sabato", "Domenica"};
int programmaOrario[3];

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";
int timezone = 3600;
int dst = 3600;

struct storedDataStruct {
    int timerAspirapolvere;
    int giornoSettimana[7];
    int ora;
    int minuti;
} sStoredData;

int addr = 0;

//time input
BLYNK_WRITE(V0) {
  /*
  Serial.print("Start Time : ");
  Serial.println(param[0].asLong());
  Serial.print("Stop Time : ");
  Serial.println(param[1].asLong());
  Serial.print("Time Zone : ");
  Serial.println(param[4].asLong());
  */

  TimeInputParam t(param);
  // Process start time
  if (t.hasStartTime())
  {
   
    Serial.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
   
  }
  else if (t.isStartSunrise())
  {
    //Serial.println("Start at sunrise");
  }
  else if (t.isStartSunset())
  {
    //Serial.println("Start at sunset");
  }
  else
  {
    // Do nothing
  }

  // Process stop time

  if (t.hasStopTime())
  {
    Serial.println(String("Stop: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());
  }
  else if (t.isStopSunrise())
  {
    Serial.println("Stop at sunrise");
  }
  else if (t.isStopSunset())
  {
    Serial.println("Stop at sunset");
  }
  else
  {
    // Do nothing: no stop time was set
  }

  // Process timezone
  // Timezone is already added to start/stop time

  //Serial.println(String("Time zone: ") + t.getTZ());

  // Get timezone offset (in seconds)
  //Serial.println(String("Time zone offset: ") + t.getTZ_Offset());

  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)

  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      if (i == 7){
        sStoredData.giornoSettimana[0]=1;
      }
      else{
        sStoredData.giornoSettimana[i]=1;
      }
      sStoredData.ora = t.getStartHour();
      sStoredData.minuti = t.getStartMinute();
      Serial.println(sStoredData.ora);
      Serial.println(sStoredData.minuti);
      Serial.println(t.getStartMinute());
      EEPROM.put(0, sStoredData);
      EEPROM.commit();
      
    }
    else{
      if (i == 7){
        sStoredData.giornoSettimana[0]=0;
      }
      else{
        sStoredData.giornoSettimana[i]=0;
      }

      sStoredData.ora = 0;
      sStoredData.minuti = 0;
      EEPROM.put(0, sStoredData);
      EEPROM.commit();
    }

  }

  Serial.println();
}

//accendi aspirapolvere bottone
BLYNK_WRITE(V1) {

  int i=param.asInt();
  if (i==1)
  {
      irsend.sendRaw(rawData, 89, 38);  // Send a raw data capture at 38kHz.
      Serial.println("hai acceso l'aspirapolvere");
      Blynk.setProperty(V1,"onLabel", "Sto avviando...");

      delay(2000);
      //Blynk.setProperty(V1,"onLabel", "Sto pulendo...");
      Blynk.virtualWrite(V1,LOW);

  }
  else
  {

  }
}

//accendi il timer aspirapolvere
BLYNK_WRITE(V2) {

  int i=param.asInt();
  if (i==1)
  {
      EEPROM.get(0, sStoredData);
      sStoredData.timerAspirapolvere = 1;
      EEPROM.put(0, sStoredData);
      EEPROM.commit();

  }
  else
  {
      EEPROM.get(0, sStoredData);
      sStoredData.timerAspirapolvere = 0;
      EEPROM.put(0, sStoredData);
      EEPROM.commit();
  }
}

WidgetLED led1(V3);
//accendi luci divano
BLYNK_WRITE(V6) {

  int i=param.asInt();
  if (i==1)
  {
      irsend.sendNEC(0xFFE21D);
      Serial.println("hai acceso luci divano");
      led1.on();
  }
  else
  {

  }
}


//spegni luci divano
BLYNK_WRITE(V7) {

  int i=param.asInt();
  if (i==1)
  {
      irsend.sendNEC(0xFFC23D);
      Serial.println("hai spento luci divano");
      led1.off();
  }
  else
  {

  }
}

//controlla colori luce divano
BLYNK_WRITE(V5) {
  switch (param.asInt())
  {
    case 1: // Item 1
      irsend.sendNEC(0xFFA25D);
      Serial.println("Alza luminosità");
      break;
    case 2: // Item 2
      irsend.sendNEC(0xFF629D);
      Serial.println("Abbassa luminosità");
      break;
    case 3: // Item 3
      irsend.sendNEC(0xFF22DD);
      Serial.println("Luce normale");
      break;
    case 4: // Item 4
      irsend.sendNEC(0xFF6897);
      Serial.println("Fuoco");
      break;
    case 5: // Item 5
      irsend.sendNEC(0xFF9867);
      Serial.println("Fuoco verde");
      break;
    case 6: // Item 6
      irsend.sendNEC(0xFFB04F);
      Serial.println("Fuoco viola");
      break;
    case 7: // Item 7
      irsend.sendNEC(0xFF30CF);
      Serial.println("Rosso");
      break;
    case 8: // Item 8
      irsend.sendNEC(0xFF18E7);
      Serial.println("Verde");
      break;
    case 9: // Item 9
      irsend.sendNEC(0xFF7A85);
      Serial.println("Blu");
      break;
    case 10: // Item 10
      irsend.sendNEC(0xFF10EF);
      Serial.println("Arancione");
      break;
    case 11: // Item 11
      irsend.sendNEC(0xFF38C7);
      Serial.println("Verde acqua");
      break;
    case 12: // Item 12
      irsend.sendNEC(0xFF42BD);
      Serial.println("Giallo");
      break;
    case 13: // Item 13
      irsend.sendNEC(0xFF5AA5);
      Serial.println("Celeste");
      break;
    case 14: // Item 14
      irsend.sendNEC(0xFF4AB5);
      Serial.println("Viola");
      break;
    case 15: // Item 15
      irsend.sendNEC(0xFF52AD);
      Serial.println("Fucsia");
      break;
      
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  WiFi.begin(ssid,pass);
  Blynk.begin(auth, ssid, pass);

    irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266

  configTime(timezone, dst, "0.it.pool.ntp.org");

}
int controllo = 1;
void loop()
{
  EEPROM.begin(512);
  EEPROM.get(0, sStoredData);


  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  
  if (sStoredData.giornoSettimana[p_tm->tm_wday]){

    if (sStoredData.timerAspirapolvere){
      //Serial.println(sStoredData.giornoSettimana[p_tm->tm_wday]);
      Serial.println("");
      Serial.print("oggi è ");
      Serial.print(days[p_tm->tm_wday]);
      Serial.print(" ed è programmata una pulizia alle: ");
      Serial.print(sStoredData.ora);
      Serial.print(":");
      Serial.println(sStoredData.minuti);
      Serial.println("");
    }

    if(p_tm->tm_hour == sStoredData.ora && p_tm->tm_min == sStoredData.minuti){
      if (controllo && sStoredData.timerAspirapolvere){
        irsend.sendRaw(rawData, 89, 38);  // Send a raw data capture at 38kHz.
        controllo = 0;
        Serial.println("Ho appena avviato l'aspirapolvere come da scheduling!");
      }
    }
    else {
      controllo = 1;
    }
  }
  else{
    /*
    Serial.println(sStoredData.giornoSettimana[p_tm->tm_wday]);
    Serial.println("oggi non è programmata una pulizia");
    Serial.println(days[p_tm->tm_wday]);
    */
  }
  
  Serial.println("");
  Serial.println("-----------------------------------");

  Serial.println("Orario attuale:");
  Serial.print(p_tm->tm_mday);
  Serial.print("/");
  Serial.print(p_tm->tm_mon + 1);
  Serial.print("/");
  Serial.print(p_tm->tm_year + 1900);
  
  Serial.print(" ");
  
  Serial.print(p_tm->tm_hour);
  Serial.print(":");
  Serial.print(p_tm->tm_min);
  Serial.print(":");
  Serial.print(p_tm->tm_sec);
  Serial.print(", oggi è: ");
  Serial.println(days[p_tm->tm_wday]);
  Serial.println("-----------------------------------");

  
  //delay(1000);
  
  
  Blynk.run();
  EEPROM.end();

}
