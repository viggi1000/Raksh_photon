

//#include <Wire.h>
#include "ThingSpeak.h"
#include "Adafruit_MLX90614.h"
#include "MAX30100_PulseOximeter.h"
#include "application.h"
#define REPORTING_PERIOD_MS     1000
#define slaveAddress 0x5A
unsigned int myChannelNumber = ; // replace with your ChannelID
const char * myWriteAPIKey = ""; // replace with your WriteAPIKey
TCPClient client;

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
int BPM,sp;
unsigned long t, period_respiration;
uint32_t tsLastReport = 0;
uint32_t tsLastReport1 = 0;
int f;
float peakValue = 0;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{

    Serial.begin(115200);
    ThingSpeak.begin(client);
    mlx.begin();
    // Initialize the PulseOximeter instance and register a beat-detected callback
    pox.begin();
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
pox.update();
BPM=pox.getHeartRate();
sp=pox.getSpO2();
float sensorValue=mlx.readObjectTempC();
float threshold=mlx.readAmbientTempC();
Serial.print("Heart rate:");
Serial.print(BPM);
Serial.print("bpm / SpO2:");
Serial.print(sp);
Serial.print("% ");
Serial.print("Temp:");
Serial.println(sensorValue);
int i = 0;
int sensorValue = mlx.readObjectTempC();
  //  vals[i] = sensorValue;
if (sensorValue > peakValue) 
  {
     t=millis();
    peakValue = sensorValue;
   // tot++;
  }

  if (sensorValue <= threshold) 
  {
    if (peakValue > threshold) {
      // you have a peak value:
     signed long currentTime = millis();
      period_respiration = currentTime-t;
      Serial.println(peakValue);
      p = (float)period_respiration*6;
      f=60000/p;
    }
  }   
if (millis() - tsLastReport1 > 20000) {
  ThingSpeak.setField(1,BPM);
  ThingSpeak.setField(2,sp);
  ThingSpeak.setField(3,f);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
Particle.publish("BPM", String(BPM));
Particle.publish("spo2", String(sp));
Particle.publish("RR", String(f));
tsLastReport1 = millis();
}

}
