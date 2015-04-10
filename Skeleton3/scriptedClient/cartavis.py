#!/usr/bin/env python
# -*- coding: utf-8 -*-

from tagconnector import TagConnector
from application import Application
from statistics import Statistics
from cartaview import CartaView
from histogram import Histogram
from animator import Animator
from colormap import Colormap
from image import Image

def start(
        executable = "/home/jeff/scratch/build/cpp/desktop/desktop", 
        configFile = "/home/jeff/.cartavis/config.json", 
        port = 9999, 
        htmlFile = "/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html", 
        imageFile = "/scratch/Images/m42_40min_ir.fits"
    ):
    return Application(executable, configFile, port, htmlFile, imageFile)

def startPavol(
        executable = "/scratch/builds/Skeleton3/Dev/cpp/desktop/desktop", 
        configFile = "/home/pfederl/.cartavis/config.json", 
        port = 9999, 
        htmlFile = "/home/pfederl/Work/ALMAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html", 
        imageFile = "/scratch/Images/smallcube.fits"
    ):
    return Application(executable, configFile, port, htmlFile, imageFile)

def parseDirectory(directory, prefix):
    """ This is not working the way I want it to yet.
        Since it is only used for getFileList(), which in turn only lists the
        files in /scratch/Images, perhaps getFileList() can just be scrapped?
        The scripted client can load files from anywhere using
        loadLocalFile().
    """
    print "prefix = " + prefix
    fileList = []
    for i in range (0, len(directory)):
        print "directory[" + str(i) + "] = " + str(directory[i])
        name = directory[i]['name']
        print "name = " + name
        try:
            if directory[i]['dir'] != "":
                print "     This should be a subdirectory"
                subdirectory = directory[i]['dir']
                print "subdirectory = " + str(subdirectory)
                subFileList = parseDirectory(subdirectory, prefix + "/" + name)
                print "subFileList = " + str(subFileList)
                fileList.append(subFileList)
                print "fileList = " + str(fileList)
        except (KeyError):
            "KeyError"
            if prefix != "":
                name = prefix + "/" + name
            print "Appending " + name
            fileList.append(name)
            print "fileList = " + str(fileList)
    return fileList
