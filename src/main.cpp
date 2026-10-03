#include "OrderBook.h"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Tick {
    bool is_buy = true;
    std::int64_t price = 0;
    std::int64_t qty = 0;
};

std::vector<Tick> loadTicks(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("cannot open " + path);
    }
    std::vector<Tick> ticks;
    std::string line;
    std::getline(in, line);  // header
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        std::stringstream ss(line);
        std::string ts, side, price, qty;
        std::getline(ss, ts, ',');
        std::getline(ss, side, ',');
        std::getline(ss, price, ',');
        std::getline(ss, qty, ',');
        auto trim = [](std::string s) {
            while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\t')) {
                s.pop_back();
            }
            std::size_t i = 0;
            while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) {
                ++i;
            }
            return s.substr(i);
        };
        side = trim(side);
        price = trim(price);
        qty = trim(qty);
        Tick t;
        t.is_buy = (side == "B" || side == "BUY");
        try {
            t.price = std::stoll(price);
            t.qty = std::stoll(qty);
        } catch (const std::exception&) {
            throw std::runtime_error("bad tick row: " + line);
        }
        ticks.push_back(t);
    }
    return ticks;
}

void printTrades(const std::vector<Trade>& trades) {
    for (const auto& t : trades) {
        std::cout << "trade buy=" << t.buy_id << " sell=" << t.sell_id
                  << " px=" << t.price << " qty=" << t.qty
                  << " buy_left=" << t.buy_remaining
                  << " sell_left=" << t.sell_remaining << '\n';
    }
}

}  // namespace

int main(int argc, char** argv) {
    try {
        std::cout << "== demo: add, partial fill, cancel ==\n";
        OrderBook book;
        printTrades(book.addLimit(false, 101, 10));
        printTrades(book.addLimit(true, 100, 4));
        printTrades(book.addLimit(true, 101, 7));
        const std::uint64_t resting_sell = 1;
        std::cout << "cancel sell #" << resting_sell << " -> "
                  << (book.cancel(resting_sell) ? "ok" : "miss") << '\n';

        const std::string csv = (argc > 1) ? argv[1] : std::string("data/ticks.csv");
        std::cout << "\n== back-test replay " << csv << " ==\n";
        OrderBook replay;
        const auto ticks = loadTicks(csv);
        for (const auto& tick : ticks) {
            replay.addLimit(tick.is_buy, tick.price, tick.qty);
        }
        std::cout << "orders=" << ticks.size() << " trades=" << replay.trades().size()
                  << " bid_levels=" << replay.bidLevels()
                  << " ask_levels=" << replay.askLevels() << '\n';

        std::cout << "\n== benchmark ==\n";
        OrderBook bench;
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> side(0, 1);
        std::uniform_int_distribution<int> px(90, 110);
        std::uniform_int_distribution<int> qty(1, 5);
        constexpr int n = 100000;
        std::vector<double> us_per_order;
        us_per_order.reserve(n);
        const auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < n; ++i) {
            const auto a = std::chrono::steady_clock::now();
            bench.addLimit(side(rng) == 1, px(rng), qty(rng));
            const auto b = std::chrono::steady_clock::now();
            us_per_order.push_back(
                std::chrono::duration<double, std::micro>(b - a).count());
        }
        const auto t1 = std::chrono::steady_clock::now();
        const double total_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        double sum = 0;
        for (double u : us_per_order) {
            sum += u;
        }
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "n=" << n << " total_ms=" << total_ms
                  << " throughput=" << (n / (total_ms / 1000.0)) << " orders/s"
                  << " mean_us=" << (sum / n) << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "error: " << ex.what() << '\n';
        return 1;
    }
}
