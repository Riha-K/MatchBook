# MatchBook

C++ matching engine that keeps bids and asks in memory, matches trades by price-time priority, and measures latency. Better prices match first. Equal prices follow arrival order. Partial fills stay on the book.

## Stack

C++17, STL (`map`, `deque`, `unordered_map`), CSV replay, `<chrono>` benchmarks. No database on the matching path.

## Book

- **Bids:** `map<price, deque<Order>>` with highest price first
- **Asks:** `map<price, deque<Order>>` with lowest price first
- Same price follows arrival order (FIFO at that level)
- **Add / cancel / match** for limit orders, including partial fills
- Trades log leftover size on both sides

Matching is single-threaded.

Prices and quantities are 64-bit integers, so price comparison is exact. Cancel is O(1): an `unordered_map` from order id to its side and price level finds the queue without scanning the book.

## Run

Needs CMake 3.16 or newer and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
./build/matchbook
./build/matchbook data/ticks.csv
```

The binary prints a small demo, a tick-CSV back-test, then throughput and mean per-order latency.

## Tick CSV

Four columns, header included. `side` is `B` for buy or `S` for sell.

```text
ts,side,price,qty
1,S,101,10
2,B,100,4
3,B,101,6
```
