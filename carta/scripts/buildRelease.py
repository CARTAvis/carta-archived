import os
import json
import sys
import readline

defaultSettings = {
    "version" : "0.0.0",
    "destination" : "/tmp/cartaBuild-0.0.0",
    "qmakebin" : "/usr/bin/qmake",
    "makeflags" : [ "-j12" ]

}


def rlinput(prompt, prefill=''):
    """kind of like ras_input but with readline support and pre-filled answer"""
    readline.set_startup_hook(lambda: readline.insert_text(prefill))
    try:
        return raw_input(prompt)
    finally:
        readline.set_startup_hook()

def checkMkPath( path):
    try:
        os.mkdir(path)
    except Exception:
        pass
    return os.path.isdir(path)

def getJsonPath( srcRoot):
    return srcRoot + "/carta/scripts/lastBuild.json"

def saveSettings( srcRoot, settings):
    """Save settings as json"""
    jsonPath = getJsonPath(srcRoot)
    try:
        jsonFile = open(jsonPath, "w+")
        jsonFile.write(json.dumps(settings,
                                  sort_keys=True,
                                  indent=4,
                                  separators=(',', ': ')))
        jsonFile.close()
        return
    except Exception, e:
        print str(e)
    except:
        print "Could not save settings!!!"
        print "Unexpected error:", sys.exc_info()[0]

    sys.exit( "Aborting")


def getLastSettings(srcRoot):
    """Retrieve last build settings from json file"""
    jsonPath = getJsonPath(srcRoot)
    try:
        jsonFile = open( jsonPath, "r")
        data = json.load(jsonFile)
        jsonFile.close()
        settings = defaultSettings.copy()
        settings.update( data)
        return settings
    except:
        print "Previous build settings not found!"

    saveSettings( srcRoot, defaultSettings)

# figure out root of sources
srcRoot = os.path.dirname( os.path.abspath(__file__))
srcRoot = os.path.abspath( srcRoot)
srcRoot = os.path.abspath( srcRoot + "/../..")

print "srcRoot", srcRoot

# get the last settings
settings = getLastSettings( srcRoot)
print settings

# ask user if this is ok
while True:
    print "Current settings (from", getJsonPath(srcRoot), "):"
    print "  1) Version:" , settings["version"]
    print "  2) Destination:" , settings["destination"]
    print "  3) qmake:" , settings["qmakebin"]
    print "   ) makeflagse:" , " ".join(settings["makeflags"])
    print "  0) Go"

    try:
        choice = int(raw_input("Choice:"))
    except ValueError:
        print "Invalid!"
        continue

    if choice == 1:
        settings["version"] = rlinput( "New version: ", settings["version"])
    elif choice == 2:
        settings["destination"] = rlinput( "New destination: ", settings["destination"])
    elif choice == 3:
        settings["qmakebin"] = rlinput( "New qmakebin: ", settings["qmakebin"])
    elif choice == 0:
        break
    else:
        print "Invalid!"

# save settings in case we modified them
saveSettings( srcRoot, settings)

# prepare the structure of the destination
checkMkPath( settings["destination"])

# change directory to destination
os.chdir( settings["destination"])

# invoke qmake in destination directory
from subprocess import call
call([ settings["qmakebin"], "CARTA_BUILD_TYPE=release","-r", srcRoot + "/carta/carta.pro"])

# invoke make on c++ code
makeArgs = settings["makeflags"][:]
makeArgs.insert( 0, "make")
call( makeArgs)

# invoke qooxdoo
os.chdir( srcRoot + "/carta/html5/common/skel")
qooxdooCall = [ "./generate.py" ]
qooxdooCall.append( "-m")
qooxdooCall.append( "BUILD_PATH:" + settings["destination"] + "/html")
qooxdooCall.append( "build")
call( qooxdooCall)

# copy serverIndex release mode
import shutil
shutil.copy( srcRoot + "/carta/html5/desktop/desktopIndexRelease.html",
             settings["destination"] + "/html")
shutil.copy( srcRoot + "/carta/html5/server/serverIndexRelease.html",
             settings["destination"] + "/html")
shutil.copy( srcRoot + "/carta/html5/common/libs.js",
             settings["destination"] + "/html")
shutil.copy( srcRoot + "/carta/html5/common/CallbackList.js",
             settings["destination"] + "/html")
shutil.copy( srcRoot + "/carta/html5/desktop/desktopConnector.js",
             settings["destination"] + "/html")
shutil.copy( srcRoot + "/carta/html5/server/pureweb.min.js",
             settings["destination"] + "/html")
shutil.copy( srcRoot + "/carta/html5/server/serverConnector.js",
             settings["destination"] + "/html")



print "Done"