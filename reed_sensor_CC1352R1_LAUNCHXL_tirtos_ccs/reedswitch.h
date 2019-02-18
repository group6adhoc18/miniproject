#ifndef _REEDSWITCH_H_
#define _REEDSWITCH_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/* Temperature sensor temperature string */
#define REEDSWITCH_URI     "door/state"

#define THERMOSTAT_TEMP_URI     "thermostat/temperature"

#ifndef THERMOSTAT_ADDRESS_LSB
#define THERMOSTAT_ADDRESS_LSB  7
#endif

#define REEDSWITCH_CLOSED "closed"
#define REEDSWITCH_OPEN "open"

/**
 * Temperature Sensor events.
 */
typedef enum
{
    ReedSwitch_evtReportReed     = Event_Id_00, /* report timeout event */
    ReedSwitch_evtNwkSetup       = Event_Id_01, /* openthread network is setup */
    ReedSwitch_evtAddressValid   = Event_Id_02, /* GUA registered, we may begin reporting */
    ReedSwitch_evtKeyRight       = Event_Id_03, /* Right key is pressed */
    ReedSwitch_evtNwkJoined      = Event_Id_04, /* Joined the network */
    ReedSwitch_evtNwkJoinFailure = Event_Id_05  /* Failed joining network */
} ReedSwitch_evt;

/******************************************************************************
 External functions
 *****************************************************************************/

/**
 * @brief Posts an event to the Temperature Sensor task.
 *
 * @param event event to post.
 * @return None
 */
extern void ReedSwitch_postEvt(ReedSwitch_evt event);

/**
 * @brief Sets the reporting address for the temp sensor task.
 *
 * @param [in] aAddress address with prefix to copy.
 *
 * @note The reporting address least significant byte *will* be set to
 *       @ref THERMOSTAT_ADDRESS_LSB. The address passed into this function
 *       is only used for the prefix assignment.
 */
extern void ReedSwitch_notifyGUA(otNetifAddress *aAddress);

#ifdef __cplusplus
}
#endif

#endif /* _REEDSWITCH_H_ */

