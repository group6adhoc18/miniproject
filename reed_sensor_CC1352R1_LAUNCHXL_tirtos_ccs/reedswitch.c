/******************************************************************************
 Includes
 *****************************************************************************/
#include <openthread/config.h>
#include <openthread-core-config.h>

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* OpenThread public API Header files */
#include <openthread/coap.h>
#include <openthread/link.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/uart.h>

/* TI DRIVERS */
#include <ti/drivers/GPIO.h>

/* TIRTOS specific header files */
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/BIOS.h>

/* driverlib specific header */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aon_batmon.h)

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Board Header files */
#include "Board.h"

#include "reedswitch.h"
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

/* Reporting interval in milliseconds */
#define REPORTING_INTERVAL  10000

#define DEFAULT_COAP_HEADER_TOKEN_LEN 2

/* coap attribute descriptor */
typedef struct
{
    const char*    uriPath; /* attribute URI */
    uint16_t       type;    /* type of resource: read only or read write */
    uint8_t*       pValue;  /* pointer to value of attribute state */

} attrDesc_t;

/**
 * Pre shared key of the device used during the commissioning
 * stage.
 */
const uint8_t pskd[] = "REEDSENS1";

/******************************************************************************
 Local variables
 *****************************************************************************/

/* clock handle */
Clock_Handle reportClkHandle;

/* clock structure for reporting timer */
Clock_Struct reportClkStruct;

/* ipv6 address to send the reporting temperature to */
otIp6Address thermostatAddress;

/* port to report the temperature to */
uint16_t peerPort = OT_DEFAULT_COAP_PORT;

/* TI-RTOS events structure for passing state to the processing loop */
static Event_Struct reedSwitchEvents;

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_REEDSWITCH_TASK_STACK_SIZE];

/* coap resource for the application */
static otCoapResource coapResource;

/* coap attribute state of the application */
static uint8_t attrReed[11] = REEDSWITCH_CLOSED;
//TODO delete this if unused:
//static int temperatureValue = 70;

/* coap attribute descriptor for the application */
const attrDesc_t coapAttr = {
    REEDSWITCH_URI,
    (ATTR_READ|ATTR_REPORT),
    attrReed,
};

/* Holds the server setup state: 1 indicates CoAP server has been setup */
static bool serverSetup;

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Temperature Sensor processing thread. */
static void *ReedSwitch_task(void *arg0);
/*  timeout call back for reporting. */
static void reportingTimeoutCB(UArg a0);

/******************************************************************************
 Local Functions
 *****************************************************************************/

/**
 * @brief Configure the timer.
 *
 * @param  timeout  Time in milliseconds.
 *
 * @return None
 */

/* Callback funcrtions */
void gpioReedSwitchFxn(uint_least8_t index)
  {
    /* Clear the GPIO interrupt and increment threshold */
    if(GPIO_read(Board_GPIO_SPICS))
    {
        //OPEN
        strcpy(attrReed, REEDSWITCH_OPEN);
        GPIO_write(Board_GPIO_LED1, 1);
        //Display_printf(displayHandle, 1, 0, "Reed Switch Event write open");
    }
    else
     {
         //CLOSED
        strcpy(attrReed, REEDSWITCH_CLOSED);
        GPIO_write(Board_GPIO_LED1, 0);
        //Display_printf(displayHandle, 1, 0, "Reed Switch Event write closed");
     }
   

  }

static void configureReportingTimer(uint32_t timeout)
{
    Clock_Params clockParams;

    /* Convert clockDuration in milliseconds to ticks. */
    uint32_t clockTicks = timeout * (1000 / Clock_tickPeriod);

    /* Setup parameters. */
    Clock_Params_init(&clockParams);

    /* If period is 0, this is a one-shot timer. */
    clockParams.period = 0;

    /*
     Starts immediately after construction if true, otherwise wait for a
     call to start.
     */
    clockParams.startFlag = false;

    /*/ Initialize clock instance. */
    Clock_construct(&reportClkStruct, reportingTimeoutCB, clockTicks,
                    &clockParams);

    reportClkHandle = Clock_handle(&reportClkStruct);
}

/**
 * @brief Starts the reporting timer.
 *
 * Should be called after the reporting timer has been created, and after the
 * GUA has been registered.
 */
static void startReportingTimer(void)
{
    Clock_start(reportClkHandle);
}

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  a0      Argument passed by the clock if set up.
 *
 * @return None
 */
static void reportingTimeoutCB(UArg a0)
{
    ReedSwitch_postEvt(ReedSwitch_evtReportReed);
}

/**
 * @brief Reports the temperature to another coap device.
 *
 * @return None
 */
static void reedSwitchReport(void)
{
    otError error = OT_ERROR_NONE;
    otMessage *requestMessage = NULL;
    otMessageInfo messageInfo;
    otCoapHeader requestHeader;
    otInstance *instance = OtInstance_get();
    //TODO delete comments if unused:
    //int32_t celsiusTemp;

    /* make sure there is a new temperature reading otherwise just report the previous temperature */
    /*if(AONBatMonNewTempMeasureReady())
    {
        / * Read the temperature in degrees C from the internal temp sensor * /
        celsiusTemp = AONBatMonTemperatureGetDegC();

        / * convert temp to Fahrenheit * /
        temperatureValue = (int)((celsiusTemp * 9) / 5) + 32;
        / * convert temperature to string attribute * /
        snprintf((char*)attrReed, sizeof(attrReed), "%d",
                 temperatureValue);
    }*/

    /* print the reported value to the terminal */
    DISPUTILS_SERIALPRINTF(0, 0, "Reporting Reed State:");
    DISPUTILS_SERIALPRINTF(0, 0, (char*)attrReed);


    OtRtosApi_lock();
    otCoapHeaderInit(&requestHeader, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_POST);
    otCoapHeaderGenerateToken(&requestHeader, DEFAULT_COAP_HEADER_TOKEN_LEN);
    error = otCoapHeaderAppendUriPathOptions(&requestHeader,
                                             THERMOSTAT_TEMP_URI);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    OtRtosApi_lock();
    otCoapHeaderSetPayloadMarker(&requestHeader);
    requestMessage = otCoapNewMessage(instance, &requestHeader);
    OtRtosApi_unlock();
    otEXPECT_ACTION(requestMessage != NULL, error = OT_ERROR_NO_BUFS);

    OtRtosApi_lock();
    error = otMessageAppend(requestMessage, attrReed,
                            strlen((const char*) attrReed));
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    memset(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerAddr = thermostatAddress;
    messageInfo.mPeerPort = peerPort;
    messageInfo.mInterfaceId = OT_NETIF_INTERFACE_ID_THREAD;

    OtRtosApi_lock();
    error = otCoapSendRequest(instance, requestMessage, &messageInfo, NULL,
                              NULL);
    OtRtosApi_unlock();

    /* Restart the clock */
    if(Clock_isActive(reportClkHandle) == true)
    {
        Clock_stop(reportClkHandle);
    }
    Clock_start(reportClkHandle);

    exit:

    if(error != OT_ERROR_NONE && requestMessage != NULL)
    {
        OtRtosApi_lock();
        otMessageFree(requestMessage);
        OtRtosApi_unlock();
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
    otCoapHeaderSetMessageId(&responseHeader,
                             otCoapHeaderGetMessageId(aHeader));
    otCoapHeaderSetToken(&responseHeader, otCoapHeaderGetToken(aHeader),
                         otCoapHeaderGetTokenLength(aHeader));
    otCoapHeaderSetPayloadMarker(&responseHeader);

    if(OT_COAP_CODE_GET == messageCode)
    {
        responseMessage = otCoapNewMessage((otInstance*)aContext,
                                           &responseHeader);

        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
        error = otMessageAppend(responseMessage, attrReed,
                                strlen((const char*) attrReed));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:

    if(error != OT_ERROR_NONE && responseMessage != NULL)
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
static otError setupCoapServer(otInstance *aInstance, const attrDesc_t *attr)
{
    otError error = OT_ERROR_NONE;

    OtRtosApi_lock();
    error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    if(attr->type & (ATTR_READ | ATTR_WRITE))
    {
        coapResource.mHandler = &coapHandleServer;
        coapResource.mUriPath = (const char*)attr->uriPath;
        coapResource.mContext = aInstance;

        OtRtosApi_lock();
        error = otCoapAddResource(aInstance, &(coapResource));
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
    }

exit:
    return error;
}

/**
 * @brief Initialize and construct the TIRTOS events.
 *
 * @return None
 */
static void initEvent(void)
{
    Event_construct(&reedSwitchEvents, NULL);
}

/**
 * @brief Handles the key press events.
 *
 * @param keysPressed identifies which keys were pressed
 *
 * @return None
 */
static void processKeyChangeCB(uint8_t keysPressed)
{
    if (keysPressed & KEYS_RIGHT)
    {
        ReedSwitch_postEvt(ReedSwitch_evtKeyRight);
    }
}

/**
 * @brief Processes the OT stack events
 *
 * @param  evt Event identifier.
 *
 * @return None
 */
static void processOtStackEvents(uint8_t evt)
{
    switch (evt)
    {
    case OT_STACK_EVENT_NWK_JOINED:
        ReedSwitch_postEvt(ReedSwitch_evtNwkJoined);
        break;

    case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        ReedSwitch_postEvt(ReedSwitch_evtNwkJoinFailure);
        break;

    case OT_STACK_EVENT_NWK_DATA_CHANGED:
        ReedSwitch_postEvt(ReedSwitch_evtNwkSetup);
        break;

    default:
        // do nothing
        break;
    }
}

/**
 * @brief Processes the events.
 *
 * @return None
 */
static int processEvents(void)
{
    UInt events = Event_pend(Event_handle(&reedSwitchEvents), Event_Id_NONE,
                             (ReedSwitch_evtReportReed | ReedSwitch_evtNwkSetup |
                              ReedSwitch_evtAddressValid | ReedSwitch_evtKeyRight |
                              ReedSwitch_evtNwkJoined | ReedSwitch_evtNwkJoinFailure),
                             BIOS_WAIT_FOREVER);

    if(events & ReedSwitch_evtReportReed)
    {
        /* perform activity related to the report event. */
        DISPUTILS_SERIALPRINTF( 0, 0, "Report Event received");
        reedSwitchReport();
    }

    if(events & ReedSwitch_evtNwkSetup)
    {
        if (false == serverSetup)
        {
            serverSetup = true;
            (void)setupCoapServer(OtInstance_get(), &coapAttr);

            DISPUTILS_SERIALPRINTF(1, 0, "CoAP server setup done");
#ifdef TIOP_POWER_DATA_ACK
            startReportingTimer();
#endif
        }
    }

    if (events & ReedSwitch_evtKeyRight)
    {
        if ((!otDatasetIsCommissioned(OtInstance_get())) &&
            (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
        {
            GPIO_disableInt(Board_GPIO_SPICS);
            DISPUTILS_SERIALPRINTF(1, 0, "Joining Nwk ...");
            OtStack_joinNetwork((const char*)pskd);
        }
    }

    if (events & ReedSwitch_evtNwkJoined)
    {
        DISPUTILS_SERIALPRINTF( 1, 0, "Joined Nwk");
        GPIO_setConfig(Board_GPIO_SPICS, GPIO_CFG_INPUT | GPIO_CFG_IN_INT_BOTH_EDGES | GPIO_CFG_IN_PU);
        gpioReedSwitchFxn(0);
        /* install Button callback */
        //GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn0);
        GPIO_setCallback(Board_GPIO_SPICS, gpioReedSwitchFxn);
        GPIO_enableInt(Board_GPIO_SPICS);
        (void)OtStack_setupNetwork();
    }

    if (events & ReedSwitch_evtNwkJoinFailure)
    {
        DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
        GPIO_enableInt(Board_GPIO_SPICS);
    }


    if(events & ReedSwitch_evtAddressValid)
    {
        startReportingTimer();
    }
    return 0;
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

    param.sched_priority = TASK_CONFIG_REEDSWITCH_TASK_PRIORITY;

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

/*
 * Documented in reedswitch.h
 */
void ReedSwitch_postEvt(ReedSwitch_evt event)
{
    Event_post(Event_handle(&reedSwitchEvents), event);
}

/*
 * Documented in reedswitch.h
 */
void ReedSwitch_notifyGUA(otNetifAddress *aAddress)
{
    thermostatAddress = aAddress->mAddress;
    thermostatAddress.mFields.m8[OT_IP6_ADDRESS_SIZE - 1]
        = THERMOSTAT_ADDRESS_LSB;
    Event_post(Event_handle(&reedSwitchEvents), ReedSwitch_evtAddressValid);
}

/*
 * Documented in task_config.h.
 */
void ReedSwitch_taskCreate(void)
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
                                 TASK_CONFIG_REEDSWITCH_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, ReedSwitch_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);
    (void)retc;
}


/**
 *  Temp Sensor processing thread.
 */
void *ReedSwitch_task(void *arg0)
{
    int ret;
    bool commissioned;

    initEvent();

    KeysUtils_initialize(processKeyChangeCB);

    OtStack_registerCallback(processOtStackEvents);

    DispUtils_open();

    AONBatMonEnable();

    resetPriority();

    DISPUTILS_SERIALPRINTF(0, 0, "Door sensor init!");


    //GPIO_init();

    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUTPUT);
    GPIO_setConfig(Board_GPIO_LED1, GPIO_CFG_OUTPUT);
    //GPIO_setConfig(Board_GPIO_BUTTON0, GPIO_CFG_INPUT | GPIO_CFG_IN_INT_FALLING | GPIO_CFG_IN_PU);
    

    /* Enable interrupts */
    //GPIO_enableInt(Board_GPIO_BUTTON0);
    //GPIO_enableInt(Board_GPIO_SPICS);


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
    }
#endif /* !ALLOW_PRECOMMISSIONED_NETWORK_JOIN */


    configureReportingTimer(REPORTING_INTERVAL);
    /* process events */
    while(1)
    {
        ret = processEvents();
        if(ret != 0)
        {
            break;
        }
    }
    return NULL;
}

