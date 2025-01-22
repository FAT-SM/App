#include <stdexcept>
#include "Model/SnDetail.hpp"

SnDetail::SnDetail(QObject* parent) : Detail(parent) {}

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
