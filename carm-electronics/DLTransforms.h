/**************************************************************
 *
 *                     DLTransforms.h
 *
 *     Author(s):  Daniel Opara
 *     Date:       3/20/2024
 *
 *     Overview: Transforms sensor readings into DLT values based on a
 *                  predetermined range, allocated bit count, and spacing
 *
 *
 **************************************************************/

#ifndef DLTRANSFORMS_H
#define DLTRANSFORMS_H

#include <inttypes.h>
#include "BBManager.h"
#include "untransformed.h"

unsigned int *transform_poweron(BBManager bbman);

unsigned int *transform_launchready(BBManager bbman);

unsigned int *transform_launchmode(BBManager bbman);

unsigned int *transform_recovery(BBManager bbman);

powerondata untransform_poweron(unsigned int *data);

launchreadydata untransform_launchready(unsigned int *data);

launchmodedata untransform_launchmode(unsigned int *data);

recoverydata untransform_recovery(unsigned int *data);

#endif