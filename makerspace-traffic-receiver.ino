//#include "SIGFOX.h"                         //  Include the unabiz-arduino library.

#define INPUT_PIN 12

//static const Country country = COUNTRY_SG;  //  Set this to your country to configure the Sigfox transmission frequencies.
//static const bool useEmulator = false;      //  Set to true if using UnaBiz Emulator.
//static const String device = "2C3068";       //  Set this to your device name if you're using UnaBiz Emulator.
//static const bool echo = true;              //  Set to true if the Sigfox library should display the executed commands.

//static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //Create transceiver object

const unsigned int transmit_message_interval = 5;                      //Sets the duration of the interval (in seconds) between successive message transmissions.
unsigned long previous_timestamp_m = 0L;                                //Timestamp of the beginning of the previous iteration of void loop() in milliseconds.
unsigned long previous_iteration_duration = 0L;                          //Duration of execution of previous iteration of void loop() in milliseconds.
long countdown = transmit_message_interval * 1000L;                //Countdown timer for message transmission. 
unsigned int counter = 0;
unsigned long duration_low = 0L;
bool blocking_flag =  false;
const unsigned long TIMEOUT_LENGTH = 100000L;

void setup()
{
  pinMode(INPUT_PIN, INPUT);
  Serial.begin(115200);
//  if(!transceiver.begin())
//  {
//    stop(F("Unable to init Sigfox module, something is missing"));
//  }
  while(!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB
  }
}

unsigned int count_when_blocking()
{
  unsigned long reading;
  int count = 0;

  int blocking_control = 0;

  unsigned int blocking_time_m = 0;
  
  while(blocking_flag)
  {
    reading = pulseIn(INPUT_PIN, LOW, TIMEOUT_LENGTH);

    //if receive signal
    if(reading > 0)
    {
      blocking_control += 1;

      //if receive signal for 32 cycles, set blocking_flag to false
      if(blocking_control >= 32)
      {
        blocking_flag = false;  
      }
    }
    else
    {
      blocking_control = 0; 
      blocking_time_m += 95;
      if(blocking_time_m >= 2000)
      {
        count += 1;
        blocking_time_m = 0;
      }
    }
  }
  
  return count;
}

void writeCounterToSerial(unsigned int message)
{
  if(Serial.availableForWrite() > 7)
  {
    Serial.println(message);
    Serial.flush(); //Waits for the transmission of outgoing serial data to complete.
  }
}

void loop()
{
  unsigned long current_timestamp_m = millis();
  previous_iteration_duration = current_timestamp_m - previous_timestamp_m;
  countdown -= previous_iteration_duration;
  previous_timestamp_m = current_timestamp_m;
  
  duration_low = pulseIn(INPUT_PIN, LOW, TIMEOUT_LENGTH);
  if(duration_low == 0)
  {
      //detected person, add one to counter
      counter += 1;
      //person blocking signal, set blocking flag
      blocking_flag = true;
      counter += count_when_blocking();
  }
  if(countdown <= 0){
//    transceiver.sendString(String(counter));
    writeCounterToSerial(counter);


    //Reset after sending counter.
    counter = 0;
    countdown = transmit_message_interval * 1000L;
    duration_low = 0L;
    blocking_flag =  false;
  }
}






