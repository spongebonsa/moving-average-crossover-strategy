#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

// Function to read CSV data
std::vector<double> read_adj_close(const std::string& filename) {
    std::vector<double> prices;
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string date, open, high, low, close, adj_close, volume;
        getline(ss, date, ',');
        getline(ss, open, ',');
        getline(ss, high, ',');
        getline(ss, low, ',');
        getline(ss, close, ',');
        getline(ss, adj_close, ',');
        getline(ss, volume, ',');

        prices.push_back(std::stod(adj_close));
    }
    return prices;
}

// Calculate moving average
double moving_average(const std::vector<double>& data, int index, int window) {
    if (index < window) return NAN;
    double sum = 0;
    for (int i = index - window + 1; i <= index; i++)
        sum += data[i];
    return sum / window;
}

// Backtest Strategy
std::pair<double, double> backtest(const std::vector<double>& prices, int short_ma, int long_ma) {
    std::vector<int> signal(prices.size(), 0);
    std::vector<double> daily_returns(prices.size(), 0);

    for (int i = 0; i < prices.size(); i++) {
        double short_avg = moving_average(prices, i, short_ma);
        double long_avg = moving_average(prices, i, long_ma);
        signal[i] = (short_avg > long_avg) ? 1 : 0;
    }

    double cum_return = 1.0;
    for (int i = 1; i < prices.size(); i++) {
        double ret = ((prices[i] - prices[i - 1]) / prices[i - 1]) * signal[i - 1];
        daily_returns[i] = ret;
        cum_return *= (1 + ret);
    }

    double mean = 0, variance = 0;
    int count = 0;
    for (double r : daily_returns) {
        if (r != 0) {
            mean += r;
            count++;
        }
    }
    mean /= count;

    for (double r : daily_returns) {
        if (r != 0) variance += pow(r - mean, 2);
    }
    variance /= count;
    double sharpe = sqrt(252) * (mean / sqrt(variance));

    double n_years = prices.size() / 252.0;
    double annual_return = pow(cum_return, 1.0 / n_years) - 1;

    return {annual_return, sharpe};
}

int main() {
    auto prices = read_adj_close("data/SPY.csv");
    std::unordered_map<std::string, std::pair<double, double>> results;

    for (int short_ma : {1, 60}) {
        for (int long_ma : {5, 250}) {
            if (short_ma >= long_ma) continue;
            auto res = backtest(prices, short_ma, long_ma);
            std::string key = std::to_string(short_ma) + "-" + std::to_string(long_ma);
            results[key] = res;
        }
    }

    std::string best_key;
    double best_sharpe = -1;

    for (auto& r : results) {
        if (r.second.second > best_sharpe) {
            best_sharpe = r.second.second;
            best_key = r.first;
        }
    }

    std::cout << "Optimal MA Combo: " << best_key << "\n";
    std::cout << "Optimal Annual Return: " << results[best_key].first << "\n";
    std::cout << "Sharpe Ratio: " << results[best_key].second << "\n";

    return 0;
}
