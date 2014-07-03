#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import socket
import time

lastPort = 12345

class Application:
    """Represents an application"""

    def __init__(self):
        global lastPort
        print "lastPort = ", lastPort
        args = ["/scratch/builds/Skeleton3-qt5.3-Release/cpp/desktop/desktop",
            "--config",
            "/home/pfederl/.cartavis/config.json",
            "--scriptPort",
            str(lastPort),
            "--html",
            "/home/pfederl/Work/ALMAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html",
            "/scratch/small.fits"]
        self.popen = subprocess.Popen( args)
        print "Started process with pid=", self.popen.pid
        time.sleep( 3)
        self.visible = False
        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("localhost", lastPort))
        lastPort = lastPort + 1;
        return

    def setGuiVisible(self,flag):
        self.visible = flag
        return

    def isGuiVisible(self):
        return self.visible

    def kill(self):
        self.popen.kill()

    def openFile(self,fileName):
        self.socket.sendall("load " + fileName)



def start():
    return Application()


