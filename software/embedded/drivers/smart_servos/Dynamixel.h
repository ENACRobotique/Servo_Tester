#pragma once

// #include <ch.h>
// #include <hal.h>
#include "smart_servo.h"


namespace Dynamixel{

	enum RotationDirection{
		Counterclockwise = 0,
		Clockwise = 1
	};

	enum Baudrate {
		BD_1M = 1,
		BD_500K = 3,
		BD_400K = 4,
		BD_250K = 7,
		BD_200K = 9,
		BD_115200 = 16,
		BD_57600 = 34,
		BD_19200 = 103,
		BD_9600 = 207
	};


	SmartServo::Status reset(uint8_t id);
	SmartServo::Status ping(uint8_t id);

	SmartServo::Status setID(uint8_t id, uint8_t newID);
	SmartServo::Status setBD(uint8_t id, long baud);

	SmartServo::Status move(uint8_t id, uint16_t position, bool reg_write=false);
	SmartServo::Status moveSpeed(uint8_t id, uint16_t position, uint16_t speed, bool reg_write=false);
	SmartServo::Status setEndless(uint8_t id, bool status);
	SmartServo::Status turn(uint8_t id, RotationDirection direction, uint16_t speed);
	


	// SmartServo::Status setTempLimit(uint8_t id, uint8_t temperature);
	// SmartServo::Status setAngleLimit(uint8_t id, int CWLimit, int CCWLimit);
	// SmartServo::Status setVoltageLimit(uint8_t id, uint8_t DVoltage, uint8_t UVoltage);
	// SmartServo::Status setMaxTorque(uint8_t id, int maxTorque);
	// /*Should not be used with SRL != 2. This library expects the Dynamixels to return a status packet for each command*/
	// SmartServo::Status setSRL(uint8_t id, uint8_t SRL);
	// SmartServo::Status setRDT(uint8_t id, uint8_t RDT);
	// SmartServo::Status setLEDAlarm(uint8_t id, uint8_t LEDAlarm);
	// SmartServo::Status setShutdownAlarm(uint8_t id, uint8_t SALARM);
	// SmartServo::Status setCMargin(uint8_t id, uint8_t CWCMargin, uint8_t CCWCMargin);
	// SmartServo::Status setCSlope(uint8_t id, uint8_t CWCSlope, uint8_t CCWCSlope);
	// SmartServo::Status setPunch(uint8_t id, int punch);

	
	int readPosition(uint8_t id);


	// int moving(uint8_t id);
	// int lockRegister(uint8_t id);
	// int RWStatus(uint8_t id);
	// int readTemperature(uint8_t id);
	// int readVoltage(uint8_t id);
	// int readSpeed(uint8_t id);
	// int readLoad(uint8_t id);
	// int torqueStatus(uint8_t id, bool status);
	// int ledStatus(uint8_t id, bool status);


	// The Dynamixel registers
	enum eRegister
	{
	    // ---------- EEPROM ------------

	    R_ModelNumber             = 0x00, // 2 Byte
	    R_FirmwareVersion         = 0x02, //
	    R_ServoID                 = 0x03, //         Write
	    R_BaudRate                = 0x04, //         Write
	    R_ReturnDelayTime         = 0x05, //         Write
	    R_CW_AngleLimit           = 0x06, // 2 Byte  Write
	    R_CCW_AngleLimit          = 0x08, // 2 Byte  Write
	    R_HighestLimitTemperature = 0x0B, //         Write
	    R_LowestLimitVoltage      = 0x0C, //         Write
	    R_HighestLimitVoltage     = 0x0D, //         Write
	    R_MaxTorque               = 0x0E, // 2 Byte  Write
	    R_StatusReturnLevel       = 0x10, //         Write
	    R_AlarmLED                = 0x11, //         Write
	    R_AlarmShutdown           = 0x12, //         Write

	    // ----------- RAM -------------

	    R_TorqueEnable            = 0x18, //         Write
	    R_LED                     = 0x19, //         Write
	    R_CW_ComplianceMargin     = 0x1A, //         Write
	    R_CCW_ComplianceMargin    = 0x1B, //         Write
	    R_CW_ComplianceSlope      = 0x1C, //         Write
	    R_CCW_ComplianceSlope     = 0x1D, //         Write
	    R_GoalPosition            = 0x1E, // 2 Byte  Write
	    R_MovingSpeed             = 0x20, // 2 Byte  Write
	    R_TorqueLimit             = 0x22, // 2 Byte  Write
	    R_PresentPosition         = 0x24, // 2 Byte
	    R_PresentSpeed            = 0x26, // 2 Byte
	    R_PresentLoad             = 0x28, // 2 Byte
	    R_PresentVoltage          = 0x2A, //
	    R_PresentTemperature      = 0x2B, //
	    R_RegisteredInstruction   = 0x2C, //         Write
	    R_Moving                  = 0x2E, //
	    R_Lock                    = 0x2F, //         Write
	    R_Punch                   = 0x30, // 2 Byte  Write
	};


};
