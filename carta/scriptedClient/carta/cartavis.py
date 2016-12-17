#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import time
import json

from tagconnector import TagConnector
from histogram import Histogram
from animator import Animator
from colormap import Colormap
from snapshot import Snapshot
from image import Image

class Cartavis:
    """
    A Python representation of the Carta application.

    Parameters
    ----------
    executable: string
        The full path of the Carta executable file.
    configFile: string
        The full path of the cartavis config.json file.
    port: integer
        The port which will be used to send commands to C++ and receive
        results.
    htmlFile: string
        The full path of the desktopIndex.html file.
    imageFile: string
        The full path of a compatible image file to load.
    """

    def __init__(self, executable = None, configFile = None, port = 9999, htmlFile = None, imageFile = None):
        args = [executable, "--scriptPort",
                str(port), "--html", htmlFile, imageFile]
        print args
        if executable != None and configFile != None and htmlFile != None and imageFile != None:
            self.popen = subprocess.Popen(args)
            print "Started process with pid=", self.popen.pid
            time.sleep(3)
        self.visible = False
        self.con = TagConnector(port)
        return

    def setGuiVisible(self,flag):
        """
        This command currently doesn't do anything useful.
        """
        self.visible = flag
        return

    def isGuiVisible(self):
        """
        This command currently doesn't do anything useful.
        """
        return self.visible

    def kill(self):
        """
        Closes the application.
        """
        self.popen.kill()

    def quit(self):
        """
        Closes the application.
        An alias for kill().
        """
        self.kill()

    def getColormaps(self):
        """
        Return a list of the names of colormaps available on the server.

        Returns
        -------
        list
            The names of colormaps available on the server.
        """
        commandStr = "getColormaps"
        colormapsList = self.con.cmdTagList(commandStr)
        return colormapsList

    def getImageViews(self):
        """
        Return a list of the image views defined by the layout.
        If there is more than one image view in the current application
        layout, each one can be accessed via its own element in the
        list. For example, if there are 3 image views in the GUI, the
        following sequence of commands can be used to load a different
        image in each viewer:

            i = v.getImageViews()
            i[0].loadLocalFile('/tmp/m31.fits')
            i[1].loadLocalFile('/tmp/m42.fits')
            i[2].loadLocalFile('/tmp/m33.fits')

        Returns
        -------
        list
            A list of Image objects.
        """
        commandStr = "getImageViews"
        imageViewsList = self.con.cmdTagList(commandStr)
        imageViews = []
        if (imageViewsList[0] != ""):
            for iv in imageViewsList:
                imageView = Image(iv, self.con)
                imageViews.append(imageView)
        return imageViews

    def getColormapViews(self):
        """
        Return a list of the colormap views defined by the layout.
        If there is more than one colormap view in the current
        application layout, each one can be accessed via its own element
        in the list. For example, if there are 3 colormap views in the
        GUI, the following sequence of commands can be used to load a
        different colormap in each one:

            c = v.getColormapViews()
            c[0].setColormap('coolwarm')
            c[1].setColormap('spring')
            c[2].setColormap('cubehelix')

        Returns
        -------
        list
            A list of Colormap objects.
        """
        commandStr = "getColormapViews"
        colormapViewsList = self.con.cmdTagList(commandStr)
        colormapViews = []
        if (colormapViewsList[0] != ""):
            for cmv in colormapViewsList:
                colormapView = Colormap(cmv, self.con)
                colormapViews.append(colormapView)
        return colormapViews

    def getAnimatorViews(self):
        """
        Return a list of the animator views defined by the layout.
        If there is more than one animator view in the current
        application layout, each one can be accessed via its own element
        in the list. For example, if there are 3 animator views in the
        GUI, the following sequence of commands can be used to display a
        different channel in each one:

            a = v.getAnimatorViews()
            a[0].setChannel(1)
            a[1].setChannel(110)
            a[2].setChannel(5)

        Returns
        -------
        list
            A list of Animator objects.
        """
        commandStr = "getAnimatorViews"
        animatorViewsList = self.con.cmdTagList(commandStr)
        animatorViews = []
        if (animatorViewsList[0] != ""):
            for av in animatorViewsList:
                animatorView = Animator(av, self.con)
                animatorViews.append(animatorView)
        return animatorViews

    def getHistogramViews(self):
        """
        Return a list of the histogram views defined by the layout.
        If there is more than one histogram view in the current
        application layout, each one can be accessed via its own element
        in the list. For example, if there are 3 histogram views in the
        GUI, the following sequence of commands can be used to set a
        different clip range percent in each one:

            h = v.getHistogramViews()
            h[0].setClipRangePercent(1, 99)
            h[1].setClipRangePercent(5, 95)
            h[2].setClipRangePercent(0.1, 99.9)

        Returns
        -------
        list
            A list of Histogram objects.
        """
        commandStr = "getHistogramViews"
        histogramViewsList = self.con.cmdTagList(commandStr)
        histogramViews = []
        if (histogramViewsList[0] != ""):
            for hv in histogramViewsList:
                histogramView = Histogram(hv, self.con)
                histogramViews.append(histogramView)
        return histogramViews

    def setAnalysisLayout(self):
        """
        Set the layout to a predefined analysis layout.

        Returns
        -------
        list
            An empty list, as the C++ methods that it calls have void
            return values.
        """
        commandStr = "setAnalysisLayout"
        result = self.con.cmdTagList(commandStr)
        return result

    def setImageLayout(self):
        """
        Set the layout to a predefined layout displaying a single image.

        Returns
        -------
        list
            An empty list, as the C++ methods that it calls have void
            return values.
        """
        commandStr = "setImageLayout"
        result = self.con.cmdTagList(commandStr)
        return result

    def setCustomLayout(self, rows, cols):
        """
        Set the number of rows and columns in the layout grid.

        Parameters
        ----------
        rows: integer
            The number of rows in the layout grid.
        cols: integer
            The number of columns in the layout grid.

        Returns
        -------
        list
            An error message if there was a problem setting the layout;
            empty otherwise.
        """
        result = self.con.cmdTagList("setCustomLayout", nrows=rows, ncols=cols)
        return result

    def setPlugins(self, pluginList):
        """
        Set plugins for each of the views in the layout.

        Parameters
        ----------
        pluginList: list
            A list of strings representing plugin names.
            Valid plugin names [NOTE: this list may not be complete]:

                - CasaImageLoader
                - Animator
                - Colormap
                - Histogram
                - Hidden

        Returns
        -------
        list
            An error message if there was a problem setting the plugins;
            empty otherwise.
        """
        pluginString = ' '.join(pluginList)
        result = self.con.cmdTagList("setPlugins", plugins=pluginString)
        return result

    def getPluginList(self):
        """
        Returns a list of the current plugins in the view.

        Returns
        -------
        list
            A list of view plugins.
        """
        result = self.con.cmdTagList("getPluginList")
        return result

    def getEmptyWindowCount(self):
        """
        Returns the number of empty windows in the application.
        This is a convenience function that simply counts the number of
        occurrences of 'Empty' in the output of getPluginList().

        Returns
        -------
        integer
            The number of empty windows in the application.
        """
        plugins = self.getPluginList()
        result = plugins.count('Empty')
        return result

    def setEmptyWindowPlugin(self, n, plugin):
        """
        Set the nth empty window to a desired plugin.
        A convenience function.
        Note that since this function changes the number of empty
        windows, it is possible to repeatedly call it with n set to 0
        to set a different empty window to a plugin each time. e.g. if
        there are 4 empty windows, calling:

            v.setEmptyWindowPlugin(0, 'CasaImageLoader')
            v.setEmptyWindowPlugin(0, 'CasaImageLoader')
            v.setEmptyWindowPlugin(0, 'Histogram')
            v.setEmptyWindowPlugin(0, 'Animator')

        will set each of the empty windows to the desired plugins.

        Parameters
        ----------
        n: integer
            A number in [0, getEmptyWindowCount()) indicating which
            empty window should be changed.
        plugin: string
            A valid plugin name.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = []
        emptyCount = self.getEmptyWindowCount()
        if (n >= emptyCount):
            result = ["error", "Invalid empty window number: " + str(n)]
        else:
            plugins = self.getPluginList()
            emptyIndexes = [i for i, x in enumerate(plugins) if x == 'Empty']
            plugins[emptyIndexes[n]] = plugin
            result = self.setPlugins(plugins)
        return result

    def addLink(self, source, dest):
        """
        Establish a link between a source and destination.
        The parameters are CartaView objects that are obtained by
        commands such as getImageViews() and getColormapViews(). For
        example, the following sequence of commands will obtain the
        image views and histogram views, then create a link between the
        first image view and the first histogram view:

            i = v.getImageViews()
            h = v.getHistogramViews()
            v.addLink(i[0], h[0])

        Parameters
        ----------
        source: CartaView object
            The source object for the link.
        dest: CartaView object
            The destination object for the link.

        Returns
        -------
        list
            An error message if there was a problem adding the link;
            empty otherwise.
        """
        result = self.con.cmdTagList("addLink", sourceView=source.getId(),
                                     destView=dest.getId())
        return result

    def removeLink(self, source, dest):
        """
        Remove a link from a source to a destination.
        The parameters are CartaView objects that are obtained by
        commands such as getImageViews() and getColormapViews(). For
        example, the following sequence of commands will obtain the
        image views and histogram views, then remove the link between
        the first image view and the first histogram view:

            i = v.getImageViews()
            h = v.getHistogramViews()
            v.removeLink(i[0], h[0])

        Parameters
        ----------
        source: CartaView object
            The source object for the link.
        dest: CartaView object
            The destination object for the link.

        Returns
        -------
        list
            An error message if there was a problem removing the link;
            empty otherwise.
        """
        result = self.con.cmdTagList("removeLink", sourceView=source.getId(),
                                     destView=dest.getId())
        return result

    def newSnapshot(self, sessionId, saveName, saveLayout, savePreferences,
                     saveData, description):
        """
        Create a new Snapshot object that can be saved.

        Parameters
        ----------
        sessionId: string
            An identifier for a user session.

        saveName: string
            The name of the snapshot.

        saveLayout: boolean
            True if the layout is to be saved; false otherwise.

        savePreferences: boolean
            True if the preferences are to be saved; false otherwise.

        saveData: boolean
            True if the data is to be saved; false otherwise.

        description: string
            Descriptive information about the snapshot.

        Returns
        -------
        Snapshot
            A new instance of the Snapshot class.
        """
        snapshot = Snapshot(sessionId=sessionId, snapshotType='', index=0,
                            saveName=saveName, layout=saveLayout,
                            preferences=savePreferences, data=saveData,
                            description=description, dateCreated='',
                            connection=self.con)
        return snapshot

    def getSnapshots(self, sessionId):
        """
        Get a list of Snapshot objects.

        Parameters
        ----------
        sessionId: string
            An identifier for a user session.

        Returns
        -------
        list
            A list of Snapshot objects.
        """
        snapshots = self.con.cmdTagList("getSnapshotObjects",
                                        sessionId=sessionId)
        snapshotObjects = []
        if (snapshots[0] != ""):
            for snapshot in snapshots:
                j = json.loads(snapshot)
                snapObj = Snapshot(sessionId, j['type'], j['index'],
                                   j['Snapshot'], j['layout'],
                                   j['preferences'], j['data'],
                                   j['description'], j['dateCreated'],
                                   self.con)
                snapshotObjects.append(snapObj)
        return snapshotObjects
