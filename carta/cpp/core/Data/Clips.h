/***
 * List of available clip values (for histogram & image).
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"
#include <vector>

namespace Carta {

namespace Data {

class Clips : public Carta::State::CartaObject {

public:

    /**
     * Returns the clip value associated with the index.
     * @param index a integer representing a valid clip index.
     * @return the clip associated with the index.
     */
    double getClip( int index ) const;

    /**
     * Returns all clip values associated with the sorted percentile.
     * @param non.
     * @return all clips associated with the sorted percentiles from small to large.
     */
    std::vector<double> getAllClips2percentiles() const;

    /**
     * Returns the index of the corresponding clip value or -1 if there is no such clip value.
     * @param clipValue a QString containing a clip amount.
     * @return the index of the corresponding clip value if one exists or -1 otherwise.
     */
    int getIndex( const QString& clipValue ) const;

    const static QString CLASS_NAME;
    const static QString CLIP_LIST;
    virtual ~Clips();

private:

    // set clip values in UI
    std::vector<double> _initializeClipValues() const;

    void _initializeDefaultState();

    static bool m_registered;
    const static QString CLIP_COUNT;
    Clips( const QString& path, const QString& id );
    class Factory;


	Clips( const Clips& other);
	Clips& operator=( const Clips& other );
};
}
}
