#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
from layer2 import TagMessage, TagMessageSocket

class JsonMessage:
    """json message holder"""
    def __init__(self, jsonString):
        """jsonString contains the json in string format"""
        self.jsonString = jsonString
    def toTagMessage(self):
        return TagMessage( "json", self.jsonString)
    @staticmethod
    def fromTagMessage(tm):
        """constructs JsonMessage from TagMessage"""
        if tm.tag != "json":
            raise NamedError("tag message does not have 'json' as tag")
        return JsonMessage( tm.data)

    @staticmethod
    def fromKW( ** kwargs):
        """converts kwargs to json, and returns the corresponding JsonMessage"""
        return JsonMessage( json.dumps(kwargs))

class JsonSocket:
    """socket wrapper that allows sending/receiving JsonMessages"""
    def __init__( self, rawSocket):
        self.tagMessageSocket = TagMessageSocket(rawSocket)
    def send(self,jsonMessage):
        """sends a JsonMessage"""
        self.tagMessageSocket.send( jsonMessage.toTagMessage())
        return
    def receive(self):
        """receives a JsonMessage"""
        tm = self.tagMessageSocket.receive()
        return JsonMessage.fromTagMessage(tm)

