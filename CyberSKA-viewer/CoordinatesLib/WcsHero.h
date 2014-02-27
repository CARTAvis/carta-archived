/**
  * The purpose of this class is to provide WCS related algorithms to the rest of the modules
  *
  * - format coordinates (pixel to world)
  * - convert coordinates to different coordinate systems (eg fk5 -> galactic)
  * - format units
  * - provide appropriate labels for units/coordinates
  *
 **/

#pragma once

#ifndef WCSHERO_H
#define WCSHERO_H

#include <vector>
#include <memory>
#include <QString>
#include <QStringList>

namespace WcsHero {

class AxisInfo
{
public:
    QString label, unit, symbol, htmlLabel, htmlUnit, htmlSymbol, title, htmlTitle;

    bool isSky() const {
        return isLongitude() || isLatitude();
    }

    AxisInfo() {
        m_isLatitude = false;
        m_isLongitude = false;
    }

    bool isLatitude() const
    {
        return m_isLatitude;
    }
    void setIsLatitude(bool flag)
    {
        m_isLatitude = flag;
    }

    bool isLongitude() const
    {
        return m_isLongitude;
    }
    void setIsLongitude(bool flag)
    {
        m_isLongitude = flag;
    }

protected:
    bool m_isLatitude;
    bool m_isLongitude;

};

class Hero
{
public:

    typedef std::shared_ptr< Hero > SharedPtr;

    /// supported coordinate systems
    enum class SKYCS { FK5 = 0, FK4, FK4_NO_E, J2000, ICRS, GALACTIC, ECLIPTIC,
                       GAPPT, AZEL, HELIOECLIPTIC, SUPERGALACTIC, UNKNOWN,
                       PIXEL };

    /// create an instance from a set of headers
//    static Hero * constructFromFitsHeader( const QStringList & hdr);

    /// create an instance from a local fits file
    static Hero * constructFromFitsFile( const QString & fname);

    /// get the current sky coordinate system
    /// (after construction this is the default CS for the loaded file/header/etc)
    SKYCS getCurentSkyCS();

    /// get a list of coordinate systems that the loaded file can be converted to
    std::vector<SKYCS> getConvertibleSkyCS();

    /// get a list of all known coordinate systems
    static const std::vector<SKYCS> & getAllKnownCS();

    /// set a different sky coordinate system, return true if success, false if
    /// this could not be accomplished
    bool setSkyCS( SKYCS skyCS);

    /// set the original sky coordinate system
    void resetSkyCS();

    /// get more verbose description of the sky cs

    /// convert FITS pixel coordinate to world coordinates (using current SKYCS)
    /// pixel (1,1,...,1) is the center of the left-bottom most pixel (i.e. we are
    /// using the FITS coordinate system). (0.5,0.5,...) is the bottom-left corner
    /// of the bottom-left pixel... And of course top right corner of the bootom-left
    /// pixel is (1.5,1.5,...)
    /// On success, the result will contain the converted coordinates.
    /// On failure, getErrors() will contain textual error
    const std::vector<double> & fits2world( const std::vector<double> & pixel);

    /// format cursor specified in FITS coordinates
    /// if coords.size() < naxes, it'll be padded with '1.0';
    QStringList formatPixelCursor( const std::vector<double> & coords, bool normalize = true);

    /// format cursor specified in world coordinates
    /// assumes the coords have been processed using fits2world()
    /// normalize flag turns on/off calling astNorm() before astFormat()
    QStringList formatWorldCursor( const std::vector<double> & coords, bool normalize = true);

    /// formats a single value in world coordinates, the axis is specified using whichAxis
    /// whichAxis is 0-based
    QString formatWorldValue( double val, int whichAxis);

    /// calculate distance between two coordinates (FITS-1 based)
    /// if not sized properly, c1 and c2 will be internally padded by '1.0'
    double distance( const std::vector<double> & c1, const std::vector<double> &c2);

    /// get appropriate labels for the coordinates
    const std::vector<AxisInfo> & axesInfos();

    /// get a string name for the skycs
    static QString skycs2string( const SKYCS & skycs);

    /// convert string to skycs
    static SKYCS string2skycs(const QString & str);

    /// return the naxes as parsed by astlib (NAXIS)
    int naxes() const;

    /// return the unit for values (BUNIT)
    const QString & bunit() const { return m_bunit; }

    /// return a nicer BUNIT suitable for html
    const QString & htmlBunit() const { return m_bunitNiceHtml; }

    /// get the list of current errors (but don't clear them)
    const QStringList & errors();

    /// clear errors
    void clearErrors();

    /// add an error
    void addError( const QString & str);

    /// destructor
    ~Hero();

protected:

    /// protected constructor
    Hero();

    /// list of errors
    QStringList m_errors;

    /// current sky system
    SKYCS m_currentSkyCs;

    /// original sky system
    SKYCS m_originalSkyCs;

    /// labels for axes (cached)
    QStringList m_labelsForAxes;

    /// axis information (cached)
    std::vector< AxisInfo > m_axisInfos;

    /// storage for converted pixels to wcs
    std::vector<double> m_pixel2wcsCache;

    /// parse axes info from current ast frame
    void parseAxesInfo();

    /// various AST pointers groupped together
    struct {
        /// astlib pointer to the original frameset
        void * origWcsInfo;
        /// astlib pointer to the current frameset
        void * currWcsInfo;
        /// number of axes as parsed by ast
        int naxes;
    } m_ast;

    /// raw BUNIT value
    QString m_bunit;

    /// nice (html-ized) BUNIT
    QString m_bunitNiceHtml;

};

}

#endif // WCSHERO_H
