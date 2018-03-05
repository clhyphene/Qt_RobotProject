#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#include <math.h>

#define START 1.5 //define the threshold which signifies the starting light turning on
#define BLUE 1.03 //recorded using test code
#define RED 0.42 //recorded using test code
#define LIGHTTOL 0.35 //tolerance for acceptable voltage values from CdS cell

//Global Objects
FEHMotor leftMotor(FEHMotor::Motor0,9);
FEHMotor rightMotor(FEHMotor::Motor1,9);

DigitalEncoder leftEnc(FEHIO::P0_0);
DigitalEncoder rightEnc(FEHIO::P0_1);

FEHServo buttonServo(FEHServo::Servo0);
FEHServo rotateServo(FEHServo::Servo1);
FEHServo liftServo(FEHServo::Servo2);
//Servos turn CCW to 180 when facing the motor

AnalogInputPin CdSCell(FEHIO::P3_7);

//Function Prototypes
void driveStraight(int percent);
void resetEncoders();
void drive(float distance); //using encoders
void turn(int degrees);
void performanceTestOne();
void performanceTestTwo();
void check_heading(float heading); //using RPS
void check_x_plus(float x_coordinate); //using RPS while robot is in the +x direction
void check_y_minus(float y_coordinate); //using RPS while robot is in the -y direction
void check_y_plus(float y_coordinate); //using RPS while robot is in the +y direction
void check_x_minus(float x_coordinate); //using RPS while robot is in the -x direction
void setup();// runs all prior setup functions

//Global Variables
//Input standard motor power levels here
int motor_percent = 40;
int left_motor_percent = -motor_percent;
int right_motor_percent = motor_percent;
int motor_percent_turn = 26;
int left_motor_percent_turn = motor_percent_turn;
int right_motor_percent_turn = -motor_percent_turn;
float counts;

int main(void)
{
    setup();



    return 0;
}

//Functions

void driveStraight(int percent) {

    //************WE COULD MAKE THIS DRIVE UNTIL IT HITS SOMETHING**********

    //Set both motors to same percentage so they move in the same direction at the same speed
    leftMotor.SetPercent(percent);
    rightMotor.SetPercent(percent);
}

void resetEncoders() {
    //Sets both encoders counts to 0
    leftEnc.ResetCounts();
    rightEnc.ResetCounts();
}

void drive(float distance) //using encoders
{
    //make sure the left and right motor percents are correct
    left_motor_percent = -motor_percent;
    right_motor_percent = motor_percent;

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
        leftMotor.SetPercent(left_motor_percent);
        rightMotor.SetPercent(right_motor_percent);

        //Write to the screen
        LCD.WriteLine("Driving forwards");
    }

    else //backwards
    {
        //Set both motors to desired percent
        leftMotor.SetPercent(-left_motor_percent);
        rightMotor.SetPercent(-right_motor_percent);
        //Write to the screen
        LCD.WriteLine("Driving backwards");
    }



    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((rightEnc.Counts() + leftEnc.Counts()) / 2. < counts);

    //Turn off motors
    leftMotor.Stop();
    rightMotor.Stop();
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
        leftMotor.SetPercent(right_motor_percent_turn);
        rightMotor.SetPercent((-1)*(left_motor_percent_turn));

        //Write to the screen
        LCD.WriteLine("Turning clockwise");
    }

    else //counterclockwise
    {
        //Set both motors to desired percent
        leftMotor.SetPercent((-1)*(right_motor_percent_turn));
        rightMotor.SetPercent(left_motor_percent_turn);

        //Write to the screen
        LCD.WriteLine("Turning counterclockwise");
    }

    //determine the number of counts
    counts = 3.476*sqrt(degrees*degrees);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((rightEnc.Counts() + leftEnc.Counts()) / 2. < counts);

    //Turn off motors
    leftMotor.Stop();
    rightMotor.Stop();
}

void check_heading(float heading) //using RPS
{
    LCD.Clear();
    LCD.WriteLine("Check Heading");

    if (heading == 0) {
        while(RPS.Heading() >= 1 && RPS.Heading() <= 359) {
            if (RPS.Heading() >= 180) {
                turn_left(12, 1);
                Sleep(.1);
            } else if ( RPS.Heading() < 180) {
                turn_right(12,1);
                Sleep(.1);
            }
        }
    } else {
        while (RPS.Heading() > heading+1 || RPS.Heading() < heading - 1) {
            if(RPS.Heading()-heading < 180 || RPS.Heading()-heading > -180) {
                if(RPS.Heading() > heading) {
                    turn_right(12, 1);
                    Sleep(.1);
                } else if (RPS.Heading() < heading) {
                    turn_left(12, 1);
                    Sleep(.1);
                }
            } else {
                if(RPS.Heading() > heading) {
                    turn_left(12, 1);
                    Sleep(.1);
                } else if (RPS.Heading() < heading) {
                    turn_right(12, 1);
                    Sleep(.1);
                }
            }
        }
    }
}

void check_x_minus(float x_coordinate) //using RPS while robot is in the -x direction
{
    LCD.Clear();
    LCD.WriteLine("Check X minus");

    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            right_motor.SetPercent(20);
            left_motor.SetPercent(20);

            Sleep(.1);

            right_motor.Stop();
            left_motor.Stop();
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            right_motor.SetPercent(-20);
            left_motor.SetPercent(-20);

            Sleep(.1);

            right_motor.Stop();
            left_motor.Stop();
        }
    }
}

void check_x_plus(float x_coordinate) //using RPS while robot is in the +x direction
{
    LCD.Clear();
    LCD.WriteLine("Check X plus");

    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            right_motor.SetPercent(-20);
            left_motor.SetPercent(-20);

            Sleep(.1);

            right_motor.Stop();
            left_motor.Stop();
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            right_motor.SetPercent(20);
            left_motor.SetPercent(20);

            Sleep(.1);

            right_motor.Stop();
            left_motor.Stop();
        }
    }
}

void check_y_minus(float y_coordinate) //using RPS while robot is in the -y direction
{
    LCD.Clear();
    LCD.WriteLine("Check Y minus");

    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            right_motor.SetPercent(20);
            left_motor.SetPercent(20);

            Sleep(.1);

            right_motor.Stop();
            left_motor.Stop();
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            right_motor.SetPercent(-20);
            left_motor.SetPercent(-20);

            Sleep(.1);

            right_motor.Stop();
            left_motor.Stop();
        }
    }
}

void check_y_plus(float y_coordinate) //using RPS while robot is in the +y direction
{
    LCD.Clear();
    LCD.WriteLine("Check Y plus");

    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            right_motor.SetPercent(-20);
            left_motor.SetPercent(-20);

            Sleep(.1);

            right_motor.Stop();
            left_motor.Stop();
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            right_motor.SetPercent(20);
            left_motor.SetPercent(20);

            Sleep(.1);

            right_motor.Stop();
            left_motor.Stop();
        }
    }
}

void setup() {
    //Servo min and max values from calibration
    buttonServo.SetMin(500);
    buttonServo.SetMax(2319);


    RPS.InitializeTouchMenu();

}


void performanceTestOne() {

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Code has begun");

    //wait for CdS cell to start
    while(CdSCell.Value()>START);


    //move forward 1
    drive(7);
    Sleep(1.5);

    //turn 90 degrees clockwise
    turn(90);
    Sleep(1.5);

    //move forward 2
    drive(8.275);
    Sleep(1.5);

    //turn 90 degrees counterclockwise
    turn(-90);
    Sleep(1.5);

    //move forward 2.1
    drive(6);
    Sleep(1.5);

    //turn 90 degrees clockwise
    turn(90);
    Sleep(1.5);

    //move forward 2.2
    drive(1.25);
    Sleep(1.5);

    //turn 90 degrees counterclockwise
    turn(-90);
    Sleep(1.5);

    //move forward 3
    drive(4.25);
    Sleep(1.5);

    //turn 90 degrees counterclockwise
    turn(-90);
    Sleep(1.5);

    //move forward 4
    drive(3.5);
    Sleep(1.5);

    //move backward 5
    drive(-1.75);
    Sleep(1.5);

    //turn 90 degrees counterclockwise
    turn(-90);
    Sleep(1.5);

    //move forward 6
    drive(12.85);
    Sleep(1.5);

    //turn 90 degrees clockwise
    turn(90);
    Sleep(1.5);

    //move forward 7
    drive(20);
    Sleep(1.5);

    //turn 90 degrees clockwise
    turn(90);
    Sleep(1.5);

    //move forward 8
    drive(4.75);
    Sleep(1.5);

    //move backward 9
    drive(-3);
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
    drive(-24);
    Sleep(1.5);

    //All done
    dance_party();
    Sleep(10.0);
}

void performanceTestTwo() {
    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Code has begun");

    buttonServo.SetDegree(90);

    //wait for CdS cell to start
    while(CdSCell.Value()>START);

    //move forward 1
    drive(5.5);
    Sleep(1.5);

    //turn 90 degrees CW
    turn(90);
    Sleep(1.5);

    //move backwards 2
    drive(-10);
    Sleep(1.50);

    //turn 90 degrees CW
    turn(90);
    Sleep(1.5);

    //read CdS cell
    double lightColor = CdSCell.Value();

    //Red color will be 0, blue will be 1
    if (lightColor < (RED+LIGHTTOL) && lightColor > (RED-LIGHTTOL)) {
        buttonServo.SetDegree(180);
        LCD.Clear();
        LCD.WriteLine("I see RED");
    } else if (lightColor < (BLUE+LIGHTTOL) && lightColor > (BLUE-LIGHTTOL)) {
        buttonServo.SetDegree(-10);
        LCD.Clear();
        LCD.WriteLine("I see BLUE");
    } else {
        LCD.Clear();
        LCD.WriteLine("BLUE for sure");

        Sleep(3.0);
        buttonServo.SetDegree(-10);
    }

    Sleep(1.0);

    //move backwards 3
    drive(-5.25);

    //Wait for just over 5 seconds
    Sleep(5.5);

    //move forwards 4
    drive(4);
    Sleep(1.5);

    //Reset buttonServo
    buttonServo.SetDegree(90);

    //turn 90 degrees CCW
    turn(-94);
    Sleep(1.5);

    //move forwards 5
    drive(25);
    Sleep(1.5);

    //move backwards 6
    drive(-13.25);
    Sleep(1.5);

    //turn 90 degrees CCW
    turn(-83);
    Sleep(1.5);

    //move backwards 7
    drive(-8);

    //Performance test 2 complete



}
