#include "CanState.h"
#include "lcd.h"
#include "stdio.h"
#include "stdutil.h"
#include "printf.h"
#include "MainMenuState.h"

int32_t cmd_speed = 0;

float angle;
float speed;
int16_t torque;
uint32_t identifier;

inline uint16_t swap16(uint16_t x) {
	return x>>8 | x<<8;
}

static THD_WORKING_AREA(can_tx1_wa, 512);
static THD_FUNCTION(can_tx, arg) {
	(void)arg;
	chRegSetThreadName("can_tx");

	CANTxFrame txmsg;
	txmsg.IDE = CAN_IDE_STD;
	txmsg.EID = 0x200;
	txmsg.RTR = CAN_RTR_DATA;
	txmsg.DLC = 8;

	while(true) {

		txmsg.data16[0] = swap16(cmd_speed);
		txmsg.data16[1] = swap16(cmd_speed);
		txmsg.data16[2] = swap16(cmd_speed);
		txmsg.data16[3] = swap16(cmd_speed);

		// set cmd_speed to motors 1..4
		txmsg.EID = 0x200;
		canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));

		// set cmd_speed to motors 5..8
		txmsg.EID = 0x1FF;
		canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
		chThdSleepMilliseconds(50);
	}
}

static THD_WORKING_AREA(can_rx1_wa, 512);
static THD_FUNCTION(can_rx, arg) {
	(void)arg;
	chRegSetThreadName("can_rx");

	event_listener_t el;
	CANRxFrame rxmsg;
	chEvtRegister(&CAND1.rxfull_event, &el, 0);
	while(!chThdShouldTerminateX()) {
		if (chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100)) == 0)
			continue;
		while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK) {
			identifier =  rxmsg.SID - 0x200;

			angle = (int16_t)swap16(rxmsg.data16[0]) * 360.0 / 8191.0;	// 0->8191 for 0->360°
			speed = (int16_t)swap16(rxmsg.data16[1]) / 36.0;		//Reduction gear ratio: 36:1
			torque = (int16_t)swap16(rxmsg.data16[2]);
			// last 2 bytes unused	
		}
	}
	chEvtUnregister(&CAND1.rxfull_event, &el);

}


extern "C" void run_can_tx() {
	chThdCreateStatic(can_tx1_wa, sizeof(can_tx1_wa), NORMALPRIO + 7, can_tx, NULL);
	chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7, can_rx, NULL);
}

CANState canState = CANState();


CANState::CANState() {
	
}


int32_t start_enc = 0;

void CANState::enter(int32_t pos_enc) {
	(void)pos_enc;
	char can_txt[] = "CAN mot         ";
    char blank[] = "                ";
	lcdWriteString(&LCDD1, blank, 0);
	lcdWriteString(&LCDD1, blank, 40);
	lcdWriteString(&LCDD1, can_txt, 0);
	start_enc = pos_enc;
}

void CANState::leave() {
    
}

AbstractState* CANState::onUiEvent(struct UiState ui_state) {
	(void)ui_state;

	cmd_speed = (ui_state.pos_enc - start_enc) * 100;

	char txt_speed[8];
	chsnprintf(txt_speed, sizeof(txt_speed), "C=%5ld", cmd_speed);
	lcdWriteString(&LCDD1, txt_speed, 8);


	return nullptr;
}

AbstractState* CANState::periodic() {
	char txt_speed[16];
	chsnprintf(txt_speed, sizeof(txt_speed), "%ld:  %.0f RPM       ",identifier, speed);
	lcdWriteString(&LCDD1, txt_speed, 40);
	return NULL;
}
