#ifndef LBTAGAUSS1DFITTER_H
#define LBTAGAUSS1DFITTER_H

#include <QString>
#include <QRectF>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <queue>
#include "common.h"
#include "Gauss1d.h"

/*
 * 1d LBTA fitter for an array of doubles
 */

namespace Optimization {
namespace Gaussian1DFitting {

class LBTAFitter
{

public:

    typedef Optimization::VD VD;

    LBTAFitter( DataInterface & dataInterface)
        : di( dataInterface)
    {
        firstTime = true;

        neighborStepSize = 1.0; // TODO: make user configurable
        currIter = 0;
        bestCost = 0/0.0;
        nFailed = 0;
    }

    DataInterface & di;

    const VD & getResults();

    /// initialize stuff for iterate()
    void initOnce();

    /// invoke an iteration
    ///  result: true = finished, fales = not finished
    bool iterate();

    void setInitialParams(const std::vector<double> &v)
    {
        params = v;
    }

    /// guard to execute initOnce()
    bool firstTime;

    /// current parameters
    std::vector<double> params;

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
    double calculateCost( const VD & x) { return di.calculateDiffSq( x); }

    /// neighbor calculation for the given parameters
    VD calculateNeighbor( const VD & x);

    std::priority_queue<double> tHeap;
};



typename LBTAFitter::VD
LBTAFitter::calculateNeighbor(const LBTAFitter::VD &x)
{
    VD res = x;
    int np = di.numParams();
    int ind = qrand() % np;
    res[ind] += (drand48() *2 - 1) * di.ranges[ind].step;
    di.constraintParameters( res);
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
    if( ! firstTime) return;
    firstTime = false;

    if( int(params.size()) != di.numParams())
        LTHROW( "Bad parm size");

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


    tHeapSize = (di.numParams()+1) * 100;
//    if( nGaussians == 1) tHeapSize = 1000;
    nFailedThreshold = tHeapSize / 5;
    nFailed = 0;
//    dbg(1) << "LBTA heap size = " << tHeapSize << "\n";

    currIter = 0;

    currX = params;
    currCost = calculateCost( currX);
    bestX = currX;
    bestCost = currCost;

    dbg(1) << "init cost = " << bestCost << "\n";
}


bool
LBTAFitter::iterate()
{

    initOnce();

    // populate the list
    while( int(tHeap.size()) < tHeapSize) {
        VD xNew = calculateNeighbor( currX);
        double costNew = calculateCost( xNew);
        if( costNew < currCost) {
            currCost = costNew;
            currX = xNew;
//            dbg(1) << "stage 1 got better solution " << currCost << "\n";
            return false;
        }
        double d = (costNew - currCost) /  currCost;
        this-> tHeap.push( d);
//        dbg(1) << "stage 1 pushing " << d << "\n";

        if( ! (int(tHeap.size()) < tHeapSize)) {
            bestCost = currCost;
            bestX = currX;
        }

        if( int(tHeap.size()) == tHeapSize) {
            dbg(1) << "stage 1 cost = " << bestCost << "\n";
        }

        return false;
    }



    currIter += 1;

    // calculate a neighbor
    VD xNew = calculateNeighbor( currX);
    double costNew = calculateCost( xNew);

    double d = (costNew - currCost) / currCost;
    double maxd = tHeap.top();
//    if( currIter % 1000 == 0) {
//        dbg(1) << "maxd = " << maxd << " nf = " << nFailed << "\n";
//    }
    if( d < maxd) {
        nFailed = 0;
        tHeap.pop();
        tHeap.push( d);
        if( costNew < currCost) {
            currCost = costNew;
            currX = xNew;
//            dbg(1) << "stage 2 updating current solution " << currCost << "\n";
            if( currCost < bestCost) { // remember best found solution
                bestCost = currCost;
                bestX = currX;
//                dbg(1) << "stage 2 updating best solution " << bestCost << "\n";
            }
        }
    } else {
        nFailed ++;
        if( nFailed > nFailedThreshold) {
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
}

} // namespace Gaussian1DFitting
} // namespace Optimization

#endif // LBTAGAUSS1DFITTER_H

