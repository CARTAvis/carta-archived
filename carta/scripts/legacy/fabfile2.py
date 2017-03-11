#!/usr/bin/python

# intended usage:
# 1) cd into build directory
#    cd /scratch/builds/carta/Dev/
# 2) run the deploy script from the source directory
#    python -m fabric -f ~/Work/CARTAvis/carta/scripts/fabfile2.py deploy

from __future__ import with_statement
from fabric.api import *
from fabric.contrib.console import confirm
import os, os.path
from fabric.contrib.files import exists
from fabric.contrib.project import rsync_project

env.user = "calsci"
env.hosts = ['almatest.cyberska.org']
pureWebRoot = "PureWeb/4.1.1"
remoteDeployDir = "PavolDeploy2"
localSourceDir = "/home/pfederl/Work/CARTAvis"

if localSourceDir.endswith("/"):
    abort("Please don't terminate localSourceDir with a slash")

def checkLocalDirectory():
    puts("checking local directory")
    ok = True
    ok = ok and os.path.isfile("Makefile")
    ok = ok and os.path.isfile("cpp/server/server")
    if not ok:
        abort("Please run me from the build directory")
    ok = ok and os.path.isdir(localSourceDir)
    if not ok:
        abort("Cannot find sources in: " + localSourceDir)
    puts("OK")

def makeClean():
    puts("building...")
    local("make -j4", capture=True)
    puts("OK")

def checkServerUp():
    puts("Checking if server is reachable")
    with settings(warn_only=True),cd(pureWebRoot):
        result = run('pwd')
    if result.failed:
        abort("Server is not reachable or PureWeb directory not found.")
    puts("OK")

def checkDeployDir():
    puts("Checking if deploy directory exists")
    if not exists(remoteDeployDir,verbose=True):
        abort("Deploy directory cannot be found on server: " + remoteDeployDir)
    puts("OK")


def uploadBuildFiles():
    puts("Uploading build files")
    rsync_project(
        local_dir=".",
        remote_dir=remoteDeployDir+"/build",
        extra_opts="--links --delete")
    puts("OK")
    return

def uploadHtmlFiles():
    puts("Uploading html files")
    rsync_project(
        local_dir=localSourceDir+"/",
        remote_dir=remoteDeployDir+"/sources",
        extra_opts="--links")
    puts("OK")
    return

def fixQooxdooSymlink():
    puts("Fixing qooxdoo symlink on remote host")
    with cd(remoteDeployDir+"/sources/carta/html5/common"):
        run("ln -nfs /home/calsci/qooxdoo-3.5-sdk .")
    puts("OK")
    return

@task
def deploy():
    checkLocalDirectory()
    makeClean()
    puts("Executing on %(host)s as %(user)s" % env)
    puts("Destination: %s" % remoteDeployDir )
    if not confirm("You are about to deploy to %(host)s. Continue?" % env,default=False):
        abort("Ok, aborting")
    checkServerUp()
    checkDeployDir()
    uploadBuildFiles()
    uploadHtmlFiles()
    fixQooxdooSymlink()

