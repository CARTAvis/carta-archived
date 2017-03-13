import os
import json
import sys
import readline
from subprocess import call

defaultSettings = {
    "version" : "0.0.0",
    "destination" : "/tmp/cartaBuild-0.0.0",
    "qmakebin" : "/usr/bin/qmake",
    "makeflags" : [ "-j12" ],
    "qooxdooExtraOptions" : [ "-m", "OPTIMIZE:[]" ]

}

def writeQrc(resources=[], prefix = "/", qrcfname = "file.qrc"):
    """
    Write to the qrc file under the prefix specified
    """
    with open('%s'%qrcfname,'w') as f:
        f.write('<RCC>\n  <qresource prefix="%s">\n'%prefix)
        for r in resources:
            f.write('    <file>%s</file>\n'%r)
        f.write('  </qresource>\n</RCC>\n')


def scanDir(directory):
    """
    Scan tree starting from direc
    """
    resources = []
    for path, dirs, files in os.walk(directory):
        resources += [os.path.join(path,f) for f in files]
    return resources


def rlinput(prompt, prefill=''):
    """kind of like ras_input but with readline support and pre-filled answer"""
    readline.set_startup_hook(lambda: readline.insert_text(prefill))
    try:
        return raw_input(prompt)
    finally:
        readline.set_startup_hook()

def checkMkPathBad( path):
    try:
        os.mkdir(path)
    except Exception:
        pass
    return os.path.isdir(path)

def checkMkPath(path):
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno == os.errno.EEXIST and os.path.isdir(path):
            pass
        else: raise


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
    print "   ) makeflags:" , " ".join(settings["makeflags"])
    print "   ) qooxdoo extra options:" , " ".join(settings["qooxdooExtraOptions"])
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
checkMkPath( settings["destination"] + "/cpp/desktop")

# invoke qooxdoo (we chdir to sources, but the output will be in destination...)
os.chdir( srcRoot + "/carta/html5/common/skel")
qooxdooCall = [ "./generate.py" ]
qooxdooCall.append( "-m")
qooxdooCall.append( "BUILD_PATH:" + settings["destination"] + "/html")
qooxdooCall.extend( settings["qooxdooExtraOptions"])
qooxdooCall.append( "build")
call( qooxdooCall)

# copy other html/javascript files
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

# make a symlink inside cpp/desktop to html, so that we can create a working qrc
if os.path.exists( settings["destination"] + "/cpp/desktop/html"):
    os.unlink( settings["destination"] + "/cpp/desktop/html")
os.symlink( "../../html", settings["destination"] + "/cpp/desktop/html")

# create qrc in the desktop directory
os.chdir( settings["destination"] + "/cpp/desktop")
qrcfiles = []
qrcfiles.append( "html/desktopIndexRelease.html")
qrcfiles.append( "html/libs.js")
qrcfiles.append( "html/CallbackList.js")
qrcfiles.append( "html/desktopConnector.js")
qrcfiles += scanDir( "html/resource")
qrcfiles += scanDir( "html/script")
writeQrc( qrcfiles, "/", "files.qrc")

# change directory to destination
os.chdir( settings["destination"])

# invoke qmake in destination directory
call([ settings["qmakebin"], "CARTA_BUILD_TYPE=release","-r",
    "CONFIG+=carta_qrc",
    srcRoot + "/carta/carta.pro"])

# invoke make on c++ code
makeArgs = settings["makeflags"][:]
makeArgs.insert( 0, "make")
call( makeArgs)

# make some symlinks to executables
try:
    os.symlink( "cpp/desktop/desktop",  settings["destination"] + "/cartaviewer")
except:
    pass

try:
    os.symlink( "cpp/server/server",  settings["destination"] + "/cartaserver")
except:
    pass

print "Done"
