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

#include "BBManager.h"
#include <inttypes.h>

unsigned int *transform_poweron(BBManager bbman);

unsigned int *transform_launchready(BBManager bbman);

unsigned int *transform_launchmode(BBManager bbman);

unsigned int *transform_recovery(BBManager bbman);

#endif