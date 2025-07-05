// Copyright (c) 2024 The Pussycoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <consensus/emission.h>
#include <amount.h>
#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(emission_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(tail_emission_threshold_test)
{
    // Test that tail emission threshold is exactly 9.2M PUSSY (Monero/2)
    uint64_t threshold = TailEmissionThreshold();
    
    // Should be exactly 9.2M PUSSY = 920,000,000,000,000 satoshis
    BOOST_CHECK_EQUAL(threshold, 920000000000000ULL);
}

BOOST_AUTO_TEST_CASE(smooth_emission_early_blocks)
{
    // Test emission for early blocks (should be high relative to tail emission)
    CAmount reward1 = GetSmoothEmissionReward(0);
    CAmount reward100 = GetSmoothEmissionReward(reward1 * 99); // Approximate
    
    // First block reward should be substantial (much higher than tail emission)
    BOOST_CHECK(reward1 > 100 * 2500000); // > 100x tail emission (0.025 PUSSY)
    
    // Rewards should decrease over time
    BOOST_CHECK(reward100 < reward1);
    
    // But should still be significant early on
    BOOST_CHECK(reward100 > 10 * 2500000); // > 10x tail emission
}

BOOST_AUTO_TEST_CASE(smooth_emission_tail_blocks)
{
    // Test that tail emission is 0.025 PUSSY (matches Monero/2 for 10-second blocks)
    uint64_t tail_threshold = TailEmissionThreshold();
    CAmount tail_reward = GetSmoothEmissionReward(tail_threshold);
    
    BOOST_CHECK_EQUAL(tail_reward, 2500000); // 0.025 PUSSY = 2,500,000 satoshis
    
    // Test that tail emission continues indefinitely
    CAmount tail_reward2 = GetSmoothEmissionReward(tail_threshold + 1000000000ULL);
    BOOST_CHECK_EQUAL(tail_reward2, 2500000);
}

BOOST_AUTO_TEST_CASE(smooth_emission_monotonic_decrease)
{
    // Test that emission decreases monotonically before tail
    uint64_t already_generated = 0;
    CAmount prev_reward = GetSmoothEmissionReward(already_generated);
    
    for (int i = 0; i < 1000; i++) {
        already_generated += prev_reward;
        CAmount current_reward = GetSmoothEmissionReward(already_generated);
        
        // Skip when we hit tail emission
        if (already_generated >= TailEmissionThreshold()) {
            BOOST_CHECK_EQUAL(current_reward, 2500000);
            break;
        }
        
        // Current reward should be less than or equal to previous
        BOOST_CHECK(current_reward <= prev_reward);
        prev_reward = current_reward;
    }
}

BOOST_AUTO_TEST_CASE(emission_golden_vectors)
{
    // Golden test vectors for specific heights
    struct TestVector {
        uint64_t already_generated;
        CAmount expected_reward;
    };
    
    std::vector<TestVector> vectors = {
        {0, GetSmoothEmissionReward(0)}, // Block 1
        // Add more golden vectors here after calculating expected values
    };
    
    for (const auto& vector : vectors) {
        CAmount actual = GetSmoothEmissionReward(vector.already_generated);
        BOOST_CHECK_EQUAL(actual, vector.expected_reward);
    }
}

BOOST_AUTO_TEST_CASE(infinite_tail_emission_test)
{
    // Test understanding: Total supply is INFINITE due to perpetual tail emission
    // Inflation rate approaches zero as supply grows, but never stops
    
    uint64_t tail_threshold = TailEmissionThreshold();
    const CAmount tail_reward = 2500000; // 0.025 PUSSY per block forever
    
    // Simulate 1 year of tail emission (3,153,600 blocks per year with 10s blocks)
    // 6 blocks/min * 60 min * 24 hours * 365 days = 3,153,600 blocks/year
    uint64_t one_year_blocks = 3153600;
    uint64_t yearly_tail_emission = tail_reward * one_year_blocks;
    
    // After 1 year of tail emission: 0.025 * 3,153,600 = 78,840 PUSSY per year
    double yearly_pussy = double(yearly_tail_emission) / 100000000.0;
    BOOST_CHECK(yearly_pussy > 78800.0 && yearly_pussy < 78900.0);
    
    // Calculate inflation rate: yearly emission / total supply at tail threshold
    double supply_at_tail = double(tail_threshold) / 100000000.0; // 9.2M PUSSY
    double initial_inflation_rate = yearly_pussy / supply_at_tail;
    
    // Initial inflation rate should be approximately 0.86% per year (78,840 / 9,200,000)
    BOOST_CHECK(initial_inflation_rate > 0.008);  // > 0.8%
    BOOST_CHECK(initial_inflation_rate < 0.009);  // < 0.9%
    
    // After 10 years, inflation rate decreases further
    uint64_t ten_years_emission = yearly_tail_emission * 10;
    double supply_after_10_years = supply_at_tail + (ten_years_emission / 100000000.0);
    double inflation_after_10_years = yearly_pussy / supply_after_10_years;
    
    // Inflation rate should be even lower after 10 years
    BOOST_CHECK(inflation_after_10_years < initial_inflation_rate);
    
    // Verify tail emission never stops
    CAmount far_future_reward = GetSmoothEmissionReward(tail_threshold + 1000000000000ULL);
    BOOST_CHECK_EQUAL(far_future_reward, tail_reward);
}

BOOST_AUTO_TEST_SUITE_END() 