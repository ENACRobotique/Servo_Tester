#include "STS3032.h"

namespace STS3032 {

SmartServo::Status writeRegisterEPROM(uint8_t id, uint8_t reg, uint8_t value)
{
    // ping ID
    if(auto ret = smart_servo.ping(id)) {return ret;}

    // Unlock EEPROM
    if(auto ret=smart_servo.writeRegister(id, R_Lock, 0)) {return ret;}

    // Write new ID
    if(auto ret=smart_servo.writeRegister(id, reg, value)) {return ret;}

    // Lock EEPROM
    if(auto ret=smart_servo.writeRegister(id, R_Lock, 1)) {return ret;}

    return SmartServo::OK;

}

SmartServo::Status reset(uint8_t id)
{
    return smart_servo.reset(id);
}

SmartServo::Status ping(uint8_t id)
{
    return smart_servo.ping(id);
}

SmartServo::Status setID(uint8_t id, uint8_t newID){
    // check if newID already in use
    if (id >= 0xFE || newID >= 0xFE) {return SmartServo::INVALID_PARAMS;}

    // ping current ID
    if(auto ret = smart_servo.ping(id)) {return ret;}

    // Unlock EEPROM
    if(auto ret=smart_servo.writeRegister(id, R_Lock, 0)) {return ret;}

    // Write new ID
    if(auto ret=smart_servo.writeRegister(id, R_ServoID, newID)) {return ret;}

    // Lock EEPROM
    if(auto ret=smart_servo.writeRegister(newID, R_Lock, 1)) {return ret;}
    
    // ping newID
    return ping(newID);
}


SmartServo::Status setBD(uint8_t id, Baudrate baud){
    return writeRegisterEPROM(id, R_BaudRate, (uint8_t)baud);
}

SmartServo::Status move(uint8_t id, uint16_t position, bool reg_write){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_GoalPosition,
		.len = 2,
		.data = {0}
	};
	*(uint16_t*)rec.data = position;

	return smart_servo.write(&rec, reg_write);
}


int readPosition(uint8_t id){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_CurrentPosition,
		.len = 2,
		.data = {0}
	};

	if(smart_servo.read(&rec) == SmartServo::Status::OK) {
		return *(uint16_t*)rec.data;
	} else {
		return -1;
	}
}



int readResponseLevel(uint8_t id) {
    	SmartServo::record_t rec = {
		.id = id,
		.reg = R_ResponseStatusLevel,
		.len = 1,
		.data = {0}
	};

	if(smart_servo.read(&rec) == SmartServo::Status::OK) {
        uint8_t response_level = rec.data[0];
        smart_servo.setResponseLevel(response_level);
		return response_level;
	} else {
		return -1;
	}
}



}