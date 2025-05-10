#include "Model/LefmDetail.hpp"

LefmDetail::LefmDetail(QObject* parent) : Detail(parent), _timeUnits("s"), _repCount(1.0) {}

Detail::Approach LefmDetail::approach() const { return Approach::Lefm; }

bool LefmDetail::ignoreTime() const { return _ignoreTime; }

void LefmDetail::setIgnoreTime(bool ignoreTime) {
    if (ignoreTime == _ignoreTime) return;
    _ignoreTime = ignoreTime;
    emit modified();
}

const QString& LefmDetail::timeUnits() const { return _timeUnits; }

void LefmDetail::setTimeUnits(const QString& timeUnits) {
    if (timeUnits == _timeUnits) return;
    _timeUnits = timeUnits.simplified();
    if (_timeUnits.isEmpty()) _timeUnits = "-";
    emit modified();
}

double LefmDetail::repCount() const { return _repCount; }

void LefmDetail::setRepCount(double repCount) {
    if (repCount == _repCount) return;
    if (repCount <= 0.0) throw std::invalid_argument("A positive value is required.");
    _repCount = repCount;
    if (_rainflow.rowCount() > 0) _rainflow = Matrix<double>(); // clear old results
    // TO DO: CLEAR RAINFLOW-BASED RESULTS
    emit modified();
}

const Matrix<double>& LefmDetail::historySample() const { return _history; }

void LefmDetail::setHistorySample(const Matrix<double>& history) {
    if (history == _history) return;
    if (history.rowCount() < 3 || history.columnCount() != 2)
        throw std::invalid_argument("At least 3 data points are required.");
    for (int i = 0; i < history.rowCount() - 1; ++i)
        if (history.at(i + 1, 0) <= history.at(i, 0))
            throw std::invalid_argument("Stress-time history must be specified in time-ascending order.");
    _extrema = Utility::findExtrema(history);
    _history = history;
    if (_rainflow.rowCount() > 0) _rainflow = Matrix<double>(); // clear old results
    // TO DO: CLEAR RAINFLOW-BASED RESULTS
    emit modified();
}

const std::vector<int>& LefmDetail::historyExtrema() const { return _extrema; }

const Matrix<double>& LefmDetail::rainflowCounts() const { return _rainflow; }

void LefmDetail::executeRainflowCounting() {
    if (_history.rowCount() < 3 || _history.columnCount() != 2 || _extrema.size() <= 2)
        throw std::runtime_error("The stress-time history must first be specified.");
    _rainflow = Utility::executeRainflowCounting(_history, _extrema, _repCount);
    // TO DO: CLEAR RAINFLOW-BASED RESULTS
    emit modified();
}
