#pragma once

#include "Order.hpp"

class OrderModify
{
public:
    /**
     * @brief Constructs an OrderModify object with the specified parameters.
     * 
     * @param orderId Unique identifier for the order.
     * @param side The side of the order (buy/sell).
     * @param price The price of the order.
     * @param quantity The quantity of the order.
     */
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
        : orderId_{ orderId }
        , price_{ price }
        , side_{ side }
        , quantity_{ quantity }
    {}

    /**
     * @brief Gets the  id of the order.
     * 
     * @return The  id of the order.
     */
    OrderId GetOrderId() const { return orderId_; }

    /**
     * @brief Gets the price of the order.
     * 
     * @return The price of the order.
     */
    Price GetPrice() const { return price_; }

    /**
     * @brief Gets the side of the order (buy/sell).
     * 
     * @return The side of the order.
     */
    Side GetSide() const { return side_; }

    /**
     * @brief Gets the quantity of the order.
     * 
     * @return The quantity of the order.
     */
    Quantity GetQuantity() const { return quantity_; }

    /**
     * @brief Converts this OrderModify obj to a shared pointer to an Order obj.
     * 
     * This function creates a new Order object with the current state of OrderModify
     * returning it as a shared pointer to it.
     * 
     * @param type The type of the order (example: limit, market).
     * 
     * @return A shared pointer to an Order obj with the same attributes as          * OrderModify.
     */
    OrderPointer ToOrderPointer(OrderType type) const{
        return std::make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    }

private:
    OrderId orderId_;
    Price price_;
    Side side_;
    Quantity quantity_;
};