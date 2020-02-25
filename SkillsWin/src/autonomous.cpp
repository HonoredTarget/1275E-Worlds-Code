#include "main.h"
#include "globals.hpp"

// update in the autonomous.cpp as well
#define BACK_LEFT_WHEEL_PORT 1
#define BACK_RIGHT_WHEEL_PORT 9
#define FRONT_LEFT_WHEEL_PORT 5
#define FRONT_RIGHT_WHEEL_PORT 8
#define LEFT_LIFT_PORT 6
#define TILTER_PORT 4
#define RIGHT_INTAKE_MOTOR 10
#define LEFT_INTAKE_MOTOR 7


using namespace okapi;  

// Drive Regularly
//auto drive = ChassisControllerFactory::create( {BACK_LEFT_WHEEL_PORT, -FRONT_LEFT_WHEEL_PORT}, {-BACK_RIGHT_WHEEL_PORT, FRONT_RIGHT_WHEEL_PORT}, AbstractMotor::gearset::green,{4_in, 12_in} );
// Strafe Drive
auto drive = ChassisControllerBuilder()
                .withMotors({BACK_LEFT_WHEEL_PORT, -BACK_RIGHT_WHEEL_PORT}, {-FRONT_LEFT_WHEEL_PORT, FRONT_RIGHT_WHEEL_PORT})
                // Green gearset, 4 in wheel diam, 11.5 in wheel track
                .withDimensions(AbstractMotor::gearset::green, {{4_in, 12_in}, imev5GreenTPR})
                .build();


auto intake = AsyncPosControllerBuilder().withMotor({-RIGHT_INTAKE_MOTOR, LEFT_INTAKE_MOTOR}).build();
auto lift = AsyncPosControllerBuilder().withMotor(LEFT_LIFT_PORT).build();
auto tilter = AsyncPosControllerBuilder().withMotor(TILTER_PORT).build();
	


// Other Commands
void strafeLeft();
void strafeRight();
void dropStack();
void intakeCubes(int targetToIntakeTo);
void deploy();

// Autons
void fourStack (int isRight);
void sevenStack(int isRight);
void onePoint();
void skillsAuton();

// 1 : Blue Protected
// 2 : Blue Unprotected
// 3 : Red Protected
// 4 : Red Unprotected
// 5 : Experimental
// 6 : Skills

void autonomous() {
    //onePoint();
    deploy();
    switch (autonomousPreSet) {
        case 1:
            // Blue Protected
            sevenStack(1);
            break;
        case 2:
            // Blue Unprotected
            onePoint();
            break;
        case 3:
            // Red Protected
            sevenStack(-1);
            break;
        case 4:
            // Red Unprotected
            onePoint();
            break;
        case 5:
            // Experimental
            drive->setMaxVelocity(50);
            lift->tarePosition();
            lift->setTarget(-300);

            intakeCubes(100000000);
            drive->moveDistance(4_ft);
            //intake.stop();    

            //onePoint();
            //onePoint();
            break;
        case 6:
            // Skills
            skillsAuton();
            break;
        default:
            // No Selection
            break;
    }
};

// Autons
void onePoint() {
    drive->setMaxVelocity(127);

    drive->moveDistance(-4_ft);
    pros::delay(500);
    drive->moveDistance(4_ft);
}

void skillsAuton() {
    
    deploy();
    
    // Move down the lift
    lift->tarePosition();
    lift->setTarget(-300);
    intake->setMaxVelocity(100);
    intakeCubes(1000000);
    // Drive to the pole
    drive->setMaxVelocity(80);
    drive->moveDistance(8_ft);

    //
}

void fourStack (int isRight) {
    deploy();
    pros::delay(100);
    intakeCubes(5000000);
    drive->setMaxVelocity(70);
    drive->moveDistance(3.5_ft);
    drive->waitUntilSettled();
    drive->turnAngle(50_deg * isRight);    
    drive->waitUntilSettled();
    drive->moveDistance(0.25_ft);
    drive->waitUntilSettled();
    drive->moveDistance(-1.5_ft);
    drive->turnAngle(600_deg * isRight);
}

void sevenStack( int isRight) {

    

    // Slow max velocity
    drive->setMaxVelocity(70);

    // Drive 4ft, intaking cubes
    intakeCubes(10000000);
    drive->moveDistanceAsync(1.95_ft);
    pros::delay(1300);

    lift->tarePosition();
    lift->setTarget(2200);
    drive->waitUntilSettled();
    pros::delay(1500);
    drive->setMaxVelocity(30);

    drive->moveDistance(0.36_ft);
    // Wait to settles
    //drive.waitUntilSettled();
    drive->setMaxVelocity(70);
    // move the lift down, all while intaking cubes
    lift->setMaxVelocity(180);
    lift->setTarget(0);
    lift->waitUntilSettled();

    drive->moveDistance(0.6_ft);

    drive->setMaxVelocity(300);
    // now turn to the left (or right)
    drive->turnAngle(780_deg * isRight);    
    drive->waitUntilSettled();

    // drive to the edge of the stack.
    drive->moveDistance(2_ft);
    drive->waitUntilSettled();
    // Drop the stack
    //dropStack();
    
    
};

// If the bot needs to turn right, then set isRight to 1, otherwise, set it to -1.

// Commands

void strafeLeft(double seconds) {
    
};

void strafeRight(double seconds) {

};

void intakeCubes(int targetToIntakeTo) {
			
	intake->tarePosition();

	intake->setTarget(targetToIntakeTo);
};

void dropStack() {

    intake->setMaxVelocity(170);
    intake->tarePosition();
    intake->setTarget(-300);
    intake->waitUntilSettled();

    tilter->setMaxVelocity(100);
    tilter->tarePosition();
    tilter->setTarget(1250);

    intake->setMaxVelocity(10);
    intake->tarePosition();
    intake->setTarget(90000);
    
    tilter->waitUntilSettled();
    tilter->setTarget(0);

    intake->setMaxVelocity(30);
    intake->tarePosition();
    intake->setTarget(-9000);
    pros::delay(400);
    
    drive->setMaxVelocity(20);
    drive->moveDistance(-1.5_ft);
};

void deploy() {
			
	intake->tarePosition();

	intake->setTarget(-1000);
    intake->waitUntilSettled();
}

