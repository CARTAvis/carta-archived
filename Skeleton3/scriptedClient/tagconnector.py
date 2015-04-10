#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket
import json

from layer2 import TagMessage, TagMessageSocket
from layer3 import JsonMessage

class TagConnector:
    """
    Can connect to a port, then exposes the cmd() method to send commands
    and retrieve results. The commands and results are in JsonMessage format.
    Uses TagMessageSocket, which gives us freedom to later send/receive raw
    tag messages
    """
    def __init__(self,port):
        self.port = port
        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("localhost", self.port))
        self.tagMessageSocket = TagMessageSocket( self.socket)

    def cmd( self, cmd, ** kwargs):
        self.tagMessageSocket.send( JsonMessage.fromKW( cmd=cmd, args=kwargs).toTagMessage())
        tm = self.tagMessageSocket.receive()
        result = JsonMessage.fromTagMessage(tm)
        return result.jsonString

    def cmdTagList( self, cmd, ** kwargs):
        """
        Send a tag message, return a list
        """
        self.tagMessageSocket.send( JsonMessage.fromKW( cmd=cmd, args=kwargs).toTagMessage())
        tm = self.tagMessageSocket.receive()
        result = JsonMessage.fromTagMessage(tm)
        j = json.loads(str(result.jsonString))
        try:
            returnValue = j['result']
        except KeyError:
            returnValue = j['error']
        return returnValue
