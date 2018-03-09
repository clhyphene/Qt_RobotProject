#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#include <math.h>

#define START 1.5 //define the threshold which signifies the starting light turning on
#define BLUELIGHT 1.03 //recorded using test code
#define REDLIGHT 0.42 //recorded using test code
#define LIGHTTOL 0.3 //tolerance for acceptable voltage values from CdS cell

//Global Objects
FEHMotor leftMotor(FEHMotor::Motor0,9);
FEHMotor rightMotor(FEHMotor::Motor1,9);

DigitalEncoder leftEnc(FEHIO::P0_2);
DigitalEncoder rightEnc(FEHIO::P0_4);

FEHServo buttonServo(FEHServo::Servo0);
FEHServo forkServo(FEHServo::Servo2);
FEHServo liftServo(FEHServo::Servo4);
//Servos turn CCW to 180 when facing the motor

AnalogInputPin CdSCell(FEHIO::P3_7);

//Function Prototypes
void driveStraight(int percent);
void resetEncoders();
void drive(float distance); //using encoders
void turn(int degrees);
void performanceTestOne();
void performanceTestTwo();
void performanceTestThree();
void check_heading(float heading); //using RPS
void check_x_plus(float x_Ref, float delt_x); //using RPS while robot is in the +x direction
void check_y_minus(float y_Ref, float delt_y); //using RPS while robot is in the -y direction
void check_y_plus(float y_Ref, float delt_y); //using RPS while robot is in the +y direction
void check_x_minus(float x_Ref, float delt_x); //using RPS while robot is in the -x direction
void setup();// runs all prior setup functions
void setMotorSpeed(int percent);
void resetMotorSpeed();

//Global Variables
//Input standard motor power levels here
int motor_percent = 40;
int left_motor_percent = -motor_percent;
int right_motor_percent = motor_percent;
int motor_percent_turn = 26;
int left_motor_percent_turn = motor_percent_turn;
int right_motor_percent_turn = -motor_percent_turn;
float counts;
float wrenchX, wrenchY;

int main(void)
{
    setup();

    performanceTestThree();

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
    counts = 3.473*sqrt(degrees*degrees);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((rightEnc.Counts() + leftEnc.Counts()) / 2. < counts);

    //Turn off motors
    leftMotor.Stop();
    rightMotor.Stop();
}

void resetMotorSpeed() {
    left_motor_percent = -motor_percent;
    right_motor_percent = motor_percent;
}

void setMotorSpeed(int percent) {
    left_motor_percent = -percent;
    right_motor_percent = percent;
}

void check_heading(float heading) //using RPS
{
    if(RPS.Heading()<0) {
        return;
    }

    LCD.Clear();
    LCD.WriteLine("Check Heading");

    if (heading == 0) {
        while(RPS.Heading() >= 0.75 && RPS.Heading() <= 359.25) {
            if (RPS.Heading() >= 180) {
                turn(-1);
                Sleep(.1);
            } else if ( RPS.Heading() < 180) {
                turn(1);
                Sleep(.1);
            }
        }
    } else {
        while (RPS.Heading() > heading+.75 || RPS.Heading() < heading - .75) {
            if(RPS.Heading()-heading < 180 || RPS.Heading()-heading > -180) {
                if(RPS.Heading() > heading) {
                    turn(1);
                    Sleep(.1);
                } else if (RPS.Heading() < heading) {
                    turn(-1);
                    Sleep(.1);
                }
            } else {
                if(RPS.Heading() > heading) {
                    turn(-1);
                    Sleep(.1);
                } else if (RPS.Heading() < heading) {
                    turn(1);
                    Sleep(.1);
                }
            }
        }
    }
}

void check_x_minus(float x_Ref, float delt_x) //using RPS while robot is in the -x direction
{
    if(RPS.X()<0) {
        return;
    }

    float x_coordinate = x_Ref - delt_x;
    LCD.Clear();
    LCD.WriteLine("Check X minus");

    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(20);
            leftMotor.SetPercent(20);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(-20);
            leftMotor.SetPercent(-20);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
    }
}

void check_x_plus(float x_Ref, float delt_x) //using RPS while robot is in the +x direction
{
    if(RPS.X()<0) {
        return;
    }

    float x_coordinate = x_Ref+delt_x;
    LCD.Clear();
    LCD.WriteLine("Check X plus");

    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(-20);
            leftMotor.SetPercent(-20);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(20);
            leftMotor.SetPercent(20);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
    }
}

void check_y_minus(float y_Ref, float delt_y) //using RPS while robot is in the -y direction
{
    if(RPS.Y()<0) {
        return;
    }

    float y_coordinate = y_Ref - delt_y;
    LCD.Clear();
    LCD.WriteLine("Check Y minus");

    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(20);
            leftMotor.SetPercent(20);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(-20);
            leftMotor.SetPercent(-20);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
    }
}

void check_y_plus(float y_Ref, float delt_y) //using RPS while robot is in the +y direction
{
    if(RPS.Y()<0) {
        return;
    }

    float y_coordinate = y_Ref+delt_y;
    LCD.Clear();
    LCD.WriteLine("Check Y plus");

    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(-20);
            leftMotor.SetPercent(-20);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(20);
            leftMotor.SetPercent(20);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
    }
}

void setup() {
    //Servo min and max values from calibration
    buttonServo.SetMin(500);
    buttonServo.SetMax(2319);

    forkServo.SetMin(500);
    forkServo.SetMax(2404);

    liftServo.SetMin(708);
    liftServo.SetMax(2456);

    //Set servos to starting values
    buttonServo.SetDegree(90);
    forkServo.SetDegree(0);
    liftServo.SetDegree(180);


    RPS.InitializeTouchMenu();

//    float x, y;
//    LCD.Clear();
//    LCD.WriteLine("Position Wrench");

//    while(!LCD.Touch(&x, &y));

//    wrenchX = RPS.X();
//    wrenchY = RPS.Y();

//    Sleep(1.5);

//    LCD.Clear();
//    LCD.WriteLine("Put in Start location Push to Start");
//    while(!LCD.Touch(&x, &y));

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
    if (lightColor < (REDLIGHT+LIGHTTOL) && lightColor > (REDLIGHT-LIGHTTOL)) {
        buttonServo.SetDegree(180);
        LCD.Clear();
        LCD.WriteLine("I see RED");
    } else if (lightColor < (BLUELIGHT+LIGHTTOL) && lightColor > (BLUELIGHT-LIGHTTOL)) {
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

void performanceTestThree() {
    float x, y; //for touch screen

    float x_coordinate, y_coordinate;

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Code has begun");

    //wait for CdS cell to start
    while(CdSCell.Value()>START);

    //Get current location
    x_coordinate = RPS.X();
    y_coordinate = RPS.Y();

    //move forward 1
    drive(10);
    Sleep(.75);

    //check RPS
    check_y_minus(y_coordinate, 10);

    turn(90);
    Sleep(.75);

    //check RPS
    check_heading(180);

    //Get current location
    x_coordinate = RPS.X();
    y_coordinate = RPS.Y();

    //move forward 2-1
    drive(6);//add to 11.5
    Sleep(.75);

    //check RPS
    check_x_minus(x_coordinate, 6);

    turn(-90);
    Sleep(.75);

    //check RPS
    check_heading(270);

    //Get current location
    x_coordinate = RPS.X();
    y_coordinate = RPS.Y();

    //move forward 2-2
    drive(2);
    //drive(y_coordinate-wrenchY);
    Sleep(.75);

    //check RPS
   check_y_minus(y_coordinate, 2);
    // check_y_minus(y_coordinate, y_coordinate-wrenchY);

    turn(90);
    Sleep(.75);

    //check RPS
    check_heading(180);

    //Get current location
    x_coordinate = RPS.X();
    y_coordinate = RPS.Y();

    setMotorSpeed(25);

    //move forward 2-3
    drive(5);
    //drive(x_coordinate-wrenchX);
    Sleep(.75);

    resetMotorSpeed();


    Sleep(1.5);

    //SOMETHING WITH LIFT SERVO THAT PICKS UP THE WRENCH*****************************************************
    //low=180
    //high=90
    liftServo.SetDegree(65);

    Sleep(.75);

    //Get current location
    x_coordinate = RPS.X();
    y_coordinate = RPS.Y();

    //move backward 3
    drive(-5.5);
    Sleep(.75);

    //Check RPS
    check_x_plus(x_coordinate, 5.5);

    turn(90);
    Sleep(.75);

    //Check RPS
    check_heading(90);

    //Get current location
    x_coordinate = RPS.X();
    y_coordinate = RPS.Y();

    //move forward 4
    drive(3);
    Sleep(.75);

    //Check RPS
    check_y_plus(y_coordinate, 4);

    turn(90);
    Sleep(.75);

    //Check RPS
    check_heading(0);

    //Get current location
    x_coordinate = RPS.X();
    y_coordinate = RPS.Y();

    //move forward 5
    drive(14.5);
    Sleep(.75);

    //Check RPS
    check_x_plus(x_coordinate, 14.5);

    turn(-90);
    Sleep(.75);

    //Check RPS
    check_heading(90);

    //RPS goes dead from here on

    //move forward 6
    drive(25);
    Sleep(.75);

    turn(-45);
    Sleep(.75);

    //move forward 7
    drive(27);
    Sleep(.75);

    //SOMETHING WITH THE LIFT SERVO THAT LOWERS THE WRENCH************************************
    liftServo.SetDegree(180);

    Sleep(.75);

    //move backward 8
    drive(-12.5);
    Sleep(.75);

    turn(-90);
    Sleep(.75);

    //move backward 9
    drive(-15);
    Sleep(.75);

    //Done
}
