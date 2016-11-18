/**
 * Stores parameters & data for fitting a 1D curve.
 */
#pragma once

#include <QString>
#include <vector>
namespace Carta
{
namespace Lib
{
class Fit1DInfo
{
public:
    /// Status of fitting the data
    enum class StatusType {
        NOT_DONE,
        ERROR,
        PARTIAL,
        COMPLETE,
        OTHER
    };

    /**
     * Constructor.
     */
    Fit1DInfo();

    /**
     * Return the y-data values that will be fit.
     */
    std::vector<double> getData() const;

    /**
     * Return the number of Gaussians to fit.
     * @return - the number of Gaussians to fit to the curve data.
     */
    int getGaussCount() const;

    /**
     * Return an list of initial guesses for Gaussian fit parameters.
     * @return - a list of initial guesses for Gaussian fit parameters.
     */
    std::vector<std::tuple<double,double,double> > getInitialGaussianGuesses() const;

    /**
     * Return the number of terms in the polynomial to fit to the data.
     * @return - the degree of the polynomial to fit to the data.
     */
    int getPolyDegree() const;

    /**
     * Return an identifier for the curve that is being fit.
     * @return - an idenitifer for the curve that is being fit.
     */
    QString getId() const;

    /**
     * Return whether or not to use random heuristics for the initial guess.
     * @return - true if random heuristics should be used for the initial guess; false otherwise.
     */
    bool isRandomHeuristics() const;

    /**
     * Set the y-values of the 1-dimensional curve that will be
     * fit.
     * @param data - the y-values of the curve to be fit.
     */
    void setData( const std::vector<double>& data );

    /**
     * Set the number of Gaussians to fit to the curve.
     * @param gaussCount - the number of Gaussians to fit to the curve.
     */
    void setGaussCount( int gaussCount );

    /**
     * Set initial guesses for Gaussian fit parameters.
     * @param initialGuesses - initial guesses of Gaussian fit parameters.
     */
    void setInitialGaussianGuesses( const std::vector<std::tuple<double,double,double> >& initialGuesses );

    /**
     * Set the degree of the polynomial to fit to the data.
     * @param polyDegree - the degree of the polynomial to fit to the data.
     */
    void setPolyDegree( int polyDegree );

    /**
     * Set whether or not to use random heuristics for the initial guess.
     * @param random - true to use random heuristics for the initial guess;
     *      false otherwise.
     */
    void setRandomHeuristics( bool random );

    /**
     * Set an identifier for the curve to be fit.
     * @param id - an identifier for the curve to be fit.
     */
    void setId( const QString& id );


    virtual ~Fit1DInfo();

private:
    QString m_id;
    int m_polyDegree;
    int m_gaussCount;
    bool m_randomHeuristics;
    std::vector<double> m_data;
    std::vector<std::tuple<double,double,double>> m_initGuesses;
};

} // namespace Lib
} // namespace Carta
