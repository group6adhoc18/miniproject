/******************************************************************************

 @file lightsensor.c

 @brief lightsensor example application

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

/******************************************************************************
 Includes
 *****************************************************************************/
#include <openthread/config.h>
#include <openthread-core-config.h>

/* Standard Library Header files */
#include <assert.h>
#include <lightsensor.h>
#include <stddef.h>
#include <string.h>

/* OpenThread public API Header files */
#include <openthread/coap.h>
#include <openthread/link.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/uart.h>

/* TIRTOS specific header files */
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Board Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include "opt3001.h"
#include "Board.h"

#include "images.h"
#include "utils/code_utils.h"

#include "disp_utils.h"
#include "keys_utils.h"
#include "otstack.h"

/* Private configuration Header files */
#include "task_config.h"

#if (OPENTHREAD_ENABLE_APPLICATION_COAP == 0)
#error "OPENTHREAD_ENABLE_APPLICATION_COAP needs to be defined and set to 1"
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/* read attribute */
#define ATTR_READ     0x01
/* write attribute */
#define ATTR_WRITE    0x02
/* report attribute */
#define ATTR_REPORT   0x04
/* Number of attributes in  application */
#define ATTR_COUNT  3
/* Maximum number of characters for displayed temp including null terminator*/
#define TEMP_MAX_CHARS 11

/* coap attribute descriptor */
typedef struct
{
    const char*          uriPath; /* attribute URI */
    uint16_t             type; /* type of resource: read only or read write */
    char*                pValue;  /* pointer to value of attribute state */
    otCoapResource       *pAttrCoapResource; /* coap resource for this attr */
    otCoapRequestHandler pAttrHandlerCB;/* call back function for this attr */
} attrDesc_t;

/**
 * Pre shared key of the device used during the commissioning
 * stage.
 */
const uint8_t pskd[] = "LIGHTSENS";

/******************************************************************************
 Local variables
 *****************************************************************************/
/* TI-RTOS events structure for passing state to the processing loop */
static Event_Struct lightsensorEvents;

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_LIGHTSENSOR_TASK_STACK_SIZE];

/* coap resource for the application */
static otCoapResource coapResource;
static otCoapResource coapResourceThresholdMin;
static otCoapResource coapResourceThresholdMax;

/* coap attribute state of the application */
static char attrState[TEMP_MAX_CHARS] = LIGHTSENSOR_STATE_BRIGHT;

/* coap attribute state of the application */
static char attrStateTresholdMin[TEMP_MAX_CHARS] = "1000";
static int tresholdMin = 1000;
static char attrStateTresholdMax[TEMP_MAX_CHARS] = "2500";
static int tresholdMax = 2500;
static bool daylight = 0;

/* coap attribute discriptor for the application */
static attrDesc_t coapAttrs[ATTR_COUNT] = {
{
    .uriPath = LIGHTSENSOR_STATE_URI,
    .type = (ATTR_READ|ATTR_REPORT),
    .pValue = attrState,
    .pAttrCoapResource = &coapResource
},
{
    .uriPath = LIGHTSENSOR_THRESHOLD_MIN_URI,
    .type = (ATTR_READ|ATTR_WRITE),
    .pValue = attrStateTresholdMin,
    .pAttrCoapResource = &coapResourceThresholdMin
},
{
    .uriPath = LIGHTSENSOR_THRESHOLD_MAX_URI,
    .type = (ATTR_READ|ATTR_WRITE),
    .pValue = attrStateTresholdMax,
    .pAttrCoapResource = &coapResourceThresholdMax
}
};

/* Holds the server setup state: True indicates CoAP server has been setup */
static bool serverSetup;

/*******************************************************
 I2C variables and definitions
 ********************************************************/

#define CC1352R1_LAUNCHXL_DIO3  IOID_3

typedef enum CC1352R1_OPT3001Name {

    OPT3001_AMBIENT = 0, // Sensor measuring ambient light
    CC1352R1_OPT3001COUNT
} CC1352R1_OPT3001Name;


/* I2C for the light sensor on the Sensor BoosterPack */
static I2C_Handle i2cHandle;
static I2C_Params i2cParams;
static OPT3001_Handle opt3001Handle = NULL;
static OPT3001_Params opt3001Params;


OPT3001_Object OPT3001_object[CC1352R1_OPT3001COUNT];


const OPT3001_HWAttrs OPT3001_hwAttrs[CC1352R1_OPT3001COUNT] = {
    {
        .slaveAddress = OPT3001_SA4, // 0x47
        .gpioIndex = CC1352R1_LAUNCHXL_DIO3,
    },
};

OPT3001_Config OPT3001_config[] = {
    {
        .hwAttrs = &OPT3001_hwAttrs[0],
        .object  = &OPT3001_object[0],
    },
    {NULL, NULL},
};

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Lightsensor processing thread. */
void *Lightsensor_task(void *arg0);

/******************************************************************************
 Local Functions
 *****************************************************************************/

static void updateDaylight(void)
{
    float lightvalue;
    OPT3001_getLux(opt3001Handle, &lightvalue);
    DISPUTILS_SERIALPRINTF(0, 0, "Lightvalue %f\n", lightvalue);

    float threshold = daylight ? tresholdMin : tresholdMax;
    daylight = lightvalue > threshold;

    if (daylight)
    {
        strcpy((char *)attrState, LIGHTSENSOR_STATE_BRIGHT);
    }
    else
    {
        strcpy((char *)attrState, LIGHTSENSOR_STATE_DARK);
    }
}


/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  aContext      A pointer to the context information.
 * @param  aHeader       A pointer to the CoAP header.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void coapHandleServer(void *aContext, otCoapHeader *aHeader,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    otError error = OT_ERROR_NONE;
    otCoapHeader responseHeader;
    otMessage *responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapHeaderGetCode(aHeader);

    otCoapHeaderInit(&responseHeader, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapHeaderSetMessageId(&responseHeader, otCoapHeaderGetMessageId(aHeader));
    otCoapHeaderSetToken(&responseHeader, otCoapHeaderGetToken(aHeader),
                         otCoapHeaderGetTokenLength(aHeader));
    otCoapHeaderSetPayloadMarker(&responseHeader);

    if(OT_COAP_CODE_GET == messageCode)
    {
        OtRtosApi_lock();
        updateDaylight();

        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrState,strlen((const char*)attrState));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:

    if (error != OT_ERROR_NONE && responseMessage != NULL)
    {
        otMessageFree(responseMessage);
    }
}

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  aContext      A pointer to the context information.
 * @param  aHeader       A pointer to the CoAP header.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void coapHandleThresholdMin(void *aContext, otCoapHeader *aHeader,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    otError error = OT_ERROR_NONE;
    otCoapHeader responseHeader;
    otMessage *responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapHeaderGetCode(aHeader);

    otCoapHeaderInit(&responseHeader, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapHeaderSetMessageId(&responseHeader, otCoapHeaderGetMessageId(aHeader));
    otCoapHeaderSetToken(&responseHeader, otCoapHeaderGetToken(aHeader),
                         otCoapHeaderGetTokenLength(aHeader));
    otCoapHeaderSetPayloadMarker(&responseHeader);

    if(OT_COAP_CODE_GET == messageCode)
    {
        OtRtosApi_lock();
        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrStateTresholdMin,strlen((const char*)attrStateTresholdMin));
        otEXPECT(OT_ERROR_NONE == error);


        DISPUTILS_SERIALPRINTF(0, 0, "old thresh min %d\n", tresholdMin);
        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }
    else if(OT_COAP_CODE_POST == messageCode)
    {
        char data[32];
        uint16_t offset = otMessageGetOffset(aMessage);
        uint16_t read = otMessageRead(aMessage, offset, data, sizeof(data) - 1);
        data[read] = '\0';

        /* update the attribute state */
        strcpy((char *)attrStateTresholdMin, data);
        tresholdMin = atoi (data);

        DISPUTILS_SERIALPRINTF(0, 0, "new thresh min %s\n", data);
        DISPUTILS_SERIALPRINTF(0, 0, "new thresh min %d\n", tresholdMin);

        OtRtosApi_lock();
        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrStateTresholdMin, strlen((const char*)attrStateTresholdMin));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext,
                                   responseMessage, aMessageInfo);
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:

    if (error != OT_ERROR_NONE && responseMessage != NULL)
    {
        otMessageFree(responseMessage);
    }
}

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  aContext      A pointer to the context information.
 * @param  aHeader       A pointer to the CoAP header.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void coapHandleThresholdMax(void *aContext, otCoapHeader *aHeader,
                             otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    otError error = OT_ERROR_NONE;
    otCoapHeader responseHeader;
    otMessage *responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapHeaderGetCode(aHeader);

    otCoapHeaderInit(&responseHeader, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapHeaderSetMessageId(&responseHeader, otCoapHeaderGetMessageId(aHeader));
    otCoapHeaderSetToken(&responseHeader, otCoapHeaderGetToken(aHeader),
                         otCoapHeaderGetTokenLength(aHeader));
    otCoapHeaderSetPayloadMarker(&responseHeader);

    if(OT_COAP_CODE_GET == messageCode)
    {
        OtRtosApi_lock();
        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrStateTresholdMax,strlen((const char*)attrStateTresholdMax));
        otEXPECT(OT_ERROR_NONE == error);


        DISPUTILS_SERIALPRINTF(0, 0, "old thresh max %d\n", tresholdMax);
        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }
    else if(OT_COAP_CODE_POST == messageCode)
    {
        char data[32];
        uint16_t offset = otMessageGetOffset(aMessage);
        uint16_t read = otMessageRead(aMessage, offset, data, sizeof(data) - 1);
        data[read] = '\0';

        /* update the attribute state */
        strcpy((char *)attrStateTresholdMax, data);
        tresholdMin = atoi (data);

        DISPUTILS_SERIALPRINTF(0, 0, "new thresh max %s\n", data);
        DISPUTILS_SERIALPRINTF(0, 0, "new thresh max %d\n", tresholdMax);

        OtRtosApi_lock();
        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrStateTresholdMax, strlen((const char*)attrStateTresholdMax));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext,
                                   responseMessage, aMessageInfo);
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:

    if (error != OT_ERROR_NONE && responseMessage != NULL)
    {
        otMessageFree(responseMessage);
    }
}

/**
 * @brief sets up the application coap server.
 *
 * @param aInstance A pointer to the context information.
 * @param attr      Attribute data
 *
 * @return OT_ERROR_NONE if successful, else error code
 */
static otError setupCoapServer(otInstance *aInstance,
                                   const attrDesc_t *attr)
{
    otError error = OT_ERROR_NONE;

    OtRtosApi_lock();
    error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    if(attr->type & (ATTR_READ | ATTR_WRITE))
    {
        attr->pAttrCoapResource->mHandler = attr->pAttrHandlerCB;
        attr->pAttrCoapResource->mUriPath = (const char*)attr->uriPath;
        attr->pAttrCoapResource->mContext = aInstance;

        OtRtosApi_lock();
        error = otCoapAddResource(aInstance, attr->pAttrCoapResource);
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:
    return error;
}


/**
 * @brief Handles the key press events.
 *
 * @param keysPressed identifies which keys were pressed
 * @return None
 */
static void processKeyChangeCB(uint8_t keysPressed)
{
    if (keysPressed & KEYS_RIGHT)
    {
        Lightsensor_postEvt(Lightsensor_evtKeyRight);
    }
}

/**
 * @brief Processes the OT stack events
 *
 * @param evt Event identifier.
 * @return None
 */
static void processOtStackEvents(uint8_t evt)
{
    switch (evt)
    {
    case OT_STACK_EVENT_NWK_JOINED:
        Lightsensor_postEvt(Lightsensor_evtNwkJoined);
        break;

    case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        Lightsensor_postEvt(Lightsensor_evtNwkJoinFailure);
        break;

    case OT_STACK_EVENT_NWK_DATA_CHANGED:
        Lightsensor_postEvt(Lightsensor_evtNwkSetup);
        break;

    default:
        // do nothing
        break;
    }
}

/**
 * @brief Initialize and construct the TIRTOS events.
 *
 * @return None
 */
static void initEvent(void)
{
    Event_construct(&lightsensorEvents, NULL);
}

static void initLightSensor(void)
{
    GPIO_init();

    I2C_init();
    OPT3001_init();
    I2C_Params_init(&i2cParams);
    i2cHandle = I2C_open(Board_I2C0, &i2cParams);
    OPT3001_Params_init(&opt3001Params);
    opt3001Handle = OPT3001_open(OPT3001_AMBIENT, i2cHandle, &opt3001Params);
}

/**
 * @brief Processes the events.
 *
 * @return None
 */
static void processEvents(void)
{
    UInt events = Event_pend(Event_handle(&lightsensorEvents), Event_Id_NONE,
                             (Lightsensor_evtOpen | Lightsensor_evtClosed |
                              Lightsensor_evtDrawn | Lightsensor_evtNwkSetup |
                              Lightsensor_evtKeyRight | Lightsensor_evtNwkJoined |
                              Lightsensor_evtNwkJoinFailure),
                             BIOS_WAIT_FOREVER);

    if (events & Lightsensor_evtOpen)
    {
        /* perform activity related to the lightsensor open event. */
        DISPUTILS_SERIALPRINTF( 0, 0, "Lightsensor Open Event received");
        DispUtils_lcdDraw(&Images_lightsensorOpen);
    }

    if (events & Lightsensor_evtClosed)
    {
        /* perform activity related to the lightsensor closed event */
        DISPUTILS_SERIALPRINTF( 0, 0, "Lightsensor close Event received");
        DispUtils_lcdDraw(&Images_lightsensorClosed);
    }

    if (events & Lightsensor_evtDrawn)
    {
       /* perform activity related to the lightsensor drawn event */
        DISPUTILS_SERIALPRINTF(0, 0, "Lightsensor drawn Event received");
        DispUtils_lcdDraw(&Images_lightsensorDrawn);
    }

    if (events & Lightsensor_evtNwkSetup)
    {
        if (false == serverSetup)
        {
            serverSetup = true;

            /* set callback functions */
            coapAttrs[0].pAttrHandlerCB = &coapHandleServer;
            coapAttrs[1].pAttrHandlerCB = &coapHandleThresholdMin;
            coapAttrs[2].pAttrHandlerCB = &coapHandleThresholdMax;
            /* register coap attributes */
            (void)setupCoapServer(OtInstance_get(), &coapAttrs[0]);
            (void)setupCoapServer(OtInstance_get(), &coapAttrs[1]);
            (void)setupCoapServer(OtInstance_get(), &coapAttrs[2]);


            /* display unlock image on LCD */
            DISPUTILS_SERIALPRINTF(1, 0, "CoAP server setup done");
            DispUtils_lcdDraw(&Images_lightsensorOpen);
        }
    }

    if (events & Lightsensor_evtKeyRight)
    {
        if ((!otDatasetIsCommissioned(OtInstance_get())) &&
            (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
        {
            DISPUTILS_SERIALPRINTF(1, 0, "Joining Nwk ...");
            DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");

            OtStack_joinNetwork((const char*)pskd);
        }
    }

    if (events & Lightsensor_evtNwkJoined)
    {
        DISPUTILS_SERIALPRINTF( 1, 0, "Joined Nwk");
        DISPUTILS_LCDPRINTF(1, 0, "Joined Nwk");

        (void)OtStack_setupNetwork();
    }

    if (events & Lightsensor_evtNwkJoinFailure)
    {
        DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
        DISPUTILS_LCDPRINTF(1, 0, "Join Failure");
    }
}

/**
 * Return thread priority after initialization.
 */
static void resetPriority(void)
{
    pthread_t           self;
    int                 policy;
    struct sched_param  param;
    int                 ret;

    self = pthread_self();

    ret = pthread_getschedparam(self, &policy, &param);
    assert(ret == 0);

    param.sched_priority = TASK_CONFIG_LIGHTSENSOR_TASK_PRIORITY;

    ret = pthread_setschedparam(self, policy, &param);
    assert(ret == 0);
    (void)ret;
}


/******************************************************************************
 External Functions
 *****************************************************************************/

#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP)
/**
 * Documented in openthread/platform/logging.h.
 */
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    (void)aLogLevel;
    (void)aLogRegion;
    (void)aFormat;
    /* Do nothing. */
}
#endif

/**
 * Documented in openthread/platform/uart.h.
 */
void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{
    (void)aBuf;
    (void)aBufLength;
    /* Do nothing. */
}

/**
 * Documented in openthread/platform/uart.h.
 */
void otPlatUartSendDone(void)
{
    /* Do nothing. */
}

/* Documented in lightsensor.h */
void Lightsensor_postEvt(Lightsensor_evt_t event)
{
    Event_post(Event_handle(&lightsensorEvents), event);
}

/**
 * Documented in task_config.h.
 */
void Lightsensor_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = sched_get_priority_max(SCHED_OTHER);
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc = pthread_attr_setstack(&pAttrs, (void *)stack,
                                 TASK_CONFIG_LIGHTSENSOR_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, Lightsensor_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void)retc;
}

/**
 *  Lightsensor processing thread.
 */
void *Lightsensor_task(void *arg0)
{
    bool commissioned;
    initEvent();
    initLightSensor();


    KeysUtils_initialize(processKeyChangeCB);

    OtStack_registerCallback(processOtStackEvents);

    DispUtils_open();

    resetPriority();

    DISPUTILS_SERIALPRINTF(0, 0, "Lightsensor init!");

#ifndef ALLOW_PRECOMMISSIONED_NETWORK_JOIN
    OtRtosApi_lock();
    commissioned = otDatasetIsCommissioned(OtInstance_get());
    OtRtosApi_unlock();
    if (false == commissioned)
    {
        otExtAddress extAddress;

        OtRtosApi_lock();
        otLinkGetFactoryAssignedIeeeEui64(OtInstance_get(), &extAddress);
        OtRtosApi_unlock();

        DISPUTILS_SERIALPRINTF(2, 0, "pskd: %s", pskd);
        DISPUTILS_SERIALPRINTF(3, 0, "EUI64: 0x%02x%02x%02x%02x%02x%02x%02x%02x",
                               extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                               extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                               extAddress.m8[6], extAddress.m8[7]);

        DISPUTILS_LCDPRINTF(2, 0, "pskd:");
        DISPUTILS_LCDPRINTF(3, 0, "%s", pskd);
        DISPUTILS_LCDPRINTF(4, 0, "EUI64:");
        DISPUTILS_LCDPRINTF(5, 0, "%02x%02x%02x%02x%02x%02x%02x%02x",
                            extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                            extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                            extAddress.m8[6], extAddress.m8[7]);

    }
#endif /* !ALLOW_PRECOMMISSIONED_NETWORK_JOIN */

   /* process events */
    while (1)
    {
        processEvents();
    }
}

