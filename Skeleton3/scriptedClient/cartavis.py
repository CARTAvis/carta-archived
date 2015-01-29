#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import socket
import time
import os.path

"""
Sample run:

import cartavis
v = cartavis.start()
v.setResolution( 1000, 1000)
v.openFile("1.fits")
v.applyAutoClip( 95)
v.applyColormap( "helix1")
v.showCoordinates()
v.saveScreenshot( "/scratch/1.png")
"""

#lastPort = 12345
lastPort = 9999

class Application:
    """Represents an application"""

    def __init__(self, executable, configFile, port, htmlFile, imageFile):
        global lastPort
        print "lastPort = ", lastPort
#        args = ["/home/jeff/scratch/build/2014-10-24/cpp/desktop/desktop",
#            "--config",
#            "/home/jeff/.cartavis/config.json",
#            "--scriptPort",
#            str(lastPort),
#            "--html",
#            "/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html",
#            "/home/jeff/Dropbox/Astronomy/m31_cropped.fits"]
        args = [executable, "--scriptPort", 
                str(port), "--html", htmlFile, imageFile]
        print args
        self.popen = subprocess.Popen( args)
        print "Started process with pid=", self.popen.pid
        time.sleep( 3)
        self.visible = False
        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("localhost", lastPort))
        lastPort = lastPort + 1;
        return

    # It would be nice if this function could actually tun the GUI on and off
    # with whatever the current state is.
    def setGuiVisible(self,flag):
        self.visible = flag
        return

    def isGuiVisible(self):
        return self.visible

    def kill(self):
        self.popen.kill()

    def openFile(self,fileName):
        if (os.path.isfile(fileName)):
            self.socket.sendall("loadImageFile " + fileName + "\n")
            #self.socket.send("load " + fileName + "\n")
        else:
            print "Invalid filename: " + fileName

    def setResolution(self, xres, yres):
        print "Setting resolution to (" + str(xres) + ", " + str(yres) + ")"

    def applyAutoClip(self, percentage):
        print "Applying autoclip of " + str(percentage) + "%"
        self.socket.sendall("applyAutoClip " + str(percentage) + "\n")

    def applyColormap(self, colormap):
        print "Applying " + colormap + " colormap"

    def setCoordinates(self, coordinateSystem):
        print "Setting coordinates to " + coordinateSystem + " system"
        self.socket.sendall("setCoordinates " + coordinateSystem + "\n")

    def showCoordinates(self):
        self.socket.send("showcoordinates" + "\n")

    def hideCoordinates(self):
        self.socket.send("hidecoordinates" + "\n")

    def toggleCoordinates(self):
        self.socket.send("togglecoordinates" + "\n")

    def saveScreenshot(self, fileName):
        print "Saving screenshot as " + fileName

def start(
        executable = "/home/jeff/scratch/build/2015-01-27/cpp/desktop/desktop", 
        configFile = "/home/jeff/.cartavis/config.json", 
        port = 9999, 
        htmlFile = "/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html", 
        imageFile = "/home/jeff/Dropbox/Astronomy/m31_cropped.fits"
    ):
    print "Starting with arguments:\n"
    print "    executable: " + executable + "\n"
    print "    configFile: " + configFile + "\n"
    print "    port: " + str(port) + "\n"
    print "    htmlFile: " + htmlFile + "\n"
    print "    imageFile: " + imageFile + "\n"
    return Application(executable, configFile, port, htmlFile, imageFile)
