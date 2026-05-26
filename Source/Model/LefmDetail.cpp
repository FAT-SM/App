#include <cmath>
#include <numbers>
#include <stdexcept>
#include "Model/LefmDetail.hpp"

const QStringList& LefmDetail::availableDetails() {
    static const QStringList details {
        "IIW - Cruciform joint K-butt weld",
    };
    return details;
}

const std::vector<std::tuple<QString, QString, QString>>& LefmDetail::parameters(const QString& detail) {
    static const std::unordered_map<QString, std::vector<std::tuple<QString, QString, QString>>> map {
        { "IIW - Cruciform joint K-butt weld", {
            { "w", "Out-of-plane width",         "mm"  },
            { "H", "Fillet weld leg length",     "mm"  },
            { "T", "Attachment plate thickness", "mm"  },
            { "t", "Main plate thickness",       "mm"  },
            { "W", "Fillet weld leg length",     "mm"  },
            { "A", "Weld throat size",           "mm"  },
            { "θ", "Weld angle",                 "deg" },
        } },
    };
    if (auto it = map.find(detail); it != map.end()) return it->second;
    else throw std::invalid_argument("Unexpected LEFM constructional detail selection.");
}

LefmDetail::LefmDetail(QObject* parent) : Detail(parent), _timeUnits("s"), _repCount(1.0), _initialCrackSample(5, 1) {
    _initialCrackSample.at(0) =  0.5;
    _initialCrackSample.at(1) =  1.0;
    _initialCrackSample.at(2) =  5.0;
    _initialCrackSample.at(3) = 10.0;
    _initialCrackSample.at(4) = 50.0;
}

Detail::Approach LefmDetail::approach() const { return Approach::Lefm; }

const QString& LefmDetail::selectedDetail() const { return _selectedDetail; }

void LefmDetail::setSelectedDetail(const QString& detail) {
    if (detail == _selectedDetail) return;
    if (!detail.isEmpty() && !availableDetails().contains(detail))
        throw std::invalid_argument("Unexpected LEFM constructional detail selection.");
    _selectedDetail = detail;
    _paramValues.reset();
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

double LefmDetail::parameterValue(const QString& symbol) const {
    if (!_paramValues.has_value()) throw std::logic_error("Detail parameters not set.");
    if (auto it = (*_paramValues).find(symbol); it != (*_paramValues).end()) return it->second;
    else throw std::invalid_argument("Invalid parameter symbol.");
}

void LefmDetail::setParameterValues(const std::unordered_map<QString, double>& params) {
    try {
        if (_selectedDetail == "IIW - Cruciform joint K-butt weld") {
            double w = params.at("w");                         // Out-of-plane width, mm
            double H = params.at("H");                         // Fillet weld leg length, mm
            double T = params.at("T");                         // Attachment plate thickness, mm
            double t = params.at("t");                         // Main plate thickness, mm
            double W = params.at("W");                         // Fillet weld leg length, mm
            double θ = std::atan(H/W)*180.0/ std::numbers::pi; // Weld angle, deg
            double A = H * std::cos(θ*std::numbers::pi/180.0); // Weld throat size, mm
            if (A <= 0.0) throw std::invalid_argument("'A' must be positive.");
            if (w <= 0.0) throw std::invalid_argument("'w' must be positive.");
            if (H <= 0.0) throw std::invalid_argument("'H' must be positive.");
            if (T <= 0.0) throw std::invalid_argument("'T' must be positive.");
            if (t <= 0.0) throw std::invalid_argument("'t' must be positive.");
            if (W <= 0.0) throw std::invalid_argument("'W' must be positive.");
            if (θ < 15 || θ > 60) throw std::invalid_argument("'θ' must be between 15° and 60°.");
            if (H/T < 0.2 || H/T > 1.0) throw std::invalid_argument("'H/T' must be between 0.2 and 1.");
            if (W/T < 0.2 || W/T > 1.0) throw std::invalid_argument("'W/T' must be between 0.2 and 1.");
            if (A/T < 0.175 || A/T > 1.3) throw std::invalid_argument("'A/T' must be between 0.175 and 1.3.");
            if (t/T < 0.5 || t/T > 20) throw std::invalid_argument("'t/T' must be between 0.5 and 20.");
            _paramValues = {
                { "A", A },
                { "w", w },
                { "H", H },
                { "T", T },
                { "t", t },
                { "W", W },
                { "θ", θ },
            };
        }
        else throw std::logic_error("Not implemented.");
    }
    catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid parameter symbol.");
    }
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

bool LefmDetail::hasParameters() const { return _paramValues.has_value(); }

std::optional<double> LefmDetail::parisCoefficient() const { return _parisCoefficient; }

void LefmDetail::setParisCoefficient(double parisCoefficient) {
    if (_parisCoefficient && parisCoefficient == *_parisCoefficient) return;
    if (parisCoefficient <= 0.0) throw std::invalid_argument("A positive value is required.");
    _parisCoefficient = parisCoefficient;
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

std::optional<double> LefmDetail::parisExponent() const { return _parisExponent; }

void LefmDetail::setParisExponent(double parisExponent) {
    if (_parisExponent && parisExponent == *_parisExponent) return;
    if (parisExponent <= 0.0) throw std::invalid_argument("A positive value is required.");
    _parisExponent = parisExponent;
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

std::optional<double> LefmDetail::sifThreshold() const { return _sifThreshold; }

void LefmDetail::setSifThreshold(double sifThreshold) {
    if (_sifThreshold && sifThreshold == *_sifThreshold) return;
    if (sifThreshold <= 0.0) throw std::invalid_argument("A positive value is required.");
    if (_sifCritical && sifThreshold >= *_sifCritical)
        throw std::invalid_argument("Threshold ΔK must be less than critical ΔK.");
    _sifThreshold = sifThreshold;
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

std::optional<double> LefmDetail::sifCritical() const { return _sifCritical; }

void LefmDetail::setSifCritical(double sifCritical) {
    if (_sifCritical && sifCritical == *_sifCritical) return;
    if (sifCritical <= 0.0) throw std::invalid_argument("A positive value is required.");
    if (_sifThreshold && sifCritical <= *_sifThreshold)
        throw std::invalid_argument("Critical ΔK must be greater than threshold ΔK.");
    _sifCritical = sifCritical;
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

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
    _lifeSample.reset(); _stopCriteria.reset();
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
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

const std::vector<int>& LefmDetail::historyExtrema() const { return _extrema; }

const Matrix<double>& LefmDetail::rainflowCounts() const { return _rainflow; }

void LefmDetail::executeRainflowCounting() {
    if (_history.rowCount() < 3 || _history.columnCount() != 2 || _extrema.size() <= 2)
        throw std::runtime_error("The stress-time history must first be specified.");
    _rainflow = Utility::executeRainflowCounting(_history, _extrema, _repCount);
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

const Matrix<double>& LefmDetail::initialCrackSample() const { return _initialCrackSample; }

void LefmDetail::setInitialCrackSample(const Matrix<double>& sample) {
    if (sample == _initialCrackSample) return;
    if (sample.size() < 1) throw std::invalid_argument("At least 1 value is required.");
    for (int i = 0; i < sample.size() - 1; ++i)
        if (sample.at(i + 1) <= sample.at(i))
            throw std::invalid_argument("Values must be specified in ascending order.");
    if (sample.at(0) <= 0) throw std::invalid_argument("Positive values are required.");
    _initialCrackSample = sample;
    _lifeSample.reset(); _stopCriteria.reset();
    emit modified();
}

const std::optional<Matrix<double>>& LefmDetail::lifeSample() const { return _lifeSample; }

const std::optional<Matrix<QString>>& LefmDetail::stopCriteria() const { return _stopCriteria; }

void LefmDetail::computeRemainingFatigueLife() {
    if (_selectedDetail.isEmpty()) throw std::runtime_error("A constructional detail must first be selected.");
    if (!_paramValues) throw std::runtime_error("Constructional detail parameters must first be specified.");
    if (!_parisCoefficient || !_parisExponent) throw std::runtime_error("Missing Paris' law parameters.");
    if (!_sifThreshold || !_sifCritical) throw std::runtime_error("Missing fatigue crack growth parameters.");
    if (_rainflow.rowCount() == 0 || _rainflow.columnCount() != 5)
        throw std::runtime_error("The rainflow counting algorithm must first be executed.");
    _lifeSample = Matrix<double>(_initialCrackSample.size(), 1);
    _stopCriteria = Matrix<QString>(_initialCrackSample.size(), 1);
    for (int i = 0; i < _initialCrackSample.size(); ++i) {
        double a0 = _initialCrackSample.at(i);
        auto [rfl, stop] = Utility::computeRemainingFatigueLife(_selectedDetail, *_paramValues, _history, _rainflow,
            *_parisCoefficient, *_parisExponent, *_sifThreshold, *_sifCritical, a0);
        (*_lifeSample).at(i) = rfl;
        (*_stopCriteria).at(i) = stop;
    }
    emit modified();
}

std::pair<double, QString> LefmDetail::computeRemainingFatigueLife(double a0) {
    if (_selectedDetail.isEmpty()) throw std::runtime_error("A constructional detail must first be selected.");
    if (!_paramValues) throw std::runtime_error("Constructional detail parameters must first be specified.");
    if (!_parisCoefficient || !_parisExponent) throw std::runtime_error("Missing Paris' law parameters.");
    if (!_sifThreshold || !_sifCritical) throw std::runtime_error("Missing fatigue crack growth parameters.");
    if (_rainflow.rowCount() == 0 || _rainflow.columnCount() != 5)
        throw std::runtime_error("The rainflow counting algorithm must first be executed.");
    if (a0 <= 0) throw std::invalid_argument("A positive value is required.");
    return Utility::computeRemainingFatigueLife(_selectedDetail, *_paramValues, _history, _rainflow, *_parisCoefficient,
        *_parisExponent, *_sifThreshold, *_sifCritical, a0);
}
