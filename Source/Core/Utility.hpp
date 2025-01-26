#pragma once

#include <utility>

/* Static class of utilities. */
class Utility {

private:

    /* Deleted constructor (static class). */
    Utility() = delete;

public:

    /* Returns a "human-readable" range. */
    static std::pair<double, double> getHumanReadableRange(const std::pair<double, double>& range);

};
