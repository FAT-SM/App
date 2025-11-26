#include <tuple>
#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>
#include "Core/Utility.hpp"

std::pair<double, double> Utility::getHumanReadableRange(const std::pair<double, double>& range) {
    auto [min, max] = range;
    double magnitude = std::pow(10, std::floor(std::log10(max - min)));
    min = std::floor(min/magnitude)*magnitude;
    max = std::ceil(max/magnitude)*magnitude;
    return std::make_pair(min, max);
}

std::vector<int> Utility::findExtrema(const Matrix<double>& history) {

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

Matrix<double> Utility::executeRainflowCounting(const Matrix<double>& history, const std::vector<int>& extrema,
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

Matrix<double> Utility::executeMinerSummation(double category, double resistanceFactor, const Matrix<double>& slopes,
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

std::pair<double, QString> Utility::computeRemainingFatigueLife(const QString& detail,
    const std::unordered_map<QString, double>& parameters, const Matrix<double>& history,
    const Matrix<double>& rainflow, double C, double m, double ΔKth, double ΔKcr, double a0) {
    double sampledTimePeriod = (history.at(history.rowCount() - 1, 0) - history.at(0, 0))/(60*60*24*365.2425); // years
    auto c = rainflow.getColumn(0);
    auto Δσ = rainflow.getColumn(1);
    if (detail == "IIW - Cruciform joint K-butt weld") {
        double w = parameters.at("w"); // Out-of-plane width         [mm]
        double H = parameters.at("H"); // Fillet weld leg length     [mm]
        double T = parameters.at("T"); // Attachment plate thickness [mm]
        double t = parameters.at("t"); // Main plate thickness       [mm]
        double W = parameters.at("W"); // Fillet weld leg length     [mm]
        double A = parameters.at("A"); // Weld throat size           [mm]
        double θ = parameters.at("θ"); // Weld angle                 [deg]
        int i = 0;
        int N = 0;
        Matrix<double> ΔK(Δσ.size(), 1);
        double a = a0;
        double af = T;
        double rfl = 0.0; // years
        double dfl = 250.0; // years
        double π = std::numbers::pi;
        while (true) {
            double limit1 = (2*a)/w;
            if (a >= af) return { rfl, "a ≥ a_f" };
            if (ΔK.at(i) >= ΔKcr) return {rfl, "ΔK ≥ ΔK_cr"};
            if (limit1 >= 0.8) return { rfl, "(2*a)/w ≥ 0.8" };
            if (rfl >= dfl) return { dfl, "RFL ≥ 250 years" };
            double fw = std::sqrt(1.0/std::cos(π*a/w*std::sqrt(a/T)));
            double c1 =  0.7061 - 0.4091*H/T + 0.1596*std::pow(H/T, 2) + 0.3739*W/T - 0.1329*std::pow(W/T, 2);
            double k1 = -0.2434 - 0.3939*H/T + 0.1536*std::pow(H/T, 2) + 0.3004*W/T - 0.0995*std::pow(W/T, 2);
            double Mk = c1*std::pow(a/T, k1);
            double Mm = (1.04 + 0.202*std::pow(a/T, 2) - 0.106*std::pow(a/T, 4))/2.464;
            double Mb = (1.0 - 1.34*a/T - 0.03*std::pow(a/T, 2))*Mm;
            double Y = fw*Mk*Mm;
            ΔK.at(i) = Y * Δσ.at(i) * std::sqrt(π * a);
            if (ΔK.at(i) >= ΔKth) {
                double Δa = c.at(i)*C*std::pow(ΔK.at(i), m);
                a += Δa;
            }
            if (i < Δσ.size() - 1) {
                ++i;
            }
            else {
                if (ΔK.max() < ΔKth) return { dfl, "ΔK < ΔK_th" };
                ++N;
                i = 0;
            }
            rfl = sampledTimePeriod*N;
        }
    }
    throw std::logic_error("Not implemented.");
}
