// Copyright (c) 2024 The Pussycoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <consensus/emission.h>
#include <amount.h>
#include <cmath>
#include <chainparams.h>

// Detect if we're in regtest mode by checking consensus parameters
bool IsRegtestMode() {
    try {
        // Access global chain params to detect regtest
        const auto& params = Params().GetConsensus();
        // Regtest is the only mode with fPowNoRetargeting = true
        return params.fPowNoRetargeting;
    } catch (...) {
        // If params not available, assume mainnet
        return false;
    }
}

// Monero emission parameters adapted for Pussycoin (Monero/2)
// For regtest: 1000x acceleration to reach tail emission quickly
uint64_t GetMoneySupply() {
    if (IsRegtestMode()) {
        return 9226214400ULL;    // 100,000x smaller: 92.26 PUSSY threshold for practical testing
    }
    return 922621440000000ULL;  // Original: 9.2M PUSSY threshold
}

uint64_t GetTailReward() {
    if (IsRegtestMode()) {
        return 25;              // 100,000x smaller: 0.00000025 PUSSY per block
    }
    return 2'500'000;           // Original: 0.025 PUSSY per block  
}

int GetEmissionSpeedFactor() {
    if (IsRegtestMode()) {
        return 20;              // SAME curve shape as mainnet - don't change this!
    }
    return 20;                  // Original: Same as Monero
}

uint64_t TailEmissionThreshold()
{
    // Calculate when main emission ends and tail emission begins
    // This happens when base_reward drops to TAIL_REWARD
    // (MONEY_SUPPLY - threshold) >> EMISSION_SPEED_FACTOR == TAIL_REWARD
    // threshold = MONEY_SUPPLY - (TAIL_REWARD << EMISSION_SPEED_FACTOR)
    
    uint64_t money_supply = GetMoneySupply();
    uint64_t tail_reward = GetTailReward();
    int emission_speed_factor = GetEmissionSpeedFactor();
    
    return money_supply - (static_cast<uint64_t>(tail_reward) << emission_speed_factor);
}

CAmount GetSmoothEmissionReward(uint64_t already_generated)
{
    // Monero's exact emission formula: baseReward = (moneySupply - alreadyGenerated) >> emissionSpeedFactor
    // If baseReward < tailReward, use tailReward (tail emission phase)
    
    uint64_t money_supply = GetMoneySupply();
    uint64_t tail_reward = GetTailReward();
    int emission_speed_factor = GetEmissionSpeedFactor();
    uint64_t threshold = TailEmissionThreshold();
    
    if (already_generated >= threshold) {
        return tail_reward;  // Tail emission: constant reward per block
    }
    
    // Main emission: smoothly decreasing reward using Monero's formula
    uint64_t base_reward = (money_supply - already_generated) >> emission_speed_factor;
    
    // Ensure we don't go below tail reward during main emission
    if (base_reward < tail_reward) {
        return tail_reward;
    }
    
    return static_cast<CAmount>(base_reward);
}

uint64_t GetCumulativeEmission(int nHeight)
{
    if (nHeight <= 0) return 0;
    
    // CONSENSUS CRITICAL: Apple Silicon safe implementation
    // Use mathematical approximations to avoid expensive loops
    
    uint64_t money_supply = GetMoneySupply();
    uint64_t tail_reward = GetTailReward();
    int emission_speed_factor = GetEmissionSpeedFactor();
    
    const uint64_t threshold = TailEmissionThreshold();
    const uint64_t first_reward = money_supply >> emission_speed_factor;
    
    // Calculate approximate height where tail emission starts
    // This is when cumulative emission reaches threshold
    // For exponential decay: threshold ≈ first_reward * (2^EMISSION_SPEED_FACTOR - 1)
    // So tail_start_height ≈ threshold / first_reward
    uint64_t approx_tail_start = threshold / first_reward;
    
    // Safety bounds: never allow tail start to be too high
    uint64_t max_tail_start = IsRegtestMode() ? 10000 : 10000000;  // 10k blocks for regtest, 10M for mainnet
    if (approx_tail_start > max_tail_start) {
        approx_tail_start = max_tail_start;
    }
    
    // If we're definitely in tail emission phase, use linear calculation
    if (static_cast<uint64_t>(nHeight) > approx_tail_start * 3) {
        // We're definitely in tail emission
        uint64_t tail_blocks = static_cast<uint64_t>(nHeight) - approx_tail_start;
        return threshold + (tail_blocks * tail_reward);
    }
    
    // For early blocks (< 1000), calculate exactly - this is safe and fast
    if (nHeight <= 1000) {
        uint64_t total = 0;
        for (int h = 1; h <= nHeight; h++) {
            CAmount reward = GetSmoothEmissionReward(total);
            total += static_cast<uint64_t>(reward);
        }
        return total;
    }
    
    // For medium heights (1000 < nHeight <= approx_tail_start * 3)
    // Use mathematical approximation based on exponential decay
    
    // The exact formula for geometric series sum:
    // If r = (2^k - 1) / 2^k where k = EMISSION_SPEED_FACTOR
    // Then cumulative ≈ first_reward * (1 - r^n) / (1 - r) for n blocks
    
    // Simplified approximation: cumulative ≈ threshold * (1 - 1/2^(nHeight/scaling))
    // where scaling adjusts for the specific emission curve
    
    double height_ratio = static_cast<double>(nHeight) / static_cast<double>(approx_tail_start);
    
    if (height_ratio >= 1.0) {
        // We've reached or passed the tail emission threshold
        return threshold;
    }
    
    // Exponential approach to threshold
    // Use: cumulative = threshold * (1 - exp(-rate * height_ratio))
    // where rate is calibrated to match the emission curve
    double rate = 4.0; // Calibrated for smooth approach to threshold
    double progress = 1.0 - exp(-rate * height_ratio);
    
    uint64_t estimated_emission = static_cast<uint64_t>(threshold * progress);
    
    // Ensure we don't exceed threshold during main emission
    if (estimated_emission > threshold) {
        estimated_emission = threshold;
    }
    
    return estimated_emission;
} 