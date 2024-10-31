//
// Created by fabien on 10/10/2019.
//

#ifndef EMBEDDED_DYNAMIXELSTATE_H
#define EMBEDDED_DYNAMIXELSTATE_H

#include "AbstractState.h"
#include "Dynamixel.h"

enum DynamixelFunction {
    E_DynamixelMove,
    E_DynamixelSetId,
    E_DynamixelSetSpeed,
    E_DynamixelSetTorque,
};

enum SmartServoType {
    S_Dynamixel,
    S_STS3032,
};

class SmartServoFnMenuState;

class SmartServoIDState : public AbstractState {
public:
    SmartServoIDState();
    void enter(int32_t pos_enc);
    AbstractState* onUiEvent(struct UiState ui_state);
    AbstractState* periodic();
    void leave();

    uint8_t get_servo_id() {
        return smart_servo_id;
    }

    void set_dynamixel_id(uint8_t new_id) {
        smart_servo_id = new_id;
    }

    void set_servo_type(enum SmartServoType type) {servo_type = type;}

private:
    int32_t pos_enc_init;
    char txt_id[17];
    uint8_t smart_servo_id;
    enum SmartServoType servo_type;

    friend SmartServoFnMenuState;

};

////////////////////////////////////////////////////////////////////////////////

class SmartServoFnMenuState : public AbstractState {
public:
    SmartServoFnMenuState();
    void enter(int32_t pos_enc);
    AbstractState* onUiEvent(struct UiState ui_state);
    AbstractState* periodic();
    void leave();
    DynamixelFunction get_function() {
        return selectedFn;
    }

private:
    int32_t pos_enc_init;
    DynamixelFunction selectedFn;

};

////////////////////////////////////////////////////////////////////////////////

class SmartServoMoveState : public AbstractState {
public:
    SmartServoMoveState();
    void enter(int32_t pos_enc);
    AbstractState* onUiEvent(struct UiState ui_state);
    AbstractState* periodic();
    void leave();

private:
    int32_t pos_enc_init;
    int32_t last_enc;
    int16_t pos;
    char txt_position[17];
    Dynamixel::RotationDirection endless_direction;

};

extern SmartServoIDState smartServoIDState;
extern SmartServoFnMenuState smartServoFnMenuState;
extern SmartServoMoveState smartServoMoveState;


#endif //EMBEDDED_DYNAMIXELSTATE_H
