/* Arduino to Hitechnic IRSeeker V2
 
 Wiring:
 -------
 NXT sensors have 6 pins.  For I2C sensors like the IRSeeker V2
 the wiring is the following:
 
 NXT Pin  Wire     Arduino Connect  Comment
 -------  ----     ---------------  -------
 1        White    3.3 volt            
 2        Black    GND
 3        Red      GND
 4*       Green    Pwr              Lego says 4.3v but others have hooked up 5v ok
 5*       Yellow   Analog 5         SCL/Clock pin for I2C DIGIAI0 on NXT Hardware Dev pg6
 6*       Blue     Analog 4         Data/SDA pin for I2C DIGIAI1 on NXT Hardware Dev pg6
 */

#include <Wire.h>  //I2C library
#include <VL53L0X.h>

VL53L0X sensor;


// macro to convert 8 bit I2C address to 7 bit.
// See http://www.arduino.cc/en/Reference/Wire for explanation

#define SEVEN_BIT_I2C_ADDRESS(x) ((x) >> 1)

const int I2CIRSensorAddress = SEVEN_BIT_I2C_ADDRESS(0x10) ;          
const int registerAddress = 0x49;   //address of register to read from accelerometer
const char* fields[] = {
  "Zone",
  "S1",
  "S2",
  "S3",
  "S4",
  "S5"};

const int numberoffields = sizeof(fields)/sizeof(char *);
long waittime = 0;

void setup(){
  Wire.begin();        
  Serial.begin(115200); 

  sensor.init();
  sensor.setTimeout(50);
  sensor.startContinuous();
}

int getIRSeekerMode(){
  Wire.beginTransmission(I2CIRSensorAddress);
  Wire.write(0x41); //register we want values from
  if (int err = Wire.endTransmission()) {
    Serial.print("Fail\t");
    Serial.println(err);
    return;
  }

  int retbytes = Wire.requestFrom(I2CIRSensorAddress,1);   
  int values[numberoffields];
  if (retbytes == 1){
    return (int)Wire.read();
  }
  else{  // I dunno what happened... just make sure nothing in i2c buffer and do again.
    while(Wire.available()){
      Wire.read();
    }
    return -1;
  }
}


void printHiTechnicIRSeeker(){
  Wire.beginTransmission(I2CIRSensorAddress);
  Wire.write(registerAddress); //register we want values from
  if (int err = Wire.endTransmission()) {
    Serial.print("Fail\t");
    Serial.println(err);
    return;
  }

  int retbytes = Wire.requestFrom(I2CIRSensorAddress,numberoffields);   
  int values[numberoffields];
  if (retbytes == numberoffields){
    for (int i =0;i<numberoffields;i++){
      values[i] = Wire.read();
    }
    for(int i=1;i<numberoffields;i++){
      Serial.print(fields[i]);
      Serial.print(':');
      Serial.print(values[i]);
      Serial.print(' ');
    }
  }
  else{  // I dunno what happened... just make sure nothing in i2c buffer and do again.
    while(Wire.available()){
      Wire.read();
    }
    return;
  }
}

void printTOF(int val){
  Serial.print("dist:");
  Serial.print(val);
  Serial.print(' ');
}


void loop(){
//  Serial.println(getIRSeekerMode());
//  return;
  auto val = sensor.readRangeContinuousMillimeters();
  if (sensor.timeoutOccurred()) { val = -10000; }
  //printTOF(val);
  printHiTechnicIRSeeker();
  Serial.println();
  delay(100);
 
}
