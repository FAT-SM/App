#include <stdexcept>
#include "Model/SnDetail.hpp"

void SnDetail::findExtrema() {

    // clear previous and check if the stress-time history is available
    _extrema.clear(); _extrema.shrink_to_fit();
    if (_history.rowCount() < 3 || _history.columnCount() != 2) return;

    // consider the first point as a local extremum
    _extrema.push_back(0);

    // loop history points except first and last points (first and last points are always extrema)
    for (int i = 1; i < _history.rowCount() - 1; ++i) {

        // check if the current point is a local extremum
        if (
            (_history.at(i, 1) < _history.at(_extrema.back(), 1) && _history.at(i, 1) < _history.at(i + 1, 1)) ||
            (_history.at(i, 1) > _history.at(_extrema.back(), 1) && _history.at(i, 1) > _history.at(i + 1, 1))
        ) { _extrema.push_back(i); }

    }

    // consider the last point as a local extremum
    _extrema.push_back(_history.rowCount() - 1);

    // check if valid
    if (_extrema.size() <= 2) {
        _history = Matrix<double>();
        _extrema.clear(); _extrema.shrink_to_fit();
        throw std::runtime_error("Stress-time history does not have any load reversals.");
    }

}

SnDetail::SnDetail(QObject* parent) : Detail(parent), _timeUnits("s"), _repCount(1.0) {}

Detail::Approach SnDetail::approach() const { return Approach::Sn; }

std::optional<double> SnDetail::category() const { return _category; }

void SnDetail::setCategory(double category) {
    if (_category && category == *_category) return;
    if (category <= 0.0) throw std::invalid_argument("A positive value is required.");
    _category = category;
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
    if (slopes.at(0, 0) < 1) throw std::invalid_argument("Slope parameter must be greater than or equal to 1.");
    if (slopes.at(0, 1) <= 2e6) throw std::invalid_argument("Endurance limit must be greater than 2 million cycles.");
    _slopes = slopes;
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
    _history = history;
    findExtrema(); // may throw, in which case "_history" is cleared
    emit modified();
}

const std::vector<int>& SnDetail::historyExtrema() const { return _extrema; }
