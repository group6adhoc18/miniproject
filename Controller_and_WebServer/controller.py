# original source: https://aiocoap.readthedocs.io/en/latest/examples.html
# changed by group6
# PR Sensor Networks, TU Berlin
# Winter 2018/19
# Sivert Kittelsen
# Jeremias Eichelbaum
# Sascha Roesler

import time
from sniffer import MACSniffer
import logging
import asyncio
import signal
import sys
from enum import Enum

from aiocoap import *

logging.basicConfig(level=logging.INFO)

class LIGHTSENSOR_RESOURCE(Enum):
    DAYLIGHT = "/lightsensor/daylight",
    THRESHOLD_MIN = "/lightsensor/threshold/min",
    THRESHOLD_MAX = "/lightsensor/threshold/max",
    UNDEFINED = "UNDEFINED"

LIGHTSENSOR_ID  = "[fd11:22::4]"
LIGHTSENSOR_BRIGHT = "bright"
LIGHTSENSOR_DARK = "dark"

LIGHTSWITCH_ID  = "[fd11:22::3]"
LIGHTSWITCH_RESOURCE = "/lamp/state"
CMD_LIGHT_ON = 'on'
CMD_LIGHT_OFF = 'off'

DOOR_ID  = "[fd11:22::9]"
DOOR_RESOURCE = "/door/state"
DOOR_OPEN = "open"

DOOR_TIMEOUT = 15



macsniff = None

async def get_lightsensor_resource(RESOURCE):
    protocol = await Context.create_client_context()
    print('Request GET', 'coap://' + LIGHTSENSOR_ID + RESOURCE.value[0])
    request = Message(code=GET, uri='coap://' + LIGHTSENSOR_ID + RESOURCE.value[0])
    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)
    else:
        return response.payload.decode("utf-8")
    return None

async def get_doorstate():
    protocol = await Context.create_client_context()
    request = Message(code=GET, uri='coap://' + DOOR_ID + DOOR_RESOURCE)
    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)
    else:
        return response.payload.decode("utf-8")
    return None

async def set_lightsensor_threshold(RESOURCE, payload):
    protocol = await Context.create_client_context()
    request = Message(code=POST, uri='coap://' + LIGHTSENSOR_ID + RESOURCE.value[0], payload=payload.encode('utf-8'))
    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)
    else:
        return True
    return False

async def set_light_on():
    protocol = await Context.create_client_context()
    request = Message(code=POST, uri='coap://' + LIGHTSWITCH_ID + LIGHTSWITCH_RESOURCE, payload=CMD_LIGHT_ON.encode('utf-8'))
    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)
    else:
        return True
    return False

async def set_light_off():
    protocol = await Context.create_client_context()
    request = Message(code=POST, uri='coap://' + LIGHTSWITCH_ID + LIGHTSWITCH_RESOURCE, payload=CMD_LIGHT_OFF.encode('utf-8'))
    try:
        response = await protocol.request(request).response
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)
    else:
        return True
    return False


def calculate_new_state(lightOutside, smartphoneDetection, lastState, doorState):
    if(lightOutside is None or (lightOutside != LIGHTSENSOR_BRIGHT and lightOutside != LIGHTSENSOR_DARK)):
        return None
    
    if lightOutside == LIGHTSENSOR_BRIGHT:
        return False
    
    if smartphoneDetection or doorState:
        return True
    
    return False


def signal_handler(sig, frame):
        print('Controller wird beendet')
        if(macsniff):
            macsniff.exit()
        sys.exit(0)

async def main():
    # endless loop
    print("####################################")
    print("# Light COntroller                  ")
    print("# Sensor Network Lab, WS 2018/19    ")
    print("# Sivert Kittelsen ")
    print("# Jeremias Eichelbaum ")
    print("# Sascha Roesler")
    print("###################################\n")
    
    signal.signal(signal.SIGINT, signal_handler)
    
    macsniff = MACSniffer('mac.conf', 'mac_available.txt')
    lightState = False
    lastDoorTime = 0#time.time() - 40
    
    while True:
        # request light sensor
        print("new controller run")
        door = await get_doorstate()
        
        
        #mint = await get_lightsensor_resource(LIGHTSENSOR_RESOURCE.THRESHOLD_MIN)
        #maxt = await get_lightsensor_resource(LIGHTSENSOR_RESOURCE.THRESHOLD_MAX)
        #print("\tget light threshold:",mint, maxt)
        
        if door == DOOR_OPEN:
            lastDoorTime = time.time()
            print("\tGet open door")
        
        lightOutside = await get_lightsensor_resource(LIGHTSENSOR_RESOURCE.DAYLIGHT)
        print("\tget light value:",lightOutside)
        smartphoneDetection = macsniff.detect_mac()
        doorOpenState = (lastDoorTime + DOOR_TIMEOUT) > time.time()
        
        print("\tconneted smart phone:", smartphoneDetection)
        print("\tdoor trigger:", doorOpenState)
        
        
        newLigtstate = calculate_new_state(lightOutside, smartphoneDetection, lightState, doorOpenState)
        
        with open("controllerState.txt", 'w') as f:
            f.write("Light outsite: \t{}\n".format(lightOutside))
            f.write("Smart phone detection: \t{}\n".format(smartphoneDetection))
            f.write("Door current action: \t{}\n".format(door))
            f.write("Door state: \t{}\n".format(doorOpenState))
            f.write("Light relays: \t{}\n".format(newLigtstate))
            f.close()
        
        if (newLigtstate is not None) and (newLigtstate != lightState):
            lightState = newLigtstate
            print("\tstate changes")
            if lightState:
                print("\tswitch on the light")
                await set_light_on()
            else:
                print("\tswitch off the light")
                await set_light_off()
        print("\tGo to sleep for 5 seconds\n")
        
        await asyncio.sleep(5)

if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
