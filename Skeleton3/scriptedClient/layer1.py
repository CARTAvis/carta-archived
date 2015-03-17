#!/usr/bin/env python
# -*- coding: utf-8 -*-

import struct

class VarLenMessage:
    """Variable length message. Essentially a bytearray wrapper with a nicer name?"""
    def __init__(self,data):
        self.data = data

class VarLenSocket:
    """Wraps a raw socket into something that can be used to send/receive VarLenMessages"""
    def __init__(self,rawSocket):
        self.rawSocket = rawSocket
    def send(self,varLenMessage):
        """send a VarLenMessage"""
        # encode and send the length of the message as 64bit, little endian
        self.rawSocket.sendall( struct.pack('<Q', len( varLenMessage.data)))
        # send the actual raw data
        self.rawSocket.sendall( varLenMessage.data)
    def receiveN(self,n):
        """helper method that receives n raw bytes and returns them as bytearray"""
        result = bytearray()
        remaining = n
        while remaining > 0:
            buff = self.rawSocket.recv(remaining)
            result.extend(buff)
            remaining -= len(buff)  
        return result
    def receive(self):
        """receive VarLenMessage"""
        # get the first 8 bytes
        sizeBuff = self.receiveN(8)
        # decode the 8 bytes into size
        size = struct.unpack('<Q', sizeBuff)
        # unpack always returns a tuple, so take the first element
        size = size[0]
        # receive the raw data ('size' bytes)
        data = self.receiveN(size)
        # convert the raw data into VarLenMessage and return that
        return VarLenMessage(data)
