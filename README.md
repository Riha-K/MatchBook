# MatchBook

C++ matching engine that keeps bids and asks in memory, matches trades by price-time priority, and measures latency. Better prices match first. Equal prices follow arrival order. Partial fills remain on the book.

## Stack

C++17, STL (`map`, `deque`, `unordered_map`), CSV tick replay, `<chrono>` benchmarks. No database on the matching path. Matching is single-threaded.

## Book layout

| Side | Structure | Order |
| --- | --- | --- |
| Bids | `map<price, deque<Order>, greater<>>` | Highest price first |
| Asks | `map<price, deque<Order>, less<>>` | Lowest price first |
| Cancel index | `unordered_map<order_id, Locator>` | Side + price level for O(1) lookup |

Same price follows arrival order (FIFO deque at that level). Prices and quantities are 64-bit integers, so comparison is exact. Non-positive price or quantity is rejected.

## Operations

- **Add limit** — assign id, try match, rest leftover size on the book
- **Match** — while best bid ≥ best ask, trade min(qty); record leftover on both sides
- **Cancel** — locate level via index, scan that price’s deque (O(L) at the level), erase

Cancel is **not** amortized O(1) end-to-end: lookup is O(1), removing from the deque at that price is O(L). Say that clearly in interviews.

## Complexity (typical)

| Action | Cost |
| --- | --- |
| Add resting order | O(log P) map + deque push |
| Match one price level | O(1) front ops until qty done |
| Cancel | O(1) index + O(L) scan at level |

P = distinct price levels on that side; L = orders at one price.

## Run

Needs CMake 3.16 or newer and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
./build/matchbook
./build/matchbook data/ticks.csv
```

The binary prints:

1. A small **demo** (add, partial fill, cancel)
2. A **CSV back-test** replay
3. A **benchmark** (throughput + mean / percentile latency per order)

Prefer an optimised build when quoting latency numbers.

## Tick CSV

Four columns, header included. `side` is `B` or `BUY` for buy, and `S` or `SELL` for sell.

```text
ts,side,price,qty
1,S,101,10
2,B,100,4
3,B,101,6
```

## What this is / is not

**Is:** in-memory limit order book, price-time priority, partial fills, CSV replay, chrono micro-benchmarks.

**Is not:** market orders, stop orders, multi-symbol router, persistence, networking, multi-threaded matching, exchange risk checks.

## Interview one-liner

“MatchBook is a C++17 in-memory limit order book: bids and asks in ordered maps of FIFO queues, price-time matching with partial fills, cancel via id index, plus CSV replay and latency timing.”

<!-- refreshed: 2026-11-02 -->
