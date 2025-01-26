#include <cmath>
#include "Core/Utility.hpp"

std::pair<double, double> Utility::getHumanReadableRange(const std::pair<double, double>& range) {
    auto [min, max] = range;
    double magnitude = std::pow(10, std::floor(std::log10(max - min)));
    min = std::floor(min/magnitude)*magnitude;
    max = std::ceil(max/magnitude)*magnitude;
    return std::make_pair(min, max);
}
