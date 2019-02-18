from aiohttp import web
import re
import json

from controller import LIGHTSENSOR_RESOURCE, get_lightsensor_resource, set_lightsensor_threshold


'''
------------------- CONSTANS ------------------
'''

FILE_INDEX = 'index.html'
FILE_MAC_CONFIG = 'mac.conf'
FILE_MAC_AVAILABLE = 'mac_available.txt'
FILE_STATE = 'controllerState.txt'

'''
------------------- GET ------------------
'''


# load index.html template and send it back
async def get_index(request):
    return web.FileResponse(FILE_INDEX)

async def get_controller_state(request):
    f = open(FILE_STATE)
    state = f.read()
    f.close()
    states = re.findall(': \t(.*?)\n', state)
    states[0] = {'dark': False, 'bright': True}[states[0]]
    states[2] = {'closed': False, 'open': True}[states[2]]
    for i in [1, 3, 4]:
       states[i] = {'False': False, 'True': True}[states[i]]

    return web.Response(text=json.dumps({'states': states}))

async def get_current_mac(request):
    f = open(FILE_MAC_CONFIG)
    html = f.read()
    f.close()
    return web.Response(text=html)


async def get_macs(request):
    f = open(FILE_MAC_AVAILABLE)
    macs = f.read().split('\n')
    f.close()

    # build json response
    html = '{"macs":['
    for i, mac in enumerate(macs):
        if i > 0:
            html += ','
        html += '"{}"'.format(mac)
    html += ']}'

    return web.Response(text=html)


def get_threshold_uri_from_resource(resource):
    if resource == 'min':
        return LIGHTSENSOR_RESOURCE.THRESHOLD_MIN
    if resource == 'max':
        return LIGHTSENSOR_RESOURCE.THRESHOLD_MAX
    return None


async def get_threshold(request):
    resource = request.match_info.get('resource', None)
    resource_uri = get_threshold_uri_from_resource(resource)
    if resource_uri is None:
        return web.Response(text="ERROR")

    html = await get_lightsensor_resource(resource_uri)
    return web.Response(text=html)


'''
------------------- POST ------------------
'''


async def post_mac(request):
    newMac = request.match_info.get('macAddress', None)
    f = open(FILE_MAC_CONFIG, 'w')
    f.write(newMac)
    f.close()
    return web.Response(text='')


async def post_threshold(request):
    newTreshold = request.match_info.get('thresholdValue', None)

    resource = request.match_info.get('resource', None)
    resource_uri = get_threshold_uri_from_resource(resource)
    if resource_uri is None:
        return web.Response(text="ERROR")

    await set_lightsensor_threshold(resource_uri, newTreshold)
    return web.Response(text='')


'''
--------------------- MAIN ------------------
'''

if __name__ == "__main__":
    app = web.Application()
    app.add_routes([web.get('/', get_index),
                    web.get('/state', get_controller_state),
                    web.get('/devices', get_macs),
                    web.get('/device', get_current_mac),
                    web.get('/lightsensor/threshold/{resource}', get_threshold),

                    web.post('/device&mac={macAddress}', post_mac),
                    web.post('/lightsensor/threshold/{resource}&val={thresholdValue}', post_threshold),
                    ])
    web.run_app(app, port=8081)
