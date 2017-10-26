/***
 * List the available methods for producing a profile.
 *
 */

#pragma once

#include "State/ObjectManager.h"
#include "State/StateInterface.h"

namespace Carta {

namespace Data {



class GenerateModes : public Carta::State::CartaObject {

public:

    /**
     * Returns the default generation mode used to compute profiles.
     * @return the default generation mode used to compute profiles.
     */
    QString getDefault() const;

    /**
     * Translates a possible case insensitive profile generation mode into one
     * that matches the actual generation mode exactly.
     * @param genMode - an identifier for the type of profiles that should be automatically
     *      generated.
     * @return - the actual mode for profiles that should be automatically generated.
     */
    QString getActualMode( const QString& genMode ) const;

    /**
     * Returns true if the given mode identifier indicates only profiles of selected
     * images should be generated.
     * @param mode - an identifier for a method of automatically generating profiles.
     * @return - whether or not the identifier indicates only profiles of selected images
     *      should be generated.
     */
    bool isCurrent( const QString& mode ) const;

    /**
     * Returns true if the given mode identifier indicates profiles of all images should
     * be generated.
     * @param mode - an identifier for a method of automatically generating profiles.
     * @return - whether or not the identifier indicates profiles of all images
     *      should be generated.
     */
    bool isAll( const QString& mode) const;

    /**
     * Returns true if the given mode identifier indicates profiles of all images should
     * be generated except those whose profile is a single point.
     * @param mode - an identifier for a method of automatically generating profiles.
     * @return - whether or not the identifier indicates profiles of all images
     *      should be generated except for those whose profiles would be a single point.
     */
    // bool isAllExcludeSingle( const QString& mode ) const;

    const static QString CLASS_NAME;
    const static QString GEN_LIST;

    virtual ~GenerateModes();

private:

    const static QString GEN_CURRENT;
    const static QString GEN_ALL;
    //const static QString GEN_CUSTOM;
    const static QString GEN_NO_SINGLE_PLANES;

    void _initializeDefaultState();
    void _initMode( int * index, const QString& name);

    static bool m_registered;
    GenerateModes( const QString& path, const QString& id );
    class Factory;

	GenerateModes( const GenerateModes& other);
	GenerateModes& operator=( const GenerateModes& other );
};
}
}
