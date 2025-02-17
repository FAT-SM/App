#pragma once

#include <QtCore>
#include <vector>
#include <optional>
#include "Core.hpp"
#include "Model/Detail.hpp"

/* Represents a constructional S-N detail. */
class SnDetail : public Detail {
    Q_OBJECT

public:

    /* Design concept options. */
    enum class DesignConcept { SafeLife, DamageTolerant };

    /* Consequence of failure options. */
    enum class ConsequenceOfFailure { LowConsequence, MediumConsequence, HighConsequence };

private:
    std::optional<double> _category;
    Matrix<double> _slopes;
    bool _ignoreTime;
    QString _timeUnits;
    double _repCount;
    Matrix<double> _history;
    std::vector<int> _extrema;
    Matrix<double> _rainflow;
    DesignConcept _designConcept;
    ConsequenceOfFailure _consequenceOfFailure;
    bool _useCustomResistanceFactor;
    double _customResistanceFactor;
    double _stressFactor;

    /* Finds the local extrema (minima and maxima) of the stress-time history (finds the load reversals). */
    static std::vector<int> findExtrema(const Matrix<double>& history);

    /* Performs the rainflow counting algorithm. */
    static Matrix<double> executeRainflowCounting(const Matrix<double>& history, const std::vector<int>& extrema,
        double repCount);

public:

    /* S-N detail constructor. */
    explicit SnDetail(QObject* parent = nullptr);

    /* The selected approach for the detail analysis. */
    Approach approach() const override;

    /* The detail category. */
    std::optional<double> category() const;

    /* Sets the detail category. */
    void setCategory(double category);

    /* The S-N curve slopes. */
    const Matrix<double>& slopes() const;

    /* Sets the S-N curve slopes. */
    void setSlopes(const Matrix<double>& slopes);

    /* Flags if the stress-time history does not include a time variable. */
    bool ignoreTime() const;

    /* Sets if the stress-time history does not include a time variable. */
    void setIgnoreTime(bool ignoreTime);

    /* The stress-time history time units. */
    const QString& timeUnits() const;

    /* Sets the stress-time history time units. */
    void setTimeUnits(const QString& timeUnits);

    /* The number of times the sampled stress-time history repeats. */
    double repCount() const;

    /* Sets the number of times the sampled stress-time history repeats. */
    void setRepCount(double repCount);

    /* The sampled stress-time history. */
    const Matrix<double>& historySample() const;

    /* Sets the sampled stress-time history. */
    void setHistorySample(const Matrix<double>& history);

    /* The local extrema (minima and maxima) of the stress-time history (the load reversals). */
    const std::vector<int>& historyExtrema() const;

    /* The results of the rainflow counting algorithm. */
    const Matrix<double>& rainflowCounts() const;

    /* Performs the rainflow counting algorithm. */
    void executeRainflowCounting();

    /* The chosen design concept. */
    DesignConcept designConcept() const;

    /* Sets the chosen design concept. */
    void setDesignConcept(DesignConcept designConcept);

    /* The chosen consequence of failure. */
    ConsequenceOfFailure consequenceOfFailure() const;

    /* Sets the chosen consequence of failure. */
    void setConsequenceOfFailure(ConsequenceOfFailure consequenceOfFailure);

    /* Flags if a custom resistance factor is used. */
    bool useCustomResistanceFactor() const;

    /* Sets if a custom resistance factor is used. */
    void setUseCustomResistanceFactor(bool value);

    /* The user-defined resistance factor. */
    double customResistanceFactor() const;

    /* Sets the user-defined resistance factor. */
    void setCustomResistanceFactor(double value);

    /* The considered resistance factor. */
    double resistanceFactor() const;

    /* The considered stress factor. */
    double stressFactor() const;

    /* Sets the considered stress factor. */
    void setStressFactor(double value);

};
