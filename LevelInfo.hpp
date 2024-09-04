#pragma once

#include "Using.hpp"

struct LevelInfo{
    Price price_;
    Quantity quantity_;
};

/**
 * @typedef LevelInfos
 * @brief A collection of LevelInfo structs.
 * 
 * Defined as a vector of LevelInfo and is used to for
 * multiple price levels in the order book.
 */
using LevelInfos = std::vector<LevelInfo>;