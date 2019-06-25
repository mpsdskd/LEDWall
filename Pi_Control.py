#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun 12 21:32:56 2019

@author: mps
"""

import asyncio
import websockets
from random import randint
import json
import logging

import nest_asyncio
nest_asyncio.apply()


import subprocess



logger = logging.getLogger("SmartLightControl")

verbosity = 5
level = logging.ERROR
if verbosity > 0:
    level=logging.INFO
    if verbosity>9:
        level=logging.DEBUG
if not logger.handlers:
    logging.basicConfig(#filename='SLC.log',
                filemode='w',
                format='%(asctime)s,%(msecs)d %(name)s %(levelname)s %(message)s',
                datefmt='%H:%M:%S',
                level=level)

    if len(logging.getLogger().handlers) < 2:
        logging.getLogger().addHandler(logging.StreamHandler())
        print("Verbosity high for console output")


def ping(address):
    try:
        subprocess.check_output(['ping', '-c1', address])
        logger.info("Successful ping to " + address)
        return True
    except:
        logger.info("Ping to " + address + " did not succeed")
        return False


with open("rules.json") as json_file:
    rules=json.load(json_file)
    logger.debug(rules)

async def set_brightness(address, brightness, fade=False, start=0, delay=0.1):
    logger.info("Setting brightness " + str(brightness))
    if not fade:
        start = brightness
    if start > brightness:
        step_list= list(range(start, brightness-1, -1))
    else:
        step_list= list(range(start, brightness+1))
    for i in step_list:
        try:
            async with websockets.connect(
                        "ws://"+address+":81") as websocket:
                await websocket.send("%" + str(i))
                answer = await websocket.recv()
                logger.debug("Set brightness "+str(i))
                logger.debug("Websocket: " + answer)
            asyncio.sleep(delay)
        except Exception as e:
            logger.error(e)
            

async def periodic():
    while True:
        try:
            for rule in rules:
                if rule["type"]=="ping":
                    if not "last_state" in rule.keys():

                        logger.debug("nonexistent last_state")
                        rule["last_state"]="asdf"
                    
                    actual_state = False
                    for device in rule["ping_devices"]:
                        if ping(device):
                            logger.info("Found active device")
                            actual_state = True
                    if actual_state != rule["last_state"]:
                        if actual_state:
                            logger.info("Activating devices")
                            for device in rule["activate_devices"]:
                                await set_brightness(device["address"], device["brightness"], fade=device["fade"], delay=0)
                        else:
                            logger.info("Deactivating devices")
                            for device in rule["activate_devices"]:
                                await set_brightness(device["address"], 0, fade=device["fade"], start=10, delay=1)
                    else:
                        logger.info("State did not change")
                    rule["last_state"]  = actual_state
                
        except Exception as e:
            logger.error(e)

                    
        await asyncio.sleep(5)

def stop():
    task.cancel()


loop = asyncio.get_event_loop()
#loop.call_later(10, stop)
task = loop.create_task(periodic())
#loop.create_task(
loop.run_until_complete(task)
