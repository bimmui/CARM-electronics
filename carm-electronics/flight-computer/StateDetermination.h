/**************************************************************
 *
 *                     StateDetermination.h
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Uses sensor readings stored in BBManager to determine
 *                  the current state of the rocket
 *
 *
 **************************************************************/

#ifndef STATE_DETERMINATION_H
#define STATE_DETERMINATION_H

// forward declaration
class BBManager;

enum class state
{
    POWER_ON = 0,
    LAUNCHING,
    APOGEE_PHASE,
    DROGUE_DEPLOYED,
    MAIN_DEPLOY_ATTEMPT,
    MAIN_DEPLOYED,
    RECOVERY
};

void state_determiner(BBManager manager);

#endif