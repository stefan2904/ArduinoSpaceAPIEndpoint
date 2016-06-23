import time
import socket
import requests
# import datetime

ENDPOINTFILE = 'endpoints.txt'
GRAPHITEHOST = '127.0.0.1'
GRAPHITEPORT = 2003


def logSensor(logtime, sock, space, sensorname, value):
    name = 'sensors.' + space + '.' + sensorname
    data = '%s %s %s' % (name, value, logtime)
    print data
    # sock.send(data)


def logEndpoint(logtime, sock, url):
    data = requests.get(url).json()
    space = data['space']
    for sensor in data['sensors']['temperature']:
        location = sensor['location'].replace(' ', '')
        value = sensor['value']
        # unit = sensor['unit']  # assume degreeC
        logSensor(logtime, sock, space, location, value)


def init():
    now = time.time()
    logtime = int(now - (now % (15 * 60)))
    # print datetime.datetime.fromtimestamp(logtime).strftime('%H:%M:%S')
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # sock.connect((GRAPHITEHOST, GRAPHITEPORT))
    return logtime, sock

if __name__ == '__main__':
    logtime, sock = init()
    with open(ENDPOINTFILE, 'r') as f:
        for endpoint in f:
            logEndpoint(logtime, sock, endpoint.strip())
