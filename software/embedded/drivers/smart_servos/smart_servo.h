#pragma once
#include <ch.h>
#include <hal.h>

constexpr size_t MAX_DATA_LEN = 10;
constexpr uint8_t BROADCAST_ID = 254;

class SmartServo {
public:

    typedef struct {
        uint8_t id;
        uint8_t reg;
        uint8_t data[MAX_DATA_LEN];
        uint8_t len;
    } record_t;


    enum Status {
        OK = 0,
        INVALID_PARAMS,
        TX_BUFFER_OVERFLOW,
    };

    SmartServo(SerialDriver* s): sd(s){}
    
    /**
     * Send ping to servo.
     */
    Status ping(uint8_t id);

    /**
     * Read @record.len bytes from servo @record.id, starting at address record.reg.
     */
    Status read(record_t* record);

    /**
     * Write data to servo.
     * @param is_reg_write Perform a registered write. The servo then wait for ACTION to execute the command.
     */
    Status write(record_t* record, bool is_reg_write=false);

    /**
     * Execute command given with reg_write
     */
    Status action(uint8_t id);

    /**
     * Reset servo to factorey default values
     */
    Status reset(uint8_t id);

    /**
     * Sync write: write to multiples servos with a single packet.
     * All records must share the same register address and the same data lenght.
     * Data can differ for each ID.
     */
    Status sync_write(record_t* records, size_t nb_records);
    

    


private:
    enum Instruction {
        SMART_SERVO_PING = 0x01,
        SMART_SERVO_READ = 0x02,
        SMART_SERVO_WRITE = 0x03,
        SMART_SERVO_REG_WRITE = 0x04,
        SMART_SERVO_ACTION = 0x05,
        SMART_SERVO_RESET = 0x06,
        //SMART_SERVO_REBOOT = 0x08,
        SMART_SERVO_SYNC_WRITE = 0x83,
        //SMART_SERVO_BULK_READ = 0x92,
    };

    SerialDriver* sd;
};