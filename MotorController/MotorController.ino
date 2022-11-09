
#include <TMCStepper.h>         // TMCstepper - https://github.com/teemuatlut/TMCStepper
#include <SoftwareSerial.h>     // Software serial for the UART to TMC2209 - https://www.arduino.cc/en/Reference/softwareSerial
#include <Streaming.h>          // For serial debugging output - https://www.arduino.cc/reference/en/libraries/streaming/
#include <Servo.h>

#define EN_PIN           2      // Enable - PURPLE
#define DIR_PIN          3      // Direction - WHITE
#define STEP_PIN         4      // Step - ORANGE
#define SW_SCK           5      // Software Slave Clock (SCK) - BLUE
#define SW_TX            7      // SoftwareSerial receive pin - BROWN
#define SW_RX            6      // SoftwareSerial transmit pin - YELLOW
#define DRIVER_ADDRESS   0b00   // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f           // SilentStepStick series use 0.11 ...and so does my fysetc TMC2209 (?)

SoftwareSerial SoftSerial(SW_RX, SW_TX);                          // Be sure to connect RX to TX and TX to RX between both devices

TMC2209Stepper TMCdriver(&SoftSerial, R_SENSE, DRIVER_ADDRESS);   // Create TMC driver

int accel = 500; 
int32_t targetSpeed = 0;
int32_t speed = 0;

int shoulderPos = 0;
int elbowPos = 0;

Servo shoulder;
Servo elbow;


//== Setup ===============================================================================

void setup() {

  Serial.begin(115200);               // initialize hardware serial for communication
  SoftSerial.begin(115200);           // initialize software serial for UART motor control
  TMCdriver.beginSerial(115200);      // Initialize UART
  
  pinMode(EN_PIN, OUTPUT);           // Set pinmodes
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);         // Enable TMC2209 board  

  TMCdriver.begin();                                                                                                                                                                                                                                                                                                                            // UART: Init SW UART (if selected) with default 115200 baudrate
  TMCdriver.toff(5);                 // Enables driver in software
  TMCdriver.rms_current(500);        // Set motor RMS current
  TMCdriver.microsteps(256);         // Set microsteps

  TMCdriver.en_spreadCycle(false);
  TMCdriver.pwm_autoscale(true);     // Needed for stealthChop

  TMCdriver.VACTUAL(0);
  
  shoulder.attach(9);
  elbow.attach(10);
}

//== Loop =================================================================================

void readData() {
  if (Serial.available()) {
    unsigned char data[3][4]; // data is given as 3 * 32 bit int

    Serial.readBytes((char*)data, 12);

    targetSpeed = *((int32_t*)data[0]);
    shoulderPos = *((int32_t*)data[1]);
    elbowPos = *((int32_t*)data[2]);

    Serial << "Received " << targetSpeed << " " << shoulderPos << " " << elbowPos << endl;
  }
}

void loop() {
  readData();
  
  if (targetSpeed > speed) {
    speed += accel;
    speed = min(speed, targetSpeed);
    TMCdriver.VACTUAL(speed);
    //Serial << TMCdriver.VACTUAL() << endl;
    delay(10);
  }

  if (targetSpeed < speed){ 
    speed -= accel;
    speed = max(speed, targetSpeed);            
    TMCdriver.VACTUAL(speed);
    //Serial << TMCdriver.VACTUAL() << endl;
    delay(10);
  } 

  shoulder.write(shoulderPos);
  elbow.write(elbowPos);
}
