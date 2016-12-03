/***
 * A set of (value,count) pairs that represent a histogram as well as information about
 * how the pairs should be drawn.
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include "CartaLib/Hooks/HistogramResult.h"
#include <QColor>
#include <QObject>


namespace Carta {
namespace Data {

class BinData : public Carta::State::CartaObject {
public:

    /**
     * Return the color to use in plotting the points of the curve.
     * @return - the color to use in plotting the points of the curve.
     */
    QColor getColor() const;

    /**
     * Return the (value,count) pairs that are the result of the histogram
     * calculation.
     * @return - the (value,count) pairs that are the result of the histogram
     * 		calculation.
     */
    Carta::Lib::Hooks::HistogramResult getHistogramResult() const;


    /**
     * Return an identifier for the data.
     * @return - a data identifier.
     */
    QString getName() const;

    /**
     * Return the internal state of the data as a string.
     * @return - the data state.
     */
    QString getStateString() const;

    /**
     * Set the color to use in plotting the points of the data.
     * @param color - the color to use in plotting data points.
     */
    void setColor( QColor color );

    /**
     * Set the (value,count) data values that comprise the data.
     * @param results - the results of the histogram computation.
     */
    void setHistogramResult( const Carta::Lib::Hooks::HistogramResult& result  );

    /**
     * Set an identifier for the data.
     * @param name - an identifier for the data.
     */
    void setName( const QString& name );

    virtual ~BinData();
    const static QString CLASS_NAME;

private:

    void _initializeDefaultState();

    static bool m_registered;

    BinData( const QString& path, const QString& id );
    class Factory;

    Carta::Lib::Hooks::HistogramResult m_result;

	BinData( const BinData& other);
	BinData& operator=( const BinData& other );
};
}
}
