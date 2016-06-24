import os
import time
import socket
import requests
# import datetime

ENDPOINTFILE = 'endpoints.txt'
GRAPHITEHOST = '127.0.0.1'
GRAPHITEPORT = 2003

interesting_sensors = [
    'ext_batterycharge',
    'ext_illumination',
    'ext_voltage',
    'humidity',
    'temperature',
    'total_member_count']


def logSensor(logtime, sock, space, sensorname, value):
    name = 'sensors.' + space + '.' + sensorname
    data = '%s %s %s\n' % (name, value, logtime)
    print data,
    sock.send(data)


def logEndpoint(logtime, sock, url):
    data = requests.get(url).json()
    space = data['space']
    for sensortype in data['sensors']:
        if sensortype not in interesting_sensors:
            continue
        for sensor in data['sensors'][sensortype]:
            try:
                location = sensor['location'].replace(' ', '')
                location = sensortype + '.' + location
                value = sensor['value']
                # unit = sensor['unit']  # assume degreeC
                logSensor(logtime, sock, space, location, value)
            except Exception as e:
                print e


def init():
    now = time.time()
    logtime = int(now - (now % (15 * 60)))
    # print datetime.datetime.fromtimestamp(logtime).strftime('%H:%M:%S')
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((GRAPHITEHOST, GRAPHITEPORT))
    return logtime, sock

if __name__ == '__main__':
    cwd = os.path.dirname(os.path.realpath(__file__))
    file = os.path.join(cwd, ENDPOINTFILE)
    logtime, sock = init()
    with open(file, 'r') as f:
        for endpoint in f:
            try:
                logEndpoint(logtime, sock, endpoint.strip())
            except Exception as e:
                print e
