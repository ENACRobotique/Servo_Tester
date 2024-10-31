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
        uint8_t len;
        uint8_t data[MAX_DATA_LEN];
    } record_t;


    enum Status {
        OK = 0,
        // dynamixel status packet errors
        INPUT_VOLTAGE_ERROR = 1<<0,
        ANGLE_LIMIT_ERROR   = 1<<1,
        OVERHEATING_ERROR   = 1<<2,
        RANGE_ERROR         = 1<<3,
        CHECKSUM_ERROR      = 1<<4,
        OVERLOAD_ERROR      = 1<<5,
        INSTRUCTION_ERROR   = 1<<6,

        // errors from this lib
        INVALID_PARAMS      = 1<<8,
        TX_BUFFER_OVERFLOW  = 1<<9,
        STATUS_TIMEOUT      = 1<<10,
        ECHO_ERROR          = 1<<11,
    };

    SmartServo(SerialDriver* s): sd(s), response_level(RL_NORMAL), timeout(TIME_MS2I(10)), echo_timeout(TIME_MS2I(2)){}

    void init();
    void setBaudrate(uint32_t speed);
    
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

    // Write one byte to register
    Status writeRegister(uint8_t id, uint8_t reg, uint8_t value);

    void setResponseLevel(uint8_t rl) {response_level = (ResponseLevel)rl;}

private:

    Status readStatus();
    Status readEcho();
    void flushSerialInput();
    

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

    enum ResponseLevel {
        RL_SILENT = 0,
        RL_NORMAL = 1,
    };

    SerialDriver* sd;
    enum ResponseLevel response_level;
    sysinterval_t timeout;
    sysinterval_t echo_timeout;
};

extern SmartServo smart_servo;
