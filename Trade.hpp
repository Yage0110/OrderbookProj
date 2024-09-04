#pragma once 

#include "Using.hpp" 

/**
 * @struct TradeInfo
 * @brief Information of a trade, including order ID, price, and quantity.
 * Details about a single trade involving an order.
 */
struct TradeInfo
{
    OrderId orderId_;    ///< Unique id for the order .
    Price price_;        ///< The price at which the trade was executed.
    Quantity quantity_; ///< The quantity of the asset traded.
};

/**
 * @class Trade
 * @brief Trade between a bid and an ask.
 * Details of both the bid and ask sides of a trade.
 */
class Trade
{
public:
    /**
     * @brief Trade object with specified bid and ask info.
     * @param bidTrade Info for the bid side of the trade.
     * @param askTrade Info for the ask side of the trade.
     */
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
        : bidTrade_{ bidTrade }
        , askTrade_{ askTrade }
    { }

    /**
     * @brief Trade info for the bid side.
     * @return Const ref. to the TradeInfo obj representing the bid trade.
     */
    const TradeInfo& GetBidTrade() const { return bidTrade_; }

    /**
     * @brief Trade info for the ask side.
     * @return Const ref. to the TradeInfo obj representing the ask trade.
     */
    const TradeInfo& GetAskTrade() const { return askTrade_; }

private:
    TradeInfo bidTrade_; 
    TradeInfo askTrade_; 
};

/**
 * @typedef Trades
 * @brief Trade objs.
 * Alias represents a list of trades.
 */
using Trades = std::vector<Trade>;
