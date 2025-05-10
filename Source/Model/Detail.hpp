#pragma once

#include <QtCore>
#include <vector>
#include "Core.hpp"

/* Represents a constructional detail. */
class Detail : public QObject {
    Q_OBJECT

public:

    /* Available detail approaches. */
    enum class Approach { Sn, Lefm };

private:
    QString _name;
    QString _description;

public:

    /* Detail constructor. */
    explicit Detail(QObject* parent = nullptr);

    /* The name of the detail. */
    const QString& name() const;

    /* Sets the name of the detail. */
    void setName(const QString& name);

    /* The description of the detail. */
    const QString& description() const;

    /* Sets the description of the detail. */
    void setDescription(const QString& description);

    /* The selected approach for the detail analysis. */
    virtual Approach approach() const = 0;

    /* Flags if the stress-time history does not include a time variable. */
    virtual bool ignoreTime() const = 0;

    /* Sets if the stress-time history does not include a time variable. */
    virtual void setIgnoreTime(bool ignoreTime) = 0;

    /* The stress-time history time units. */
    virtual const QString& timeUnits() const = 0;

    /* Sets the stress-time history time units. */
    virtual void setTimeUnits(const QString& timeUnits) = 0;

    /* The number of times the sampled stress-time history repeats. */
    virtual double repCount() const = 0;

    /* Sets the number of times the sampled stress-time history repeats. */
    virtual void setRepCount(double repCount) = 0;

    /* The sampled stress-time history. */
    virtual const Matrix<double>& historySample() const = 0;

    /* Sets the sampled stress-time history. */
    virtual void setHistorySample(const Matrix<double>& history) = 0;

    /* The local extrema (minima and maxima) of the stress-time history (the load reversals). */
    virtual const std::vector<int>& historyExtrema() const = 0;

    /* The results of the rainflow counting algorithm. */
    virtual const Matrix<double>& rainflowCounts() const = 0;

    /* Performs the rainflow counting algorithm. */
    virtual void executeRainflowCounting() = 0;

signals:

    /* Signal emitted when the detail is modified. */
    void modified();

};
