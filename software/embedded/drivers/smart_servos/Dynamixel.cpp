#include "Dynamixel.h"
#include <ch.h>
#include <hal.h>
#include <string.h>
#include "stdutil.h"
#include "smart_servo.h"

#define NO_OP asm("NOP")//__asm nop

namespace Dynamixel {
	
SmartServo::Status reset(uint8_t id){
	return smart_servo.reset(id);
}

SmartServo::Status ping(uint8_t id){
	return smart_servo.ping(id);
}

SmartServo::Status setID(uint8_t id, uint8_t newID){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_ServoID,
		.len = 1,
		.data = {newID}
	};

	return smart_servo.write(&rec);
}

SmartServo::Status setBD(uint8_t id, long baud){
	uint8_t baud_rate = (2000000/baud) - 1;

	SmartServo::record_t rec = {
		.id = id,
		.reg = R_BaudRate,
		.len = 1,
		.data = {baud_rate}
	};

	return smart_servo.write(&rec);
}

SmartServo::Status move(uint8_t id, uint16_t position, bool reg_write){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_GoalPosition,
		.len = 2,
		.data = {0}
		//.data = {(uint8_t)position, (uint8_t)(position >> 8)}
	};
	*(uint16_t*)rec.data = position;

	return smart_servo.write(&rec, reg_write);
}

SmartServo::Status moveSpeed(uint8_t id, uint16_t position, uint16_t speed, bool reg_write){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_GoalPosition,
		.len = 4,
		.data = {0}
		//.data = {(uint8_t)position, (uint8_t)(position >> 8)}
	};
	*(uint16_t*)rec.data = position;
	*((uint16_t*)rec.data + 1 )= speed;

	return smart_servo.write(&rec, reg_write);
}

SmartServo::Status setEndless(uint8_t id, bool status){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_CCW_AngleLimit,
		.len = 2,
		.data = {0}
	};

	if(!status) {
		turn(id, RotationDirection::Clockwise, 0);
		*(uint16_t*)rec.data = 1023;
	}

	return smart_servo.write(&rec);
}

SmartServo::Status turn(uint8_t id, RotationDirection direction, uint16_t speed){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_MovingSpeed,
		.len = 2,
		.data = {0}
	};
	*(uint16_t*)rec.data = speed | ((int)direction) << 10;

	return smart_servo.write(&rec);
}


// SmartServo::Status DynamixelSerial::moveSpeedRW(uint8_t id, int position, int speed){
// 	uint8_t data[5];
// 	data[0] = R_GoalPosition;
// 	data[1] = position & 0xFF;
// 	data[2] = (position >> 8) & 0xFF;
// 	data[3] = speed & 0xFF;
// 	data[4] = (speed >> 8) & 0xFF;
// 	sendInstruction(ID, I_RegWrite, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::torqueStatus(uint8_t id, bool status){
// 	uint8_t data[2];
// 	data[0] = R_TorqueEnable;
// 	data[1] = status ? 1 : 0;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::ledStatus(uint8_t id, bool status){
// 	uint8_t data[2];
// 	data[0] = R_LED;
// 	data[1] = status ? 1 : 0;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::readTemperature(uint8_t id){
// 	uint8_t data[2];
// 	data[0] = R_PresentTemperature;
// 	data[1] = 1;  // Temperature is only 1 byte
// 	sendInstruction(ID, I_ReadData, data, sizeof(data));
// 	int ret = readResponse(data);
// 	if(ret == 0) {
// 		return data[0];
// 	}
// 	return -1;
// }

int readPosition(uint8_t id){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_PresentPosition,
		.len = 2,
		.data = {0}
	};

	if(smart_servo.read(&rec) == SmartServo::Status::OK) {
		return *(uint16_t*)rec.data;
	} else {
		return -1;
	}
}

// int DynamixelSerial::readVoltage(uint8_t id){
// 	uint8_t data[2];
// 	data[0] = R_PresentVoltage;
// 	data[1] = 1;  // Voltage is 1 byte long
// 	sendInstruction(ID, I_ReadData, data, sizeof(data));
// 	int ret = readResponse(data);
// 	if(ret == 0) {
// 		return data[0];
// 	}
// 	return -1;
// }

// int DynamixelSerial::setTempLimit(uint8_t id, uint8_t temperature){
// 	uint8_t data[2];
// 	data[0] = R_HighestLimitTemperature;
// 	data[1] = temperature;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setVoltageLimit(uint8_t id, uint8_t DVoltage, uint8_t UVoltage){
// 	uint8_t data[3];
// 	data[0] = R_LowestLimitVoltage;
// 	data[1] = DVoltage;
// 	data[2] = UVoltage;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setAngleLimit(uint8_t id, int CWLimit, int CCWLimit){
// 	uint8_t data[6];
// 	data[0] = R_CW_AngleLimit;
// 	data[1] = CWLimit & 0xFF;
// 	data[2] = (CWLimit >> 8) & 0xFF;
// 	data[3] = R_CCW_AngleLimit;
// 	data[4] = CCWLimit & 0xFF;
// 	data[5] = (CCWLimit >> 8) & 0xFF;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setMaxTorque(uint8_t id, int maxTorque){
// 	uint8_t data[3];
// 	data[0] = R_MaxTorque;
// 	data[1] = maxTorque & 0xFF;
// 	data[2] = (maxTorque >> 8) & 0xFF;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setSRL(uint8_t id, uint8_t SRL){
// 	uint8_t data[2];
// 	data[0] = R_StatusReturnLevel;
// 	data[1] = SRL;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setRDT(uint8_t id, uint8_t RDT){
// 	uint8_t data[2];
// 	data[0] = R_ReturnDelayTime;
// 	data[1] = RDT;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setLEDAlarm(uint8_t id, uint8_t LEDAlarm){
// 	uint8_t data[2];
// 	data[0] = R_AlarmLED;
// 	data[1] = LEDAlarm;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setShutdownAlarm(uint8_t id, uint8_t SALARM){
// 	uint8_t data[2];
// 	data[0] = R_AlarmShutdown;
// 	data[1] = SALARM;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setCMargin(uint8_t id, uint8_t CWCMargin, uint8_t CCWCMargin){
// 	uint8_t data[4];
// 	data[0] = R_CW_ComplianceMargin;
// 	data[1] = CWCMargin;
// 	data[2] = R_CCW_ComplianceMargin;
// 	data[3] = CCWCMargin;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setCSlope(uint8_t id, uint8_t CWCSlope, uint8_t CCWCSlope){
// 	uint8_t data[4];
// 	data[0] = R_CW_ComplianceSlope;
// 	data[1] = CWCSlope;
// 	data[2] = R_CCW_ComplianceSlope;
// 	data[3] = CCWCSlope;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::setPunch(uint8_t id, int punch){
// 	uint8_t data[3];
// 	data[0] = R_Punch;
// 	data[1] = punch & 0xFF;
// 	data[2] = (punch >> 8) & 0xFF;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::moving(uint8_t id){
// 	uint8_t data[2];
// 	data[0] = R_Moving;
// 	data[1] = 1;  // moving is 1 byte long
// 	sendInstruction(ID, I_ReadData, data, sizeof(data));
// 	int ret = readResponse(data);
// 	if(ret == 0) {
// 		return data[0];
// 	}
// 	return -1;
// }

// int DynamixelSerial::lockRegister(uint8_t id){
// 	uint8_t data[2];
// 	data[0] = R_Lock;
// 	data[1] = 1;
// 	sendInstruction(ID, I_WriteData, data, sizeof(data));
// 	return readResponse(nullptr);
// }

// int DynamixelSerial::RWStatus(uint8_t id){
// 	uint8_t data[2];
// 	data[0] = R_RegisteredInstruction;
// 	data[1] = 1;  // Registered Instruction is 1 byte long
// 	sendInstruction(ID, I_ReadData, data, sizeof(data));
// 	int ret = readResponse(data);
// 	if(ret == 0) {
// 		return data[0];
// 	}
// 	return -1;
// }

// int DynamixelSerial::readSpeed(uint8_t id){
// 	uint8_t data[2];
// 	data[0] = R_PresentSpeed;
// 	data[1] = 2;  // Speed is 2 bytes long
// 	sendInstruction(ID, I_ReadData, data, sizeof(data));
// 	int ret = readResponse(data);
// 	if(ret == 0) {
// 		return (data[1]<<8) | data[0];
// 	}
// 	return -1;
// }

// int DynamixelSerial::readLoad(uint8_t id){
// 	uint8_t data[2];
// 	data[0] = R_PresentLoad;
// 	data[1] = 2;  // Load is 2 bytes long
// 	sendInstruction(ID, I_ReadData, data, sizeof(data));
// 	int ret = readResponse(data);
// 	if(ret == 0) {
// 		return (data[1]<<8) | data[0];
// 	}
// 	return -1;
// }




}