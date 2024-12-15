//HELLO
#include <iostream>
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <limits>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <variant>
#include <optional>
#include <tuple>
#include <format>

enum class OrderType {
    GoodTillCancel,
    FillAndKill
};

enum class Side {
    Buy,
    Sell
};

using Price = std::int32_t; // (negative or non-negative) price
using Quantity = std::uint32_t; //non neg int
using OrderId = std::uint64_t; // non neg int

struct LevelInfo {
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>; //alias the vector version

//represent order book sides

class OrderBookLevelInfos {
    public:
        OrderBookLevelInfos(const LevelInfos& bids, const LevelInfos& asks)
        : bids_ (bids), 
        asks_ (asks)
        { }

        const LevelInfos& GetBids() const { return bids_; }
        const LevelInfos& GetAsks() const { return asks_; }

    private:
        LevelInfos bids_;
        LevelInfos asks_;
};

class Order {
    public:
        Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
            : orderType_( orderType ),
            orderId_( orderId ),
            side_( side ),
            price_( price ),
            initialQuantity_( quantity ), //intital quanity because the quanity is likely to be modified
            remainingQuantity_( quantity )
        { }
        
        OrderType GetOrderType() const { return orderType_; }
        OrderId GetOrderId() const { return orderId_; }
        Side GetSide() const { return side_; }
        Price GetPrice() const { return price_; }
        Quantity GetInitiatalQuantity() const { return initialQuantity_; }
        Quantity GetRemainingQuantity() const { return remainingQuantity_; }
        Quantity GetFillQuantity() const { return GetInitiatalQuantity() - GetRemainingQuantity(); }

        void fill(Quantity quantity) {
            if (quantity > GetRemainingQuantity())
                throw std::logic_error("Order (" + std::to_string(GetOrderId()) + 
                    ") cannot be filled for more than its remaining quantity.");

            remainingQuantity_ -= quantity;
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
using OrderPointers = std::list<OrderPointer>; // using a list rather than a vector for simplicity in this basic project (room for industry improvement)

class OrderModify {
    public:
        OrderModify (OrderId orderId, Side side, Price price, Quantity quantity)
        : orderId_ (orderId),
        side_ (side),
        price_ (price),
        quantity_ (quantity)
    { }

    OrderId GetOrderId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    Quantity GetQuantity() const { return quantity_; }

    OrderPointer ToOrderPointer(OrderType type) const {
        return std::make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    }

    private:
        OrderId orderId_;
        Side side_;
        Price price_;
        Quantity quantity_;
};

//what happens when an order is matched? create trades (bids and asks)
struct TradeInfo {
    OrderId orderId_;
    Price price_;
    Quantity quantity_;
};

class Trade { // trade class (bids and asks)
    public:
        Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
        : bidTrade_ (bidTrade),
        askTrade_ (askTrade)
        { }

    const TradeInfo& GetBidTrade() const { return bidTrade_;}
    const TradeInfo& GetAskTrade() const { return askTrade_;}

    private:
    TradeInfo bidTrade_;
    TradeInfo askTrade_;
};

using Trades = std::vector<Trade>; //vector containing multiple trades to sweep and execute at once

class OrderBook {
    private:

        struct OrderEntry {
            OrderPointer order_ { nullptr };
            OrderPointers::iterator location_;
        };

        /*future consideration to use unordered maps instead to make thread-safe and prevent data races
         */
        std::map<Price, OrderPointers, std::greater<Price>> bids_;
        std::map<Price, OrderPointers, std::less<Price>> asks_;
        std::unordered_map<OrderId, OrderEntry> orders_;

        bool CanMatch(Side side, Price price) const {
            if (asks_.empty()) {
                return false;

                const auto& [bestAsk_, _] = *asks_.begin();
                return price >= bestAsk_;
            }
        }
};

int main() {

    return 0;
}
