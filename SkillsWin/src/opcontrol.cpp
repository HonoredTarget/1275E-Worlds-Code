// update in the autonomous.cpp as well
#define BACK_LEFT_WHEEL_PORT 1
#define BACK_RIGHT_WHEEL_PORT 9
#define FRONT_LEFT_WHEEL_PORT 5
#define FRONT_RIGHT_WHEEL_PORT 8
#define LEFT_LIFT_PORT 6
#define TILTER_PORT 4
#define RIGHT_INTAKE_MOTOR 10
#define LEFT_INTAKE_MOTOR 7

// Ports 2, 3 Are dead

#define POTENTIOMETER_PORT 7

#include "main.h"
#include <cmath>

using namespace okapi;

double trayCurveV1(double c);
double trayCurveV2(double c);
double trayCurveV3(double c);
double EdwinTrayCurveV1(double c);
double EdwinTrayCurveV2(double c);
double EdwinTrayCurveV3(double c);
double convert100(double c);
double convert200(double c);

double vCurve(double val);
double correct(double c);

void opcontrol()
{
	pros::Controller master(pros::E_CONTROLLER_MASTER);

	pros::ADIPotentiometer potent(POTENTIOMETER_PORT);

	/*
	Motors on wheelbase: 200rpm
	Motors on intake: 200rpm
	Motor on tilter 100rpm
	Motor on lift: 100rpm

	Red: 100rpm
	Green: 200rpm
	Blue: 600rpm
	*/

	pros::Motor leftLift(LEFT_LIFT_PORT, MOTOR_GEARSET_36);
	pros::Motor backLeft(BACK_LEFT_WHEEL_PORT, MOTOR_GEARSET_18);
	pros::Motor frontLeft(FRONT_LEFT_WHEEL_PORT, MOTOR_GEARSET_18, true);
	pros::Motor backRight(BACK_RIGHT_WHEEL_PORT, MOTOR_GEARSET_18, true);
	pros::Motor frontRight(FRONT_RIGHT_WHEEL_PORT, MOTOR_GEARSET_18);
	pros::Motor rightIntake(RIGHT_INTAKE_MOTOR, MOTOR_GEARSET_18, true);
	pros::Motor leftIntake(LEFT_INTAKE_MOTOR, MOTOR_GEARSET_18);
	pros::Motor tilter(TILTER_PORT, MOTOR_ENCODER_DEGREES);
	/* #define BACK_LEFT_WHEEL_PORT 1
	#define BACK_RIGHT_WHEEL_PORT 9
	#define FRONT_LEFT_WHEEL_PORT 5
	#define FRONT_RIGHT_WHEEL_PORT 8
	#define LEFT_LIFT_PORT 6
	#define TILTER_PORT 4
	#define RIGHT_INTAKE_MOTOR 10
	#define LEFT_INTAKE_MOTOR 7
	*/


	//int roatationMax = 1523;
	int backupSpeed = -27;
	int multiplier = 1;
	int liftMove, lx, ly, rx, fr, fl, bl, br;
	int intakeSpeed = -60;
	tilter.set_zero_position(0);
	leftLift.set_brake_mode(MOTOR_BRAKE_HOLD);
	while (true)
	{

		// Mechanum Drive
		ly = vCurve(master.get_analog(ANALOG_LEFT_Y)) * multiplier;
		lx = vCurve(master.get_analog(ANALOG_LEFT_X));
		rx = vCurve(master.get_analog(ANALOG_RIGHT_X)) * multiplier;

		if (rx <= 30 && rx >= -30)
		{
			rx = 0;
		}
		lx = 0.89 * lx;

		fr = correct(ly - lx - rx);
		br = correct(ly - lx + rx);
		fl = correct(ly + lx + rx);
		bl = correct(ly + lx - rx);

		frontRight.move_velocity(convert200(fr));
		frontLeft.move_velocity(convert200(fl));
		backLeft.move_velocity(convert200(bl));
		backRight.move_velocity(convert200(br));
		// End Mechanum Drive

		// Lift Control
		liftMove = vCurve(master.get_analog(ANALOG_RIGHT_Y));

		if (!(liftMove <= 10 && liftMove >= -10))
		{
			leftLift.move_velocity(convert100(1.0 * liftMove));
		}
		else
		{
			leftLift.move_velocity(0);
		}
		// End Lift Control

		// Start Intake Control
		auto right1 = master.get_digital(DIGITAL_R1);
		auto right2 = master.get_digital(DIGITAL_R2);

		if (right1)
		{
			leftIntake.move_velocity(convert200(127));
			rightIntake.move_velocity(convert200(127));
		}
		else if (right2)
		{
			leftIntake.move_velocity(convert200(-127));
			rightIntake.move_velocity(convert200(-127));
		}
		else
		{
			leftIntake.move_velocity(convert200(0));
			rightIntake.move_velocity(convert200(0));
		}
		// End Intake Control

		// Start Tilter Control
		auto left1 = master.get_digital(DIGITAL_L1);
		auto left2 = master.get_digital(DIGITAL_L2);
		if (left1)
		{
			tilter = -127;
		}
		else if (left2)
		{
			tilter = EdwinTrayCurveV3(potent.get_value());
		}
		else
		{
			tilter = 0;
		}

		// End Tilter Control

		//ON / OFF Hold
		auto aPress = master.get_digital_new_press(DIGITAL_A);
		if (aPress)
		{
			leftLift.set_brake_mode(MOTOR_BRAKE_COAST);
		}

		// Start Backup Macro
		auto autoBackup = master.get_digital(DIGITAL_DOWN);
		auto firstPress = master.get_digital_new_press(DIGITAL_DOWN);
		if (autoBackup)
		{
			if (firstPress)
			{
				tilter = -127;
				pros::delay(300);
				tilter = 0;
			}

			backLeft.move_velocity(convert200(backupSpeed));
			backRight.move_velocity(convert200(backupSpeed));
			frontLeft.move_velocity(convert200(backupSpeed));
			frontRight.move_velocity(convert200(backupSpeed));
			leftIntake.move_velocity(convert200(intakeSpeed + 5));
			rightIntake.move_velocity(convert200(intakeSpeed + 5));
		}
		// End Backup Macro

		// Switch rotation
		/*
		auto rightArrow = master.get_digital_new_press(DIGITAL_RIGHT);
		if (rightArrow)
		{
			if (multiplier == 1)
				multiplier = -1;
			else
				multiplier = 1;
		}
		*/
		// Intake + Lift
		if (master.get_digital(DIGITAL_L2) && !master.get_digital(DIGITAL_R1) && !master.get_digital(DIGITAL_R2) && potent.get_value() < 930)
		{
			leftIntake.move_velocity(convert200(67));
			rightIntake.move_velocity(convert200(67));
		}

		int rotations = potent.get_value();
		pros::delay(20);
	}
}

double vCurve(double val)
{
	double modVal = pow(0.0292 * val, 3.0) + 0.59 * val;
	if (modVal >= 127)
		modVal = 127;
	else if (modVal <= -127)
		modVal = -127;
	return modVal;
}
double correct(double c)
{
	double x = c;
	if (c > 127)
		x = 127;
	else if (c < -127)
		x = -127;
	return x;
}

double trayCurveV1(double c)
{
	double curve = ((-1) * pow(1.0032, (c + 230)) + 120);
	if (curve < 30)
		curve = 30;
	if (curve >= 127)
		curve = 127;
	return curve;
}

double trayCurveV2(double c)
{
	double curve = 195 - sqrt((1.0 / 9000000.0) * pow(c, 3) + 120);
	if (curve < 30)
		curve = 30;
	if (curve >= 127)
		curve = 127;
	return curve;
}

double EdwinTrayCurveV1(double c)
{
	double curve = 60 + (0.00000004 * pow((1000 - c), 3));
	if (curve < 30)
		curve = 30;
	if (curve >= 127)
		curve = 127;
	return curve;
}

double EdwinTrayCurveV2(double c)
{
	double curve;
	if (c >= 900)
	{
		curve = 100 - sqrt(5 * (c - 570));
	}
	else
	{
		curve = 110 + (0.0000003 * pow((350 - c), 3));
	}
	if (curve < 30)
		curve = 30;
	if (curve >= 127)
		curve = 127;
	return curve*1.2;
}

double EdwinTrayCurveV3(double c)
{
	double curve;
	if (c >= 900)
	{
		curve = 100 - sqrt(5 * (c - 570));
	}
	else
	{
		curve = 110 + (0.0000003 * pow((350 - c), 3));
	}
	if (curve < 30)
		curve = 30;
	if (curve >= 127)
		curve = 127;
	return curve*1.1;
}

double convert100(double c)
{
	double convert = 100.0 * (c / 127.0);
	return convert;
}

double convert200(double c)
{
	double convert = 200.0 * (c / 127.0);
	return convert;
}