//
// Created by fabien on 10/10/2019.
//

#include "MainMenuState.h"
#include "lcd.h"
#include "stdio.h"
#include "stdutil.h"
#include "servos.h"
#include "ServoState.h"
#include "SmartServoState.h"
#include "CanState.h"
#include "I2CState.h"
#include "STS3032.h"

MainMenuState mainMenuState = MainMenuState();

enum {
	MENU_SERVO,
	MENU_DYNAMIXELS,
	MENU_STS3032,
	MENU_I2C,
	MENU_CAN,
};

char* titles_mainmenu[] = {
	(char*)"Servos    ",
	(char*)"Dynamixels",
	(char*)"STS3032   ",
	(char*)"I2C       ",
	(char*)"CAN       "
};

void MainMenuState::enter(int32_t pos_enc) {
	pos_enc_init = pos_enc;
    char title[] = "                ";
	lcdWriteString(&LCDD1, title, 0);
	lcdWriteString(&LCDD1, titles_mainmenu[0], 0);
	char blank[] = "                ";
	lcdWriteString(&LCDD1, blank, 40);
}

void MainMenuState::leave() {
    
}

AbstractState* MainMenuState::onUiEvent(struct UiState ui_state) {
	int i = (ui_state.pos_enc - pos_enc_init) % (sizeof(titles_mainmenu)/sizeof(*titles_mainmenu));
	
	if(ui_state.enc_changed) {	
		lcdWriteString(&LCDD1, titles_mainmenu[i], 0);
	}

	if(ui_state.ok_clicked) {
		switch (i) {
		case MENU_SERVO:
			return &servoState;
		case MENU_DYNAMIXELS:
			smartServoIDState.set_servo(&dynamixel);
			return &smartServoIDState;
		case MENU_STS3032:
			smartServoIDState.set_servo(&sts3032);
			return &smartServoIDState;
		case MENU_I2C:
			return &i2cState;
		case MENU_CAN:
			return &canState;
		}
	}

	return nullptr;
}

AbstractState* MainMenuState::periodic() {
    return nullptr;
}
