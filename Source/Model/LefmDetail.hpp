#pragma once

#include <QtCore>
#include <tuple>
#include <utility>
#include <vector>
#include <optional>
#include <unordered_map>
#include "Core.hpp"
#include "Model/Detail.hpp"

/* Represents a constructional LEFM detail. */
class LefmDetail : public Detail {
    Q_OBJECT

public:

    /* Returns the available LEFM details. */
    static const QStringList& availableDetails();

    /* Returns the parameters associated with the chosen LEFM detail. */
    static const std::vector<std::tuple<QString, QString, QString>>& parameters(const QString& detail);

private:
    QString _selectedDetail;
    std::optional<std::unordered_map<QString, double>> _paramValues;
    std::optional<double> _parisCoefficient;
    std::optional<double> _parisExponent;
    std::optional<double> _sifThreshold;
    std::optional<double> _sifCritical;
    bool _ignoreTime;
    QString _timeUnits;
    double _repCount;
    Matrix<double> _history;
    std::vector<int> _extrema;
    Matrix<double> _rainflow;
    Matrix<double> _initialCrackSample;
    std::optional<Matrix<double>> _lifeSample;
    std::optional<Matrix<QString>> _stopCriteria;

public:

    /* LEFM detail constructor. */
    explicit LefmDetail(QObject* parent = nullptr);

    /* The selected approach for the detail analysis. */
    Approach approach() const override;

    /* The selected detail. */
    const QString& selectedDetail() const;

    /* Sets the selected detail. */
    void setSelectedDetail(const QString& detail);

    /* Returns a parameter value by its symbol. */
    double parameterValue(const QString& symbol) const;

    /* Sets the parameter values by their symbols. */
    void setParameterValues(const std::unordered_map<QString, double>& params);

    /* Flags if the detail parameters have been set. */
    bool hasParameters() const;

    /* The Paris' law coefficient. */
    std::optional<double> parisCoefficient() const;

    /* Sets the Paris' law coefficient. */
    void setParisCoefficient(double parisCoefficient);

    /* The Paris' law exponent. */
    std::optional<double> parisExponent() const;

    /* Sets the Paris' law exponent. */
    void setParisExponent(double parisExponent);

    /* The stress intensity factor threshold. */
    std::optional<double> sifThreshold() const;

    /* Sets the stress intensity factor threshold. */
    void setSifThreshold(double sifThreshold);

    /* The critical stress intensity factor. */
    std::optional<double> sifCritical() const;

    /* Sets the critical stress intensity factor. */
    void setSifCritical(double sifCritical);

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

    /* Sample of initial crack depths. */
    const Matrix<double>& initialCrackSample() const;

    /* Sets the sample of initial crack depths. */
    void setInitialCrackSample(const Matrix<double>& sample);

    /* Sample of remaining fatigue life. */
    const std::optional<Matrix<double>>& lifeSample() const;

    /* Stop criteria for the remaining fatigue life. */
    const std::optional<Matrix<QString>>& stopCriteria() const;

    /* Compute remaining fatigue life. */
    void computeRemainingFatigueLife();

    /* Compute remaining fatigue life. */
    std::pair<double, QString> computeRemainingFatigueLife(double a0);

};
