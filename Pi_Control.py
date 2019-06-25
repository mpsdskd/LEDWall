#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun 12 21:32:56 2019

@author: mps
"""

import asyncio
import websockets
from random import randint

import nest_asyncio
nest_asyncio.apply()

async def periodic():
    while True:
        print('Checking')
        await asyncio.sleep(1)
        
        async with websockets.connect(
                'ws://192.168.178.24:81') as websocket:
            await websocket.send("%" + str(randint(0, 200)))
            
            answer = await websocket.recv()
            
            print("Answer: " + answer)

def stop():
    task.cancel()

loop = asyncio.get_event_loop()
loop.call_later(5, stop)
task = loop.create_task(periodic())

try:
    loop.run_until_complete(task)
except asyncio.CancelledError:
    pass