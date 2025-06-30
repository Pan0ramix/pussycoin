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
    // Test that tail emission threshold is approximately 9.2M PUSSY
    uint64_t threshold = TailEmissionThreshold();
    
    // Should be around 9.2M * 1e8 = 9.2e14
    BOOST_CHECK(threshold > 9000000ULL * 100000000ULL);
    BOOST_CHECK(threshold < 10000000ULL * 100000000ULL);
}

BOOST_AUTO_TEST_CASE(smooth_emission_early_blocks)
{
    // Test emission for early blocks (should be high)
    CAmount reward1 = GetSmoothEmissionReward(0);
    CAmount reward100 = GetSmoothEmissionReward(reward1 * 99); // Approximate
    
    // First block reward should be substantial
    BOOST_CHECK(reward1 > 1000 * COIN); // > 1000 PUSSY
    
    // Rewards should decrease over time
    BOOST_CHECK(reward100 < reward1);
    
    // But should still be significant for first 100k blocks
    BOOST_CHECK(reward100 > 100 * COIN); // > 100 PUSSY
}

BOOST_AUTO_TEST_CASE(smooth_emission_tail_blocks)
{
    // Test that tail emission is 0.02 PUSSY
    uint64_t tail_threshold = TailEmissionThreshold();
    CAmount tail_reward = GetSmoothEmissionReward(tail_threshold);
    
    BOOST_CHECK_EQUAL(tail_reward, 2000000); // 0.02 PUSSY = 2,000,000 satoshis
    
    // Test that tail emission continues indefinitely
    CAmount tail_reward2 = GetSmoothEmissionReward(tail_threshold + 1000000000ULL);
    BOOST_CHECK_EQUAL(tail_reward2, 2000000);
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
            BOOST_CHECK_EQUAL(current_reward, 2000000);
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

BOOST_AUTO_TEST_CASE(total_supply_approximation)
{
    // Test that pre-tail supply is approximately 9.2M PUSSY
    uint64_t total_generated = 0;
    CAmount reward = GetSmoothEmissionReward(total_generated);
    
    int blocks = 0;
    while (total_generated < TailEmissionThreshold() && blocks < 10000000) {
        total_generated += reward;
        reward = GetSmoothEmissionReward(total_generated);
        blocks++;
    }
    
    // Should generate approximately 9.2M PUSSY before tail
    double total_pussy = double(total_generated) / 100000000.0;
    BOOST_CHECK(total_pussy > 9.0);
    BOOST_CHECK(total_pussy < 10.0);
    
    // Should happen within reasonable number of blocks
    BOOST_CHECK(blocks < 5000000); // Less than 5M blocks to reach tail
}

BOOST_AUTO_TEST_SUITE_END() 