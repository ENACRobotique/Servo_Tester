#include "SmartServoState.h"
#include "Dynamixel.h"
#include "lcd.h"
#include "printf.h"
#include "MainMenuState.h"
#include "stdutil.h"
#include "smart_servo.h"
#include "STS3032.cpp"


SmartServoIDState smartServoIDState = SmartServoIDState();
SmartServoFnMenuState smartServoFnMenuState = SmartServoFnMenuState();
SmartServoMoveState smartServoMoveState = SmartServoMoveState();


SmartServoIDState::SmartServoIDState() {
    pos_enc_init = 0;
    smart_servo_id = 0;
}

void SmartServoIDState::enter(int32_t pos_enc) {
    pos_enc_init = pos_enc - smart_servo_id;
    if (servo_type == S_Dynamixel) {
        chsnprintf(txt_id, sizeof(txt_id), "Dynamixel ID=%3d", smart_servo_id);
    } else {
        chsnprintf(txt_id, sizeof(txt_id), "STS3032 ID=%3d", smart_servo_id);
    }
    
    lcdWriteString(&LCDD1, txt_id, 0);
    char blank[] = "                ";
    lcdWriteString(&LCDD1, blank, 40);
}

AbstractState* SmartServoIDState::onUiEvent(struct UiState ui_state) {

    if(ui_state.enc_changed) {
        smart_servo_id = (((ui_state.pos_enc - pos_enc_init) % 255) + 255) % 255;     // max ID is 254, the broadcats ID
        if (servo_type == S_Dynamixel) {
            chsnprintf(txt_id, sizeof(txt_id), "Dynamixel ID=%3d", smart_servo_id);
        } else {
            chsnprintf(txt_id, sizeof(txt_id), "STS3032 ID=%3d", smart_servo_id);
        }

        lcdWriteString(&LCDD1, txt_id, 0);
    }

    if(ui_state.ok_clicked) {
        smart_servo.setBaudrate(1000000);
        if (servo_type == S_Dynamixel) {
            Dynamixel::setBD(smart_servo_id, 500000);
        } else {
            STS3032::setBD(smart_servo_id, STS3032::BD_500K);
        }
        
        smart_servo.setBaudrate(500000);
        STS3032::readResponseLevel(smart_servo_id);
        return &smartServoFnMenuState;
    }

    if(ui_state.ret_clicked) {
        return &mainMenuState;
    }

    return nullptr;
}

AbstractState* SmartServoIDState::periodic() {
    return nullptr;
}

void SmartServoIDState::leave() {
    
}



////////////////////////////////////////////////////////////////////////////////


char* titles_dyn_fn[] = {
	(char*)"Move            ",
	(char*)"Set ID          ",
	(char*)"Move Speed      ",
	(char*)"Set Torque      "
};

SmartServoFnMenuState::SmartServoFnMenuState() {
    pos_enc_init = 0;
    selectedFn = E_DynamixelMove;
}

void SmartServoFnMenuState::enter(int32_t pos_enc) {
    pos_enc_init = pos_enc - (int) selectedFn;
    //pos_enc_init = pos_enc;
    char txt_id[17];
    if (smartServoIDState.servo_type == S_Dynamixel) {
        chsnprintf(txt_id, sizeof(txt_id), "Dynamixel ID=%3d", smartServoIDState.get_servo_id());
    } else {
        chsnprintf(txt_id, sizeof(txt_id), "STS3032 ID=%3d", smartServoIDState.get_servo_id());
    }
    
    lcdWriteString(&LCDD1, txt_id, 0);
    lcdWriteString(&LCDD1, titles_dyn_fn[(int) selectedFn], 40);
}

AbstractState* SmartServoFnMenuState::onUiEvent(struct UiState ui_state) {
    int i = (ui_state.pos_enc - pos_enc_init) % (sizeof(titles_dyn_fn)/sizeof(*titles_dyn_fn));
    selectedFn = (DynamixelFunction) i;
    
    if(ui_state.enc_changed) {
        lcdWriteString(&LCDD1, titles_dyn_fn[i], 40);
    }

    if(ui_state.ok_clicked) {
        return &smartServoMoveState;
    }

    if(ui_state.ret_clicked) {
        return &smartServoIDState;
    }

    

    return nullptr;
}

AbstractState* SmartServoFnMenuState::periodic() {
    return nullptr;
}

void SmartServoFnMenuState::leave() {

}


////////////////////////////////////////////////////////////////////////////////


SmartServoMoveState::SmartServoMoveState() {
    pos_enc_init = 0;
    last_enc = 0;
    pos = 512;
    endless_direction = Dynamixel::Clockwise;
}

void SmartServoMoveState::enter(int32_t pos_enc) {
    
    last_enc = pos_enc_init = pos_enc;
    char txt_id[17];
    

    switch (smartServoFnMenuState.get_function())
    {
    case E_DynamixelMove:
        pos = 512;
        Dynamixel::setEndless(smartServoIDState.get_servo_id(), false);
        {
            int current_pos = Dynamixel::readPosition(smartServoIDState.get_servo_id());
            if(current_pos >= 0) {
                pos = current_pos;
                Dynamixel::moveSpeed(smartServoIDState.get_servo_id(), pos, 0);     // after going back from endless mode, speed must be set ?
            }
        }
        chsnprintf(txt_id, sizeof(txt_id), "Dyn Move     %3d", smartServoIDState.get_servo_id());
        chsnprintf(txt_position, sizeof(txt_position), "Pos = %4d      ", pos);
        break;
    case E_DynamixelSetId:
        chsnprintf(txt_id, sizeof(txt_id), "Dyn SetId    %3d", smartServoIDState.get_servo_id());
        chsnprintf(txt_position, sizeof(txt_position), "ID : %3d       ", 0);
        break;
    case E_DynamixelSetSpeed:
        pos = 512;
        Dynamixel::setEndless(smartServoIDState.get_servo_id(), true);
        Dynamixel::turn(smartServoIDState.get_servo_id(), endless_direction, 0);
        chsnprintf(txt_id, sizeof(txt_id), "Dyn Speed    %3d", smartServoIDState.get_servo_id());
        chsnprintf(txt_position, sizeof(txt_position), "Speed = %4d CW ", pos);
    default:
        break;
    }

    lcdWriteString(&LCDD1, txt_id, 0);
    lcdWriteString(&LCDD1, txt_position, 40);
}

AbstractState* SmartServoMoveState::onUiEvent(struct UiState ui_state) {
    (void) ui_state.encoder_speed;
    if(ui_state.enc_changed) {
        
        if(smartServoFnMenuState.get_function() == E_DynamixelMove) {
            pos = CLAMP_TO(0, 1023, pos + (ui_state.pos_enc - last_enc) * ui_state.encoder_speed);
            last_enc = ui_state.pos_enc;
            chsnprintf(txt_position, sizeof(txt_position), "Pos = %4d      ", pos);
            lcdWriteString(&LCDD1, txt_position, 40);
            Dynamixel::move(smartServoIDState.get_servo_id(), pos);
        }
        else if(smartServoFnMenuState.get_function() == E_DynamixelSetId) {
            pos = (((ui_state.pos_enc - pos_enc_init) % 254) + 254) % 254;
            chsnprintf(txt_position, sizeof(txt_position), "Id : %4d      ", pos);
            lcdWriteString(&LCDD1, txt_position, 40);
        }
        else if(smartServoFnMenuState.get_function() == E_DynamixelSetSpeed) {
            pos = CLAMP_TO(0, 1023, pos + (ui_state.pos_enc - last_enc)*ui_state.encoder_speed);
            last_enc = ui_state.pos_enc;

            if(endless_direction == Dynamixel::Clockwise) {
                chsnprintf(txt_position, sizeof(txt_position), "Speed = %4d CW ", pos);
            } else {
                chsnprintf(txt_position, sizeof(txt_position), "Speed = %4d CCW", pos);
            }

            
            lcdWriteString(&LCDD1, txt_position, 40);
            Dynamixel::turn(smartServoIDState.get_servo_id(), endless_direction, pos);
        }
        
        
    }

    if(ui_state.ok_clicked) {
        if(smartServoFnMenuState.get_function() == E_DynamixelSetId) {
            Dynamixel::setID(smartServoIDState.get_servo_id(), pos);
            smartServoIDState.set_dynamixel_id(pos);
            char txt_id[17];
            chsnprintf(txt_id, sizeof(txt_id), "Dyn SetId    %3d", pos);
            chsnprintf(txt_position, sizeof(txt_position), "Id set to %3d   ", pos);
            lcdWriteString(&LCDD1, txt_id, 0);
            lcdWriteString(&LCDD1, txt_position, 40);
        }
        else if(smartServoFnMenuState.get_function() == E_DynamixelSetSpeed) {
            if(endless_direction == Dynamixel::Clockwise) {
                endless_direction = Dynamixel::Counterclockwise;
                chsnprintf(txt_position, sizeof(txt_position), "Speed = %4d CCW", pos);
            } else {
                endless_direction = Dynamixel::Clockwise;
                chsnprintf(txt_position, sizeof(txt_position), "Speed = %4d CW ", pos);
            }
            Dynamixel::turn(smartServoIDState.get_servo_id(), endless_direction, pos);
            lcdWriteString(&LCDD1, txt_position, 40);
        }
    }

    if(ui_state.ret_clicked) {
        return &smartServoFnMenuState;
    }

    

    return nullptr;
}

AbstractState* SmartServoMoveState::periodic() {
    return nullptr;
}

void SmartServoMoveState::leave() {

}