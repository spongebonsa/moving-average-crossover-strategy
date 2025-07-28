import pandas as pd
import yfinance as yf
import numpy as np

# Download SPY data
spy = yf.download("SPY", start="2010-01-01", end="2021-12-31")

def backtest_strategy(short_ma, long_ma):
    spy["short_ma"] = spy["Adj close"].rolling(short_ma).mean()
    spy["long_ma"] = spy["Adj close"].rolling(long_ma).mean()

    # Buy signal: short MA crosses above long MA
    spy["signal"] = (spy["short_ma"] > spy["long_ma"]).astype(int)

    # Calculate daily returns and cumulative returns
    spy["returns"] = spy["Adj Close"].pct_change() * spy["signal"].shift(1)
    spy["cum_returns"] = (1 + spy["returns"]).cumprod()

    # Calculate total return and annualized return
    total_return = spy["cum_returns"].iloc[-1]
    n_years = len(spy) / 252
    annualized_return = (total_return ** (1 / n_years)) - 1

    # Calculate Sharpe ratio
    daily_returns = spy["returns"].dropna()
    sharpe_ratio = np.sqrt(252) * (daily_returns.mean() / daily_returns.std())

    return annualized_return, sharpe_ratio

results = {}
for short_ma in [1, 60]:
    for long_ma in [5, 250]:
        if short_ma >= long_ma:
            continue
        annualized_return, sharpe_ratio = backtest_strategy(short_ma, long_ma)
        results[(short_ma, long_ma)] = (annualized_return, sharpe_ratio)

# Find optimal ma combo based on Sharpe ratio
if results:
    optimal_ma = max(results, key=lambda x: results[x][1])
    optimal_return, optimal_sharpe = results[optimal_ma]
    print("Optimal MA Combo:", optimal_ma)
    print("optimal ma crossover:", optimal_ma)
    print("Optimal Annualized Return:", optimal_return)
    print("Sharpe Ratio:", optimal_sharpe)
else:
    print("No valid MA combinations found.")