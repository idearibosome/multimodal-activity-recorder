import argparse
import asyncio
import importlib
import json
import os
import re

import cv2 as cv
import numpy as np
from PySide2.QtCore import *
import websockets


class MMRWSData():
  def __init__(self):
    self.requestType = ''
    self.dataType = ''
    self.data = {}
  
  def load_from_bytes(self, data):
    byteArray = QByteArray(data)
    inStream = QDataStream(byteArray)
    inStream.setVersion(QDataStream.Qt_5_9)

    self.requestType = inStream.readQString()
    self.dataType = inStream.readQString()
    self.data = inStream.readQVariant()
  
  def to_bytes(self):
    byteArray = QByteArray()
    outStream = QDataStream(byteArray, QIODevice.WriteOnly)
    outStream.setVersion(QDataStream.Qt_5_9)

    outStream.writeQString(self.requestType)
    outStream.writeQString(self.dataType)
    outStream.writeQVariant(self.data)

    return byteArray.data()


# parser
parser = argparse.ArgumentParser()

parser.add_argument('--config_path', help='Path of the config file', type=str)

args = parser.parse_args()


# config
config = json.load(open(args.config_path, 'r'))


# model
model_module = importlib.import_module(config['model']['name'])
model = model_module.create_model(config['model']['config'])


# modalities
modality_identifier_list = []
modality_info_list = []
modality_identifier_to_name_dict = {}
for modality_info in config['modalities']:
  if (not 'identifier' in modality_info):
    modality_info['identifier'] = ''
  if (len(modality_info['identifier']) <= 0):
    identifier = input('- input identifier for %s: ' % (modality_info['name']))
    modality_info['identifier'] = identifier
  
  modality_identifier_to_name_dict[modality_info['identifier']] = modality_info['name']
  
  modality_identifier_list.append(modality_info['identifier'])
  modality_info_list.append(modality_info)


# websocket functions
def wsPrepare():
  model.prepare()

def wsStart():
  model.start()

def wsRecognize(data):
  new_data = {}
  for (identifier, item) in data.items():
    new_item = {}
    for subitem in item:
      if (subitem['type'] == 'value'):
        new_item[subitem['name']] = subitem['value']
      elif (subitem['type'] == 'image'):
        new_item[subitem['name']] = subitem['image'].data()
    new_data[modality_identifier_to_name_dict[identifier]] = new_item
  return model.recognize(new_data)

def wsStop():
  model.stop()

def wsFinalize():
  model.finalize()


# run websocket
async def run():
  print('- connecting...')
  async with websockets.connect(config['server']['url']) as websocket:
    wsData = MMRWSData()
    wsData.requestType = 'register'
    wsData.dataType = 'request'
    wsData.data['name'] = config['model']['name']
    wsData.data['identifier'] = config['model']['identifier']
    wsData.data['modalities'] = modality_identifier_list
    wsData.data['fps'] = config['model']['fps']

    await websocket.send(wsData.to_bytes())
    receivedData = await websocket.recv()
    print('- connected')

    while True:
      receivedData = await websocket.recv()
      wsData = MMRWSData()
      wsData.load_from_bytes(receivedData)

      print(wsData.requestType)

      if (wsData.requestType == 'prepare'):
        wsPrepare()
      elif (wsData.requestType == 'start'):
        wsStart()
      elif (wsData.requestType == 'recognize'):
        res = wsRecognize(wsData.data)

        resWsData = MMRWSData()
        resWsData.requestType = 'recognize'
        resWsData.dataType = 'response'
        resWsData.data = res
        await websocket.send(resWsData.to_bytes())

      elif (wsData.requestType == 'stop'):
        wsStop()
      elif (wsData.requestType == 'finalize'):
        wsFinalize()
        break


asyncio.get_event_loop().run_until_complete(run())


# finalize