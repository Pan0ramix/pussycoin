#include <iostream>
#include <iomanip>

// Copy the constants from emission.cpp
static constexpr uint64_t MONEY_SUPPLY = UINT64_MAX;
static constexpr uint64_t TAIL_REWARD = 2500000;
static constexpr int EMISSION_SPEED_FACTOR = 21;

uint64_t TailEmissionThreshold() {
    return MONEY_SUPPLY - (static_cast<uint64_t>(TAIL_REWARD) << EMISSION_SPEED_FACTOR);
}

int main() {
    uint64_t threshold = TailEmissionThreshold();
    double threshold_pussy = double(threshold) / 100000000.0;
    
    std::cout << "Tail emission threshold: " << threshold << " satoshis" << std::endl;
    std::cout << "In PUSSY coins: " << std::fixed << std::setprecision(2) << threshold_pussy << " PUSSY" << std::endl;
    std::cout << "In millions: " << threshold_pussy / 1000000.0 << " million PUSSY" << std::endl;
    
    return 0;
} 