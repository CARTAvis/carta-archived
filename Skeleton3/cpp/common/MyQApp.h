/**
 *
 **/


#ifndef MYQAPP_H
#define MYQAPP_H

#include <QApplication>
#include <functional>
#include "IConnector.h"

// I suspect somewhere down the road we'll want to override notify() or some
// other functionality of QApplication, so we might as well provision for it...

// also, right now I use this to hold globally available functions/data

class MyQApp : public QApplication
{
    Q_OBJECT
public:
    explicit MyQApp(int & argc, char **argv = nullptr);

//    virtual bool notify( QObject * obj, QEvent * event) Q_DECL_OVERRIDE ;

    /**
     * @brief set the platform
     * @param return the platform
     */

protected:

};


// convenience function for deferred function calls

///
/// \brief defer
/// \param function
///
void defer( const std::function< void()> & function );


///
/// \brief Internal class to implement defer()
///
class DeferHelper2 : public QObject
{
    Q_OBJECT

public:

    typedef std::function<void ()> VoidFunc;

//    DeferHelper2();

//    using QObject::QObject;

    void queue( const VoidFunc & func);

public slots:
    void execute( const VoidFunc & func);

};

// TODO: clean up the commented code (both here and in .cpp)

#endif // MYQAPP_H
