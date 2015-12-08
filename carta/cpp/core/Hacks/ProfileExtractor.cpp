/**
 *
 **/

#include "ProfileExtractor.h"

Profiles::IProfileExtractor *
Profiles::getBestProfileExtractor( Carta::Lib::NdArray::RawViewInterface * rv,
                                   Profiles::ProfilePathType pt )
{
    Q_UNUSED( rv );
    Q_UNUSED( pt );
    return new DefaultPrincipalProfileExtractor;
}
