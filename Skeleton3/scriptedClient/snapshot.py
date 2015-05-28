#!/usr/bin/env python
# -*- coding: utf-8 -*-

class Snapshot:
    """
    Represents a snapshot of a saved application state.

    Paramters
    ---------
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
        self.sessionId = sessionId
        self.snapshotType = snapshotType
        self.index = index
        self.saveName = saveName
        self.layout = layout
        self.preferences = preferences
        self.data = data
        self.description = description
        self.dateCreated = dateCreated
        self.con = connection
        return

    def restore(self):
        """
        Read and restore this state.

        Returns
        -------
        list
            An error message if there was a problem restoring the snapshot;
            empty otherwise.
        """
        result = self.con.cmdTagList("restoreSnapshot",
                                     sessionId=self.sessionId,
                                     saveName=self.saveName)
        return result

    def delete(self):
        """
        Delete this state.

        Returns
        -------
        list
            An error message if there was a problem deleting the snapshot;
            empty otherwise.
        """
        result = self.con.cmdTagList("deleteSnapshot",
                                     sessionId=self.sessionId,
                                     saveName=self.saveName)
        return result
