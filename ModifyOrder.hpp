#pragma once

#include "Order.hpp"

/**
 * @class OrderModify
 * @brief Represents a modification to an existing order, including updates to price, quantity, or side.
 */
class OrderModify{
    public:
    /**
     * @brief Constructs an OrderModify obj with the specified order details.
     * 
     * @param orderId The unique identifier of the order.
     * @param side The side of the order (Buy or Sell).
     * @param price The price of the order.
     * @param quantity The quantity of the order.
     */
        OrderModify(OrderId orderId, Side side, Price price, Quantity quantity) : orderId_ {orderId}, side_ {side}, price_ {price}, quantity_ {quantity} {};

    /**
     * @brief Gets the order ID.
     * 
     * @return The order id
     */
        OrderId GetOrderId() const {return orderId_;}

    /**
     * @brief Gets the side of the order.
     * 
     * @return The side of the order (Buy or Sell).
     */
        Side GetSide() const {return side_;}
    
    /**
     * @brief Gets the price of the order.
     * 
     * @return The order price.
     */
        Price GetPrice() const {return price_;}

    /**
     * @brief Gets the quantity of the order.
     * 
     * @return The quantity of the order.
     */
        Quantity GetQuantity() const {return quantity_;}

     /**
     * @brief Converts the mod. data into a pointer to a new Order obj.
     * 
     * @param type The type of the order (ex. FillAndKill, GoodTillCancel).
     * @return A shared pointer to the new Order object.
     */
        OrderPointer ToOrderPointer(OrderType type) const {
            return std::make_shared<Order> (type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
        }

    private:
        OrderId orderId_;
        Price price_;
        Side side_;
        Quantity quantity_;
};

