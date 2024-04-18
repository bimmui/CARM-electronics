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

#include "StateDetermination.h"
#include "BBManager.h"
#include "bitpack.h"
#include "actions.h"

StateDeterminer::StateDeterminer() : estimator(SIGMA_GYRO, SIGMA_ACCEL, SIGMA_BARO, CA, ACCEL_THRESHOLD)
{
    main_attempted = false;
}

StateDeterminer::~StateDeterminer()
{
}

// TODO: Complete this func, takes in bbman and modifies its curr_state attribute
void StateDeterminer::determineState(BBManager &manager)
{
    float accel_data[3] = {manager.accel_x, manager.accel_y, manager.accel_z};
    float gyro_data[3] = {manager.gyro_x, manager.gyro_y, manager.gyro_z};
    estimator.estimate(accel_data, gyro_data, manager.altitude, manager.curr_launch_time * 1000);

    float curr_alt = estimator.getCurrAltitude();
    float curr_velo = estimator.getCurrVerticalVelocity();
    float curr_accel = estimator.getCurrVerticalAcceleration();

    manager.k_altitude = curr_alt;
    manager.k_vert_velocity = curr_velo;
    manager.k_vert_acceleration = curr_accel;

    if (manager.curr_state == state::LAUNCH_READY)
    {
        if (curr_velo > prev_velo && curr_accel > prev_accel && curr_velo > 3)
        {
            manager.curr_state = state::POWERED_FLIGHT_PHASE;
            manager.launch_start_time = mills();
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }

    if (manager.curr_state == state::LAUNCH_READY || manager.curr_state == state::POWER_ON)
    {
        if (curr_velo > prev_velo && curr_alt > prev_alt && curr_velo > 3)
        {
            manager.curr_state = state::POWERED_FLIGHT_PHASE;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }

    // TODO: what if I don't detect the burnout phase. I think I solved it with the next conditional
    if (manager.curr_state == state::POWERED_FLIGHT_PHASE)
    {
        if (curr_accel < prev_accel)
        {
            manager.curr_state = state::BURNOUT_PHASE;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }

    if (manager.curr_state == state::BURNOUT_PHASE || manager.curr_state == state::POWERED_FLIGHT_PHASE)
    {
        if (curr_accel < prev_accel)
        {
            manager.curr_state = state::COAST_PHASE;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }

    if (manager.curr_state == state::COAST_PHASE)
    {
        if (curr_accel < prev_accel)
        {
            manager.curr_state = state::COAST_PHASE;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }

    if (manager.curr_state == state::COAST_PHASE)
    {
        if (curr_velo >= 0 || curr_velo <= 3)
        {
            manager.curr_state = state::APOGEE_PHASE;
            apogee = curr_alt;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }

    if (manager.curr_state == state::APOGEE_PHASE)
    {
        // should i check for continuity or what
        // if it deploys at apogee, there shouldnt be much happening
        if (simulate_drogue_deploy())
        {
            manager.curr_state = state::DROGUE_DEPLOYED;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }

    // fall back if we are unable to deploy the drogue, we skip the drogue deployment
    // and move on to deploy the main parachute
    if (manager.curr_state == state::APOGEE_PHASE)
    {
        // check if altitude and if it is less than some altitude at which we say fuck it,
        // we switch to the states to the main deployment attempt phase
        if (curr_alt <= 800)
        {
            manager.curr_state = state::MAIN_DEPLOY_ATTEMPT;
            // do something from actions.h here
            // main_attempted = true;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
    }

    if (manager.curr_state == state::MAIN_DEPLOY_ATTEMPT)
    {
        // uncomment below when it is an active system
        //  if (main_attempted)
        //  {
        //      // probably safer to implement a moving average here since the difference between
        //      // curr and prev will be so great and takes place in a small window of time
        //      if (curr_accel > prev_accel)
        //      {
        //          manager.curr_state = state::MAIN_DEPLOYED;
        //      }
        //      main_attempted = false;
        //  }
        //  else
        //  {
        //      // do something from actions.h here
        //      main_attempted = true;
        //  }

        if (curr_accel > prev_accel)
        {
            manager.curr_state = state::MAIN_DEPLOYED;
        }
        if (curr_alt < 50)
        {
            manager.curr_state = state::RECOVERY;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }

    if (manager.curr_state == state::MAIN_DEPLOYED)
    {
        if (curr_alt < 50)
        {
            manager.curr_state = state::RECOVERY;
        }
        updatePrevEstimates(curr_alt, curr_accel, curr_velo);
        return;
    }
}

void StateDeterminer::switchGroundState(BBManager &manager, uint64_t packet)
{
    // 0x53504F = SPO in ASCII = 5460047 in decimal = Switch Power On
    // 0x534C52 = SLR in ASCII = 5459026 in decimal = Switch Launch Ready
    if (packet == 5460047)
    {
        manager.curr_state = state::POWER_ON;
        return;
    }
    if (packet == 5459026)
    {
        manager.curr_state = state::LAUNCH_READY;
        return;
    }
}

void StateDeterminer::updatePrevEstimates(float altitude, float acceleration, float velocity)
{
    prev_alt = altitude;
    prev_velo = velocity;
    prev_accel = acceleration;
}