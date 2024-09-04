#pragma once

/**
 * @enum OrderType
 * @brief Various types of orders that can be placed in the order book.
 */
enum class OrderType
{
	GoodTillCancel, ///< Order  active until it is either filled or cancelled.
    FillAndKill,   ///< Fill as much as possible immediately and cancel remainder.
    FillOrKill,   ///< Fully fill immediately, or it is cancelled entirely.
    GoodForDay,  ///< Valid only for the trading day, then cancelled at EOD if not filled.
    Market,    ///< Order to buy or sell immediately at the best available price.
};