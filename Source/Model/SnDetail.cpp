#include <cmath>
#include <tuple>
#include <limits>
#include <stdexcept>
#include "Model/SnDetail.hpp"

std::vector<int> SnDetail::findExtrema(const Matrix<double>& history) {

    // initialize storage and consider the first point as a local extremum
    std::vector<int> extrema;
    extrema.push_back(0);

    // loop history points except first and last points (first and last points are always extrema)
    for (int i = 1; i < history.rowCount() - 1; ++i) {

        // check if the current point is a local extremum
        if (
            (history.at(i, 1) < history.at(extrema.back(), 1) && history.at(i, 1) < history.at(i + 1, 1)) ||
            (history.at(i, 1) > history.at(extrema.back(), 1) && history.at(i, 1) > history.at(i + 1, 1))
        ) { extrema.push_back(i); }

    }

    // consider the last point as a local extremum
    extrema.push_back(history.rowCount() - 1);

    // check if valid
    if (extrema.size() <= 2) throw std::runtime_error("Stress-time history does not have any load reversals.");
    else return extrema;

}

Matrix<double> SnDetail::executeRainflowCounting(const Matrix<double>& history, const std::vector<int>& extrema,
    double repCount) {

    // initialize storage
    std::vector<std::tuple<double, double, double, double, double>> counts;

    // the "reversals" array keeps track of the peaks and valleys being referenced for the current count
    std::vector<int> reversals;

    // 1. advance through peaks and valleys
    for (int i = 0; i < extrema.size(); ++i) {
        reversals.push_back(extrema[i]);

        // 2. if there are less than 3 points, go to step 1
        //    otherwise, form ranges X and Y using the 3 most recent peaks and valleys
        while (reversals.size() >= 3) {
            int A = *(reversals.end() - 3);
            int B = *(reversals.end() - 2);
            int C = *(reversals.end() - 1);
            double xRange = std::abs(history.at(B, 1) - history.at(C, 1));
            double yRange = std::abs(history.at(A, 1) - history.at(B, 1));
            double yMean = 0.5*(history.at(A, 1) + history.at(B, 1));

            // 3. if X < Y, go to step 1
            //    otherwise, go to step 4
            if (xRange < yRange) break;

            // 4. if Y contains the starting point (i.e., if there are exactly 3 points), go to step 5
            if (reversals.size() == 3) {

                // 5. do:
                // * count range Y as one-half cycle
                // * discard the first point of Y
                // * move the starting point to the second point of Y
                // * go to step 2
                counts.push_back({ 0.5*repCount, yRange, yMean, history.at(A, 0), history.at(B, 0) });
                reversals.erase(reversals.begin());

            } else {

                // 4. (cont.) otherwise:
                // * count range Y as one cycle
                // * discard the peak and valley of Y
                // * go to step 2
                counts.push_back({ 1.0*repCount, yRange, yMean, history.at(A, 0), history.at(B, 0) });
                reversals.erase(reversals.end() - 3, reversals.end() - 1);

            }

        }

    }

    // 6. count each remaining range as one-half cycle
    for (int i = 0; i < reversals.size() - 1; ++i) {
        int a = reversals[i];
        int b = reversals[i + 1];
        double range = std::abs(history.at(a, 1) - history.at(b, 1));
        double mean = 0.5*(history.at(a, 1) + history.at(b, 1));
        counts.push_back({ 0.5*repCount, range, mean, history.at(a, 0), history.at(b, 0) });
    }

    // finalize
    Matrix<double> results(static_cast<int>(counts.size()), 5);
    for (int i = 0; i < counts.size(); ++i) {
        results.at(i, 0) = std::get<0>(counts[i]);
        results.at(i, 1) = std::get<1>(counts[i]);
        results.at(i, 2) = std::get<2>(counts[i]);
        results.at(i, 3) = std::get<3>(counts[i]);
        results.at(i, 4) = std::get<4>(counts[i]);
    }
    return results;

}

Matrix<double> SnDetail::executeMinerSummation(double category, double resistanceFactor, const Matrix<double>& slopes,
    const Matrix<double>& rainflow, double stressFactor) {

    // initialize storage
    Matrix<double> results(rainflow.rowCount(), 4);

    // s-n curve calculations
    double NC = 2e6;
    double ΔσC = category/resistanceFactor;
    std::vector<std::tuple<double, double, double>> snPoints;
    snPoints.push_back({ ΔσC, NC, 0.0 }); // dummy reference point
    for (int i = 0; i < slopes.rowCount(); ++i) {
        auto m = slopes.at(i, 0);
        auto N2 = slopes.at(i, 1);
        auto [Δσ1, N1, _] = snPoints.back();
        auto Δσ2 = Δσ1*std::pow(N1/N2, 1.0/m);
        snPoints.push_back({ Δσ2, N2, m });
    }
    snPoints.erase(snPoints.begin()); // remove dummy reference point

    // loop counts
    for (int i = 0; i < results.rowCount(); ++i) {
        double ni = rainflow.at(i, 0);
        double Δσ = rainflow.at(i, 1)*stressFactor;

        // compute endurance and damage
        double Di = 0.0;
        double Ni = std::numeric_limits<double>::infinity();
        for (auto [ΔσL, NL, m] : snPoints) {
            double _Ni = NL*std::pow(ΔσL/Δσ, m);
            if (_Ni < NL) {
                Ni = _Ni;
                Di = ni/Ni;
                break;
            }
        }

        // store results
        results.at(i, 0) = ni;
        results.at(i, 1) = Δσ;
        results.at(i, 2) = Ni;
        results.at(i, 3) = Di;
    }

    // done
    return results;

}

SnDetail::SnDetail(QObject* parent) : Detail(parent), _timeUnits("s"), _repCount(1.0), _customResistanceFactor(1.0),
    _stressFactor(1.0) {}

Detail::Approach SnDetail::approach() const { return Approach::Sn; }

std::optional<double> SnDetail::category() const { return _category; }

void SnDetail::setCategory(double category) {
    if (_category && category == *_category) return;
    if (category <= 0.0) throw std::invalid_argument("A positive value is required.");
    _category = category;
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

const Matrix<double>& SnDetail::slopes() const { return _slopes; }

void SnDetail::setSlopes(const Matrix<double>& slopes) {
    if (slopes == _slopes) return;
    if (slopes.rowCount() < 1 || slopes.columnCount() != 2)
        throw std::invalid_argument("At least 1 slope is required.");
    for (int i = 0; i < slopes.rowCount() - 1; ++i) {
        if (slopes.at(i + 1, 0) <= slopes.at(i, 0))
            throw std::invalid_argument("Slope parameters must be specified in ascending order.");
        if (slopes.at(i + 1, 1) <= slopes.at(i, 1))
            throw std::invalid_argument("Endurance limits must be specified in ascending order.");
    }
    if (slopes.at(0, 0) < 1)
        throw std::invalid_argument("Slope parameter must be greater than or equal to 1.");
    if (slopes.at(0, 1) < 2e6)
        throw std::invalid_argument("Endurance limit must be greater than or equal to 2 million cycles.");
    _slopes = slopes;
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

bool SnDetail::ignoreTime() const { return _ignoreTime; }

void SnDetail::setIgnoreTime(bool ignoreTime) {
    if (ignoreTime == _ignoreTime) return;
    _ignoreTime = ignoreTime;
    emit modified();
}

const QString& SnDetail::timeUnits() const { return _timeUnits; }

void SnDetail::setTimeUnits(const QString& timeUnits) {
    if (timeUnits == _timeUnits) return;
    _timeUnits = timeUnits.simplified();
    if (_timeUnits.isEmpty()) _timeUnits = "-";
    emit modified();
}

double SnDetail::repCount() const { return _repCount; }

void SnDetail::setRepCount(double repCount) {
    if (repCount == _repCount) return;
    if (repCount <= 0.0) throw std::invalid_argument("A positive value is required.");
    _repCount = repCount;
    if (_rainflow.rowCount() > 0) _rainflow = Matrix<double>(); // clear old results
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

const Matrix<double>& SnDetail::historySample() const { return _history; }

void SnDetail::setHistorySample(const Matrix<double>& history) {
    if (history == _history) return;
    if (history.rowCount() < 3 || history.columnCount() != 2)
        throw std::invalid_argument("At least 3 data points are required.");
    for (int i = 0; i < history.rowCount() - 1; ++i)
        if (history.at(i + 1, 0) <= history.at(i, 0))
            throw std::invalid_argument("Stress-time history must be specified in time-ascending order.");
    _extrema = findExtrema(history);
    _history = history;
    if (_rainflow.rowCount() > 0) _rainflow = Matrix<double>(); // clear old results
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

const std::vector<int>& SnDetail::historyExtrema() const { return _extrema; }

const Matrix<double>& SnDetail::rainflowCounts() const { return _rainflow; }

void SnDetail::executeRainflowCounting() {
    if (_history.rowCount() < 3 || _history.columnCount() != 2 || _extrema.size() <= 2)
        throw std::runtime_error("The stress-time history must first be specified.");
    _rainflow = executeRainflowCounting(_history, _extrema, _repCount);
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

SnDetail::DesignConcept SnDetail::designConcept() const { return _designConcept; }

void SnDetail::setDesignConcept(DesignConcept designConcept) {
    if (designConcept == _designConcept) return;
    _designConcept = designConcept;
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

SnDetail::ConsequenceOfFailure SnDetail::consequenceOfFailure() const { return _consequenceOfFailure; }

void SnDetail::setConsequenceOfFailure(ConsequenceOfFailure consequenceOfFailure) {
    if (consequenceOfFailure == _consequenceOfFailure) return;
    _consequenceOfFailure = consequenceOfFailure;
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

bool SnDetail::useCustomResistanceFactor() const { return _useCustomResistanceFactor; }

void SnDetail::setUseCustomResistanceFactor(bool value) {
    if (value == _useCustomResistanceFactor) return;
    _useCustomResistanceFactor = value;
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

double SnDetail::customResistanceFactor() const { return _customResistanceFactor; }

void SnDetail::setCustomResistanceFactor(double value) {
    if (value == _customResistanceFactor) return;
    if (value <= 0.0) throw std::invalid_argument("A positive value is required.");
    _customResistanceFactor = value;
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

double SnDetail::resistanceFactor() const {
    if (_useCustomResistanceFactor) return _customResistanceFactor;
    else switch (_designConcept) {
        case DesignConcept::SafeLife:
            switch (_consequenceOfFailure) {
                case ConsequenceOfFailure::LowConsequence:    return 1.15;
                case ConsequenceOfFailure::MediumConsequence: return 1.25;
                case ConsequenceOfFailure::HighConsequence:   return 1.35;
                default: throw std::logic_error("Case not implemented.");
            }
        case DesignConcept::DamageTolerant:
            switch (_consequenceOfFailure) {
                case ConsequenceOfFailure::LowConsequence:    return 1.00;
                case ConsequenceOfFailure::MediumConsequence: return 1.15;
                case ConsequenceOfFailure::HighConsequence:   return 1.25;
                default: throw std::logic_error("Case not implemented.");
            }
        default:
            throw std::logic_error("Case not implemented.");
    }
}

double SnDetail::stressFactor() const { return _stressFactor; }

void SnDetail::setStressFactor(double value) {
    if (value == _stressFactor) return;
    if (value <= 0.0) throw std::invalid_argument("A positive value is required.");
    _stressFactor = value;
    if (_damage.rowCount() > 0) _damage = Matrix<double>(); // clear old results
    emit modified();
}

const Matrix<double>& SnDetail::damageCounts() const { return _damage; }

void SnDetail::executeMinerSummation() {
    if (!_category || _slopes.rowCount() == 0 || _slopes.columnCount() != 2)
        throw std::runtime_error("The S-N curve must first be specified.");
    if (_rainflow.rowCount() == 0 || _rainflow.columnCount() != 5)
        throw std::runtime_error("The rainflow counting algorithm must first be executed.");
    _damage = executeMinerSummation(*_category, resistanceFactor(), _slopes, _rainflow, _stressFactor);
    emit modified();
}
