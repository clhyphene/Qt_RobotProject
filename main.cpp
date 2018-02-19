#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>

//Global Objects
FEHMotor rightMotor(FEHMotor::Motor0,9);
FEHMotor leftMotor(FEHMotor::Motor1,9);

DigitalEncoder rightEnc(FEHIO::P0_0);
DigitalEncoder leftEnc(FEHIO::P0_1);

FEHServo buttonServo(FEHServo::Servo0);
FEHServo rotateServo(FEHServo::Servo1);
FEHServo liftServo(FEHServo::Servo2);

AnalogInputPin CdSCell(FEHIO::P0_2);

//Function Prototypes
void driveStraight(int percent);
void getWorldState(int *rEncVal, int *lEncVal, double *CdSVal);
void resetEncoders();


int main(void)
{


    return 0;
}

//Functions
void driveStraight(int percent) {
    //Set both motors to same percentage so they move in the same direction at the same speed
    rightMotor.SetPercent(percent);
    leftMotor.SetPercent(percent);
}

void getWorldState(int *rEncVal, int *lEncVal, double *CdSVal) {
    //Reads current data from all input sources and sends them to main via pointers
    &rEncVal = rightEnc.Counts();
    &lEncVal = leftEnc.Counts();
    &CdSVal = CdSCell.Value();
}

void resetEncoders() {
    //Sets both encoders counts to 0
    rightEnc.ResetCounts();
    leftEnc.ResetCounts();
}
