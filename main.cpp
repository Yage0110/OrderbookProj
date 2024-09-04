#include "test_include.hpp"

int main() {
    InputHandle handle;
    std::filesystem::path inputFilePath;

    std::cout << "Enter the path to the input file: ";
    std::cin >> inputFilePath;

    try {
        // Get the updates and result from the file
        auto [updates, result] = handle.GetInfos(inputFilePath);

        Orderbook orderbook;
        for (const auto& update : updates) {
            switch (update.type_) {
                case ActionType::Add: { //Adds new order
                    const Trades& trades = orderbook.AddOrder(std::make_shared<Order>(
                        update.orderType_,
                        update.orderId_,
                        update.side_,
                        update.price_,
                        update.quantity_
                    ));
                }
                break;
                case ActionType::Modify: { //Modify Existing Order
                    const Trades& trades = orderbook.ModifyOrder(OrderModify(
                        update.orderId_,
                        update.side_,
                        update.price_,
                        update.quantity_
                    ));
                }
                break;
                case ActionType::Cancel: { //Cancel existing order
                    orderbook.CancelOrder(update.orderId_);
                }
                break;
                default:
                    throw std::logic_error("Unsupported Update");
            }
        }
        //Output what was found vs what was expected
        const auto& orderbookInfos = orderbook.GetOrderInfos();
        std::cout << "Orderbook Size: " << orderbook.Size() << std::endl;
        std::cout << "Bid Count: " << orderbookInfos.GetBids().size() << std::endl;
        std::cout << "Ask Count: " << orderbookInfos.GetAsks().size() << std::endl;
        std::cout << "Expected Total Orders: " << result.allCount_ << std::endl;
        std::cout << "Expected Bid Orders: " << result.bidCount_ << std::endl;
        std::cout << "Expected Ask Orders: " << result.askCount_ << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

