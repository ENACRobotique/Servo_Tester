//
// Created by fabien on 10/10/2019.
//

#pragma once

#include "AbstractState.h"

class CANState : public AbstractState {
public:
    CANState();
    void enter(int32_t pos_enc);
    AbstractState* onUiEvent(struct UiState ui_state);
    AbstractState* periodic();
    void leave();
    static const uint16_t SERVO_MIN = 500;
    static const uint16_t SERVO_MAX = 2500;

private:


};

extern CANState canState;
