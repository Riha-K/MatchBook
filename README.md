# MatchBook

C++ matching engine that keeps bids and asks in memory, matches trades by price-time priority, and measures latency.

## Stack

C++17, STL (`map`, `deque`, `unordered_map`), CSV replay, `<chrono>` benchmarks. No database on the matching path.

## Book

- **Bids:** `map<price, deque<Order>>` with highest price first
- **Asks:** `map<price, deque<Order>>` with lowest price first
- Same price follows arrival order (FIFO at that level)
- **Add / cancel / match** for limit orders, including partial fills
- Trades log leftover size on both sides

Matching is single-threaded.

## Run

```bash
cmake -S . -B build
cmake --build build
./build/matchbook
./build/matchbook data/ticks.csv
```

The binary prints a small demo, a tick-CSV back-test, then throughput and mean per-order latency.
