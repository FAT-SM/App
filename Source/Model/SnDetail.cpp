#include <stdexcept>
#include "Model/SnDetail.hpp"

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
    _extrema = Utility::findExtrema(history);
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
    _rainflow = Utility::executeRainflowCounting(_history, _extrema, _repCount);
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
    _damage = Utility::executeMinerSummation(*_category, resistanceFactor(), _slopes, _rainflow, _stressFactor);
    emit modified();
}
