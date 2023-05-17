const int motorRPin1 = 4; // signal pin 1 for the right motor, connect to IN1               
const int motorRPin2 = 6;  // signal pin 2 for the right motor, connect to IN2
const int motorREnable = 10; // enable pin for the right motor (PWM enabled)

const int motorLPin1 = 3; // signal pin 1 for the left motor, connect to IN3           
const int motorLPin2 = 7; // signal pin 2 for the left motor, connect to IN4
const int motorLEnable = 9;

const int irPins[5] = {A1, A2, A3, A4,A5};

/* Define values for the IR Sensor readings */

// an array to hold values from analogRead on the ir sensor (0-1023)
int irSensorAnalog[5] = {0,0,0,0,0}; 

// an array to hold boolean values (1/0) for the ir sensors
int irSensorDigital[5] = {0,0,0,0,0};


int treashold = 700;

//from left to right when facing the same direction as the robot
int irSensors = B00000; 



int count = 0; // number of sensors detecting the line

// A score to determine deviation from the line [-180 ; +180]. 
// Negative means the robot is left of the line.
int error = 0;  

int errorLast = 0;  //  store the last value of error

// A correction value, based on the error from target. 
// It is used to change the relative motor speed with PWM.
int correction = 0; 

int lap = 0; // keep track of the laps

/* Set up maximum speed and speed for turning (to be used with PWM) */
int maxSpeed = 150; // used for PWM to control motor speed [0 - 255]

/* variables to keep track of current speed of motors */
int motorLSpeed = 0;
int motorRSpeed = 0;


void setup() {
   pinMode(motorLPin1,OUTPUT);        
  pinMode(motorLPin2,OUTPUT);
  pinMode(motorLEnable,OUTPUT);
  
  pinMode(motorRPin1,OUTPUT);        
  pinMode(motorRPin2,OUTPUT);
  pinMode(motorREnable,OUTPUT);
  Serial.begin(9600);

    for (int i = 0; i < 5; i++) {
    pinMode(irPins[i], INPUT);
  }

   TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00); 
  TCCR0B = _BV(CS00); 
    

}

void loop() {
  Scan();
 UpdateError();
 UpdateCorrection();
 Drive();


}


void Scan() {
  // Initialize the sensor counter and binary value
  count = 0;
  irSensors = B00000;
    
  for (int i = 0; i < 5; i++) {
    irSensorAnalog[i] = analogRead(irPins[i]);

    if (irSensorAnalog[i] >= treashold) {
        irSensorDigital[i] = 0;
    }
    else {irSensorDigital[i] = 1;}
   
    count = count + irSensorDigital[i];
    int b = 4-i;
    irSensors = irSensors + (irSensorDigital[i]<<b);//B0000
    }    
}

void UpdateError() {
  
  errorLast = error;  
  switch (irSensors) {
    case B00000:
          if(errorLast < 0){ error = -180; }
          else if(errorLast > 0) { error = 180; }
          Serial.println("00000");
          break;

    
     case B10000: // leftmost sensor on the line
       error = -150;
       Serial.println("10000");
       break;
       
     case B01000:
       error = -75;
       Serial.println("01000");
       break;
       
      case B00100:         // cheack wheather this need?                                                                                                                                                                                                                                                 
       error = 0;
       Serial.println("00100");
       break;

     case B00010:  
       error = 75;
       Serial.println("00010");
       break;
       
     case B00001: 
       error = 150;
       Serial.println("00001");
       break;


              
/* 2 Sensors on the line */         
    
     case B11000:
       error = -120;
       Serial.println("11000");
       break;
      
     case B01100:
       error = -60;
       Serial.println("01100");// change the value of this of the that
       break;

     case B00110: 
       error = 60;
       Serial.println("00110");
       break;

     case B00011: 
    Serial.println("00011");
       error = 120;
       break;  

      // extra condition for the sensore 
      

 /* 3 Sensors on the line */    
       
     case B11100:
     error = -160;
     Serial.println("11100");
       break;
       
     case B01110:
       error = 0;
       Serial.println("01110");
       break;
      
     case B00111:
       error = 160;
       Serial.println("00111");
       break;

    /* 4 Sensors on the line */       
     case B11110:
       error = -150;
       Serial.println("11110");
       break;
       
     case B11101:
       error = -150;
       Serial.println("11101");
       break;
      
     case B01111:
       error = 150;
       Serial.println("01111");
       break;
       
     case B10111:
       error = 150;
       Serial.println("10111");
       break;


     case B11111:
       //lap = lap + 1; // the bot should stop at the 
       error = 0;
       Serial.println("11111");
       break;

        default:
       error = errorLast;
       

     
  }
}
void UpdateCorrection() {

  if (error >= 0 && error < 30) {
    correction = 0;
  }
  
  else if (error >=30 && error < 60) {
    correction = 15;
  }
  
  else if (error >=60 && error < 75) {
    correction = 40;
  }
  
  else if (error >=75 && error < 120) {
    correction = 55;
  }  
  
  else if (error >=120 && error < 150) {
    correction = 75;
  } 
  
  else if (error >=150 && error < 180) {
    correction = 255;
  }   

  else if (error <=900 && error >=180) {
    correction = 305;
  }

   if (error <= 0 && error > -30) {
    correction = 0;
  }
  
  else if (error <= -30 && error > -60) {
    correction = -15;
  }
  
  else if (error <= -60 && error > -75) {
    correction = -40;
  }
  
  else if (error <= -75 && error > -120) {
    correction = -55;
  }  
  
  else if (error <= -120 && error > -150) {
    correction = -75;
  } 
  
  else if (error <= -150 && error > -180) {
    correction = -255;
  }   

  else if (error <= -180) {
    correction = -305;
    
  }
  else if (error == 1000) {
    correction = 1000;
  }
  /* Adjust the correction value if maxSpeed is less than 255 */
  correction = (int) (correction * maxSpeed / 150 + 0.5);
  
  if (correction <=900 && correction >= 0) {
    motorRSpeed = maxSpeed-correction ;
    motorLSpeed = maxSpeed;
  }
  
  else if (correction < 0) {
    motorRSpeed = maxSpeed;
    motorLSpeed = maxSpeed + correction;
  }
 else if (correction ==1000)
 {
  motorRSpeed= 0;
  motorLSpeed= 0;
 }
 }
void Drive() {
  if (motorRSpeed > 255) {motorRSpeed = 255;}
  else if (motorRSpeed < -255) {motorRSpeed = -255;}
  
  if (motorLSpeed > 255) {motorLSpeed = 255;}
  else if (motorLSpeed < -255) {motorLSpeed = -255;}
  else {}
  if (motorRSpeed > 0) { // right motor forward (using PWM)
     analogWrite(motorREnable, motorRSpeed);
     digitalWrite(motorRPin1, HIGH);
     digitalWrite(motorRPin2, LOW);
  } 
  else if (motorRSpeed < 0) {// right motor reverse (using PWM)
     analogWrite(motorREnable, abs(motorRSpeed));
     digitalWrite(motorRPin1, LOW);
     digitalWrite(motorRPin2, HIGH);
  } 
  else if (motorRSpeed == 0) { // right motor fast stop
     digitalWrite(motorREnable, LOW);
     digitalWrite(motorRPin1, LOW);
     digitalWrite(motorRPin2, LOW);
  }
  else {}
  
  if (motorLSpeed > 0) { // left motor forward (using PWM)
     analogWrite(motorLEnable, motorLSpeed);
     digitalWrite(motorLPin1, HIGH);
     digitalWrite(motorLPin2, LOW);
  } 
  else if (motorLSpeed < 0) { // right motor reverse (using PWM)
     analogWrite(motorLEnable, abs(motorLSpeed));
     digitalWrite(motorLPin1, LOW);
     digitalWrite(motorLPin2, HIGH);
  } 
    else if (motorLSpeed == 0) { // left motor fast stop
     digitalWrite(motorLEnable, LOW);
     digitalWrite(motorLPin1, LOW);
     digitalWrite(motorLPin2, LOW);
  }
  
}

  
