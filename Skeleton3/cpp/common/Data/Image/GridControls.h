/***
 * Entry point for clients wishing to change grid settings.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {

class DataGrid;

class GridControls : public QObject, public Carta::State::CartaObject{

    Q_OBJECT

public:
    /**
     * Set the grid axes color.
     * @param redAmount - an integer in [0, 255] indicating the amount of red.
     * @param greenAmount  an integer in [0,255] indicating the amount of green.
     * @param blueAmount - an integer in [0,255] indicating the amount of blue.
     * @return a list of errors or an empty list if the color was successfully set.
     */
    QStringList setAxesColor( int redAmount, int greenAmount, int blueAmount );

    /**
     * Set the grid coordinate system.
     * @param coordSystem - an identifier for a grid coordinate system.
     * @return an error message if there was a problem setting the coordinate system;
     *  an empty string otherwise.
     */
    QString setCoordinateSystem( const QString& coordSystem );

    /**
     * Set the font family used for grid labels.
     * @param fontFamily - an identifier for a font family.
     * @return an error message if there was a problem setting the font family;
     *  an empty string otherwise.
     */
    QString setFontFamily( const QString& fontFamily );

    /**
     * Set the font size used for grid labels.
     * @param fontSize - an identifier for a font point size.
     * @return an error message if there was a problem setting the font point size;
     *  an empty string otherwise.
     */
    QString setFontSize( int fontSize );

    /**
     * Set the grid color.
     * @param redAmount - an integer in [0, 255] indicating the amount of red.
     * @param greenAmount  an integer in [0,255] indicating the amount of green.
     * @param blueAmount - an integer in [0,255] indicating the amount of blue.
     * @return a list of errors or an empty list if the color was successfully set.
     */
    QStringList setGridColor( int redAmount, int greenAmount, int blueAmount );

    /**
     * Set the spacing between grid lines.
     * @param spacing - the grid spacing in [0,1] with 1 having the least amount of spacing.
     * @return an error message if there was a problem setting the grid spacing; an empty
     *      string otherwise.
     */
    QString setGridSpacing( double spacing );

    /**
     * Set the thickness of the grid lines.
     * @param thickness - the grid line thickness in [0,1] with 1 having maximum thickness.
     * @return an error message if there was a problem setting the grid line thickness; an empty
     *      string otherwise.
     */
    QString setGridThickness( double thickness );

    /**
     * Set the transparency of the grid.
     * @param transparency - the amount of transparency in [0,1] with 1 completely opaque.
     * @return an error message if there was a problem setting the transparency; an empty
     *      string otherwise.
     */
    QString setGridTransparency( double transparency );

    /**
     * Set the color of grid labels color.
     * @param redAmount - an integer in [0, 255] indicating the amount of red.
     * @param greenAmount  an integer in [0,255] indicating the amount of green.
     * @param blueAmount - an integer in [0,255] indicating the amount of blue.
     * @return a list of errors or an empty list if the color was successfully set.
     */
    QStringList setLabelColor( int redAmount, int greenAmount, int blueAmount );

    /**
     * Set whether or not the axes should be shown.
     * @param showAxis - true if the axes should be shown; false otherwise.
     * @return an error message if there was a problem changing the visibility of the
     *      axes; an empty string otherwise.
     */
    QString setShowAxis( bool showAxis );

    /**
     * Set whether or not the grid lines should be shown.
     * @param showLines - true if the grid lines should be shown; false otherwise.
     * @return an error message if there was a problem changing the visibility of the
     *     grid; an empty string otherwise.
     */
    QString setShowGridLines( bool showLines );

    /**
     * Set whether or not the axis should be internal or external.
     * @param showInternalLabels - true if the axes should be internal; false otherwise.
     * @return an error message if there was a problem setting the axes internal/external;
     *      false otherwise.
     */
    QString setShowInternalLabels( bool showInternalLabels );

    virtual ~GridControls();
    const static QString CLASS_NAME;

signals:
    void gridChanged( const Carta::State::StateInterface& gridState );

private:

    void _initializeDefaultState();
    void _initializeDefaultState( const QString& key );
    void _initializeCallbacks();
    QStringList _parseColorParams( const QString& params, const QString& label,
            int* red, int* green, int* blue ) const;
    void _updateGrid();

    const static QString ALL;
    const static QString GRID;
    static bool m_registered;

    GridControls( const QString& path, const QString& id );

    class Factory;

    std::unique_ptr<DataGrid> m_dataGrid;



	GridControls( const GridControls& other);
	GridControls& operator=( const GridControls& other );
};
}
}
