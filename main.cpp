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
//Servos turn CCW to 180 when facing the motor

AnalogInputPin CdSCell(FEHIO::P3_7);

//Function Prototypes
void driveStraight(int percent);
void getWorldState(int *rEncVal, int *lEncVal, double *CdSVal);
void resetEncoders();
void move_forward(float distance); //using encoders
void turn(int degrees);
void dance_turn(int degrees);
void dance_party();

//Global Variables
//Input standard motor power levels here
int motor_percent = 40;
int right_motor_percent = -motor_percent;
int left_motor_percent = motor_percent;
int motor_percent_turn = 28;
int left_motor_percent_turn = motor_percent_turn;
int right_motor_percent_turn = -motor_percent_turn;
float counts;

int main(void)
{
    //Servo min and max values from calibration
    buttonServo.SetMin(500);
    buttonServo.SetMax(2319);

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Code has begun");

    //wait for CdS cell to start
    while(CdSCell.Value()>START);


    //move forward 1
    move_forward(7);
    Sleep(1.5);

    //turn 90 degrees clockwise
    turn(90);
    Sleep(1.5);

    //move forward 2
    move_forward(8.275);
    Sleep(1.5);

    //turn 90 degrees counterclockwise
    turn(-90);
    Sleep(1.5);

    //move forward 2.1
    move_forward(6);
    Sleep(1.5);

    //turn 90 degrees clockwise
    turn(90);
    Sleep(1.5);

    //move forward 2.2
    move_forward(1.25);
    Sleep(1.5);

    //turn 90 degrees counterclockwise
    turn(-90);
    Sleep(1.5);

    //move forward 3
    move_forward(4.25);
    Sleep(1.5);

    //turn 90 degrees counterclockwise
    turn(-90);
    Sleep(1.5);

    //move forward 4
    move_forward(3.5);
    Sleep(1.5);

    //move backward 5
    move_forward(-1.75);
    Sleep(1.5);

    //turn 90 degrees counterclockwise
    turn(-90);
    Sleep(1.5);

    //move forward 6
    move_forward(12.85);
    Sleep(1.5);

    //turn 90 degrees clockwise
    turn(90);
    Sleep(1.5);

    //move forward 7
    move_forward(20);
    Sleep(1.5);

    //turn 90 degrees clockwise
    turn(90);
    Sleep(1.5);

    //move forward 8
    move_forward(4.75);
    Sleep(1.5);

    //move backward 9
    move_forward(-3);
    Sleep(1.5);

    //        //turn 30 degrees clockwise
    //        turn(30);
    //        Sleep(1.5);

    //        //move backward 10
    //        move_forward(-4.2);
    //        Sleep (1.5);

    //        //turn 30 degrees counterclockwise
    //        turn(-30);
    //        Sleep(1.5);

    //move backward 11
    motor_percent = 60;
    move_forward(-24);
    Sleep(1.5);

    //All done
    dance_party();
    Sleep(10.0);

    return 0;
}

//Functions
void dance_party() {
    //party time
    LCD.Clear(GOLDENROD);
    LCD.SetFontColor(BLACK);
    LCD.WriteLine("All done !");
    LCD.WriteLine(" ");
    LCD.WriteLine("Dance partyyyy");
    //dance
    motor_percent_turn = 60;
    turn(360);
    Sleep(.5);
    motor_percent_turn = 28;
    dance_turn(30);
    LCD.Clear(GOLDENROD);
    LCD.SetFontColor(BLACK);
    LCD.WriteLine("Dance partyyyy");
    dance_turn(-30);
    LCD.Clear(CYAN);
    LCD.SetFontColor(BLACK);
    LCD.WriteLine("Dance partyyyy");
    dance_turn(30);
    LCD.Clear(RED);
    LCD.SetFontColor(BLACK);
    LCD.WriteLine("Dance partyyyy");
    dance_turn(-30);
    LCD.Clear(GREEN);
    LCD.SetFontColor(BLACK);
    LCD.WriteLine("Dance partyyyy");
    dance_turn(30);
    LCD.Clear(PINK);
    LCD.SetFontColor(BLACK);
    LCD.WriteLine("Dance partyyyy");
    dance_turn(-30);
    LCD.Clear(GOLDENROD);
    LCD.SetFontColor(BLACK);
    LCD.WriteLine("Dance partyyyy");
    dance_turn(30);
    LCD.Clear(WHITE);
    LCD.SetFontColor(BLACK);
    LCD.WriteLine("Dance partyyyy");
    dance_turn(-30);
    LCD.WriteLine("All done !");
}

void driveStraight(int percent) {

    //************WE COULD MAKE THIS DRIVE UNTIL IT HITS SOMETHING**********

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

void move_forward(float distance) //using encoders
{
    //make sure the left and right motor percents are correct
    right_motor_percent = -motor_percent;
    left_motor_percent = motor_percent;

    //convert inputed distance (abs val) to counts
    counts = sqrt(distance*distance)*40.409;

    //Reset encoder counts
    resetEncoders();

    //Prepare to input to the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    //determine if driving forwards or backwards
    if (distance >= 0) //forwards
    {
        //Set both motors to desired percent
        rightMotor.SetPercent(right_motor_percent);
        leftMotor.SetPercent(left_motor_percent);

        //Write to the screen
        LCD.WriteLine("Driving forwards");
    }

    else //backwards
    {
        //Set both motors to desired percent
        rightMotor.SetPercent(-right_motor_percent);
        leftMotor.SetPercent(-left_motor_percent);
        //Write to the screen
        LCD.WriteLine("Driving backwards");
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
    //make sure motor percents are at correct values
    left_motor_percent_turn = motor_percent_turn;
    right_motor_percent_turn = -motor_percent_turn;

    //Reset encoder counts
    resetEncoders();

    //Prepare to input to the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    //detect whether turning clockwise or counterclockwise
    if (degrees >= 0) //clockwise
    {
        //Set both motors to desired percent
        rightMotor.SetPercent(right_motor_percent_turn);
        leftMotor.SetPercent((-1)*(left_motor_percent_turn));

        //Write to the screen
        LCD.WriteLine("Turning clockwise");
    }

    else //counterclockwise
    {
        //Set both motors to desired percent
        rightMotor.SetPercent((-1)*(right_motor_percent_turn));
        leftMotor.SetPercent(left_motor_percent_turn);

        //Write to the screen
        LCD.WriteLine("Turning counterclockwise");
    }

    //determine the number of counts
    counts = 3.489*sqrt(degrees*degrees);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((leftEnc.Counts() + rightEnc.Counts()) / 2. < counts);

    //Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
}

void dance_turn(int degrees) //using encoders
{
    //make sure motor percents are at correct values
    left_motor_percent_turn = motor_percent_turn;
    right_motor_percent_turn = -motor_percent_turn;

    //Reset encoder counts
    resetEncoders();


    //detect whether turning clockwise or counterclockwise
    if (degrees >= 0) //clockwise
    {
        //Set both motors to desired percent
        rightMotor.SetPercent(right_motor_percent_turn);
        leftMotor.SetPercent((-1)*(left_motor_percent_turn));

    }

    else //counterclockwise
    {
        //Set both motors to desired percent
        rightMotor.SetPercent((-1)*(right_motor_percent_turn));
        leftMotor.SetPercent(left_motor_percent_turn);

    }

    //determine the number of counts
    counts = 3.435*sqrt(degrees*degrees);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((leftEnc.Counts() + rightEnc.Counts()) / 2. < counts);

    //Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
}
