#pragma once

#include "Gauss1d.h"
#include <QString>
#include <QRectF>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <queue>

/*
 * 1d LBTA fitter for an array of doubles
 *
 * list based threshold accepting
 *
 * http://www.sciencedirect.com/science/article/pii/S0305054803001722?np=y#FIG3
 */

namespace Optimization
{
namespace Gaussian1DFitting
{
class LBTAFitter
{
public:

    typedef Optimization::VD VD;

    LBTAFitter( FitterInput & dataInterface )
        : di( dataInterface )
    {
        firstTime = true;

        neighborStepSize = 1.0; // TODO: make user configurable
        currIter = 0;
        bestCost = 0 / 0.0;
        nFailed = 0;
    }

    FitterInput & di;

    const VD &
    getResults();

    /// initialize stuff for iterate()
    void
    initOnce();

    /// invoke an iteration
    ///  result: true = finished, fales = not finished
    bool
    iterate();

    void
    setInitialParams( const std::vector < double > & v )
    {
        params = v;
    }

    /// guard to execute initOnce()
    bool firstTime;

    /// current parameters
    std::vector < double > params;

    /// TA specific parameters
    int currIter;
    VD currX; // current solution
    double currCost; // cost of current solution
    VD bestX; // best solution so far
    double bestCost; // cost of best solution
    /// LBTA inputs
    double neighborStepSize; // how much can we move to find neighbor
    int tHeapSize; // size of the heap to maintain
    int nFailedThreshold; // how many consecutive unsuccessful attempts before stopping
    int nFailed; // number of failed attempts to improve

    /// TODO: make tHeapSize & nFailedThreshold user configurable

    /// cost calculation for the given parameters
    double
    calculateCost( const VD & x ) { return di.calculateDiffSq( x ); }

    /// neighbor calculation for the given parameters
    VD
    calculateNeighbor( const VD & x );

    std::priority_queue < double > tHeap;
};

typename LBTAFitter::VD
LBTAFitter::calculateNeighbor( const LBTAFitter::VD & x )
{
//    VD res = x;
//    int np = di.numParams();
//    int ind = qrand() % np;
//    res[ind] += ( drand48() * 2 - 1 ) * di.ranges[ind].heuristicMaxStep;
//    di.constraintParameters( res );
//    return res;
    VD res = x;
    int np = di.numParams();
    for ( int ind = 0 ; ind < np ; ++ind ) {
//        int ind = qrand() % np;
        res[ind] += ( drand48() * 2 - 1 ) * di.ranges[ind].heuristicMaxStep;
        di.clampParams( res );
    }
    return res;
}

const typename LBTAFitter::VD &
LBTAFitter::getResults()
{
    return bestX;
}

void
LBTAFitter::initOnce()
{
    if ( ! firstTime ) {
        return;
    }
    firstTime = false;

    if ( int ( params.size() ) != di.numParams() ) {
        LTHROW( "Bad parm size" );
    }

//    dbg(1) << "ranges: ------------------------------------------------\n";
//    for( size_t i = 0 ; i < di.ranges.size() ; i ++ ) {
//        dbg(1) << QString("  %1%2) %3 .. %4  /  %5\n")
//                  .arg(di.ranges[i].isSet ? "X" : " ")
//                  .arg(i, 3)
//                  .arg(di.ranges[i].min, 15)
//                  .arg(di.ranges[i].max, 15)
//                  .arg(di.ranges[i].step, 15);
//    }
//    dbg(1) << "---------------------------------------------------------\n";

    tHeapSize = ( di.numParams() + 1 ) * 1000;
    nFailedThreshold = tHeapSize / 5;
    nFailed = 0;

    currIter = 0;

    currX = params;
    currCost = calculateCost( currX );
    bestX = currX;
    bestCost = currCost;

    qDebug() << "init cost = " << bestCost;
} // LBTAFitter::initOnce

bool
LBTAFitter::iterate()
{
    initOnce();

    auto printHeap = [this] ( QString /*pf*/ )
                     {
                         auto q = tHeap;
                         QVector < double > v1, v2;
                         while ( ! q.empty() ) {
                             if ( v1.size() < 10 ) {
                                 v1.push_back( q.top() );
                             }
                             else if ( q.size() <= 10 ) {
                                 v2.push_back( q.top() );
                             }
                             q.pop();

//                             if ( v1.size() > 10 ) {
//                                 break;
//                             }
                         }
                         //qDebug() << "LBTA " << pf << "theap:" << v1 <<
                         //    "..." << v2;
                     };

    // stage 1 : populate the list
    while ( int ( tHeap.size() ) < tHeapSize ) {
        currIter++;

        // make a neighbor of currX
        VD xNew = calculateNeighbor( currX );

        // calculate the new neighobor's cost
        double costNew = calculateCost( xNew );

//        qDebug() << "LBTA xNew (1)" << costNew << "==>" << QVector<double>::fromStdVector(xNew);
        // calculate tnew
        double tnew = ( costNew - currCost ) / currCost;

        // if tnew is negative, it means we accidentally found a better starting point...
        if ( tnew < 0 ) {
//            qDebug() << "LBTA: restarting stage 1 with better starting point";
//            tHeap = decltype (tHeap)();
            //qDebug() << "LBTA: moving stage 1 to a better starting point";
            currCost = costNew;
            currX = xNew;
            bestCost = costNew;
            bestX = xNew;
            return false;
        }

        // get the max value in our heap (or -1 if heap empty)
        double tmax = - 1;
        if ( tHeap.size() > 0 ) {
            tmax = tHeap.top();
        }

        // insert tnew into the list if it's smaller than tmax (or heap is empty)
        if ( tnew < tmax || tHeap.empty() ) {
            tHeap.push( tnew );

//            return false;
        }
        else {
            // do it anyways
            tHeap.push( tnew );
        }

        //qDebug() << "LBTA" << tnew << ">" << tmax;

        if ( int ( tHeap.size() ) == tHeapSize ) {
            //qDebug() << "LBTA: stage 1 cost = " << bestCost;
            printHeap( "At end of stage 1" );
        }

        return false;
    }

//    printHeap( "during stage2");

    currIter += 1;

    // calculate a neighbor
    VD xNew = calculateNeighbor( currX );
    double costNew = calculateCost( xNew );

    double tnew = ( costNew - currCost ) / currCost;
    double tmax = tHeap.top();
    //qDebug() << "LBTA stage 2 tmax=" << tmax << "tnew" << tnew;
    if ( tnew < tmax ) {
        nFailed = 0;
        tHeap.pop();
        tHeap.push( tnew );
        //qDebug() << "LBTA after pop/push max=" << tHeap.top();

//        if ( costNew < currCost ) {
        currCost = costNew;
        currX = xNew;
        //qDebug() << "LBTA: stage 2 updating current solution " << currCost << "\n";
        if ( currCost < bestCost ) {
            // remember best found solution
            bestCost = currCost;
            bestX = currX;
            //qDebug() << "LBTA: stage 2 updating best solution " << bestCost << "\n";
        }

//        }
    }
    else {
        //qDebug() << "LBTA nfailed" << nFailed;
        nFailed++;
        if ( nFailed > nFailedThreshold ) {
//            QString str;
//            QTextStream out(&str);
//            out << "=================== LBTA finished ======================\n";
//            out << "best cost = " << bestCost << "\n";
//            out << "ngauss / npoly = " << di.nGaussians << " " << di.nPolyTerms << "\n";
//            out << "ranges:\n";
//            for( size_t i = 0 ; i < di.ranges.size() ; i ++ ) {
//                out << QString("  %1) %2 .. %3  /  %4\n")
//                          .arg(i, 3)
//                          .arg(di.ranges[i].min, 15)
//                          .arg(di.ranges[i].max, 15)
//                          .arg(di.ranges[i].step, 15);
//            }
//            out << "x12 = " << di.x1 << " .. " << di.x2 << "\n";
//            out << "dataRange = " << di.rangeMin << " .. " << di.rangeMax << "\n";
//            out << "params: ";
//            for( size_t i = 0 ; i < bestX.size() ; i ++)
//                out << bestX[i] << " ";
//            out << "\n";
//            out << "---------------------------------------------------------\n";

//            dbg(1) << str;
            return true;
        }

//        dbg(1) << "stage 2 nFailed = " << nFailed << "\n";
    }

    return false;
} // LBTAFitter::iterate
} // namespace Gaussian1DFitting
} // namespace Optimization

