#pragma once

#include <map>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "Using.hpp"
#include "Order.hpp"
#include "Change.hpp"
#include "ObookLevelInfos.hpp"
#include "Trade.hpp"

/**
 * @class Orderbook
 * @brief Manages a collection of buy and sell orders, matching them to execute trades.
 */
class Orderbook{
private:

    /**
     * @struct OrderEntry
     * @brief Entry in the orders map, storing the order and its location in the bid/ask list.
     */
    struct OrderEntry{
        OrderPointer order_{ nullptr };
        OrderPointers::iterator location_;
    };

    /**
     * @struct LevelData
     * @brief Data at each price level, like the total quantity and order count.
     */
    struct LevelData{
        Quantity quantity_{ };
        Quantity count_{ };

        /**
         * @enum Action
         * @brief Actions that affect level data (adding, removing, or matching) orders.
         */
        enum class Action{
            Add,
            Remove,
            Match,
        };
    };

    std::unordered_map<Price, LevelData> data_;
    std::map<Price, OrderPointers, std::greater<Price>> bids_;
    std::map<Price, OrderPointers, std::less<Price>> asks_;
    std::unordered_map<OrderId, OrderEntry> orders_;
    mutable std::mutex ordersMutex_;
    std::thread ordersPruneThread_;
    std::condition_variable shutdownConditionVariable_;
    std::atomic<bool> shutdown_{ false };

     // Prune Good-for-Day orders that are no longer valid.
    void PruneGoodForDayOrders();

    /**
     * @brief Cancel multiple orders given their IDs.
     * @param orderIds A list of IDs to be canceled.
     */
    void CancelOrders(OrderIds orderIds);

    /**
     * @brief Internal function to handle the cancellation of a single order.
     * @param orderId The ID of the order to be canceled.
     */
    void CancelOrderInternal(OrderId orderId);

    /**
     * @brief Called when an order is cancelled.
     * @param order Pointer to the order that was cancelled.
     */
    void OnOrderCancelled(OrderPointer order);

    /**
     * @brief Called when a new order is added to the order book.
     * @param order Pointer to the new added order.
     */
    void OnOrderAdded(OrderPointer order);

    /**
     * @brief Order is matched and executed, providing details of the match.
     * @param price Price at which the order was matched.
     * @param quantity Quantity of the matched order.
     * @param isFullyFilled If the order was fully filled.
     */
    void OnOrderMatched(Price price, Quantity quantity, bool isFullyFilled);

    /**
     * @brief Updates level data for a specific price when an action occurs.
     * @param price Price level affected.
     * @param quantity Quantity associated with the action.
     * @param action Action performed (add, remove, match).
     */
    void UpdateLevelData(Price price, Quantity quantity, LevelData::Action action);

     /**
     * @brief Order can be fully filled at a given price and quantity.
     * @param side Side of the order (buy/sell).
     * @param price Price at which to check for fill capability.
     * @param quantity Quantity to be filled.
     * @return True / false.
     */
    bool CanFullyFill(Side side, Price price, Quantity quantity) const;

    /**
     * @brief Order can match at the specified price.
     * @param side Side of the order (buy/sell).
     * @param price Price at which to check for match.
     * @return True / false.
     */
    bool CanMatch(Side side, Price price) const;
    Trades MatchOrders();

public:

    Orderbook();
    //Disable move and copy assignment opperator and constructors 
    Orderbook(const Orderbook&) = delete;
    void operator=(const Orderbook&) = delete;
    Orderbook(Orderbook&&) = delete;
    void operator=(Orderbook&&) = delete;
    ~Orderbook();

      /**
     * @brief Adds order to the order book,  returns any resulting trades.
     * @param order Pointer to the order added.
     * @return Trades resulting from the new order.
     */
    Trades AddOrder(OrderPointer order);

    /**
     * @brief Cancels an existing order given its  ID.
     * @param orderId The ID of the order canceling.
     */
    void CancelOrder(OrderId orderId);

    /**
     * @brief Modify existing order and returns resulting trades.
     * @param order Order mod. details.
     * @return Trades resulting from the modified order.
     */
    Trades ModifyOrder(OrderModify order);
    /**
     * @brief Number of active orders in the order book.
     * @return Total number of orders.
     */
    std::size_t Size() const;
    OrderbookLevelInfos GetOrderInfos() const;



};