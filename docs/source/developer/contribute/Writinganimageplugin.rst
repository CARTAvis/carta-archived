Writing an image plugin for CARTA in C++
========================================

The purpose of this document is to describe how to write a C++ plugin that allows opening images of new file formats in *CARTA*.

    1. Setting up build environment
    2. Writing the plugin
    3. Testing the plugin

While reading this guide, please refer to appendices B, C and D, which contain some general
information about the plugin architecture in *CARTA*.


Setting up a build environment
==============================

At the moment we do not have an easy mechanism to set up a build environment for just compiling and testing plugins.
That means for now you will need to set up your computer to build all of *CARTA*.
In the future we hope to have a simplified setup just to be able to build plugins.
Please follow the instructions in Appendix A on how to setup your machine to be able to compile *CARTA* and your plugin.


Writing the plugin
==================


The easiest way to write a new plugin for CARTA is to take an existing one, clone it and modify it to do what you need.
For adding a new image format the best plugin to clone is the qimage plugin, e.g.::

    > cd /tmp/xxx/CARTAvis/carta/cpp/plugins
    > cp -a qimage myimage
    > cd myimage
    > mv qimage.pro myimage.pro
    … rename the header/sources as you see fit
    > emacs plugin.json
    … fix up entries to match your plugin
    > emacs ../plugins.pro
    … add myimage to the list of plugins
    > cd ..
    > make

At this point you should have a functional plugin which does exactly the same thing qimage plugin does.
It is now time to modify the sources to make it do what you want.
The best way to go about that is to understand what the existing code does.


The following interface classes are of particular interest, as these are the ones you may need to implement for
your plugin to work. They should have sufficient in-code documentation to get you started::

    IPlugin
    Carta::Lib::NdArray::RawViewInterface
    CoordinateFormatterInterface
    Carta::Lib::Image::MetaDataInterface
    Carta::Lib::Image::ImageInterface
    Carta::Core::Hooks::LoadAstroImage


The main class (the one that implements the plugin), looks like this (:file:`qimage/QImagePlugin.h`)::

    /// This plugin can read image formats that Qt supports.
    #pragma once
    #include "CartaLib/IPlugin.h"
    #include <QObject>
    #include <QString>
    class QImagePlugin : public QObject, public IPlugin
    {
       Q_OBJECT
       Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
       Q_INTERFACES( IPlugin)
    public:
       QImagePlugin(QObject *parent = 0);
       virtual bool handleHook(BaseHook & hookData) override;
       virtual std::vector<HookId> getInitialHookList() override;
    };


Study the implementation of these methods (:file:`qimage/QImagePlugin.cpp`).


Appendix A : setting up the build environment
=============================================

Here we try to explain how to setup your machine to be able to build CARTA from sources.
You will most likely need to install some prerequisites first, then download the CARTA sources and compile them.

CARTA prerequisites
-------------------

Operating system
~~~~~~~~~~~~~~~~
We highly recommend you do your development on Ubuntu 14.04 LTS 64-bit.
The instructions in this document are written for this OS.

Compiler
~~~~~~~~

You will need g++ version 4.8.1 or newer. If you have not installed it already, you can::

    > sudo apt-get install g++

Qt
~~

Make sure you have qt installed, at least 5.3. For example::

    > sudo apt-get install qtbase5-dev


Casacore
~~~~~~~~

You will need casacore 2.0.1 or newer to be able to compile all of CARTA.
This can be a lengthy procedure, please read the documentation here:

    https://github.com/casacore/casacore

CARTA’s core does not actually depend on casacore, only some of the bundled plugins do.
Therefore it’s possible to adjust the build process so that you don’t need to install casacore at all.
This is done by disabling the bundled plugins that require casacore:

    + WcsPlotter
    + Histogram
    + CasaImageLoader
    + CasaCore-2.0.01

You can disable them by editing plugins.pro and removing the corresponding entries for these plugins.

Python
~~~~~~

If you want python plugins to work, you will need to install python dev files::

    > sudo apt-get install libpython2.7-dev

You can disable python support by disabling the python273 plugin (you have to edit plugins.pro).

QWT
~~~

You will need libqwt version 6.1.2 or above.
You need to make sure that the qwt you use is compiled against the same version of Qt that you will be using!
This is very important. You can get installation instructions here:

    http://qwt.sourceforge.net/

Qooxdoo
~~~~~~~

You need to install qooxdoo version 3.5.1 from:

    http://qooxdoo.org/downloads

Please don’t install a newer version, as it will likely not work properly.
At the time of writing this document, the following link works:

    http://sourceforge.net/projects/qooxdoo/files/qooxdoo-current/3.5.1/qooxdoo-3.5.1-sdk.zip/download

Download  CARTA sources:
~~~~~~~~~~~~~~~~~~~~~~~~

Next you need to download the sources for CARTA from github.
We recommend you create an empty directory and do all your work there.
For example below we create :file:`/tmp/xxx`::

    > mkdir /tmp/xxx
    > cd /tmp/xxx
    > github clone git@github.com:Astroua/CARTAvis.git
      or use Pavol’s branch:
    > github clone git@github.com:pfederl/CARTAvis.git


Next you need to configure CARTA.

    * create ../CARTAvis-externals and setup links
    * edit common_config.pri

Compile CARTA::

    > cd /tmp/xxx
    > mkdir build
    > cd build
    > qmake CARTA_BUILD_TYPE=dev /tmp/xxx/CARTAvis/carta/carta.pro

Run qooxdoo compiler::

    > cd /tmp/xxx/CARTAvis/carta/html5/common/skel/
    > ./generate.py

Compile C++::

    > cd /tmp/xxx/build
    > make

Try to run the desktop application::

    > ./cpp/desktop/desktop --html /tmp/xxx/CARTAvis/carta/VFS/DesktopDevel/desktop/desktopIndex.html /tmp/some.fits

Appendix B : how plugins work in CARTA
======================================

The main purpose of a plugin is to extend some aspect of the core viewer functionality.
For example, the core could implement some basic colormaps, and a plugin could be used to add additional ones.
Or the core could support FITS and CASA input image formats, while a plugin could add support for HDF5.
A plugin could also be used to replace/override existing functionality, for example changing the
formatting or the type of information present in the status bar.
The support for a particular extension will need to be coded into the core.
To illustrate how this might work, consider the pseudo code that loads an image from a file.
Below is what the implementation might look like without any plugin support::

    IImage * loadImage( const string & fileName):
        IImage * result = loadFITSorCASA( fileName);
        if( result) return result;
        reportError( "Could not load image...");
        return nullptr; // or throw exception
    }


The above code handles only FITS and CASA images.
If neither can be used, the core gives up and reports an error to the user.
To add support for additional image formats, we could augment the above code like this::

    IImage * loadImage( const string & fileName):
        IImage * result = loadFITSorCASA( fileName);
        if( result) return result;
        // core could not handle this format, let's see if one of the plugins can
        for( auto plugin : allPlugins_that_implement_loadImage) {
            result = plugin-> loadImage( fileName);
            if( result) return result;
        }
        reportError( "Could not load image...");
        return nullptr; // or throw exception
    }

By adding couple of lines of (pseudo) code, we can now extend supported image formats using plugins.
The plugin method 'loadImage' is something I like to refer as a hook (vaguely motivated by http://en.wikipedia.org/wiki/Hooking).
Hooks are strategically placed pieces of code throughout the core, which call one ore more plugins implementing that particular hook.
Some hooks are only called once (for example, there is a hook that is executed when the viewer starts, and another when the viewer exits).
Other hooks are executed multiple times (e.g. user is trying to load an image with an unknown format).
Some hooks have input parameters, others don't.
Some hooks have results, some don't. Some hooks are always executed by all plugins that implement them.
Some hooks are only executed by the first plugin that returns a valid result.


Since the core of the viewer is written in C++, we decided we'll support plugins written in C++ (for maximum performance).
Each plugin is essentially a C++ code compiled into a shared library.
We use Qt's lower-level plugin mechanism to gain some platform independence (http://qt-project.org/doc/qt-5/plugins-howto.html).
Each plugin implements roughly the following interface::

    class IPlugin {
    public:
       virtual std::vector<HookId> getInitialHookList() = 0;
       virtual bool handleHook( BaseHook & hookData) = 0;
    };

After a plugin is loaded, it's *'getInitialHookList()'* method is invoked, which tells the core the
list of 'hooks' the plugin implements.
This is used as an optimization technique, so that we don't call every single plugin for every hook.
The second method *'handleHook()'* is called when the core is executing a hook.
The name of the hook, it's parameters and the result are all encoded in the single parameter *'hookData'* of an abstract type BaseHook.
Each hook in the core is essentially a subclass of BaseHook.
The BaseHook class is very simple::

    class BaseHook {
    public:
       explicit BaseHook( const HookId & id) : m_hookId(id) {}
       /// dynamic ID
       HookId hookId() const { return m_hookId; }
    protected:
       HookId m_hookId;
    };

The only thing of note here is the HookId type, which is currently an integer.
We use these IDs to quickly decide whether we want to do downcast to the proper hook type.


And here is an example of how the loadImage hook might be implemented::

    class LoadImageHook : public BaseHook {
    public:
       typedef IImage * ResultType;
       struct Params {
           Params( QString p_fileName) {
               fileName = p_fileName;
           }
           QString fileName;
       };
       enum { StaticHookId = 5 };
       LoadImage(Params * pptr) : BaseHook( StaticHookId), paramsPtr( pptr) {}
       ResultType result;
       Params * paramsPtr;
    };

Here is pseudo-code for a C++ plugin that implements only the loadImage hook::

    class QImagePlugin : public IPlugin {
       virtual std::vector<HookId> getInitialHookList() override;
       virtual bool handleHook(BaseHook & hookData) override;
    };
    std::vector<HookId> QImagePlugin::getInitialHookList() {
        return { LoadImageHook::StaticHookId };
    }
    bool MyPlugin::handleHook(BaseHook & hookData) {
       if( hookData.hookId() == LoadImageHook::StaticHookId) {
           LoadImageHook & hook = static_cast<LoadImageHook &>( hookData);
           hook.result = readSomeFileFormatForExampleHDF5( hook.paramsPtr->fileName);
           return hook.result != nullptr;
       }
       warning << "Sorry, don't know how to handle this hook" << hookData.hookId();
       return false;
    }

The plugins will be loaded and initialized in the same process/thread as the core viewer.
All communication (via handleHook) will also be called from the same thread.
If they want, plugins can spawn additional threads or processes, but then it's
up to them to set up synchronization with the core thread.
The core won't provide any such functionality.


At startup all subdirectories of the main ‘plugins’ directory are scanned (by reading and parsing the plugin.json files).
All plugins are compiled into a list.
The list is then topologically sorted using the specified dependencies. Then each plugin from the sorted list is processed:

  + we attempt to load the plugin as native (type=c++ | lib)
     + first we load all libraries under plugin’s /lib/ directory (matching `*.so` and `*.so.*` patterns)
     + the loading order of these is determined heuristically, by repeating these steps:
        + if the list of libraries to load is empty, break out of this loop
        + try the libraries on the list that have not been loaded and load them
        + if a library was loaded, mark it as loaded
        + if any library was loaded, continue the loop
        + if no library was loaded, break out indicating a warning
     + if the plugin’s type is c++, we load plugin.so, or report error
  + if the plugin was not loaded as native, we attempt to load it using loadPlugin hook, i.e. using some other plugin
  + if the plugin was loaded (either as native or foreign), we execute it’s initialize method


Appendix C : plugin structure
=============================

Each carta plugin lives in its own directory, with the following contents:

.. csv-table::
   :widths: 10, 10, 35

   "``plugin.json``", "required file", "meta-information about the plugin"
   "``libplugin.so``", "optional file", "the actual C++ plugin"
   "``libs/``", "optional subdirectory", "directory of libraries the plugin provides and/or uses"
   "``libs/lib1.so ...``", "optional files", "the actual libraries the plugin provides/uses. |br|
   These can be organized in additional subdirectories. |br|
   Any .so file will be treated as a provided library |br| and will be loaded (if possible)."
   "other files or subdirectories", "optional", "anything else the plugin needs?"

.. |br| raw:: html

   <br />


Appendix D : plugin.json
========================

.. csv-table::
   :widths: 10, 7, 35

   "api:", "required integer", "which plugin-API does this plugin use"
   "name:", "required string", "Unique name by which the plugin will be identified. |br|
   It is a good idea to name the plugin’s directory the same |br| as this name."
   "version:", "required string", "version as a string in x.y.z format"
   "type:", "required string", "type of plugin, currently supported types are: “C++” |br|  and “libs”. Other types can be added via plugins |br| (e.g. “python”)."
   "depends:", "optional array of strings", "array of other plugins that must be present for this |br| plugin to work"
   "description:", "optional (ascii string)", "short description of the plugin’s functionality"
   "about:", "optional (html string)", "any other info about the plugin (e.g. home page, authors, |br| organization, etc)"
   "provides: |br| not implemented yet", "string list", "what does the plugin provide, can be used |br| as dependencies"
   "other:", "json object", "configuration options for the plugin, not parsed by core. |br|  The plugin needs to parse it itself"

Example plugin.json file::

    {
       "api"        : "1",
       "name"       : "qimage",
       "version"    : "1",
       "type"       : "C++",
       "description": "Adds ability to load jpeg/png images. (anything Qt supports)",
       "about"      : "Part of carta. Written by Pavol",
       "depends"    : [ ]
    }


Appendix E: CARTA config file
=============================

The configuration file is used to change some settings that are installation wide,
for example the search paths for plugins. The location of the main configuration file is by default in:

:file:`$HOME/.cartavis/config.json`

This location can be changed in two ways:

  * via a command line switch passed to the executable, e.g.

    ``--config /tmp/myconfig.json``

    note that for the server-side viz this would require modifying
    the appropriate plugin.xml file in the PureWeb installation

  * via an environment variable called CARTAVIS_CONFIG_FILE, e.g:

    ``setenv CARTAVIS_CONFIG=/tmp/myconfig.json``

If both options are used, the command line takes precedence.

What’s in the config file?
For example search paths for plugins::

    {
        "pluginDirs": [
            "$(APPDIR)/../plugins",
            "$(HOME)/.cartavis/plugins"
        ], ...
    }

The above config.json tells carta to look for plugins in two places:

  * relative to the executable
  * relative to the user’s home directory

You can also disable some plugins::

    {
        "disabledPlugins" : [
            "ColormapsPy",
            "Colormaps1"
        ],
    }

Or provide particular plugins some settings::

    {
        "plugins": {
            "CyberSKA": {
                "vizmanUrl1": "http://localhost:1234/testDir/%%",
                "vizmanUrl2": "http://localhost:8080/test.txt"
            },
            "DevIntegration" : {
                "enabled" : true
            }
        },
    }

Appendix F: libraries & versions
================================

Here is a list of libraries and their versions that we currently use in development:

.. csv-table::
   :widths: 10, 10

   "qooxdoo", "3.5"
   "qwt", "6.1.2"
   "qt", "5.3 and 5.4"
   "ast lib", "8.0.2"
   "casacore", "2.0.1"
   "pureweb", "4.1.1"
   "wcslib", "4.23"
   "cfitsio", "3360"
   "rapidjson", "from github:|br|
   https://github.com/drewnoakes/rapidjson/commit/8307f0f4c9035bd63853bdf9e1b951e8c0e37b62"
