/**************************************************************
 *
 *                     StateDetermination.cpp
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Implementation of StateDetermination.h
 *
 *
 **************************************************************/

#include "StateDetermination.hpp"

StateDeterminer::StateDeterminer() : estimator(SIGMA_GYRO, SIGMA_ACCEL, SIGMA_BARO, CA, ACCEL_THRESHOLD)
{
    main_attempted = false;
    curr_state = state::LAUNCH_READY;
    first_step = false;
}

StateDeterminer::~StateDeterminer()
{
}

std::string stateToString(state s)
{
    switch (s)
    {
    case state::POWER_ON:
        return "POWER_ON";
    case state::LAUNCH_READY:
        return "LAUNCH_READY";
    case state::POWERED_FLIGHT_PHASE:
        return "POWERED_FLIGHT_PHASE";
    case state::BURNOUT_PHASE:
        return "BURNOUT_PHASE";
    case state::COAST_PHASE:
        return "COAST_PHASE";
    case state::APOGEE_PHASE:
        return "APOGEE_PHASE";
    case state::DROGUE_DEPLOYED:
        return "DROGUE_DEPLOYED";
    case state::MAIN_DEPLOY_ATTEMPT:
        return "MAIN_DEPLOY_ATTEMPT";
    case state::MAIN_DEPLOYED:
        return "MAIN_DEPLOYED";
    case state::RECOVERY:
        return "RECOVERY";
    default:
        return "UNKNOWN";
    }
}

// TODO: Complete this func, takes in bbman and modifies its curr_state attribute
void StateDeterminer::determineState(float accel_data[], float gyro_data[],
                                     float alt, float time, std::ofstream &ofstream)
{
    if (first_step == false)
    {
        estimator.setInitTime(time);
        first_step = true;
    }
    estimator.estimate(accel_data, gyro_data, alt, time);

    float curr_alt = estimator.getAltitude();
    float curr_velo = estimator.getVerticalVelocity();
    float curr_accel = estimator.getVerticalAcceleration();

    if (curr_state == state::LAUNCH_READY)
    {
        if ((curr_accel > prev_accel) && (curr_velo > 0.1))
        {
            curr_state = state::POWERED_FLIGHT_PHASE;
            launch_start_time = time;
            // estimator.resetPriors();
            // curr_alt = 0;
            // curr_accel = 0;
            // curr_velo = 0;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else if (curr_state == state::LAUNCH_READY || curr_state == state::POWER_ON)
    {
        if ((curr_accel > prev_accel) && (curr_velo > 0.1))
        {
            curr_state = state::POWERED_FLIGHT_PHASE;
            launch_start_time = time;
            // estimator.resetPriors();
            // curr_alt = 0;
            // curr_accel = 0;
            // curr_velo = 0;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else if (curr_state == state::POWERED_FLIGHT_PHASE)
    // TODO: what if I don't detect the burnout phase. I think I solved it with the next conditional
    {
        if (curr_accel < prev_accel)
        {
            curr_state = state::BURNOUT_PHASE;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else if (curr_state == state::BURNOUT_PHASE)
    {
        curr_state = state::COAST_PHASE;
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else if (curr_state == state::COAST_PHASE)
    {
        if (curr_velo >= 0 && curr_velo <= 1)
        {
            curr_state = state::APOGEE_PHASE;
            apogee = curr_alt;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else if (curr_state == state::APOGEE_PHASE)
    {
        // should i check for continuity or what
        // if it deploys at apogee, there shouldnt be much happening
        curr_state = state::DROGUE_DEPLOYED;
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else if (curr_state == state::DROGUE_DEPLOYED)
    // fall back if we are unable to deploy the drogue, we skip the drogue deployment
    // and move on to deploy the main parachute
    {
        // check if altitude and if it is less than some altitude at which we say fuck it,
        // we switch to the states to the main deployment attempt phase
        if (curr_alt <= 212)
        {
            curr_state = state::MAIN_DEPLOY_ATTEMPT;
            // do something from actions.h here
            // main_attempted = true;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else if (curr_state == state::MAIN_DEPLOY_ATTEMPT)
    {
        if (main_attempted)
        {
            // probably safer to implement a moving average here since the difference between
            // curr and prev will be so great and takes place in a small window of time
            if (curr_velo > prev_velo)
            {
                curr_state = state::MAIN_DEPLOYED;
            }
            main_attempted = false;
        }
        else
        {
            // do something from actions.h here
            main_attempted = true;
        }

        if (curr_alt < 50)
        {
            curr_state = state::RECOVERY;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else if (curr_state == state::MAIN_DEPLOYED)
    {
        if (curr_alt < 50)
        {
            curr_state = state::RECOVERY;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }
    else
    {
        ofstream << time << "," << curr_alt << "," << curr_velo << "," << curr_accel << "," << stateToString(curr_state) << "\n";
        return;
    }

    ofstream << time << "," << curr_alt << "," << curr_velo << "," << curr_accel << "," << stateToString(curr_state) << "\n";
    return;
}

void StateDeterminer::updatePrevEstimates(float altitude, float acceleration, float velocity)
{
    prev_alt = altitude;
    prev_velo = velocity;
    prev_accel = acceleration;
}