#pragma once

#include <QtCore>
#include <vector>
#include "Core.hpp"
#include "Model/Detail.hpp"

/* Represents a constructional LEFM detail. */
class LefmDetail : public Detail {
    Q_OBJECT

private:
    bool _ignoreTime;
    QString _timeUnits;
    double _repCount;
    Matrix<double> _history;
    std::vector<int> _extrema;
    Matrix<double> _rainflow;

public:

    /* LEFM detail constructor. */
    explicit LefmDetail(QObject* parent = nullptr);

    /* The selected approach for the detail analysis. */
    Approach approach() const override;

    /* Flags if the stress-time history does not include a time variable. */
    bool ignoreTime() const override;

    /* Sets if the stress-time history does not include a time variable. */
    void setIgnoreTime(bool ignoreTime) override;

    /* The stress-time history time units. */
    const QString& timeUnits() const override;

    /* Sets the stress-time history time units. */
    void setTimeUnits(const QString& timeUnits) override;

    /* The number of times the sampled stress-time history repeats. */
    double repCount() const override;

    /* Sets the number of times the sampled stress-time history repeats. */
    void setRepCount(double repCount) override;

    /* The sampled stress-time history. */
    const Matrix<double>& historySample() const override;

    /* Sets the sampled stress-time history. */
    void setHistorySample(const Matrix<double>& history) override;

    /* The local extrema (minima and maxima) of the stress-time history (the load reversals). */
    const std::vector<int>& historyExtrema() const override;

    /* The results of the rainflow counting algorithm. */
    const Matrix<double>& rainflowCounts() const override;

    /* Performs the rainflow counting algorithm. */
    void executeRainflowCounting() override;

};
