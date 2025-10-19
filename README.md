# Moving Average Crossover Strategy (C++ Backtest)

This project tests a **moving-average crossover trading strategy** on **SPY (S&P 500 ETF)**.

## Strategy Rules

- **Buy Signal** → When **Short-Term MA crosses above Long-Term MA**
- **Sell Signal** → When **Short-Term MA crosses below Long-Term MA**
- Strategy is **long-only** (only buys, no shorting)

## Steps

1. **Download Historical Data**
   - Go to Yahoo Finance → SPY → Export CSV (Adj Close column used)

2. **Load Data & Backtest**
   - Calculates moving averages and applies signals
   - Computes **Annualized Return & Sharpe Ratio**

3. **Optimize Strategy**
   - Tests different MA combinations → Picks best based on **Sharpe Ratio**

## Compile & Run

```bash
g++ main.cpp -o backtest
./backtest
