/**
 * Information for known statistics.
 **/

#pragma once

#include <QString>

namespace Carta {
namespace Lib {
class StatInfo {
public:
    ///Major statistics types.  A plugin-in can decide to provide
    //more than the basic types (or less).
    enum class StatType : int {
        Name = 0,
        //Image statistics
        Shape,
        RestoringBeam,
        MedianRestoringBeam,
        RightAscensionRange,
        DeclinationRange,
        FrequencyRange,
        VelocityRange,
        Frequency,
        DirectionType,
        Velocity,
        Stokes,
        BrightnessUnit,
        FrameCount,
        BeamArea,
        //Region statistics
        Sum,
        FluxDensity,
        Mean,
        RMS,
        Sigma,
        SumSq,
        Min,
        Max,
        Blc,
        Trc,
        MinPos,
        MaxPos,
        Blcf,
        Trcf,
        MinPosf,
        MaxPosf,
        PluginDefined
    };

    /**
     * Return a UI label for the statistic.
     * @return - user displayable description for the statistic.
     */
    QString getLabel() const;

    /**
     * Return the value of the statistic.
     * @return - a computed statistic value.
     */
    QString getValue() const;

    /**
     * Return the type of statistic (region,image, etc).
     * @return - the statistic type.
     */
    StatType getType() const;

    /**
     * Return true if the statistic is an image statistic; false
     * otherwise.
     * @return - true if the statistic is computed from an image;
     *    false if it is computed from additional information such as a region.
     */
    bool isImageStat() const;

    /**
     * Set whether or not the information represents an image statistic.
     * @param imageStat - true if the statistic if an image statistic; false otherwise.
     */
    void setImageStat( bool imageStat );

    /**
     * Set a descriptive label for the statistic.
     * @param label - a descriptive label for the statistic.
     */
    void setLabel( const QString& label );

    /**
     * Set a computed value for the statistic.
     * @param value - a computed statistic value.
     */
    void setValue( const QString& value );

    /**
     * Return a string representation of the statistic type.
     * @return - a string representation of the type of statistic.
     */
    static QString toString( StatType statType);

    /**
     * Return a list of image statistics.
     * @return - a list of image statistics.
     */
    static QList<StatType> getStatTypesImage();

    /**
     * Return a list or region statistics.
     * @return - a list of region statistics.
     */
    static QList<StatType> getStatTypesRegion();

    /**
     * Constructor.
     */
    StatInfo( StatType statType );
    virtual ~StatInfo();
private:
    StatType m_statType;
    QString m_label;
    QString m_value;
    bool m_imageStat;
    static QList<StatType> m_regionStatTypes;
    static QList<StatType> m_imageStatTypes;


};
}
}

