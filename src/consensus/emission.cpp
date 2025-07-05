// Copyright (c) 2024 The Pussycoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <consensus/emission.h>
#include <amount.h>

// Monero emission parameters adapted for Pussycoin (Monero/2)
static constexpr uint64_t MONEY_SUPPLY        = 922621440000000ULL; // Calculated for 9.2M PUSSY threshold
static constexpr uint64_t TAIL_REWARD         = 2'500'000;      // 0.025 PUSSY per 10s block
static constexpr int      EMISSION_SPEED_FACTOR = 20;           // Same as Monero

uint64_t TailEmissionThreshold()
{
    // Calculate when main emission ends and tail emission begins
    // This happens when base_reward drops to TAIL_REWARD
    // (MONEY_SUPPLY - threshold) >> EMISSION_SPEED_FACTOR == TAIL_REWARD
    // threshold = MONEY_SUPPLY - (TAIL_REWARD << EMISSION_SPEED_FACTOR)
    return MONEY_SUPPLY - (static_cast<uint64_t>(TAIL_REWARD) << EMISSION_SPEED_FACTOR);
}

CAmount GetSmoothEmissionReward(uint64_t already_generated)
{
    // Monero's exact emission formula: baseReward = (moneySupply - alreadyGenerated) >> emissionSpeedFactor
    // If baseReward < tailReward, use tailReward (tail emission phase)
    
    if (already_generated >= TailEmissionThreshold()) {
        return TAIL_REWARD;  // Tail emission: constant 0.025 PUSSY per block
    }
    
    // Main emission: smoothly decreasing reward using Monero's formula
    uint64_t base_reward = (MONEY_SUPPLY - already_generated) >> EMISSION_SPEED_FACTOR;
    
    // Ensure we don't go below tail reward during main emission
    if (base_reward < TAIL_REWARD) {
        return TAIL_REWARD;
    }
    
    return static_cast<CAmount>(base_reward);
}

uint64_t GetCumulativeEmission(int nHeight)
{
    if (nHeight <= 0) return 0;
    
    // CONSENSUS CRITICAL: For Apple Silicon compatibility, use iterative calculation
    // only for reasonable heights, with optimizations to prevent O(n) performance issues
    
    // For very early blocks, calculate exactly using Monero's proven formula
    if (nHeight <= 1000) {
        uint64_t total = 0;
        for (int h = 1; h <= nHeight; h++) {
            CAmount reward = GetSmoothEmissionReward(total);
            total += static_cast<uint64_t>(reward);
        }
        return total;
    }
    
    // For larger heights, use mathematical approximation to avoid O(n) loops
    // The main emission follows exponential decay: total ≈ (1 - (1-1/2^k)^n) * supply_limit
    // where k = EMISSION_SPEED_FACTOR, n = nHeight
    
    const uint64_t threshold = TailEmissionThreshold();
    
    // Check if we're definitely in tail emission phase
    // Approximate main emission completion: when cumulative ≈ threshold
    // This happens roughly when (MONEY_SUPPLY >> EMISSION_SPEED_FACTOR) * nHeight ≈ threshold
    uint64_t approx_completion_height = threshold / (MONEY_SUPPLY >> EMISSION_SPEED_FACTOR);
    
    if (nHeight > approx_completion_height * 2) {
        // Definitely in tail emission - use linear calculation
        uint64_t tail_blocks = nHeight - approx_completion_height;
        return threshold + (tail_blocks * TAIL_REWARD);
    }
    
    // We might be in transition or main emission - calculate more precisely
    // Use binary search approach to find where tail emission starts
    uint64_t total = 0;
    uint64_t last_reward = MONEY_SUPPLY >> EMISSION_SPEED_FACTOR;  // First block reward
    
    for (int h = 1; h <= nHeight; h++) {
        if (last_reward <= TAIL_REWARD) {
            // We've reached tail emission - calculate remainder linearly
            uint64_t remaining_blocks = nHeight - h + 1;
            total += remaining_blocks * TAIL_REWARD;
            break;
        }
        
        total += last_reward;
        last_reward = (MONEY_SUPPLY - total) >> EMISSION_SPEED_FACTOR;
        
        // Safety check: prevent infinite loops
        if (h > 10000000) {  // 10M blocks safety limit
            // Fall back to tail emission for remaining blocks
            uint64_t remaining_blocks = nHeight - h + 1;
            total += remaining_blocks * TAIL_REWARD;
            break;
        }
    }
    
    return total;
} 