#include "smart_servo.h"
#include <ch.h>
#include <hal.h>
#include "string.h"

// au pif
constexpr size_t TX_BUF_LEN = 10*MAX_DATA_LEN;

typedef struct __attribute__((packed)) {
    uint16_t STX;
    uint8_t id;
    uint8_t len;
    uint8_t instruction;
    uint8_t params[TX_BUF_LEN+1];
} servo_msg_t;

servo_msg_t servo_msg;

static uint8_t compute_chk(servo_msg_t* msg);

static void set_chk(servo_msg_t* msg, uint8_t chk) {
    *((uint8_t*)msg + msg->len + 5) = chk;
}

static void send_msg(SerialDriver* sd, servo_msg_t* msg) {
    sdWrite(sd, (uint8_t*)msg, msg->len+6);
}

SerialConfig sdconf = {
		.speed = 1000000,
		.cr1 = 0,
		.cr2 = USART_CR2_STOP1_BITS,
		.cr3 = USART_CR3_HDSEL
};



SmartServo::Status SmartServo::ping(uint8_t id) {
    servo_msg.STX = 0xFFFF;
    servo_msg.id = id;
    servo_msg.len = 2;
    servo_msg.instruction = Instruction::SMART_SERVO_PING;
    set_chk(&servo_msg, compute_chk(&servo_msg));
    send_msg(sd, &servo_msg);

    return Status::OK;
}

SmartServo::Status SmartServo::read(record_t *record) {
    servo_msg.STX = 0xFFFF;
    servo_msg.id = record->id;
    servo_msg.len = 4;
    servo_msg.instruction = Instruction::SMART_SERVO_READ;
    servo_msg.params[0] = record->reg;
    servo_msg.params[1] = record->len;
    set_chk(&servo_msg, compute_chk(&servo_msg));
    send_msg(sd, &servo_msg);

    return Status::OK;
}

SmartServo::Status SmartServo::write(record_t *record, bool is_reg_write) {
    servo_msg.STX = 0xFFFF;
    servo_msg.id = record->id;
    servo_msg.len = record->len + 3;
    if(is_reg_write) {
        servo_msg.instruction = Instruction::SMART_SERVO_REG_WRITE;
    } else {
        servo_msg.instruction = Instruction::SMART_SERVO_WRITE;
    }
    servo_msg.params[0] = record->reg;
    memcpy(&servo_msg.params[1], record->data, record->len);
    set_chk(&servo_msg, compute_chk(&servo_msg));
    send_msg(sd, &servo_msg);

    return Status::OK;
}

SmartServo::Status SmartServo::action(uint8_t id) {
    servo_msg.STX = 0xFFFF;
    servo_msg.id = id;
    servo_msg.len = 2;
    servo_msg.instruction = Instruction::SMART_SERVO_ACTION;
    set_chk(&servo_msg, compute_chk(&servo_msg));
    send_msg(sd, &servo_msg);

    return Status::OK;
}

SmartServo::Status SmartServo::reset(uint8_t id) {
    servo_msg.STX = 0xFFFF;
    servo_msg.id = id;
    servo_msg.len = 2;
    servo_msg.instruction = Instruction::SMART_SERVO_RESET;
    set_chk(&servo_msg, compute_chk(&servo_msg));
    send_msg(sd, &servo_msg);

    return Status::OK;
}

SmartServo::Status SmartServo::sync_write(record_t *records, size_t nb_records) {
    if(nb_records < 1) {
        return Status::INVALID_PARAMS;
    }

    

    uint8_t records_reg = records[0].reg;
    uint8_t records_len = records[0].len;
    
    servo_msg.STX = 0xFFFF;
    servo_msg.id = BROADCAST_ID;
    servo_msg.len = nb_records * (records_len+1) + 4;
    if(servo_msg.len > TX_BUF_LEN) {
        return Status::TX_BUFFER_OVERFLOW;
    }
    servo_msg.instruction = Instruction::SMART_SERVO_SYNC_WRITE;
    servo_msg.params[0] = records_reg;
    servo_msg.params[1] = records_len;

    for(size_t i=0; i<nb_records; i++) {
        if(records[i].len != records_len || records[i].reg != records_reg) {
            // len and reg must be the same in all records
            return Status::INVALID_PARAMS;
        }
        uint8_t* rec_id_p = &servo_msg.params[2] + i * (records_len+1);
        uint8_t* data_p = rec_id_p + 1;
        *rec_id_p = records[i].id;
        memcpy(data_p, records[i].data, records_len);
    }

    set_chk(&servo_msg, compute_chk(&servo_msg));
    send_msg(sd, &servo_msg);

    return Status::OK;
}

static uint8_t compute_chk(servo_msg_t* msg) {
    uint8_t chk = 0;
    size_t chk_len = msg->len + 1;
    uint8_t* data = (uint8_t*) &msg->id;
    for(size_t i=0; i<chk_len; i++) {
        chk += data[i];
    }
    chk = ~chk;
    return chk;
}
