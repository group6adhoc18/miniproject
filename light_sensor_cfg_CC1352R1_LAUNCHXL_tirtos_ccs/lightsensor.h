/******************************************************************************

 @file lightsensor.h

 @brief lightsensor derived from lightsensor example application definitions.

 Group: CMCU, LPC
 Target Device: CC13xx

 ******************************************************************************
 
 Copyright (c) 2017-2018, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc13x2_sdk_2_30_00_
 Release Date: 2018-10-03 19:52:52
 *****************************************************************************/

#ifndef _LIGHTSENSOR_H_
#define _LIGHTSENSOR_H_

#include <ti/sysbios/knl/Event.h>
#include <openthread/coap.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/
/** Lightsensor state string */
#define LIGHTSENSOR_STATE_URI     "lightsensor/daylight"
/** Lightsensor open state string */
#define LIGHTSENSOR_STATE_BRIGHT    "bright"
/** Lightsensor closed state string */
#define LIGHTSENSOR_STATE_DARK  "dark"

/** Lightsensor state string */
#define LIGHTSENSOR_THRESHOLD_MIN_URI    "lightsensor/threshold/min"

/** Lightsensor state string */
#define LIGHTSENSOR_THRESHOLD_MAX_URI    "lightsensor/threshold/max"


/**
 * Lightsensor events.
 */
typedef enum
{
    Lightsensor_evtOpen           = Event_Id_00, /* Lightsensor openLock event */
    Lightsensor_evtClosed         = Event_Id_01, /* Lightsensor closed event */
    Lightsensor_evtDrawn          = Event_Id_02, /* Lightsensor drawn event */
    Lightsensor_evtNwkSetup       = Event_Id_03, /* openthread network is setup */
    Lightsensor_evtKeyLeft        = Event_Id_04, /* Left Key is pressed */
    Lightsensor_evtKeyRight       = Event_Id_05, /* Right key is pressed */
    Lightsensor_evtNwkJoined      = Event_Id_06, /* Joined the network */
    Lightsensor_evtNwkJoinFailure = Event_Id_07  /* Failed joining network */

} Lightsensor_evt_t;

/******************************************************************************
 External functions
 *****************************************************************************/

/**
 * @brief Posts an event to the Lightsensor task.
 *
 * @param event event to post.
 * @return None
 */
extern void Lightsensor_postEvt(Lightsensor_evt_t event);

#ifdef __cplusplus
}
#endif

#endif /* _LIGHTSENSOR_H_ */
