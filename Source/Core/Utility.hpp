#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <QtCore>
#include "Core/Matrix.hpp"

/* Static class of utilities. */
class Utility {

private:

    /* Deleted constructor (static class). */
    Utility() = delete;

public:

    /* Returns a "human-readable" range. */
    static std::pair<double, double> getHumanReadableRange(const std::pair<double, double>& range);

    /* Finds the local extrema (minima and maxima) of the provided stress-time history (finds the load reversals). */
    static std::vector<int> findExtrema(const Matrix<double>& history);

    /* Performs the rainflow counting algorithm. */
    static Matrix<double> executeRainflowCounting(const Matrix<double>& history, const std::vector<int>& extrema,
        double repCount);

    /* Performs Miner's summation algorithm. */
    static Matrix<double> executeMinerSummation(double category, double resistanceFactor, const Matrix<double>& slopes,
        const Matrix<double>& rainflow, double stressFactor);

    /* Computes the remaining fatigue life of a LEFM detail. */
    static std::pair<double, QString> computeRemainingFatigueLife(const QString& detail,
        const std::unordered_map<QString, double>& parameters, const Matrix<double>& history,
        const Matrix<double>& rainflow, double C, double m, double ΔKth, double ΔKcr, double a0);

};
