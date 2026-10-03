#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

struct Order {
    std::uint64_t id = 0;
    bool is_buy = true;
    std::int64_t price = 0;
    std::int64_t qty = 0;
};

struct Trade {
    std::uint64_t buy_id = 0;
    std::uint64_t sell_id = 0;
    std::int64_t price = 0;
    std::int64_t qty = 0;
    std::int64_t buy_remaining = 0;
    std::int64_t sell_remaining = 0;
};

class OrderBook {
public:
    std::vector<Trade> addLimit(bool is_buy, std::int64_t price, std::int64_t qty);
    bool cancel(std::uint64_t order_id);
    std::uint64_t lastId() const { return next_id_ > 1 ? next_id_ - 1 : 0; }
    std::size_t bidLevels() const { return bids_.size(); }
    std::size_t askLevels() const { return asks_.size(); }
    const std::vector<Trade>& trades() const { return trades_; }

private:
    using BidBook = std::map<std::int64_t, std::deque<Order>, std::greater<std::int64_t>>;
    using AskBook = std::map<std::int64_t, std::deque<Order>, std::less<std::int64_t>>;

    struct Locator {
        bool is_buy = true;
        std::int64_t price = 0;
    };

    std::vector<Trade> match();
    static void dropEmpty(BidBook& book, std::int64_t price);
    static void dropEmpty(AskBook& book, std::int64_t price);

    BidBook bids_;
    AskBook asks_;
    std::unordered_map<std::uint64_t, Locator> index_;
    std::vector<Trade> trades_;
    std::uint64_t next_id_ = 1;
};
