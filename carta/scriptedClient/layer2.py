#!/usr/bin/env python
# -*- coding: utf-8 -*-

import struct
from layer1 import VarLenMessage, VarLenSocket

class TagMessage:
    """
    TagMessage contains a string tag and string data.

    Paramters
    ---------
    tag: string
        The type of the message. Currently supported tags are "json" and
        "async".
    data: string
        The data content of the message.
    """

    def __init__(self, tag, data):
        self.tag = tag
        self.data = data

    def toVarLenMessage(self):
        """
        Convert a TagMessage to a VarLenMessage

        Returns
        -------
        VarLenMessage
            A VarLenMessage representation of this TagMessage.
        """
        binData = bytearray(self.tag)
        binData.append(0)
        binData.extend(self.data)
        return VarLenMessage(binData)

    @staticmethod
    def fromVarLenMessage(vlm):
        """
        Create a TagMessage from a VarLenMessage.

        Parameters
        ----------
        vlm: VarLenMessage
            The message to convert to a TagMessage.

        Returns
        -------
        TagMessage
            A TagMessage representation of the input VarLenMessage.
        """
        # find the position of the '\0'
        end = vlm.data.find('\0')
        if end < 0:
            raise NameError('received tag message has no null char')
        # extract the tag (null terminated string)
        fmt = "{0}s".format(end)
        tag = struct.unpack_from(fmt, vlm.data)[0]
        # the data is the rest of the message
        data = vlm.data[end+1:]
        # return the tag message
        return TagMessage(tag,data)

class TagMessageSocket:
    """
    A socket wrapper that allows sending and receiving of TagMessages.

    Parameters
    ----------
    rawSocket: socket
    """

    def __init__(self, rawSocket):
        self.varLenSocket = VarLenSocket(rawSocket)

    def send(self, tagMessage):
        """
        Send a TagMessage by converting it to a VarLenMessage.

        Parameters
        ----------
        tagMessage: TagMessage
            The message to send.
        """
        self.varLenSocket.send(tagMessage.toVarLenMessage())

    def receive(self):
        """
        Receive a TagMessage.

        Returns
        -------
        TagMessage
            A TagMessage that has been converted from a VarLenMessage.
        """
        # get the VarLenMessage
        vlm = self.varLenSocket.receive()
        # convert the VarLenMessage to TagMessage
        return TagMessage.fromVarLenMessage(vlm)
