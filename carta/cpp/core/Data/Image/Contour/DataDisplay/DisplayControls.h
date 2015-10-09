/***
 * Manages state for which display axes are to be shown.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/AxisInfo.h"



namespace Carta {

namespace Data {

class DataGrid;

class DisplayControls : public QObject, public Carta::State::CartaObject{

    friend class Controller;
    Q_OBJECT

public:

    /**
     * Set whether or not display axis settings should apply to all images of the set.
     * @param applyAll - true if the settings apply to all images on the stack;
     *      false otherwise.
     */
    void setApplyAll( bool applyAll );

    /**
     * Set a particular display axis.
     * @param axisId - an identifier for a display axis.
     * @param purpose - the purpose of the display axis.
     * @return - an error message if the display axis could not be set; an empty string
     *      otherwise.
     */
    QString setAxis( const QString& axisId, const QString& purpose );

    virtual ~DisplayControls();
    const static QString SUPPORTED_AXES;
    const static QString CLASS_NAME;


signals:

    /**
     * Notification that the display axes have changed.
     * @param types - the list of new display axes.
     * @param applyAll - true if the display axes apply to all images on the stack; false if
     *      the display axes apply only to the current image.
     */
    void displayAxesChanged( std::vector<Carta::Lib::AxisInfo::KnownType> types, bool applyAll );

private:

    void _addUsedPurpose( const QString& key, const QString& targetPurpose,
            QList<QString>& usedPurposes, QString& usedPurposeKey );

    void _initializeCallbacks();
    void _initializeDefaultState();
    void _notifyAxesChanged();

    /**
     * Set the list of available axes that can be displayed based on the image.
     * @param supportedAxes - the list of available display axes.
     */
    void setAxisTypes( std::vector<Carta::Lib::AxisInfo::KnownType> supportedAxes );
    void _updateDisplay();

    const static QString ALL;
    static bool m_registered;

    DisplayControls( const QString& path, const QString& id );

    class Factory;


	DisplayControls( const DisplayControls& other);
	DisplayControls& operator=( const DisplayControls& other );
};
}
}
