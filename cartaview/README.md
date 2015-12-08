# Prerequisites

## Install the CARTA scripted client

In order to run the `cartaview()` task, the CARTA scripted client needs to be installed so that it can be imported from anywhere, not just the scriptedClient directory.

Starting from the scriptedClient directory, run:

```bash
python setup.py sdist
```

Change to the dist directory and untar the file in it:

```bash
tar -xvf cartavis-alpha.tar.gz
```

Change to the resulting directory and run:

```bash
sudo python setup.py install
```

To test that this has worked, run Python from a directory outside of the CARTA installation and try to import cartavis:

```python
import carta.cartavis
```

If this does not generate any error messages, then the CARTA scripted client has been installed properly on your system and can be imported from anywhere.

## The scriptedConfig.json file

The `scriptedConfig.json` file contains entries which will point the `cartaview()` to the CARTA installation. These entries are:

- "port": this can usually be set to "9999" without any problems
- "executable": the full path of the CARTA executable file
- "htmlFile": the full path of the CARTA desktopIndex.html file

A sample scriptedConfig.json file looks like this:

```json
{
    "port" : "9999",                                                            
    "executable" : "$(HOME)/scratch/build/dev/cpp/desktop/desktop",          
    "htmlFile" : "$(HOME)/dev/CARTAvis/carta/html5/desktop/desktopIndex.html"
}              
```

Note that the name of this file does not actually need to be `scriptedConfig.json` - it can have any name as long as it contains the appropriate entries and the `cartaview()` task knows where to find it.

# Placing the files

The `cartaview()` task consists of two files: `cartaview.xml` and `task_cartaview.py`. Both files need to be placed in their proper locations within the CASA installation. These locations can vary depending on the installation. For example, if CASA is installed in `~/software/casa/`, then the XML and Python files may belong in `~/software/casa/xml/` and `~/software/casa/lib/python2.7`.

Instructions for finding the exact locations for these files in any installation of CASA can be found here: [Exploring the built-in CASA tasks](https://casaguides.nrao.edu/index.php?title=Writing_a_CASA_Task#Exploring_the_built-in_CASA_tasks).

# Building the task

Instructions for building a CASA task can be found here: [Building the task](https://casaguides.nrao.edu/index.php?title=Writing_a_CASA_Task#Building_the_task). Note that if the `cartaview()` task is the only one you need to build, you can save a lot of time by passing an argument to `buildmytasks`; e.g.:

```python
os.system('buildmytasks cartaview')
```

# Running the task

After the task has been built, it can be run like any other CASA task. For example:

```python
# In CASA
tget cartaview
inp
raster = '~/CARTA/Images/mexinputtest.fits'
configFile = '~/.cartavis/config.json'
go
```

To gain access to the scripted client from within CASA, `cartaview()` can be invoked like this (as an alternative to the `go` command):

```python
v = cartaview()
```
