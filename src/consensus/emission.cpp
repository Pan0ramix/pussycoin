// Copyright (c) 2024 The Pussycoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <consensus/emission.h>
#include <amount.h>

static constexpr uint64_t ATOMIC_UNITS   = 100'000'000;     // 1 PUSSY = 1e8
static constexpr uint64_t MONEY_SUPPLY   = UINT64_MAX;      // 2^64 - 1
static constexpr uint64_t TAIL_REWARD    = 2'000'000;       // 0.02 PUSSY
static constexpr int      SHIFT_CONSTANT = 25;              // 20 + 1 + 4

uint64_t TailEmissionThreshold()
{
    // Pre-tail supply ≈ 9.2 M PUSSY
    return MONEY_SUPPLY - (TAIL_REWARD << SHIFT_CONSTANT);
}

CAmount GetSmoothEmissionReward(uint64_t already_generated)
{
    if (already_generated < TailEmissionThreshold()) {
        return (MONEY_SUPPLY - already_generated) >> SHIFT_CONSTANT;
    }
    return TAIL_REWARD;
} 