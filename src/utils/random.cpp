/*!
 * \file src/utils/random.cpp
 * \brief Random number generation
 *
 * \author xythobuz
 */

#include <chrono>
#include <map>
#include <random>
#include <utility>

#include "global.h"
#include "utils/random.h"

static std::map<int, std::uniform_int_distribution<int>> distributions;
static std::default_random_engine engine;
static bool engineIsSeeded = false;

int randomInteger(int max, int min) {
    if (max == min)
        return max;

    orAssertGreaterThan(max, min);

    if (!engineIsSeeded) {
        engine.seed(std::chrono::system_clock::now().time_since_epoch().count());
        engineIsSeeded = true;
    }

    int range = max - min;
    auto elem = distributions.find(range);
    if (elem == distributions.end()) {
        distributions[range] = std::uniform_int_distribution<int>(0, range);
        return distributions[range](engine) + min;
    } else {
        return std::get<1>(*elem)(engine) + min;
    }
}

