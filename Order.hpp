#pragma once
#include <list>
#include <exception>
#include <sstream>

#include "OrderTypes.hpp"
#include "Side.hpp"
#include "Using.hpp"
#include "Constants.hpp"


/**
 * @class Order
 * @brief Represents an individual order in an order book.
 */
class Order{
    public:
    /**
     * @brief Order with the specified parameters.
     * 
     * @param orderType The type of the order (e.g., Market, GoodTillCancel).
     * @param orderId The unique identifier for the order.
     * @param side The side of the order (e.g., Buy or Sell).
     * @param price The price at which the order is placed.
     * @param quantity The initial quantity of the order.
     */
        Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity): orderType_ {orderType}, orderId_ {orderId}, side_ {side}, price_ {price}, initialQuantity_ {quantity}, remainingQuantity_ {quantity} {};

    /**
     * @brief Constructs a market order with the specified parameters.
     * 
     * @param orderId The unique identifier for the order.
     * @param side The side of the order (e.g., Buy or Sell).
     * @param quantity The initial quantity of the order.
     * 
     * @note This constructor sets the order type to Market and uses an invalid price.
     */
        Order(OrderId orderId, Side side, Quantity quantity): Order (OrderType::Market, orderId, side, Constants::InvalidPrice, quantity){}

    /**
     * @brief Id of the order.
     * 
     * @return The order ID.
     */
        OrderId GetOrderId() const {return orderId_;}

    /**
     * @brief Side of the order (e.g., Buy or Sell).
     * 
     * @return The side of the order.
     */
        Side GetSide() const {return side_;}

    /**
     * @brief Price of the order.
     * 
     * @return Price of the order.
     */
        Price GetPrice() const {return price_;}

    /**
     * @brief Type of the order.
     * 
     * @return The type of the order.
     */
        OrderType GetOrderType() const {return orderType_;}

    /**
     * @brief Initial quantity of the order when created.
     * 
     * @return Initial quantity of the order.
     */
        Quantity GetInitialQuantity() const {return initialQuantity_;}

    /**
     * @brief Remaining quantity of the order not filled.
     * 
     * @return Remaining quantity of the order.
     */
        Quantity GetRemainingQuantity() const {return remainingQuantity_;}

    /**
     * @brief Gets the filled quantity of the order.
     * 
     * @return The quantity of the order that has been filled.
     */
        Quantity GetFilledQuantity() const {return GetInitialQuantity() - GetRemainingQuantity();}

    /**
     * @brief Checks if the order is completely filled.
     * 
     * @return Remaining Quality to be 0
     */
        bool IsFilled() const { return GetRemainingQuantity() == 0;}

    /**
     * @brief Fills the order by a specified quantity.
     * 
     * @param quantity The quantity to fill.
     * 
     * @throws std::logic_error if the fill quantity exceeds the remaining quantity.
     */
        void Fill(Quantity quantity) {
            if (quantity > GetRemainingQuantity()) {
                std::stringstream ss;
                ss << "Order (" << GetOrderId() << ") cannot be filled for more than its remaining quantity.";
                throw std::logic_error(ss.str());
            }
            remainingQuantity_ -= quantity;
        }

    /**
     * @brief Convert market order to a GoodTillCancel order with the specified price.
     * 
     * @param price The new price for the GoodTillCancel order.
     * 
     * @throws std::logic_error if the order is not a market order.
     */
        void ToGoodTillCancel(Price price) {
            if (GetOrderType() != OrderType::Market) {
                std::stringstream ss;
                ss << "Order (" << GetOrderId() << ") cannot have its price adjusted, only market orders can.";
                throw std::logic_error(ss.str());
            }
            price_ = price;
            orderType_ = OrderType::GoodTillCancel;
        }
    private:
        OrderType orderType_;
        OrderId orderId_;
        Side side_;
        Price price_;
        Quantity initialQuantity_;
        Quantity remainingQuantity_;
};
 
using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std:: list<OrderPointer>;