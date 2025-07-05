#include <iostream>
#include <consensus/emission.h>
#include <amount.h>
#include <util/moneystr.h>

int main() {
    std::cout << "=== Pussycoin Emission Schedule Test ===" << std::endl;
    
    // Test key emission parameters
    uint64_t threshold = TailEmissionThreshold();
    std::cout << "Tail emission threshold: " << threshold << " atomic units" << std::endl;
    std::cout << "Tail emission threshold: " << ValueFromAmount(threshold) << " PUSSY" << std::endl;
    
    // Test rewards at different stages
    std::cout << "\n=== Early Emission (Block 1-1000) ===" << std::endl;
    for (int height = 1; height <= 1000; height += 100) {
        uint64_t cumulative = GetCumulativeEmission(height);
        CAmount reward = GetSmoothEmissionReward(cumulative);
        std::cout << "Block " << height << ": " 
                  << "Cumulative=" << ValueFromAmount(cumulative) << " PUSSY, "
                  << "Reward=" << ValueFromAmount(reward) << " PUSSY" << std::endl;
    }
    
    // Test emission approaching threshold
    std::cout << "\n=== Approaching Tail Emission ===" << std::endl;
    std::vector<uint64_t> test_emissions = {
        threshold - 1000000,  // 0.01 PUSSY before threshold
        threshold - 100000,   // 0.001 PUSSY before threshold
        threshold - 10000,    // 0.0001 PUSSY before threshold
        threshold - 1,        // 1 atomic unit before threshold
        threshold,            // Exactly at threshold
        threshold + 1,        // 1 atomic unit after threshold
        threshold + 250000,   // 1 block after threshold
        threshold + 2500000   // 10 blocks after threshold
    };
    
    for (uint64_t emission : test_emissions) {
        CAmount reward = GetSmoothEmissionReward(emission);
        std::cout << "Emission=" << ValueFromAmount(emission) << " PUSSY -> "
                  << "Reward=" << ValueFromAmount(reward) << " PUSSY";
        if (reward == 2500000) {
            std::cout << " (TAIL EMISSION)";
        }
        std::cout << std::endl;
    }
    
    // Test tail emission reward is constant
    std::cout << "\n=== Tail Emission Verification ===" << std::endl;
    bool tail_emission_correct = true;
    for (uint64_t offset = 0; offset < 10000000; offset += 1000000) {
        uint64_t emission = threshold + offset;
        CAmount reward = GetSmoothEmissionReward(emission);
        if (reward != 2500000) {
            std::cout << "ERROR: Tail emission not constant at " << ValueFromAmount(emission) << std::endl;
            tail_emission_correct = false;
        }
    }
    
    if (tail_emission_correct) {
        std::cout << "✓ Tail emission is correctly constant at 0.025 PUSSY per block" << std::endl;
    }
    
    // Test emission never exceeds threshold during main emission
    std::cout << "\n=== Main Emission Bounds Check ===" << std::endl;
    bool bounds_correct = true;
    for (int height = 1; height <= 100000; height += 10000) {
        uint64_t cumulative = GetCumulativeEmission(height);
        if (cumulative > threshold) {
            std::cout << "ERROR: Main emission exceeded threshold at block " << height << std::endl;
            bounds_correct = false;
        }
    }
    
    if (bounds_correct) {
        std::cout << "✓ Main emission stays within bounds" << std::endl;
    }
    
    // Test mathematical consistency
    std::cout << "\n=== Mathematical Consistency ===" << std::endl;
    uint64_t prev_cumulative = 0;
    bool consistency_correct = true;
    
    for (int height = 1; height <= 1000; height++) {
        uint64_t cumulative = GetCumulativeEmission(height);
        CAmount reward = GetSmoothEmissionReward(prev_cumulative);
        
        // Check if cumulative = prev_cumulative + reward
        uint64_t expected_cumulative = prev_cumulative + static_cast<uint64_t>(reward);
        if (cumulative != expected_cumulative) {
            std::cout << "ERROR: Inconsistency at block " << height 
                      << " - expected " << expected_cumulative 
                      << ", got " << cumulative << std::endl;
            consistency_correct = false;
        }
        prev_cumulative = cumulative;
    }
    
    if (consistency_correct) {
        std::cout << "✓ Emission calculations are mathematically consistent" << std::endl;
    }
    
    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Tail emission starts at: " << ValueFromAmount(threshold) << " PUSSY" << std::endl;
    std::cout << "Tail emission reward: 0.025 PUSSY per block" << std::endl;
    std::cout << "Main emission uses smooth exponential decay" << std::endl;
    
    return 0;
} 