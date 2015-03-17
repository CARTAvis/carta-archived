#!/usr/bin/env python
# -*- coding: utf-8 -*-

import struct
from layer1 import VarLenMessage, VarLenSocket

class TagMessage:
    """TagMessage contains a string tag and raw data (bytearray)"""
    def __init__(self, tag, data):
        self.tag = tag
        self.data = data
    def toVarLenMessage(self):
        """converts itself to VarLenMessage"""
        binData = bytearray(self.tag)
        binData.append(0)
        binData.extend(self.data)
        return VarLenMessage(binData)
    @staticmethod
    def fromVarLenMessage(vlm):
        """creates a TagMessage from VarLenMessage"""
        # find the position of the '\0'
        end = vlm.data.find('\0')
        if end < 0:
            raise NameError('received tag message has no null char')
        # extract the tag (null terminated string)
        fmt = "{0}s".format(end)
        tag = struct.unpack_from( fmt, vlm.data)[0]
        # the data is the rest of the message
        data = vlm.data[end+1:]
        # return the tag message
        return TagMessage(tag,data)

class TagMessageSocket:
    """socket wrapper that allows sending/receiving of tag messages"""
    def __init__(self,rawSocket):
        self.varLenSocket = VarLenSocket(rawSocket)
    def send(self,tagMessage):
        self.varLenSocket.send( tagMessage.toVarLenMessage())
    def receive(self):
        """receive a TagMessage"""
        # get the VarLenMessage
        vlm = self.varLenSocket.receive()
        # convert the VarLenMessage to TagMessage
        return TagMessage.fromVarLenMessage(vlm)
