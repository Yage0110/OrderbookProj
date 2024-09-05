#include "test_include.hpp"


namespace googletest = ::testing;

/**
 * @class OrderbookTestFixture
 * @brief Test fixture for tests related to the Orderbook class.
 */
class OrderbookTestFixture : public googletest::TestWithParam<const char*> {
private:
    const static inline std::filesystem::path Root{std::filesystem::current_path()};
    const static inline std::filesystem::path TestFolder{"TestFiles"};
public:
    const static inline std::filesystem::path TestFolderPath{Root / TestFolder};
};

/**
 * @brief Tests the functionality of the Orderbook class.
 * @param file The name of the test file to use.
 */
TEST_P(OrderbookTestFixture, OrderbookTestSuite) {
    const auto file = OrderbookTestFixture::TestFolderPath / GetParam();

    InputHandle handle;
    const auto [updates, result] = handle.GetInfos(file);

    auto GetOrder = [] (const Info& info) {
        return std::make_shared<Order>(
            info.orderType_,
            info.orderId_,
            info.side_,
            info.price_,
            info.quantity_
        );
    };

    auto GetOrderModify = [] (const Info& info) {
        return OrderModify(
            info.orderId_,
            info.side_,
            info.price_,
            info.quantity_
        );
    };

    Orderbook orderbook;
    for (const auto& update : updates) {
        switch (update.type_) {
            case ActionType::Add: {
                const Trades& trades = orderbook.AddOrder(GetOrder(update));
            }
            break;
            case ActionType::Modify: {
                const Trades& trades = orderbook.ModifyOrder(GetOrderModify(update));
            }
            break;
            case ActionType::Cancel: {
                orderbook.CancelOrder(update.orderId_);
            }
            break;
            default:
                throw std::logic_error("Unsupported Update");
        }
    }

    const auto& orderbookInfos = orderbook.GetOrderInfos();
    ASSERT_EQ(orderbook.Size(), result.allCount_);
    ASSERT_EQ(orderbookInfos.GetBids().size(), result.bidCount_);
    ASSERT_EQ(orderbookInfos.GetAsks().size(), result.askCount_);
}

/**
 * @brief Instantiates the test suite with various test files.
 */
INSTANTIATE_TEST_SUITE_P(
    Tests,
    OrderbookTestFixture,
    googletest::ValuesIn({
        "Match_GoodTillCancel.txt",
        "Match_FillAndKill.txt",
        "Match_FillOrKill_Hit.txt",
        "Match_FillOrKill_Miss.txt",
        "Cancel_Success.txt",
        "Modify_Side.txt",
        "Match_Market.txt",
        "Invalid_Order_Line.txt",
        "Wrong_Line_Result.txt"
    })
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
