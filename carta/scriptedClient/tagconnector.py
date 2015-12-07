#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket
import json

from layer2 import TagMessage, TagMessageSocket
from layer3 import JsonMessage

class TagConnector:
    """
    Connects to a port and contains method to send commands and retrieve
    results. The commands and results are in JsonMessage format.
    Uses TagMessageSocket, which gives us freedom to later send/receive raw
    tag messages.

    Parameters
    ----------
    port: integer
        The port number to connect to.
    """

    def __init__(self, port):
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("localhost", self.port))
        self.tagMessageSocket = TagMessageSocket(self.socket)

    def cmdTagList(self, cmd, ** kwargs):
        """
        Send a tag message, return a list.

        A command may have no arguments, e.g.:

            cmdTagList("commandString").

        Arguments, when present, are specified as key-value pairs, e.g.:

            cmTagList("commandString", arg1=1, arg2=True, arg3="foo")

        Parameters
        ----------
        cmd: string
            The name of the command to send.
        kwargs: dict
            The arguments to the command, if any.

        Returns
        -------
        list
            The contents of the list vary depending on the command.
        """
        self.tagMessageSocket.send(
            JsonMessage.fromKW(cmd=cmd, args=kwargs).toTagMessage())
        tm = self.tagMessageSocket.receive()
        result = JsonMessage.fromTagMessage(tm)
        j = json.loads(str(result.jsonString))
        try:
            returnValue = j['result']
        except KeyError:
            returnValue = j['error']
        return returnValue

    def cmdAsyncList(self, cmd, ** kwargs):
        """
        Send an asynchronous message, return a list.
        Note that for now, the asynchronous behaviour is only on the C++ side.
        This may change in the future if we introduce asynchronous behaviour to
        the Python client.

        Parameters
        ----------
        cmd: string
            The name of the command to send.
        kwargs: dict
            The arguments to the command, if any.

        Returns
        -------
        list
            The contents of the list vary depending on the command.
        """
        self.tagMessageSocket.send(
            JsonMessage.fromKW(cmd=cmd, args=kwargs).toAsyncMessage())
        tm = self.tagMessageSocket.receive()
        result = JsonMessage.fromTagMessage(tm)
        j = json.loads(str(result.jsonString))
        try:
            returnValue = j['result']
        except KeyError:
            returnValue = j['error']
        return returnValue
