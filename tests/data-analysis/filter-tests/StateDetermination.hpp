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

#include <inttypes.h>
#include <fstream>
#include "altitude.hpp"

// standard noise deviation, calculated by Daniel
#define SIGMA_GYRO 0.337
#define SIGMA_ACCEL 0.640
#define SIGMA_BARO 0.488

// KF concept: how closely calculated future values are to prev values
// we set this to 0.5 to err on the side of safety
#define CA 0.5

// something for complementary filtering for the zero-velocity update feature
#define ACCEL_THRESHOLD 0.1

enum class state
{
    POWER_ON = 0,
    LAUNCH_READY,
    POWERED_FLIGHT_PHASE,
    BURNOUT_PHASE,
    COAST_PHASE,
    APOGEE_PHASE,
    DROGUE_DEPLOYED,
    MAIN_DEPLOY_ATTEMPT,
    MAIN_DEPLOYED,
    RECOVERY
};

class StateDeterminer
{
public:
    StateDeterminer();
    ~StateDeterminer();
    void determineState(float accel_data[], float gyro_data[],
                        float alt, float time, std::ofstream &ofstream);

private:
    AltitudeEstimator estimator;
    bool first_step;
    float prev_alt;
    float prev_accel;
    float prev_velo;
    float apogee;
    bool main_attempted;
    float launch_start_time;
    state curr_state;
    void updatePrevEstimates(float altitude, float acceleration, float velocity);
};

#endif