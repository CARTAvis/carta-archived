#!/usr/bin/env python
# -*- coding: utf-8 -*-

class Snapshot:
    """
    Represents a snapshot of a saved application state.

    Parameters
    ----------
    sessionId: string
        An identifier for a user session.

    snapshotType: string
        [NOTE: I don't actually know what this is for.]

    index: integer
        [NOTE: I don't actually know what this is for.]

    saveName: string
        The name of the snapshot.

    layout: boolean
        True if the layout has been saved; false otherwise.

    preferences: boolean
        True if the preferences have been saved; false otherwise.

    data: boolean
        True if the data has been saved; false otherwise.

    description: string
        Descriptive information about the snapshot.

    dateCreated: string
        The date the snapshot was created.

    connection: TagConnector object
        Provides facilities for communicating with C++.
    """

    def __init__(self, sessionId, snapshotType, index, saveName, layout,
                 preferences, data, description, dateCreated, connection):
        self.__sessionId = sessionId
        self.__snapshotType = snapshotType
        self.__index = index
        self.__saveName = saveName
        self.__layout = layout
        self.__preferences = preferences
        self.__data = data
        self.__description = description
        self.__dateCreated = dateCreated
        self.__con = connection
        return

    def restore(self):
        """
        Read and restore this state.

        Returns
        -------
        list
            An error message if there was a problem restoring the
            snapshot; empty otherwise.
        """
        result = self.__con.cmdTagList("restoreSnapshot",
                                     sessionId=self.__sessionId,
                                     saveName=self.__saveName)
        return result

    def delete(self):
        """
        Delete this state.

        Returns
        -------
        list
            An error message if there was a problem deleting the
            snapshot; empty otherwise.
        """
        result = self.__con.cmdTagList("deleteSnapshot",
                                     sessionId=self.__sessionId,
                                     saveName=self.__saveName)
        return result

    def save(self):
        """
        Save this state.

        Returns
        -------
        list
            An error message if there was a problem saving the snapshot;
            empty otherwise.
        """
        result = self.__con.cmdTagList("saveSnapshot",
                                       sessionId=self.__sessionId,
                                       saveName=self.__saveName,
                                       saveLayout=self.__layout,
                                       savePreferences=self.__preferences,
                                       saveData=self.__data,
                                       description=self.__description)
        return result

    def getSessionId(self):
        """
        Get the session ID of this snapshot.

        Returns
        -------
        string
            The session ID of this snapshot.
        """
        return self.__sessionId

    def getSnapshotType(self):
        """
        Get the type of this snapshot.

        Returns
        -------
        string
            The type of this snapshot.
        """
        return self.__snapshotType

    def getIndex(self):
        """
        Get the index of this snapshot.

        Returns
        -------
        integer
            The index of this snapshot.
        """
        return self.__index

    def getSaveName(self):
        """
        Get the save name of this snapshot.

        Returns
        -------
        string
            The save name of this snapshot.
        """
        return self.__saveName

    def getLayout(self):
        """
        Get the layout properties of this snapshot.

        Returns
        -------
        boolean
            True if this snapshot contains layout information; false
            otherwise.
        """
        return self.__layout

    def getPreferences(self):
        """
        Get the preferences of this snapshot.

        Returns
        -------
        boolean
            True if this snapshot contains preferences information;
            false otherwise.
        """
        return self.__preferences

    def getData(self):
        """
        Get the data properties of this snapshot.

        Returns
        -------
        boolean
            True if this snapshot contains data information; false
            otherwise.
        """
        return self.__data

    def getDescription(self):
        """
        Get the description of this snapshot.

        Returns
        -------
        string
            The description of this snapshot.
        """
        return self.__description

    def getDateCreated(self):
        """
        Get the creation date of this snapshot.

        Returns
        -------
        string
            The creation date of this snapshot.
        """
        return self.__dateCreated
