#include "Orderbook.hpp"

#include <numeric>
#include <chrono>
#include <ctime>
#include <iostream>
#include <algorithm>

// Prune Good-For-Day orders after market hours
void Orderbook::PruneGoodForDayOrders(){
    using namespace std::chrono;
    const auto end = hours(16);

    while (true){
        const auto now = system_clock::now();
        const auto now_c = system_clock::to_time_t(now);
        std::tm now_parts;
        
		// Convert system time to local time safely depending on the platform
        #ifdef _WIN32
        localtime_s(&now_parts, &now_c);
        #else
        localtime_r(&now_c, &now_parts);
        #endif

		// If current time is past market close, set to next day
        if (now_parts.tm_hour >= end.count())
            now_parts.tm_mday += 1;

        now_parts.tm_hour = end.count();
        now_parts.tm_min = 0;
        now_parts.tm_sec = 0;

		// Convert tm structure back to time_point
        auto next = system_clock::from_time_t(mktime(&now_parts));
        if (next == system_clock::time_point{}) // Check for mktime failure
            continue;

        auto till = next - now + milliseconds(100);
        {
            std::unique_lock ordersLock{ ordersMutex_ };

		// Wait until either shutdown signal is received or the wait duration expires
            if (shutdown_.load(std::memory_order_acquire) ||
                shutdownConditionVariable_.wait_for(ordersLock, till) == std::cv_status::no_timeout)
                return;
        }

        OrderIds orderIds;
		{
            std::scoped_lock ordersLock{ ordersMutex_ };

			// Gather all Good-For-Day orders to be pruned
            for (const auto& [key, entry] : orders_){
                const auto& [order, _] = entry;

                if (order->GetOrderType() != OrderType::GoodForDay)
                    continue;

                orderIds.push_back(order->GetOrderId());
            }
        }

        CancelOrders(orderIds);
    }
}

// Cancel a list of orders
void Orderbook::CancelOrders(OrderIds orderIds){
	std::scoped_lock ordersLock{ ordersMutex_ };

	for (const auto& orderId : orderIds)
		CancelOrderInternal(orderId);
}

// Cancel an individual order
void Orderbook::CancelOrderInternal(OrderId orderId){
	if (!orders_.contains(orderId))
		return;

	const auto [order, iterator] = orders_.at(orderId);
	orders_.erase(orderId);

	// Remove order from bids or asks map depending on the order side
	if (order->GetSide() == Side::Sell){
		auto price = order->GetPrice();
		auto& orders = asks_.at(price);
		orders.erase(iterator);
		if (orders.empty())
			asks_.erase(price);
	}else{
		auto price = order->GetPrice();
		auto& orders = bids_.at(price);
		orders.erase(iterator);
		if (orders.empty())
			bids_.erase(price);
	}

	OnOrderCancelled(order);
}

//Update data when an order is cancelled
void Orderbook::OnOrderCancelled(OrderPointer order){
	UpdateLevelData(order->GetPrice(), order->GetRemainingQuantity(), LevelData::Action::Remove);
}

//Update data when an order is added
void Orderbook::OnOrderAdded(OrderPointer order){
	UpdateLevelData(order->GetPrice(), order->GetInitialQuantity(), LevelData::Action::Add);
}
//Update data when an order is matched
void Orderbook::OnOrderMatched(Price price, Quantity quantity, bool isFullyFilled){
	UpdateLevelData(price, quantity, isFullyFilled ? LevelData::Action::Remove : LevelData::Action::Match);
}

//Update level data for a price level based on action type
void Orderbook::UpdateLevelData(Price price, Quantity quantity, LevelData::Action action){
	auto& data = data_[price];

	data.count_ += action == LevelData::Action::Remove ? -1 : action == LevelData::Action::Add ? 1 : 0;
	if (action == LevelData::Action::Remove || action == LevelData::Action::Match){
		data.quantity_ -= quantity;
	}else{
		data.quantity_ += quantity;
	}

	if (data.count_ == 0)
		data_.erase(price);
}

// Checks if an order can be fully filled based on liquidity availibility 
bool Orderbook::CanFullyFill(Side side, Price price, Quantity quantity) const{
	if (!CanMatch(side, price))
		return false;

	std::optional<Price> threshold;

	if (side == Side::Buy){
		const auto [askPrice, _] = *asks_.begin();// Get the best ask price
		threshold = askPrice;
	}else{
		const auto [bidPrice, _] = *bids_.begin();// Get the best bid price
		threshold = bidPrice;
	}

	for (const auto& [levelPrice, levelData] : data_){
		// Skip prices that are not within the fillable range
		if (threshold.has_value() &&
			(side == Side::Buy && threshold.value() > levelPrice) ||
			(side == Side::Sell && threshold.value() < levelPrice))
			continue;

		if ((side == Side::Buy && levelPrice > price) ||
			(side == Side::Sell && levelPrice < price))
			continue;

		if (quantity <= levelData.quantity_)
			return true;

		quantity -= levelData.quantity_;
	}

	return false;
}

// Checks if order can be matched at the given price
bool Orderbook::CanMatch(Side side, Price price) const{
	if (side == Side::Buy){
		if (asks_.empty())
			return false;

		const auto& [bestAsk, _] = *asks_.begin(); //Best ask price
		return price >= bestAsk;
	}else{
		if (bids_.empty())
			return false;

		const auto& [bestBid, _] = *bids_.begin(); //best bid price
		return price <= bestBid;
	}
}

// Matches orders in the orderbook to generate trades
Trades Orderbook::MatchOrders(){
	Trades trades;
	trades.reserve(orders_.size()); // Pre-allocate space for trades

	while (true){
		if (bids_.empty() || asks_.empty())
			break;

		auto& [bidPrice, bids] = *bids_.begin();
		auto& [askPrice, asks] = *asks_.begin();

		if (bidPrice < askPrice)
			break;

		while (!bids.empty() && !asks.empty()){
			auto bid = bids.front();
			auto ask = asks.front();
		//Fill quantity is the minimum of remaining quantities of bid and ask
			Quantity quantity = std::min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity());

			bid->Fill(quantity);
			ask->Fill(quantity);

			// Remove fully filled bid
			if (bid->IsFilled()){
				bids.pop_front();
				orders_.erase(bid->GetOrderId());
			}

			// Remove fully filled ask
			if (ask->IsFilled()){
				asks.pop_front();
				orders_.erase(ask->GetOrderId());
			}

			//Record of trade details
			trades.push_back(Trade{
				TradeInfo{ bid->GetOrderId(), bid->GetPrice(), quantity },
				TradeInfo{ ask->GetOrderId(), ask->GetPrice(), quantity } 
				});

			OnOrderMatched(bid->GetPrice(), quantity, bid->IsFilled());
			OnOrderMatched(ask->GetPrice(), quantity, ask->IsFilled());
		}
        if (bids.empty()){
            bids_.erase(bidPrice);
            data_.erase(bidPrice);
        }
        if (asks.empty()){
            asks_.erase(askPrice);
            data_.erase(askPrice);
        }
	}
 	// Handle Fill-And-Kill orders for bids
	if (!bids_.empty()){
		auto& [_, bids] = *bids_.begin();
		auto& order = bids.front();
		if (order->GetOrderType() == OrderType::FillAndKill)
			CancelOrder(order->GetOrderId());
	}

 	// Handle Fill-And-Kill orders for asks
	if (!asks_.empty()){
		auto& [_, asks] = *asks_.begin();
		auto& order = asks.front();
		if (order->GetOrderType() == OrderType::FillAndKill)
			CancelOrder(order->GetOrderId());
	}

	return trades;
}

// Constructor 
Orderbook::Orderbook() : ordersPruneThread_{ [this] { PruneGoodForDayOrders(); } } { }

// Destructor 
Orderbook::~Orderbook(){
    shutdown_.store(true, std::memory_order_release);
	shutdownConditionVariable_.notify_one();
	ordersPruneThread_.join();
}


Trades Orderbook::AddOrder(OrderPointer order){
	std::scoped_lock ordersLock{ ordersMutex_ };

	if (orders_.contains(order->GetOrderId()))
		return { };

	// Market orders now Good-Till-Cancel if prices match in the order book.
	if (order->GetOrderType() == OrderType::Market){
		if (order->GetSide() == Side::Buy && !asks_.empty()){
			const auto& [worstAsk, _] = *asks_.rbegin();
			order->ToGoodTillCancel(worstAsk);
		}else if (order->GetSide() == Side::Sell && !bids_.empty()){
			const auto& [worstBid, _] = *bids_.rbegin();
			order->ToGoodTillCancel(worstBid);
		}else
			return { };
	}


	//Fill-And-Kill orders check if there is a matching price in the order book.
	if (order->GetOrderType() == OrderType::FillAndKill && !CanMatch(order->GetSide(), order->GetPrice()))
		return { };
	
	if (order->GetOrderType() == OrderType::FillOrKill && !CanFullyFill(order->GetSide(), order->GetPrice(), order->GetInitialQuantity()))
		return { };

	OrderPointers::iterator iterator;

	if (order->GetSide() == Side::Buy){
		auto& orders = bids_[order->GetPrice()];
		orders.push_back(order);
		iterator = std::prev(orders.end());
	}else{
		auto& orders = asks_[order->GetPrice()];
		orders.push_back(order);
		iterator = std::prev(orders.end());
	}

	// Insert the order into the main orders map for tracking by ID
	orders_.insert({ order->GetOrderId(), OrderEntry{ order, iterator } });
	
	OnOrderAdded(order);
	
	return MatchOrders();

}

//Cancel order
void Orderbook::CancelOrder(OrderId orderId){
	std::scoped_lock ordersLock{ ordersMutex_ };

	CancelOrderInternal(orderId);
}

//Modify order by canceling old and adding new order
Trades Orderbook::ModifyOrder(OrderModify order){
	OrderType orderType;
	{
		std::scoped_lock ordersLock{ ordersMutex_ };

		if (!orders_.contains(order.GetOrderId()))
			return { };

		const auto& [existingOrder, _] = orders_.at(order.GetOrderId());
		orderType = existingOrder->GetOrderType();
	}

	CancelOrder(order.GetOrderId());
	return AddOrder(order.ToOrderPointer(orderType));
}

std::size_t Orderbook::Size() const{
	std::scoped_lock ordersLock{ ordersMutex_ };
	return orders_.size(); 
}

OrderbookLevelInfos Orderbook::GetOrderInfos() const{
	LevelInfos bidInfos, askInfos;
	bidInfos.reserve(orders_.size());
	askInfos.reserve(orders_.size());

	//Structure that has the price level and the total quantity at that level.
	auto CreateLevelInfos = [](Price price, const OrderPointers& orders){
		return LevelInfo{ price, std::accumulate(orders.begin(), orders.end(), (Quantity)0,
			[](Quantity runningSum, const OrderPointer& order)
			{ return runningSum + order->GetRemainingQuantity(); }) };
	};

	//Populate bids
	for (const auto& [price, orders] : bids_)
		bidInfos.push_back(CreateLevelInfos(price, orders));

	//Populate asks
	for (const auto& [price, orders] : asks_)
		askInfos.push_back(CreateLevelInfos(price, orders));

	return OrderbookLevelInfos{ bidInfos, askInfos };

}