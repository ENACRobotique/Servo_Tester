#pragma once

#include "AbstractState.h"

class DShotState : public AbstractState {
public:
    DShotState();
    void enter(int32_t pos_enc);
    AbstractState* onUiEvent(struct UiState ui_state);
    AbstractState* periodic();
    void leave();
private:


};

extern DShotState dshotState;
