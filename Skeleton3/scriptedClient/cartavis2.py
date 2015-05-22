#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket
import struct
import random
import json

from layer2 import TagMessage, TagMessageSocket
from layer3 import JsonMessage, JsonSocket

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

class JsonConnector:
    """
    Can connect to a port, then exposes the cmd() method to send commands
    and retrieve results. The commands and results are in JsonMessage format.
    Uses JsonSocket, which means we can only send/receive JsonMessages...
    """
    def __init__(self,port):
        self.port = port
        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("localhost", self.port))
        self.jsonSocket = JsonSocket( self.socket)
    def cmd( self, cmd, ** kwargs):
        self.jsonSocket.send( JsonMessage.fromKW( cmd=cmd, args=kwargs))
        result = self.jsonSocket.receive().jsonString
        return result

def test(con, cmd, ** kw):
    """
    Run a single test. Print the command+arguments, send the command,
    receive the result, and print the result. Return the result...
    """
    print "=================================================="
    print "Testing command ", cmd
    for key, value in kw.iteritems():
        print "    %s = %s" % (key, value)
    result = con.cmd( cmd, ** kw)
    print "Results:"
    print result

def runTests(con):
    """Run some tests on the given connector"""
    test(con, "ls")
    test(con, "getcolormapviews")
    test(con, "ls", dir="/home")
    test(con, "add", a=1, b=0.1)
    test(con, "xyz")

def jsonTest():
    """Run the tests using JsonSocket..."""
    con = JsonConnector(1234)
    runTests(con)

def tagTest():
    """Run the tests using TagSocket..."""
    con = TagConnector(1234)
    runTests(con)

print "to run JsonSocket() tests type 'jsonTest()'"
print "to run TagMessageSocket() tests type 'tagTest()'"

