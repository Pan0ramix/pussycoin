#!/usr/bin/env python3
# Copyright (c) 2024 The Pussycoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test Pussycoin network upgrades and consensus changes."""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
    connect_nodes,
    disconnect_nodes,
    wait_until,
)
from test_framework.blocktools import (
    create_block,
    create_coinbase,
)
import time

class PussycoinUpgradesTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 4
        self.extra_args = [
            ["-txindex"],  # Node 0: Control node
            ["-txindex", "-mwebspendsonly=1"],  # Node 1: MWEB-only spends
            ["-txindex", "-mwebpegsonly=1"],  # Node 2: MWEB-only pegs
            ["-txindex", "-nomweb"],  # Node 3: No MWEB
        ]

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def setup_network(self):
        self.setup_nodes()
        # Connect nodes in a line topology
        for i in range(self.num_nodes - 1):
            connect_nodes(self.nodes[i], self.nodes[i + 1])
        self.sync_all()

    def run_test(self):
        self.log.info("Testing Pussycoin network upgrades...")
        
        # Test various network upgrade scenarios
        self.test_genesis_activation()
        self.test_difficulty_adjustment()
        self.test_emission_schedule()
        self.test_version_bits()
        self.test_consensus_forks()

    def test_genesis_activation(self):
        """Test that all features are properly activated from genesis"""
        self.log.info("Testing genesis block activations...")
        
        node = self.nodes[0]
        
        # Generate some blocks
        node.generatetoaddress(10, node.getnewaddress())
        
        # Check blockchain info
        info = node.getblockchaininfo()
        
        # Verify BIP activations
        assert_equal(info["bip9_softforks"]["taproot"]["status"], "active")
        assert_equal(info["bip9_softforks"]["mweb"]["status"], "active")
        
        # Verify SegWit activation
        assert "segwit" in info["softforks"]
        assert info["softforks"]["segwit"]["active"]
        
        # Create and verify P2SH transaction
        addr = node.getnewaddress("", "p2sh-segwit")
        assert addr.startswith("p")  # Pussycoin P2SH prefix
        
        # Create and verify native SegWit transaction
        addr = node.getnewaddress("", "bech32")
        assert addr.startswith("pussy")  # Pussycoin bech32 prefix

    def test_difficulty_adjustment(self):
        """Test LWMA-3 difficulty adjustment"""
        self.log.info("Testing LWMA-3 difficulty adjustment...")
        
        node = self.nodes[0]
        
        # Generate initial blocks
        node.generatetoaddress(90, node.getnewaddress())  # LWMA-3 window
        
        # Get initial difficulty
        initial_diff = int(node.getblock(node.getbestblockhash())["difficulty"])
        
        # Generate blocks quickly
        for _ in range(30):
            node.generatetoaddress(1, node.getnewaddress())
            time.sleep(1)  # 1-second spacing
        
        # Get new difficulty
        fast_diff = int(node.getblock(node.getbestblockhash())["difficulty"])
        assert_greater_than(fast_diff, initial_diff)  # Should increase
        
        # Generate blocks slowly
        for _ in range(30):
            node.generatetoaddress(1, node.getnewaddress())
            time.sleep(20)  # 20-second spacing
        
        # Get final difficulty
        slow_diff = int(node.getblock(node.getbestblockhash())["difficulty"])
        assert_greater_than(fast_diff, slow_diff)  # Should decrease

    def test_emission_schedule(self):
        """Test emission schedule over time"""
        self.log.info("Testing emission schedule...")
        
        node = self.nodes[0]
        
        # Track rewards over time
        rewards = []
        
        # Generate blocks and collect rewards
        for height in range(1, 1001, 100):
            node.generatetoaddress(100, node.getnewaddress())
            
            block = node.getblock(node.getbestblockhash())
            coinbase = node.getrawtransaction(block["tx"][0], True)
            reward = sum(out["value"] for out in coinbase["vout"])
            rewards.append(reward)
        
        # Verify decreasing rewards
        for i in range(len(rewards) - 1):
            assert_greater_than(rewards[i], rewards[i + 1])
        
        # Generate until tail emission
        while rewards[-1] > 0.02:  # 0.02 PUSSY tail emission
            node.generatetoaddress(1000, node.getnewaddress())
            
            block = node.getblock(node.getbestblockhash())
            coinbase = node.getrawtransaction(block["tx"][0], True)
            reward = sum(out["value"] for out in coinbase["vout"])
            rewards.append(reward)
        
        # Verify tail emission
        assert_equal(rewards[-1], 0.02)

    def test_version_bits(self):
        """Test version bits signaling"""
        self.log.info("Testing version bits signaling...")
        
        node = self.nodes[0]
        
        # Generate blocks and check version bits
        for _ in range(100):
            node.generatetoaddress(1, node.getnewaddress())
            block = node.getblock(node.getbestblockhash())
            
            # Version should signal for active features
            version = block["version"]
            assert version & (1 << 2)  # Taproot
            assert version & (1 << 4)  # MWEB

    def test_consensus_forks(self):
        """Test consensus rule enforcement"""
        self.log.info("Testing consensus rule enforcement...")
        
        node0 = self.nodes[0]
        node3 = self.nodes[3]  # No MWEB node
        
        # Disconnect nodes
        disconnect_nodes(node0, node3)
        
        # Create blocks with different rules
        tip = node0.getbestblockhash()
        height = node0.getblockcount()
        block_time = int(time.time())
        
        # Create invalid block (wrong emission)
        block = create_block(int(tip, 16), create_coinbase(height + 1), block_time)
        block.vtx[0].vout[0].nValue = 1000 * 100000000  # 1000 PUSSY (invalid)
        block.solve()
        
        # Node0 should reject the block
        assert_raises_rpc_error(-1, "bad-cb-amount",
                              node0.submitblock, block.serialize().hex())
        
        # Create valid block
        valid_block = create_block(int(tip, 16), create_coinbase(height + 1), block_time)
        valid_block.solve()
        
        # Node0 should accept the block
        assert_equal(node0.submitblock(valid_block.serialize().hex()), None)

if __name__ == '__main__':
    PussycoinUpgradesTest().main() 