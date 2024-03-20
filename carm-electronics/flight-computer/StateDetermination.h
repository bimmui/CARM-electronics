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

void stateDeterminer(BBManager manager);

#endif