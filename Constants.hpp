#pragma once
#include <limits> 
#include "Using.hpp"

struct Constants{
    /**
     * @brief Represents an invalid price.
     * 
     * Uses the quiet NaN (Not a Number) value for the Price.
     * Indicates an invalid/uninitialized price.
     */
    static const Price InvalidPrice = std::numeric_limits<Price>::quiet_NaN();
};