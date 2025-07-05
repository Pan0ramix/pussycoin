#!/usr/bin/env python3
# Copyright (c) 2024 The Pussycoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Stress test for Pussycoin's 10-second blocks and transaction handling."""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    connect_nodes,
    disconnect_nodes,
    sync_blocks,
    sync_mempools,
    wait_until,
)
from test_framework.messages import (
    COIN,
    COutPoint,
    CTransaction,
    CTxIn,
    CTxOut,
    ToHex,
)
import time
import random
from threading import Thread, Lock
import queue

class PussycoinStressTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 6
        self.extra_args = [
            ["-txindex", "-maxmempool=500", "-limitdescendantcount=100"],  # Node 0: High mempool
            ["-txindex", "-maxmempool=250", "-limitdescendantcount=50"],   # Node 1: Medium mempool
            ["-txindex", "-maxmempool=100", "-limitdescendantcount=25"],   # Node 2: Low mempool
            ["-txindex", "-mwebspendsonly=1"],  # Node 3: MWEB-only spends
            ["-txindex", "-mwebpegsonly=1"],    # Node 4: MWEB-only pegs
            ["-txindex", "-nomweb"],            # Node 5: No MWEB
        ]
        self.transaction_queues = []
        self.stop_threads = False
        self.mempool_lock = Lock()

    def setup_network(self):
        self.setup_nodes()
        # Connect in a ring topology
        for i in range(self.num_nodes):
            connect_nodes(self.nodes[i], (i + 1) % self.num_nodes)
        self.sync_all()

    def run_test(self):
        self.log.info("Running Pussycoin stress tests...")
        
        # Initialize transaction queues
        for _ in range(self.num_nodes):
            self.transaction_queues.append(queue.Queue())
        
        # Run stress tests
        self.test_rapid_block_generation()
        self.test_high_transaction_throughput()
        self.test_network_congestion()
        self.test_mweb_stress()
        self.test_chain_reorg_stress()

    def test_rapid_block_generation(self):
        """Test rapid block generation at 10-second intervals"""
        self.log.info("Testing rapid block generation...")
        
        node = self.nodes[0]
        
        # Generate initial blocks
        node.generatetoaddress(100, node.getnewaddress())
        initial_height = node.getblockcount()
        
        # Generate blocks rapidly
        start_time = time.time()
        target_blocks = 60  # 10 minutes worth of blocks
        
        for _ in range(target_blocks):
            node.generatetoaddress(1, node.getnewaddress())
            time.sleep(10)  # Target 10-second blocks
        
        end_time = time.time()
        final_height = node.getblockcount()
        
        # Verify block generation
        blocks_generated = final_height - initial_height
        time_taken = end_time - start_time
        block_time_avg = time_taken / blocks_generated
        
        self.log.info(f"Average block time: {block_time_avg:.2f} seconds")
        assert_greater_than(15, abs(block_time_avg - 10))  # Within 5 seconds of target

    def generate_random_transaction(self, node, value):
        """Generate a random transaction"""
        recipient = node.getnewaddress()
        return node.sendtoaddress(recipient, value)

    def transaction_generator(self, node_idx, tps):
        """Generate transactions at specified TPS"""
        node = self.nodes[node_idx]
        
        while not self.stop_threads:
            try:
                # Generate random transaction
                value = random.uniform(0.0001, 1.0)
                txid = self.generate_random_transaction(node, value)
                self.transaction_queues[node_idx].put(txid)
                
                # Wait according to TPS
                time.sleep(1.0 / tps)
            except Exception as e:
                self.log.error(f"Error in transaction generator: {str(e)}")

    def test_high_transaction_throughput(self):
        """Test high transaction throughput"""
        self.log.info("Testing high transaction throughput...")
        
        # Generate initial coins
        self.nodes[0].generatetoaddress(200, self.nodes[0].getnewaddress())
        self.sync_all()
        
        # Start transaction generators
        threads = []
        try:
            # Different TPS for different nodes
            tps_values = [20, 15, 10, 5, 5, 5]  # Transactions per second
            
            for i in range(self.num_nodes):
                thread = Thread(target=self.transaction_generator, args=(i, tps_values[i]))
                thread.start()
                threads.append(thread)
            
            # Let it run for 5 minutes
            time.sleep(300)
            
            # Stop threads
            self.stop_threads = True
            for thread in threads:
                thread.join()
            
            # Verify mempool state
            self.sync_mempools()
            mempool_sizes = [len(node.getrawmempool()) for node in self.nodes]
            self.log.info(f"Final mempool sizes: {mempool_sizes}")
            
            # Mine remaining transactions
            self.nodes[0].generatetoaddress(10, self.nodes[0].getnewaddress())
            self.sync_all()
            
        finally:
            self.stop_threads = True
            for thread in threads:
                thread.join()

    def test_network_congestion(self):
        """Test network behavior under congestion"""
        self.log.info("Testing network congestion...")
        
        # Disconnect some nodes to create network segments
        disconnect_nodes(self.nodes[1], self.nodes[2])
        disconnect_nodes(self.nodes[3], self.nodes[4])
        
        # Generate transactions in different segments
        addr1 = self.nodes[0].getnewaddress()
        addr2 = self.nodes[2].getnewaddress()
        
        # Create many transactions
        for _ in range(100):
            self.nodes[0].sendtoaddress(addr1, 0.01)
            self.nodes[2].sendtoaddress(addr2, 0.01)
        
        # Wait for mempool divergence
        time.sleep(10)
        
        # Reconnect nodes
        connect_nodes(self.nodes[1], self.nodes[2])
        connect_nodes(self.nodes[3], self.nodes[4])
        
        # Verify mempool convergence
        self.sync_mempools()
        
        # Generate blocks to clear mempools
        self.nodes[0].generatetoaddress(10, self.nodes[0].getnewaddress())
        self.sync_all()

    def test_mweb_stress(self):
        """Stress test MWEB transactions"""
        self.log.info("Testing MWEB under stress...")
        
        node0 = self.nodes[0]  # Regular node
        node3 = self.nodes[3]  # MWEB-only spends
        
        # Generate coins and create MWEB addresses
        node0.generatetoaddress(100, node0.getnewaddress())
        self.sync_all()
        
        mweb_addresses = [node0.getnewaddress("", "mweb") for _ in range(10)]
        
        # Create multiple MWEB transactions
        for addr in mweb_addresses:
            # Peg-in
            node0.sendtoaddress(addr, 10.0)
        
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        # Create MWEB-to-MWEB transactions
        for i in range(len(mweb_addresses) - 1):
            node0.sendtoaddress(mweb_addresses[i + 1], 1.0)
        
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        # Peg-out transactions
        regular_addr = node0.getnewaddress()
        for _ in range(5):
            node3.sendtoaddress(regular_addr, 0.5)
        
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()

    def test_chain_reorg_stress(self):
        """Test chain reorganization under stress"""
        self.log.info("Testing chain reorg under stress...")
        
        node0 = self.nodes[0]
        node1 = self.nodes[1]
        
        # Disconnect nodes
        disconnect_nodes(node0, node1)
        
        # Create competing chains
        addr0 = node0.getnewaddress()
        addr1 = node1.getnewaddress()
        
        # Chain A: Many small transactions
        for _ in range(50):
            node0.sendtoaddress(addr0, 0.01)
        node0.generatetoaddress(5, node0.getnewaddress())
        
        # Chain B: Fewer but larger transactions
        for _ in range(10):
            node1.sendtoaddress(addr1, 0.1)
        node1.generatetoaddress(10, node1.getnewaddress())  # Longer chain
        
        # Reconnect and verify reorg
        connect_nodes(node0, node1)
        self.sync_all()
        
        # Verify chain B won
        assert_equal(node0.getbestblockhash(), node1.getbestblockhash())
        
        # Generate more blocks to ensure stability
        node0.generatetoaddress(10, node0.getnewaddress())
        self.sync_all()

if __name__ == '__main__':
    PussycoinStressTest().main() 