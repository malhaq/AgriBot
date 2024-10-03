#include <Servo.h>
#include <TinyGPS++.h>
#include "Wire.h"
#include <MPU6050_light.h>


void turnOffWater();
void turnOffInsecticide();
//Water tank
const int trigWater = 2;
const int echoWater = 3;

//Insecticide tank
const int trigInsecticide = 4;
const int echoInsecticide = 5;

//Stepper motors pins
const int STEP_PIN_RIGHT = 6;
const int DIR_PIN_RIGHT = 7;
const int STEP_PIN_LEFT = 8;
const int DIR_PIN_LEFT = 9;
const int EN_RIGHT = 10;
const int EN_LEFT = 11;
const int STEPS = 3200;//s1:off s2:off s3:on

//Waterpump relay
const int WaterPump = 22;
const int InsecticidePump = 24;

//servomotor pin
const int SprayerServo = 12;

// Create an instance of TinyGPS++
TinyGPSPlus gps;

//Create an instance of MPU6050
MPU6050 mpu(Wire);//Pin 21 & Pin 20 (SCL,SDA)

//Create an instance of servo
Servo myServo;

//Creating variables for base angels
float baseX = 0;
float baseY = 0;
float baseZ = 0;

//Convert -45 to 45 degree range into 0 to 180 degree range
int minAngle = 45;
int maxAngle = 115;



//declaring the variables and flags
bool drive ;
bool water ;
bool insecticide ;
bool insecticideMode;
bool manual ;
bool moveSprayer ;
bool WEmpty;
bool IEmpty;
int counter;
int servoState;  // 0 for minAngle, 1 for maxAngle
double lat ;
double lng ;

void setup() {

  //Set the pins for the water tank
  pinMode(trigWater,OUTPUT);
  pinMode(echoWater,INPUT);

  //set the pins for the insecticide tank
  pinMode(trigInsecticide,OUTPUT);
  pinMode(echoInsecticide,INPUT);

  //Set the pins for the motors
  pinMode(STEP_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_RIGHT, OUTPUT);
  pinMode(STEP_PIN_LEFT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);

  //Set the pins for the relay module
  pinMode(WaterPump,OUTPUT);
  pinMode(InsecticidePump,OUTPUT);
  digitalWrite(WaterPump,HIGH);
  digitalWrite(InsecticidePump,HIGH);

  //Set initial direction for motors
  digitalWrite(DIR_PIN_RIGHT, HIGH); // Set direction to forward
  digitalWrite(DIR_PIN_LEFT, HIGH);  // Set direction to forward

  Serial1.begin(115200);//to communicate with the esp32 cam 
  Serial2.begin(9600);//to get the data from the gps module
  Serial.begin(9600);//for debuging
  Wire.begin();// Initializes the I2C communication.
  

  //Attach the servo to the specificd pin
  myServo.attach(SprayerServo);
  myServo.write(80);
  byte status = mpu.begin();
  

  drive = false;
  water = false;
  insecticide = false;
  insecticideMode = false;
  manual = false;
  moveSprayer = false;
  lat = 0;
  lng = 0;
  IEmpty = false;
  WEmpty = false;
  counter = 0;
  servoState = 0;
  
  while(status != 0){ //stop every thing to connect the mpu6050
    status = mpu.begin();
    delay(500);
  }
  while(!Serial1){
    ;
  }
  while(!Serial2){
    ;
  }

  resetBaseAngles();// set the base angles

}

void loop() {
  
  //Read the co-ordinates and send it to the esp32
  if(Serial2.available() > 0){
    gps.encode(Serial2.read());

    if(gps.location.isUpdated()){
      lat = (gps.location.lat(),6);
      lng = (gps.location.lng(),6);

    }
    char buffer[50];
    sprintf(buffer,"%f,%f",lat,lng);
    Serial1.println(buffer);

  }
  if(Serial1.available()){
    char command = Serial1.read();
    if(isAlpha(command)){
      Serial.print("command: ");
      Serial.println(command);
      switch(command){
        case 'C'://trun on the water mode 
        water = true;
        moveSprayer = true;
        turnOnWater();
        break;
        case 'P':
        water = false;
        turnOffWater();
        break;
        case 'U'://turn off insecticide
        turnOffInsecticide();
        insecticide = false;
        break;
        case 'M'://control the robot manualy
        if(manual){
          manual = false;
        }else{
          manual = true;
        }
        break;
        case 'I'://turn on the insecticide mode
        insecticideMode = true;
        water = false;
        break;
        case 'F':
        forwardDrive();
        break;
        case 'R'://turn right
        turnRight(1.22);//add number of revs
        break;
        case 'L'://turn left
        turnLeft(1.22);//add number of revs
        break;
        case 'D'://start driving autonomously
        if(manual){
          break;
        }else{
          drive = true;
          forwardDrive();
        }
        break;
        case 'B'://You are at the base station turn every thing off and wait for comands
          turnOffWater();
          turnOffInsecticide();
          drive = false;
          manual = false;
        break;
        case 'Q':// Turn on fluides -liquids-
          if(water){
            turnOnWater();
            water = true;

          }else if(insecticide){
            turnOnInsecticide();
            insecticide = true;
          }
        break;
        case 'E':// Turn off fuides -liquids-
        if(!water){
          turnOffWater();
        }else if(!insecticide){
          turnOffInsecticide();
        }
        break;
        case 'A'://backward drive
        backwardDrive();
        break;
        case 'N':
        turnOnInsecticide();
        insecticide = true;
        moveSprayer = true;
        water = false;
        break;
        case 'X':
        turnOffInsecticide();
        break;
      }
    }

  }
  if(drive &&(!manual) ){
    if(moveSprayer){
      digitalWrite(STEP_PIN_LEFT, HIGH);
      digitalWrite(STEP_PIN_RIGHT, HIGH);
      myServo.write(minAngle);
      delayMicroseconds(500); // Decrease delay to increase speed
      digitalWrite(STEP_PIN_LEFT, LOW);
      digitalWrite(STEP_PIN_RIGHT, LOW);
      myServo.write(maxAngle);
      delayMicroseconds(500); // Decrease delay to increase speed
    }else if(!moveSprayer){
      digitalWrite(STEP_PIN_LEFT, HIGH);
      digitalWrite(STEP_PIN_RIGHT, HIGH);
      delayMicroseconds(500); // Decrease delay to increase speed
      digitalWrite(STEP_PIN_LEFT, LOW);
      digitalWrite(STEP_PIN_RIGHT, LOW);
      delayMicroseconds(500); // Decrease delay to increase speed
    }
    float adjustedZ = mpu.getAngleZ() - baseZ;
    if(adjustedZ >= 5 ){
      if (water){
        turnOffWater();
        rightBalance(baseZ);
        delayMicroseconds(100);
        turnOnWater();
      }else if(insecticide){
        turnOffInsecticide();
        rightBalance(baseZ);
        turnOnInsecticide();
      }else{
        rightBalance(baseZ);
      }
    }
    if(adjustedZ <= (-5)){
      if (water){
        turnOffWater();
        leftBalance(baseZ);
        delayMicroseconds(100);
        turnOnWater();
      }else if(insecticide){
        turnOffInsecticide();
        leftBalance(baseZ);
        turnOnInsecticide();
      }else{
        leftBalance(baseZ);
      }
      
    }

  } else if (manual){
    if (water){
      myServo.write(minAngle);
      delay(500); // Decrease delay to increase speed
      myServo.write(maxAngle);
      delay(500);
      turnOnWater();
    }else if(insecticide){
      myServo.write(minAngle);
      delay(500); // Decrease delay to increase speed
      myServo.write(maxAngle);
      delay(500);
      turnOnInsecticide();
    }
  }
  if(water){
    double dist = measureDistance(trigWater,echoWater);
    turnOnWater();
    if(dist >= 14){
    turnOffWater();
    water = false;
    WEmpty = true;
    }

  }
  if(insecticide){
    double dist = measureDistance(trigInsecticide,echoInsecticide);
    if(dist >= 15){
      turnOffInsecticide();
      insecticide = false;
      IEmpty = true;  
    }
  }
     
}
//Function to measure distance using the ultrasonic for the fluid tanks
double measureDistance(int trigPin, int echoPin) {
  // Clear the trigPin by setting it LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a HIGH pulse for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, returns the sound wave travel time in microseconds
  long durationWater = pulseIn(echoPin, HIGH);
  //Convert from time to distance in cm
  double distance = durationWater * 0.034 / 2;
  Serial.print("distance: ");
  Serial.println(distance);
  float subhi = 1.0;
  return subhi;
}
//Forward drive 
void forwardDrive(){
  for (int i=0; i<STEPS;i++){
    digitalWrite(STEP_PIN_LEFT, HIGH);
    digitalWrite(STEP_PIN_RIGHT, HIGH);
    delayMicroseconds(500); // Decrease delay to increase speed
    digitalWrite(STEP_PIN_RIGHT, LOW);
    digitalWrite(STEP_PIN_LEFT, LOW);
    delayMicroseconds(500); // Decrease delay to increase speed
    if(water || insecticide){
      Serial.println("on");
      counter ++;
      if(counter == 200){
        if(servoState == 0){
          myServo.write(minAngle);
          servoState = 1;
        } else {
          myServo.write(maxAngle);
          servoState = 0;
        }
        counter = 0;
      }
      
    }

  }
  float adjustedZ = mpu.getAngleZ() - baseZ;
    if(adjustedZ >= 5 ){
      if (water){
        turnOffWater();
        rightBalance(baseZ);
        delayMicroseconds(1000);
        turnOnWater();
      }else if(insecticide){
        turnOffInsecticide();
        rightBalance(baseZ);
        turnOnInsecticide();
      }else{
        rightBalance(baseZ);
      }
    }
    if(adjustedZ <= -5){
      if (water){
        turnOffWater();
        leftBalance(baseZ);
        delayMicroseconds(1000);
        turnOnWater();
      }else if(insecticide){
        turnOffInsecticide();
        leftBalance(baseZ);
        turnOnInsecticide();
      }else{
        leftBalance(baseZ);
      }
    }

}
//turn right function
void turnRight(float rev){
  digitalWrite(DIR_PIN_RIGHT, LOW);  // Set direction for right motor to backward
  digitalWrite(DIR_PIN_LEFT, HIGH);  // Ensure left motor is set to forward direction

  int step = rev * STEPS;
  for (int i=0; i<step;i++){
    digitalWrite(STEP_PIN_LEFT, HIGH);
    digitalWrite(STEP_PIN_RIGHT, HIGH);
    delayMicroseconds(500); // Decrease delay to increase speed
    digitalWrite(STEP_PIN_LEFT, LOW);
    digitalWrite(STEP_PIN_RIGHT, LOW);
    delayMicroseconds(500); // Decrease delay to increase speed
    
  }

  digitalWrite(DIR_PIN_RIGHT, HIGH); //RETURN THE DIRECTION TO FORWARD DRIV
  resetBaseAngles();
}
//turn left function
void turnLeft(float rev){
  digitalWrite(DIR_PIN_RIGHT, HIGH);  // Ensure Right motor is set to forward direction
  digitalWrite(DIR_PIN_LEFT, LOW);  // Set direction for left motor to backward 
  int step = rev * STEPS;
  for (int i=0; i<step;i++){
    digitalWrite(STEP_PIN_LEFT, HIGH);
    digitalWrite(STEP_PIN_RIGHT, HIGH);
    delayMicroseconds(500); // Decrease delay to increase speed
    
    digitalWrite(STEP_PIN_RIGHT, LOW);
    digitalWrite(STEP_PIN_LEFT, LOW);
    delayMicroseconds(500); // Decrease delay to increase speed
    
  }
  digitalWrite(DIR_PIN_LEFT, HIGH); //RETURN THE DIRECTION TO FORWARD DRIVE
  resetBaseAngles();
}
//turn on water pump
void turnOnWater(){
  if(WEmpty){
    double dist = measureDistance(trigInsecticide,echoInsecticide);
    if(dist >= 14){
      return;
    }else{
      // digitalWrite(WaterPump,LOW);
      digitalWrite(WaterPump,LOW);
      moveSprayer = true;
      water = true;
      WEmpty = false;
      turnOffInsecticide();
    }
    
  }else{
    digitalWrite(WaterPump,LOW);
    moveSprayer = true;
    water = true;
    turnOffInsecticide();
  }
}
//turn off water pump
void turnOffWater(){
  digitalWrite(WaterPump,HIGH);
  moveSprayer = false;
  water = false;
  myServo.write(80);
}
//turn one insecticide pump
void turnOnInsecticide(){
  if(IEmpty){
    double dist = measureDistance(trigInsecticide,echoInsecticide);
    if(dist >= 14){
      return;
    }else{
      digitalWrite(InsecticidePump,LOW);
      insecticide = true;
      moveSprayer = true;
      IEmpty = false;
      turnOffWater();
    }
    
  }else{
    digitalWrite(InsecticidePump,LOW);
    insecticide = true;
    moveSprayer = true;
    IEmpty = false;
    turnOffWater();
  }
}
//turn off insecticide pump
void turnOffInsecticide(){
  digitalWrite(InsecticidePump,HIGH);
  moveSprayer = false;
  insecticide = false;
  myServo.write(80);
}
// reset gyroscope ref. angles
void resetBaseAngles() {
  mpu.update();  // Update MPU readings to get the current angles
  baseX = mpu.getAngleX();
  baseY = mpu.getAngleY();
  baseZ = mpu.getAngleZ();
}
//adjust right skew
void rightBalance(float baseZ) {
  digitalWrite(DIR_PIN_RIGHT, LOW);  // Set direction for right motor to backward
  digitalWrite(DIR_PIN_LEFT, HIGH);  // Ensure left motor is set to forward direction

  while (true) {
    float adjustedZ = mpu.getAngleZ(); - baseZ; // Calculate the adjusted Z angle

    if (adjustedZ > 1.0) {
      // Tilted to the right, adjust left motor to bring it back
      digitalWrite(STEP_PIN_LEFT, HIGH);
      digitalWrite(STEP_PIN_RIGHT, HIGH);
      delayMicroseconds(500); // Adjust delay as needed for speed
      
      digitalWrite(STEP_PIN_LEFT, LOW);
      digitalWrite(STEP_PIN_RIGHT, LOW);
      delayMicroseconds(500); // Adjust delay as needed for speed
    } 
    else if (adjustedZ < -1.0) {
      // Tilted to the left, adjust right motor to bring it back
      digitalWrite(STEP_PIN_LEFT, HIGH);
      digitalWrite(STEP_PIN_RIGHT, HIGH);
      delayMicroseconds(500); // Adjust delay as needed for speed
      
      digitalWrite(STEP_PIN_LEFT, LOW);
      digitalWrite(STEP_PIN_RIGHT, LOW);
      delayMicroseconds(500); // Adjust delay as needed for speed
    } 
    else {
      // Z angle is within acceptable range, stop adjusting
      break;
    }
  }

  digitalWrite(DIR_PIN_RIGHT, HIGH); // Set direction back to forward drive
}
//adjust left skew
void leftBalance(float baseZ) {
  digitalWrite(DIR_PIN_RIGHT, HIGH);  // Ensure Right motor is set to forward direction
  digitalWrite(DIR_PIN_LEFT, LOW);  // Set direction for left motor to backward 

  while (true) {
    float currentZ = mpu.getAngleZ(); // Get the current Z angle (replace with your actual function)
    float adjustedZ = currentZ - baseZ; // Calculate the adjusted Z angle

    if (adjustedZ > 1.0) {
      // Tilted to the right, adjust left motor to bring it back
      digitalWrite(STEP_PIN_LEFT, HIGH);
      digitalWrite(STEP_PIN_RIGHT, HIGH);
      delayMicroseconds(500); // Adjust delay as needed for speed
      
      digitalWrite(STEP_PIN_LEFT, LOW);
      digitalWrite(STEP_PIN_RIGHT, LOW);
      delayMicroseconds(500); // Adjust delay as needed for speed
    } 
    else if (adjustedZ < -1.0) {
      // Tilted to the left, adjust right motor to bring it back
      digitalWrite(STEP_PIN_LEFT, HIGH);
      digitalWrite(STEP_PIN_RIGHT, HIGH);
      delayMicroseconds(500); // Adjust delay as needed for speed
      
      digitalWrite(STEP_PIN_LEFT, LOW);
      digitalWrite(STEP_PIN_RIGHT, LOW);
      delayMicroseconds(500); // Adjust delay as needed for speed
    } 
    else {
      // Z angle is within acceptable range, stop adjusting
      break;
    }
  }

  digitalWrite(DIR_PIN_LEFT, HIGH); // Set direction back to forward drive
}
// backward drive
void backwardDrive(){
  digitalWrite(DIR_PIN_RIGHT, LOW); // Set direction to forward
  digitalWrite(DIR_PIN_LEFT, LOW);  // Set direction to forward
  for (int i=0; i<STEPS;i++){
    if(moveSprayer){
      digitalWrite(STEP_PIN_LEFT, HIGH);
      digitalWrite(STEP_PIN_RIGHT, HIGH);
      myServo.write(minAngle);
      delayMicroseconds(500); // Decrease delay to increase speed
      digitalWrite(STEP_PIN_RIGHT, LOW);
      digitalWrite(STEP_PIN_LEFT, LOW);
      myServo.write(maxAngle);
      delayMicroseconds(500); // Decrease delay to increase speed
      

    }else {
      digitalWrite(STEP_PIN_LEFT, HIGH);
      digitalWrite(STEP_PIN_RIGHT, HIGH);
      delayMicroseconds(500); // Decrease delay to increase speed
      digitalWrite(STEP_PIN_RIGHT, LOW);
      digitalWrite(STEP_PIN_LEFT, LOW);
      delayMicroseconds(500); // Decrease delay to increase speed
    } 
  }
  digitalWrite(DIR_PIN_RIGHT, HIGH); // Set direction to forward
  digitalWrite(DIR_PIN_LEFT, HIGH);  // Set direction to forward
}
