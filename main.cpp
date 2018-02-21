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
void moveForwardDist(int percent, int distance); //using encoders
void turn(int percent, int counts);

int main(void)
{
    int motor_percent = 30; //Input power level here
        int distance = 9; //Input desired distance in inches here
        int expected_counts;

        float x, y; //for touch screen

        //Initialize the screen
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);

        LCD.WriteLine("Shaft Encoder Exploration Test");
        LCD.WriteLine("Touch the screen");
        while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
        while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed

        //Declare a CdS cell sensor as an analog input and assign it to an IO port
        AnalogInputPin CdS_cell(FEH::P0_3);

        //wait for CdS cell to start
        while(CdS_cell.Value()<1.5);

        move_forward(motor_percent, distance); //see function
        Sleep(1.0);

        //reset inputs to turn right
        motor_percent=15;
        expected_counts=222;
        turn(motor_percent, expected_counts);
        Sleep(1.0);

        //reset inputs to move forward
        motor_percent=30;
        distance=8;
        move_forward(motor_percent, distance);
        Sleep(1.0);

        //reset inputs to turn left
        motor_percent=-15;
        expected_counts=222;
        turn(motor_percent, expected_counts);
        Sleep(1.0);

        //reset inputs to move forward
        motor_percent=30;
        distance=17;
        move_forward(motor_percent, distance);
        Sleep(1.0);

        //reset inputs to turn left
        motor_percent=-15;
        expected_counts=222;
        turn(motor_percent, expected_counts);
        Sleep(1.0);

        //reset inputs to move forward
        motor_percent=30;
        distance=1;
        move_forward(motor_percent, distance);
        Sleep(1.0);

        //reset inputs to move backward
        motor_percent=-30;
        distance=2;
        move_forward(motor_percent, distance);
        Sleep(1.0);

        //reset inputs to turn left
        motor_percent=-15;
        expected_counts=222;
        turn(motor_percent, expected_counts);
        Sleep(1.0);

        //reset inputs to move forward
        motor_percent=30;
        distance=16;
        move_forward(motor_percent, distance);
        Sleep(1.0);

        //reset inputs to turn right
        motor_percent=15;
        expected_counts=222;
        turn(motor_percent, expected_counts);
        Sleep(1.0);

        //reset inputs to move forward
        motor_percent=30;
        distance=19;
        move_forward(motor_percent, distance);
        Sleep(1.0);

        //reset inputs to turn right
        motor_percent=15;
        expected_counts=222;
        turn(motor_percent, expected_counts);
        Sleep(1.0);

        //reset inputs to move forward
        motor_percent=30;
        distance=3;
        move_forward(motor_percent, distance);
        Sleep(1.0);

        //reset inputs to turn 30 degrees right
        motor_percent=15;
        expected_counts=74;
        turn(motor_percent, expected_counts);
        Sleep(1.0);

        //reset inputs to move backward
        motor_percent=-30;
        distance=4;
        move_forward(motor_percent, distance);
        Sleep(1.0);

        //reset inputs to turn 30 degrees left
        motor_percent=-15;
        expected_counts=74;
        turn(motor_percent, expected_counts);
        Sleep(1.0);

        //reset inputs to move backward
        motor_percent=-30;
        distance=18;
        move_forward(motor_percent, distance);
        Sleep(1.0);

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

void moveForwardDist(int percent, int distance) //using encoders
{
    //convert inputed distance to counts
    counts=distance*40.409;

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);



    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent((-1)*(percent+1));
    left_motor.SetPercent(percent);


    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
