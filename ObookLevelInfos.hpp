#pragma once

#include "LevelInfo.hpp"


/**
 * @class OrderbookLevelInfos
 * @brief Encapsulates the bid and ask levels of an order book.
 */
class OrderbookLevelInfos
{
public:
    /**
     * @brief  OrderbookLevelInfos obj with  specified bid and ask levels.
     * 
     * @param bids The bid levels in the order book.
     * @param asks The ask levels in the order book.
     */
    OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks)
        : bids_{ bids }
        , asks_{ asks }
    { }
    /**
     * @brief Gets the bid levels of the order book.
     * 
     * @return A constant reference to the bid levels.
     */
    const LevelInfos& GetBids() const { return bids_; }

    /**
     * @brief Gets the ask levels of the order book.
     * 
     * @return A constant reference to the ask levels.
     */
    const LevelInfos& GetAsks() const { return asks_; }

private:
    LevelInfos bids_;
    LevelInfos asks_;
};