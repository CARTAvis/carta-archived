/**
 *
 **/


#pragma once

#include "cartalib_global.h"
#include <cassert>
#include <iostream>


/// all carta code lives here (or will eventually)
namespace Carta
{
/// code we want to share with plugins should go here
/// \par for example, image interface, metadata interface, hooks
namespace Lib
{ }

/// all code that is necessary for the core functionality should go here
/// but only parts that are not specific to desktop/server/scripted clients
/// and parts that plugins do not need to know about
namespace Core
{ }

/// desktop client related code
/// probably not really necessary
namespace Desktop
{ }

/// server client related code
/// probably not really necessary
namespace Server
{ }

/// scripted client related code
/// probably not really necessary
namespace Scripted
{ }
}

/// a macro that adds 'Me' and 'SharedPtr' typedefs to a class
#define CLASS_BOILERPLATE( class ) \
public: \
    typedef class                  Me; \
    typedef std::shared_ptr < Me > SharedPtr;


/// currently does nothing, but may serve as a place to keep global settings for
/// the whole library
class CARTALIBSHARED_EXPORT CartaLib
{

public:
    CartaLib();
};


namespace Carta { namespace Lib {
inline
void cartaAssert(const char *assertion, const char *file, const char * func, int line) {
    std::cerr << "ASSERT: " << assertion << "\n"
              << "....in: " << file << ":" << line << "\n"
              << ".func.: " << func << "\n";
    std::abort();
}
}}

#define CARTA_ASSERT_ALWAYS(cond) ((!(cond)) ? Carta::Lib::cartaAssert(#cond,__FILE__,__PRETTY_FUNCTION__, __LINE__) : qt_noop())

#ifdef CARTA_RUNTIME_CHECKS
#define CARTA_ASSERT(cond) CARTA_ASSERT_ALWAYS(cond)
#else
#define CARTA_ASSERT(cond) qt_noop();
#endif


