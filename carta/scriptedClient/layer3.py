#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
from layer2 import TagMessage, TagMessageSocket

class JsonMessage:
    """
    JSON message holder.

    Parameters
    ----------
    jsonString: string
        Contains the JSON in string format.
    """
    def __init__(self, jsonString):
        self.jsonString = jsonString

    def toTagMessage(self):
        """
        Convert message to TagMessage format with a "json" tag.

        Returns
        -------
        TagMessage
        """
        return TagMessage("json", self.jsonString)

    def toAsyncMessage(self):
        """
        Convert message to TagMessage format with an "async" tag.

        Returns
        -------
        TagMessage
        """
        return TagMessage("async", self.jsonString)

    @staticmethod
    def fromTagMessage(tm):
        """
        Construct a JsonMessage from a TagMessage.

        Parameters
        ----------
        tm: TagMessage

        Returns
        -------
        A JsonMessage representation of a TagMessage.
        """
        if tm.tag != "json":
            raise NamedError("tag message does not have 'json' as tag")
        return JsonMessage(tm.data)

    @staticmethod
    def fromAsyncMessage(tm):
        """
        Construct a JsonMessage from a TagMessage with an "async" tag.

        Parameters
        ----------
        tm: TagMessage

        Returns
        -------
        A JsonMessage representation of a TagMessage with an "asyn" tag.
        """
        if tm.tag != "async":
            raise NamedError("async message does not have 'async' as tag")
        return JsonMessage(tm.data)

    @staticmethod
    def fromKW(** kwargs):
        """
        Convert kwargs to JSON, and return the corresponding JsonMessage.

        Parameters
        ----------
        kwargs: dict

        Returns
        -------
        JsonMessage
            A JsonMessage representation of the input dict object.
        """
        return JsonMessage(json.dumps(kwargs))

class JsonSocket:
    """
    A socket wrapper that allows sending and receiving of JsonMessages.

    Parameters
    ----------
    rawSocket: socket
    """
    def __init__(self, rawSocket):
        self.tagMessageSocket = TagMessageSocket(rawSocket)

    def send(self, jsonMessage):
        """
        Send a JsonMessage by converting it to a TagMessage.

        Parameters
        ----------
        jsonMessage: JsonMessage
            The JsonMessage to send.
        """
        self.tagMessageSocket.send(jsonMessage.toTagMessage())
        return

    def receive(self):
        """
        Receive a JsonMessage.

        Returns
        -------
        JsonMessage
            A JsonMessage that has been converted from a TagMessage.
        """
        tm = self.tagMessageSocket.receive()
        return JsonMessage.fromTagMessage(tm)
