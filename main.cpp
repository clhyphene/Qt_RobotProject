#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#include <math.h>

#define START 1.5 //define the threshold which signifies the starting light turning on
//#define BLUE
//#define RED

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
void move_forward(int distance); //using encoders
void turn(int degrees);

//Global Variables
//Input standard motor power levels here
int motor_percent = 30;
int right_motor_percent = -motor_percent;
int left_motor_percent = motor_percent;
int motor_percent_turn = 15;
int counts;

int main(void)
{
        float x, y; //for touch screen

        //Initialize the screen
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);

        LCD.WriteLine("Code has begun");

        //wait for CdS cell to start
        while(CdSCell.Value()>1.5);

        move_forward(9); //move forward
        Sleep(1.0);

        //turn 90 degrees clockwise
        turn(90);
        Sleep(1.0);

        //move forward
        move_forward(8);
        Sleep(1.0);

        //turn 90 degrees counterclockwise
        turn(-90);
        Sleep(1.0);

        //move forward
        move_forward(17);
        Sleep(1.0);

        //turn 90 degrees counterclockwise
        turn(-90);
        Sleep(1.0);

        //move forward
        move_forward(1);
        Sleep(1.0);

        //move backward
        move_forward(-2);
        Sleep(1.0);

        //turn 90 degrees counterclockwise
        turn(-90);
        Sleep(1.0);

        //move forward
        move_forward(16);
        Sleep(1.0);

        //turn 90 degrees clockwise
        turn(90);
        Sleep(1.0);

        //move forward
        move_forward(19);
        Sleep(1.0);

        //turn 90 degrees clockwise
        turn(90);
        Sleep(1.0);

        //move forward
        move_forward(3);
        Sleep(1.0);

        //turn 30 degrees clockwise
        turn(30);
        Sleep(1.0);

        //move backward
        move_forward(-4);
        Sleep(1.0);

        //turn 30 degrees counterclockwise
        turn(-30);
        Sleep(1.0);

        //move backward
        move_forward(-18);
        Sleep(1.0);

    return 0;
}

//Functions
void driveStraight(int percent) {
    //Set both motors to same percentage so they move in the same direction at the same speed
    rightMotor.SetPercent(percent);
    leftMotor.SetPercent(percent);
}

/*void getWorldState(int *rEncVal, int *lEncVal, double *CdSVal) {
    //Reads current data from all input sources and sends them to main via pointers
    &rEncVal = rightEnc.Counts();
    &lEncVal = leftEnc.Counts();
    &CdSVal = CdSCell.Value();
}*/

void resetEncoders() {
    //Sets both encoders counts to 0
    rightEnc.ResetCounts();
    leftEnc.ResetCounts();
}

void move_forward(int distance) //using encoders
{
    //convert inputed distance (abs val) to counts
    counts = sqrt(distance*distance)*40.409;

    //Reset encoder counts
    resetEncoders();

    //determine if driving forwards or backwards
    if (distance >= 0) //forwards
    {
        //Set both motors to desired percent
        rightMotor.SetPercent(right_motor_percent);
        leftMotor.SetPercent(left_motor_percent);
    }

    else //backwards
    {
        //Set both motors to desired percent
        rightMotor.SetPercent(-right_motor_percent);
        leftMotor.SetPercent(-left_motor_percent);
    }



    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((leftEnc.Counts() + rightEnc.Counts()) / 2. < counts);

    //Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
}

void turn(int degrees) //using encoders
{
    //Reset encoder counts
    resetEncoders();

    //detect whether turning clockwise or counterclockwise
    if (degrees >=0) //clockwise
    {
    //Set both motors to desired percent
    rightMotor.SetPercent((-1)*(motor_percent_turn));
    leftMotor.SetPercent(motor_percent_turn);
    }

    else //counterclockwise
    {
        //Set both motors to desired percent
        rightMotor.SetPercent(motor_percent_turn);
        leftMotor.SetPercent((-1)*(motor_percent_turn));
    }

    //determine the number of counts
    counts = 6.17*sqrt(degrees*degrees);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((leftEnc.Counts() + rightEnc.Counts()) / 2. < counts);

    //Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
}
