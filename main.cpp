#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#include <math.h>

#define START 1.5 //define the threshold which signifies the starting light turning on
#define BLUELIGHT 1.94 //recorded using test code
#define REDLIGHT 0.38 //recorded using test code
#define LIGHTTOL 0.3 //tolerance for acceptable voltage values from CdS cell

//Global Objects
FEHMotor leftMotor(FEHMotor::Motor0,9);
FEHMotor rightMotor(FEHMotor::Motor1,9);

DigitalEncoder leftEnc(FEHIO::P0_7);
DigitalEncoder rightEnc(FEHIO::P1_0);

FEHServo buttonServo(FEHServo::Servo0);
FEHServo forkServo(FEHServo::Servo2);
FEHServo liftServo(FEHServo::Servo4);
//Servos turn CCW to 180 when facing the motor

AnalogInputPin CdSCell(FEHIO::P3_7);

//Function Prototypes
void resetEncoders();
void drive(float distance); //using encoders
void turn(int degrees);
void performanceTestOne();
void performanceTestTwo();
void performanceTestThree();
void performanceTestFour();
void check_heading(float heading); //using RPS
void check_x_plus(float x_Ref, float delt_x); //using RPS while robot is in the +x direction
void check_y_minus(float y_Ref, float delt_y); //using RPS while robot is in the -y direction
void check_y_plus(float y_Ref, float delt_y); //using RPS while robot is in the +y direction
void check_x_minus(float x_Ref, float delt_x); //using RPS while robot is in the -x direction
void check_angle_drive(float x_Ref, float y_Ref, float dist); //using RPS while robot is travelling at 45 degree to the axes
void setup();// runs all prior setup functions
void setMotorSpeed(int percent);
void resetMotorSpeed();
void getLocation();

//Final Competition Functions
void carJack();
void getWrench();
void pushButtons();
void dropWrench();
void turnCrank();
void goHome();

//Global Variables
//Input standard motor power levels here
int motor_percent = 40;
int left_motor_percent = -motor_percent;
int right_motor_percent = motor_percent;
int motor_percent_turn = 34;
int left_motor_percent_turn = motor_percent_turn;
int right_motor_percent_turn = -motor_percent_turn;
float counts;
float wrenchX, wrenchY, buttonX, buttonY, jackX, jackY;
float refX, refY;
int fuelType; //1 if needs to be turned CW 2 for CCW
float stdSleep = .5;
float stdSleep2 = .1;

int main(void)
{
    setup();

    //for final
        carJack();
        getWrench();
        pushButtons();
        dropWrench();
        turnCrank();
        goHome();


    return 0;
}

//Functions

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

    float timeStart = TimeNow();

    if (heading == 0) {
        while(RPS.Heading() >= 0.75 && RPS.Heading() <= 359.25 && TimeNow()-timeStart<2) {
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

    float timeStart = TimeNow();

    //check whether the robot is within an acceptable range
    while((RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1) && TimeNow()-timeStart < 2)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(30);
            leftMotor.SetPercent(-30);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(-30);
            leftMotor.SetPercent(30);

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

    float timeStart = TimeNow();

    //check whether the robot is within an acceptable range
    while((RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1) && TimeNow()-timeStart<2)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(-30);
            leftMotor.SetPercent(30);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(30);
            leftMotor.SetPercent(-30);

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

    float timeStart = TimeNow();

    //check whether the robot is within an acceptable range
    while((RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1) && TimeNow()-timeStart<2)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(30);
            leftMotor.SetPercent(-30);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(-30);
            leftMotor.SetPercent(30);

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

    float timeStart = TimeNow();

    //check whether the robot is within an acceptable range
    while((RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1) && TimeNow()-timeStart<2)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(-30);
            leftMotor.SetPercent(30);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            rightMotor.SetPercent(30);
            leftMotor.SetPercent(-30);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
    }
}

void check_angle_drive(float x_Ref, float y_Ref, float dist) {

    LCD.Clear();
    LCD.WriteLine("Check Angle Drive");

    if(RPS.X()<0 || RPS.Y()<0) {
        return;
    }

    while(sqrt(pow(x_Ref-RPS.X(),2)+pow(y_Ref-RPS.Y(),2)) > dist+1 || sqrt(pow(x_Ref-RPS.X(),2)+pow(y_Ref-RPS.Y(),2)) < dist-1) {
        if(sqrt(pow(x_Ref-RPS.X(),2)+pow(y_Ref-RPS.Y(),2)) > dist+1) {
            rightMotor.SetPercent(-30);
            leftMotor.SetPercent(30);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        } else {
            rightMotor.SetPercent(30);
            leftMotor.SetPercent(-30);

            Sleep(.1);

            rightMotor.Stop();
            leftMotor.Stop();
        }
    }

}

void getLocation() {
    refX = RPS.X();
    refY = RPS.Y();
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

    fuelType = RPS.FuelType();

    float x, y;
    LCD.Clear();
    LCD.WriteLine("Position Wrench");

    while(true) {
    while(!LCD.Touch(&x, &y));

    wrenchX = RPS.X();
    wrenchY = RPS.Y();

    if(wrenchX<0) {
        LCD.Clear();
        LCD.WriteLine("Position Wrench TRY AGAIN");
        continue;
    }
    break;
    }


    Sleep(.5);


    LCD.Clear();
    LCD.WriteLine("Position Jack");
    Sleep(3.);

    liftServo.SetDegree(65);
    forkServo.SetDegree(90);


    while(true) {
    while(!LCD.Touch(&x, &y));

    jackX = RPS.X();
    jackY = RPS.Y();

    if(jackX<0) {
        LCD.Clear();
        LCD.WriteLine("Position Jack TRY AGAIN");
        continue;
    }
    break;
    }

    Sleep(.5);

    LCD.Clear();
    LCD.WriteLine("Position buttons");

    Sleep(1.);

    liftServo.SetDegree(180);
    forkServo.SetDegree(0);

    while(true) {
    while(!LCD.Touch(&x, &y));

    buttonX = RPS.X();
    buttonY = RPS.Y();

    if(buttonX<0) {
        LCD.Clear();
        LCD.WriteLine("Position buttons TRY AGAIN");
        continue;
    }
    break;
    }

    Sleep(.5);



    LCD.Clear();
    LCD.WriteLine("Put in Start location");
    Sleep(5.);

}

void carJack() {
    //add waiting for 30 seconds
    float x,y;

    LCD.Clear();
    LCD.WriteLine("Press for Final Action");

    while(!LCD.Touch(&x, &y));

    LCD.Clear();
    LCD.WriteLine("Good to GO");

    //wait for 30 seconds in case it misses start light
    float timeStart = TimeNow();

    //wait for CdS cell to start
    while(CdSCell.Value()>START && TimeNow()-timeStart < 30);

    getLocation();

    //move forward S
    drive(7);
    Sleep(stdSleep);

    //Check RPS
    check_y_minus(refY, 7);
    Sleep(stdSleep2);

    // turn W
    turn(90);
    Sleep(stdSleep);

    //Check RPS
    check_heading(180);
    Sleep(stdSleep2);

    getLocation();

    //move forward W
    drive(refX-jackX);
    Sleep(stdSleep);

    //Check RPS
    check_x_minus(refX, (refX-jackX));
    Sleep(stdSleep2);

    //turn S
    turn(-90);
    Sleep(stdSleep);

    //Check RPS
    check_heading(270);
    Sleep(stdSleep2);

    //raise fork arm
    liftServo.SetDegree(65);

    forkServo.SetDegree(90);
    Sleep(1.);

    getLocation();

    //move forward S
    drive(refY-jackY+.1);
    Sleep(stdSleep);

    //Check RPS
    check_y_minus(refY, (refY-jackY+.1));
    Sleep(stdSleep2);

    forkServo.SetDegree(180);
    Sleep(1.);
}
//*********************************************************************************************************
void getWrench() {
    getLocation();

    //move backward N
    drive(-(wrenchY-refY-.8));
    Sleep(stdSleep);

    //check RPS
    check_y_plus(refY, (wrenchY-refY-.8));
    Sleep(stdSleep2);

    //lower fork arm
    forkServo.SetDegree(0);
    Sleep(1.);

    liftServo.SetDegree(180);
    Sleep(1.);

    //turn W
    turn(90);
    Sleep(stdSleep);

    //check RPS
    check_heading(180);
    Sleep(stdSleep2);

    getLocation();

    //move forward W
    drive(2);
    Sleep(stdSleep);

    //recheck heading
    check_heading(180);
    Sleep(1.);

    getLocation();

    //set motors to lower percent
    setMotorSpeed(30);

    //move forward W
    drive(refX-wrenchX+1);
    Sleep(stdSleep);

    resetMotorSpeed();

    //raise fork arm
    liftServo.SetDegree(65);
    Sleep(1.);
}
//*********************************************************************************************************
void pushButtons() {
    getLocation();

    //move backward E
    drive(-4);
    Sleep(stdSleep);

    //check RPS
    check_x_plus(refX, 4);
    Sleep(stdSleep2);

    //turn S
    turn(-90);
    Sleep(stdSleep);

    //check RPS
    check_heading(270);
    Sleep(stdSleep2);

    getLocation();

    //move backward N
    drive(-(buttonY-refY-1));
    Sleep(stdSleep);

    //check RPS
    check_y_plus(refY, (buttonY-refY-1));
    Sleep(stdSleep2);

    //turn W
    turn(90);
    Sleep(stdSleep);

    //check RPS
    check_heading(180);
    Sleep(stdSleep2);

    getLocation();

    //move backward E
    drive(-(buttonX-refX-.5));
    Sleep(stdSleep);

    //check RPS
    check_x_plus(refX, (buttonX-refX-.5));
    Sleep(stdSleep2);

    //turn N
    turn(90);
    Sleep(stdSleep);

    //check RPS
    check_heading(90);
    Sleep(stdSleep2);

    //read CdS cell
    double lightColor = CdSCell.Value();

    //Red color will be 0, blue will be 1
    if (lightColor < (REDLIGHT+LIGHTTOL) && lightColor > (REDLIGHT-LIGHTTOL)) {
        buttonServo.SetDegree(180);
        LCD.Clear(RED);
    } else if (lightColor < (BLUELIGHT+LIGHTTOL) && lightColor > (BLUELIGHT-LIGHTTOL)) {
        buttonServo.SetDegree(-10);
        LCD.Clear(BLUE);
    } else {
        LCD.Clear();
        LCD.WriteLine("BLUE DEFAULT");

        Sleep(3.0);
        buttonServo.SetDegree(-10);
    }
    Sleep(1.);

    setMotorSpeed(30);

    //move backward S
    drive(-4.5);
    Sleep(stdSleep);

    resetMotorSpeed();

    float timeStart = TimeNow();

    while(RPS.IsDeadzoneActive() != 2 && TimeNow()-timeStart < 7) {
        rightMotor.SetPercent(-25);
        leftMotor.SetPercent(25);
    }
    rightMotor.Stop();
    leftMotor.Stop();

    Sleep(stdSleep);
}
//*********************************************************************************************************
void dropWrench() {
    getLocation();

    //move forward N
    drive(4);
    Sleep(stdSleep);

    //check RPS
    check_y_plus(refY, 4);
    Sleep(stdSleep2);

    //reset button arm
    buttonServo.SetDegree(90);

    //turn 90 degrees E
    turn(90);
    Sleep(stdSleep);

    //check RPS
    check_heading(0);
    Sleep(stdSleep2);

    getLocation();

    //move forward E
    drive(3);
    Sleep(stdSleep);

    //check RPS
    check_x_plus(refX, 3);
    Sleep(stdSleep2);

    //turn 90 degrees N
    turn(-90);
    Sleep(stdSleep);

    //check RPS
    check_heading(90);
    Sleep(stdSleep2);

    getLocation();

    //move forward N
    drive(22);
    Sleep(stdSleep);

    //check RPS
    check_y_plus(refY, 20);
    Sleep(stdSleep2);

    //turn NW
    turn(-47);
    Sleep(stdSleep);

    //check RPS
    check_heading(138);
    Sleep(stdSleep2);

    //move forward NW
    drive(20);
    Sleep(stdSleep);

    //REcheck heading
    check_heading(138);
    Sleep(stdSleep2);

    //move forward NW
    drive(11);
    Sleep(stdSleep);

    //lower fork arm
    liftServo.SetDegree(180);
    Sleep(1.);
}
//*********************************************************************************************************
void turnCrank() {
    //move backward SE
    drive(-8);
    Sleep(stdSleep);

    getLocation();

    //move backward SE with RPS
    drive(-3);
    Sleep(stdSleep);

    //check RPS
//    check_angle_drive(refX, refY, -3);
//    Sleep(stdSleep2);

    //turn NE
    turn(90);
    Sleep(stdSleep);

    //check RPS
    check_heading(45);
    Sleep(stdSleep2);

    //raise servo
    liftServo.SetDegree(65);

    //set up fork servo
    if(fuelType == 1) {
        forkServo.SetDegree(0);
    } else {
        forkServo.SetDegree(180);
    }

    Sleep(1.);

    getLocation();

    //move forward
    drive(12.5);
    Sleep(stdSleep);

    //check RPS
//    check_angle_drive(refX, refY, 12.5);
//    Sleep(stdSleep2);

    //turn fork servo correct direction
    if(fuelType == 1) {
        forkServo.SetDegree(180);
    } else {
        forkServo.SetDegree(0);
    }

    Sleep(1.5);
}
//*********************************************************************************************************
void goHome() {
    getLocation();

    //move backward
    drive(-12.5);
    Sleep(stdSleep);

    //check RPS
//    check_angle_drive(refX, refY, 12.5);
//    Sleep(stdSleep2);

    //turn 90 degrees NW
    turn(-90);
    Sleep(stdSleep);

    //check RPS
    check_heading(135);
    Sleep(stdSleep2);

    getLocation();

    //move backward SE
    drive(-18.5);
    Sleep(stdSleep);

    //check RPS
//    check_angle_drive(refX, refY, 19);
//    Sleep(stdSleep2);

    //turn N
    turn(45);
    Sleep(stdSleep);

    //check RPS
    check_heading(90);
    Sleep(stdSleep2);

    getLocation();

    //move backward S
    drive(-19);
    Sleep(stdSleep);

    //check RPS
    check_y_minus(refY, 17);
    Sleep(stdSleep2);

    //turn E
    turn(90);
    Sleep(stdSleep);

    //check RPS
    check_heading(0);
    Sleep(stdSleep2);

    getLocation();

    //move backward W
    drive(-14);
    Sleep(stdSleep);

    //check RPS
    check_x_minus(refX, 14);
    Sleep(stdSleep2);

    //turn S
    turn(90);
    Sleep(stdSleep);

    //check RPS
    check_heading(270);
    Sleep(stdSleep2);

    //move backward N
    drive(-20);
}
//*********************************************************************************************************

//Performance Tests
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
//probe
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

void performanceTestFour() {
    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Code has begun");

    //wait for CdS cell to start
    while(CdSCell.Value()>START);

    getLocation();

    //move forward
    drive(7);
    Sleep(0.5);

    //Check RPS
    check_y_minus(refY,7);
    Sleep(0.5);

    //turn 90 degrees CCW
    turn(-90);
    Sleep(0.5);

    //check RPS
    check_heading(0);

    getLocation();

    //has to add to 10.25

    //move forward (driving east to get over light)
    drive(8.55);
    Sleep(0.5);

    //check RPS
    check_x_plus(refX,8.55);
    Sleep(0.5);

    //turn 90 degrees CCW
    turn(-90);
    Sleep(0.5);

    //check RPS
    check_heading(90);
    Sleep(0.5);

    //move backward (driving south to hit buttons)
    drive(-4);
    Sleep(0.5);

    float timeStart = TimeNow();

    while(RPS.IsDeadzoneActive() != 2 && TimeNow()-timeStart < 7) {
        rightMotor.SetPercent(-25);
        leftMotor.SetPercent(25);
    }
    rightMotor.Stop();
    leftMotor.Stop();

    Sleep(0.5);

    //move forward
    drive(4);
    Sleep(0.5);

    //turn 90 degrees CW
    turn(90);
    Sleep(0.5);

    getLocation();

    //move forward (driving east towards wall to get in front of ramp)
    drive(2.25);
    Sleep(0.5);

    //check RPS
    check_x_plus(refX, 2.25);
    Sleep(0.5);

    //turn 90 degrees CCW
    turn(-90);
    Sleep(0.5);

    //check RPS
    check_heading(90);
    Sleep(0.5);

    getLocation();

    //move forward
    drive(23);
    Sleep(0.5);

    //check RPS
    check_y_plus(refY, 21);
    Sleep(0.5);

    //turn CCW
    turn(-45);
    Sleep(0.5);

    //check RPS
    check_heading(135);
    Sleep(0.5);

    getLocation();

    //move forward (northwest to get perpendicular with crank)
    drive(18);
    Sleep(0.5);

    //Check RPS
//    check_y_plus(refY, 11.67);
//    check_x_minus(refX, 11.67);
//    Sleep(1.);

    //turn 90 degrees CW
    turn(90);
    Sleep(0.5);

    //check RPS
    check_heading(45);
    Sleep(0.5);

    //raise servo
    liftServo.SetDegree(65);

    //set up fork servo
    if(fuelType == 1) {
        forkServo.SetDegree(0);
    } else {
        forkServo.SetDegree(180);
    }

    Sleep(1.);

    getLocation();

    //move forward
    drive(10.5);
    Sleep(0.5);

    //check RPS right before crank
    check_heading(45);
    Sleep(0.5);

    //move forward
    drive(2);
    Sleep(0.5);

    //check RPS
//    check_x_plus(refX, 10.6);
//    check_y_plus(refY, 10.6);
//    Sleep(1.);

    //turn fork servo correct direction
    if(fuelType == 1) {
        forkServo.SetDegree(180);
    } else {
        forkServo.SetDegree(0);
    }

    Sleep(1.5);

    getLocation();

    //move backward
    drive(-14);
    Sleep(0.5);

    //check RPS
//    check_x_minus(refX, 10.6);
//    check_y_minus(refY, 10.6);
//    Sleep(1.);

    //turn 90 degrees CCW
    turn(-90);
    Sleep(0.5);

    //check RPS
    check_heading(135);
    Sleep(0.5);

    getLocation();

    //move backward
    drive(-17.2);
    Sleep(0.5);

    //check RPS
//    check_y_minus(refY, 11.67);
//    check_x_plus(refX, 11.67);
//    Sleep(1.);

    //turn CW 45 degrees
    turn(45);
    Sleep(0.5);

    //check RPS
    check_heading(90);
    Sleep(0.5);

    getLocation();

    //move backward
    drive(-23);
    Sleep(0.5);

    //check RPS
    check_y_minus(refY, 15);
    Sleep(0.5);

    //done


}
