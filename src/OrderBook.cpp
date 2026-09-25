#include "OrderBook.h"

#include <algorithm>
#include <stdexcept>

std::vector<Trade> OrderBook::addLimit(bool is_buy, std::int64_t price, std::int64_t qty) {
    if (price <= 0 || qty <= 0) {
        throw std::invalid_argument("price and qty must be positive");
    }
    Order order;
    order.id = next_id_++;
    order.is_buy = is_buy;
    order.price = price;
    order.qty = qty;
    index_[order.id] = Locator{is_buy, price};
    if (is_buy) {
        bids_[price].push_back(order);
    } else {
        asks_[price].push_back(order);
    }
    return match();
}

bool OrderBook::cancel(std::uint64_t order_id) {
    auto found = index_.find(order_id);
    if (found == index_.end()) {
        return false;
    }
    const Locator loc = found->second;
    auto erase_from = [&](auto& book) {
        auto level = book.find(loc.price);
        if (level == book.end()) {
            return false;
        }
        auto& q = level->second;
        auto it = std::find_if(q.begin(), q.end(), [&](const Order& o) { return o.id == order_id; });
        if (it == q.end()) {
            return false;
        }
        q.erase(it);
        if (q.empty()) {
            book.erase(level);
        }
        return true;
    };
    const bool ok = loc.is_buy ? erase_from(bids_) : erase_from(asks_);
    if (ok) {
        index_.erase(found);
    }
    return ok;
}

std::vector<Trade> OrderBook::match() {
    std::vector<Trade> filled;
    while (!bids_.empty() && !asks_.empty() && bids_.begin()->first >= asks_.begin()->first) {
        auto bid_it = bids_.begin();
        auto ask_it = asks_.begin();
        auto& bid_q = bid_it->second;
        auto& ask_q = ask_it->second;
        if (bid_q.empty()) {
            bids_.erase(bid_it);
            continue;
        }
        if (ask_q.empty()) {
            asks_.erase(ask_it);
            continue;
        }
        Order& buy = bid_q.front();
        Order& sell = ask_q.front();
        const std::int64_t qty = std::min(buy.qty, sell.qty);
        const std::uint64_t buy_id = buy.id;
        const std::uint64_t sell_id = sell.id;
        const std::int64_t bid_px = bid_it->first;
        const std::int64_t ask_px = ask_it->first;
        buy.qty -= qty;
        sell.qty -= qty;

        Trade t;
        t.buy_id = buy_id;
        t.sell_id = sell_id;
        t.price = ask_px;
        t.qty = qty;
        t.buy_remaining = buy.qty;
        t.sell_remaining = sell.qty;
        filled.push_back(t);
        trades_.push_back(t);

        if (buy.qty == 0) {
            index_.erase(buy_id);
            bid_q.pop_front();
            dropEmpty(bids_, bid_px);
        }
        if (sell.qty == 0) {
            index_.erase(sell_id);
            ask_q.pop_front();
            dropEmpty(asks_, ask_px);
        }
    }
    return filled;
}

void OrderBook::dropEmpty(BidBook& book, std::int64_t price) {
    auto it = book.find(price);
    if (it != book.end() && it->second.empty()) {
        book.erase(it);
    }
}

void OrderBook::dropEmpty(AskBook& book, std::int64_t price) {
    auto it = book.find(price);
    if (it != book.end() && it->second.empty()) {
        book.erase(it);
    }
}
