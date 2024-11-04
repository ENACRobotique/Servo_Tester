#include "STS3032.h"

STS3032 sts3032(&SD1);


SmartServo::Status STS3032::writeRegisterEPROM(uint8_t id, uint8_t reg, uint8_t value)
{

	writeRegister(id, R_Lock, 0);
	writeRegister(id, reg, value);
	writeRegister(id, R_Lock, 1);

    // // ping ID
    // if(auto ret = ping(id)) {return ret;}

    // // Unlock EEPROM
    // if(auto ret=writeRegister(id, R_Lock, 0)) {return ret;}

    // // Write new ID
    // if(auto ret=writeRegister(id, reg, value)) {return ret;}

    // // Lock EEPROM
    // if(auto ret=writeRegister(id, R_Lock, 1)) {return ret;}

    return SmartServo::OK;

}

SmartServo::Status STS3032::setID(uint8_t id, uint8_t newID){
    // check if newID already in use
    if (id >= 0xFE || newID >= 0xFE) {return SmartServo::INVALID_PARAMS;}

    // ping current ID
    if(auto ret = ping(id)) {return ret;}

    // Unlock EEPROM
    if(auto ret=writeRegister(id, R_Lock, 0)) {return ret;}

    // Write new ID
    if(auto ret=writeRegister(id, R_ServoID, newID)) {return ret;}

    // Lock EEPROM
    if(auto ret=writeRegister(newID, R_Lock, 1)) {return ret;}
    
    // ping newID
    return ping(newID);
}

SmartServo::Status STS3032::setBaudrate(uint8_t id, uint32_t speed)
{
	Baudrate baud;
	switch (speed)
	{
	case 1000000:
		baud = BD_1M;
		break;
	case 500000:
		baud = BD_500K;
		break;
	case 250000:
		baud = BD_250K;
		break;
	case 128000:
		baud = BD_128K;
		break;
	case 115200:
		baud = BD_115200;
		break;
	case 76800:
		baud = BD_76800;
		break;
	case 57600:
		baud = BD_57600;
		break;
	case 38400:
		baud = BD_38400;
		break;
	default:
		baud = BD_500K;
		speed = 500000;
		break;
	}

	writeRegister(id, R_Lock, 0);
	writeRegister(id, R_BaudRate, (uint8_t)baud);
	SmartServo::setSerialBaudrate(speed);
	SmartServo::Status status = writeRegister(id, R_Lock, 1);
	
	return status;
}


SmartServo::Status STS3032::move(uint8_t id, uint16_t position, bool reg_write){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_GoalPosition,
		.len = 2,
		.data = {0}
	};
	*(uint16_t*)rec.data = position;

	return write(&rec, reg_write);
}


int STS3032::readPosition(uint8_t id){
	SmartServo::record_t rec = {
		.id = id,
		.reg = R_CurrentPosition,
		.len = 2,
		.data = {0}
	};

	if(read(&rec) == SmartServo::Status::OK) {
		return *(uint16_t*)rec.data;
	} else {
		return -1;
	}
}



int STS3032::readResponseLevel(uint8_t id) {
    	SmartServo::record_t rec = {
		.id = id,
		.reg = R_ResponseStatusLevel,
		.len = 1,
		.data = {0}
	};

	if(read(&rec) == SmartServo::Status::OK) {
		response_level = (ResponseLevel)rec.data[0];
		return response_level;
	} else {
		return -1;
	}
}
