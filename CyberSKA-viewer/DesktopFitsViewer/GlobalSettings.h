/**
 * Encapsulated global settings.
 *
 * The current implementation uses ~/.sfviewer.config file to store global settings.
 *
 **/

#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QString>

class GlobalSettings
{
public:

    /// returns debug
    static bool isDebug();
    /// returns defaultImage
    static QString defaultImage();

    struct VizMan {
        /// returns vizMan.timeout
        static double timeout();
        /// returns vizMan.urlPattern
        static QString urlPattern();
    };

protected:
    /// protected undefined constructor to prevent instantiation
    explicit GlobalSettings();
    
};

#endif // GLOBALSETTINGS_H
