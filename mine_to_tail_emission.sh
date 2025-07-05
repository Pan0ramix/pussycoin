#!/bin/bash

# Configuration for ACCELERATED EMISSION TESTING
TAIL_EMISSION_HEIGHT=1200     # Accelerated: tail emission starts around block 1023
BATCH_SIZE=100                # Smaller batches for detailed progress tracking
CLI="./src/litecoin-cli -regtest -datadir=regtest-data"
ADDRESS="rpussy1q9wlew9q7ndd22psp9c2tr2ju0rjr7u7apgcxuk"

echo "=== Mining to Accelerated Tail Emission Height ==="
echo "Target height: $TAIL_EMISSION_HEIGHT (accelerated regtest mode)"
echo "Expected tail emission starts around block: 1023"
echo "Batch size: $BATCH_SIZE blocks"
echo "Mining address: $ADDRESS"
echo "Start time: $(date)"

# Check starting state
current_height=$($CLI getblockchaininfo | grep '"blocks":' | cut -d':' -f2 | cut -d',' -f1 | tr -d ' ')
echo "Starting height: $current_height"

# Main mining loop
while [ $current_height -lt $TAIL_EMISSION_HEIGHT ]; do
    remaining=$((TAIL_EMISSION_HEIGHT - current_height))
    
    if [ $remaining -lt $BATCH_SIZE ]; then
        batch_size=$remaining
    else
        batch_size=$BATCH_SIZE
    fi
    
    echo ""
    echo "=== Mining batch: $batch_size blocks ==="
    echo "Current height: $current_height"
    echo "Remaining blocks: $remaining"
    echo "Progress: $(echo "scale=2; $current_height * 100 / $TAIL_EMISSION_HEIGHT" | bc)%"
    
    # Mine the batch
    batch_start=$(date +%s)
    $CLI generatetoaddress $batch_size $ADDRESS > /dev/null
    batch_end=$(date +%s)
    batch_duration=$((batch_end - batch_start))
    
    # Update current height
    current_height=$($CLI getblockchaininfo | grep '"blocks":' | cut -d':' -f2 | cut -d',' -f1 | tr -d ' ')
    
    # Get blockchain info
    blockchain_info=$($CLI getblockchaininfo)
    size_on_disk=$(echo "$blockchain_info" | grep '"size_on_disk":' | cut -d':' -f2 | cut -d',' -f1 | tr -d ' ')
    
    # Get balance (total mined coins)
    balance=$($CLI getbalance)
    
    echo "Batch completed in $batch_duration seconds"
    echo "New height: $current_height"
    echo "Disk usage: $(echo "scale=2; $size_on_disk / 1024 / 1024" | bc) MB"
    echo "Total balance: $balance PUSSY"
    
    # Special check as we approach tail emission
    if [ $current_height -ge 1000 ] && [ $current_height -le 1050 ]; then
        echo "*** APPROACHING TAIL EMISSION THRESHOLD ***"
        echo "Expected threshold around block 1023"
        
        # Mine a single block and check reward
        prev_balance=$balance
        $CLI generatetoaddress 1 $ADDRESS > /dev/null
        current_height=$($CLI getblockchaininfo | grep '"blocks":' | cut -d':' -f2 | cut -d',' -f1 | tr -d ' ')
        new_balance=$($CLI getbalance)
        
        reward=$(echo "$new_balance - $prev_balance" | bc)
        echo "Block $current_height reward: $reward PUSSY"
        
        # Check if we've hit tail emission (reward = 0.000025 PUSSY)
        if [ "$(echo "$reward <= 0.000025" | bc)" -eq 1 ]; then
            echo ""
            echo "🎉 TAIL EMISSION DETECTED! 🎉"
            echo "Block $current_height has tail emission reward: $reward PUSSY"
            break
        fi
    fi
    
    # Check if we need to verify tail emission
    if [ $current_height -ge $TAIL_EMISSION_HEIGHT ]; then
        echo ""
        echo "=== TAIL EMISSION VERIFICATION ==="
        echo "Final height: $current_height"
        echo "Final balance: $balance PUSSY"
        echo "Final disk usage: $(echo "scale=2; $size_on_disk / 1024 / 1024" | bc) MB"
        
        # Mine 10 more blocks to verify tail emission
        echo "Mining 10 more blocks to verify tail emission consistency..."
        prev_balance=$balance
        $CLI generatetoaddress 10 $ADDRESS > /dev/null
        
        final_height=$($CLI getblockchaininfo | grep '"blocks":' | cut -d':' -f2 | cut -d',' -f1 | tr -d ' ')
        final_balance=$($CLI getbalance)
        
        echo "After 10 more blocks:"
        echo "Height: $final_height"
        echo "Balance: $final_balance PUSSY"
        
        # Calculate reward per block in tail emission
        reward_diff=$(echo "$final_balance - $prev_balance" | bc)
        reward_per_block=$(echo "scale=8; $reward_diff / 10" | bc)
        echo "Reward per block in tail emission: $reward_per_block PUSSY"
        
        if [ "$(echo "$reward_per_block == 0.000025" | bc)" -eq 1 ]; then
            echo "✅ Tail emission verified: 0.000025 PUSSY per block"
        else
            echo "⚠️  Tail emission may not be exact: $reward_per_block PUSSY per block"
            echo "   Expected: 0.000025 PUSSY per block"
        fi
        
        break
    fi
done

echo ""
echo "=== Mining Complete ==="
echo "End time: $(date)"
echo "Final blockchain state:"
$CLI getblockchaininfo | grep -E '"blocks":|"size_on_disk":|"chainwork":'
echo "Final balance: $($CLI getbalance) PUSSY"

# Final emission schedule summary
echo ""
echo "=== EMISSION SCHEDULE SUMMARY ==="
echo "Accelerated regtest parameters used:"
echo "- Money supply: 922621440000 atomic units (9226.21 PUSSY)"
echo "- Tail reward: 2500 atomic units (0.000025 PUSSY)"
echo "- Emission speed factor: 10 (much faster decay)"
echo "- Tail emission threshold: ~1023 blocks"
echo "This is 1000x faster than mainnet for testing purposes." 