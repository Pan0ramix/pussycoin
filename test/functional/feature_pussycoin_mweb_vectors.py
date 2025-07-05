#!/usr/bin/env python3
# Copyright (c) 2024 The Pussycoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test vectors for Pussycoin MWEB functionality."""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_greater_than,
    assert_raises_rpc_error,
    connect_nodes,
    disconnect_nodes,
)
from decimal import Decimal

class PussycoinMWEBVectorsTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 4
        self.extra_args = [
            ["-txindex"],  # Node 0: Regular node
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
        self.log.info("Testing Pussycoin MWEB vectors...")
        
        # Test vectors for different MWEB scenarios
        self.test_mweb_address_types()
        self.test_pegging_vectors()
        self.test_hogex_vectors()
        self.test_kernel_vectors()
        self.test_mweb_reorg()
        self.test_mweb_mempool()
        self.test_mweb_edge_cases()

    def test_mweb_address_types(self):
        """Test different MWEB address types and formats"""
        self.log.info("Testing MWEB address types...")
        
        node = self.nodes[0]
        
        # Test standard MWEB address
        mweb_addr = node.getnewaddress("", "mweb")
        assert mweb_addr.startswith("pussymweb1")
        
        # Test stealth address
        stealth_addr = node.getnewaddress("", "mwebstealth")
        assert stealth_addr.startswith("pussymweb1")
        
        # Test invalid address type
        assert_raises_rpc_error(-5, "Invalid address type",
                              node.getnewaddress, "", "invalidtype")

    def test_pegging_vectors(self):
        """Test various pegging scenarios"""
        self.log.info("Testing MWEB pegging vectors...")
        
        node0 = self.nodes[0]  # Regular node
        node1 = self.nodes[1]  # MWEB-only spends
        
        # Generate initial coins
        node0.generatetoaddress(101, node0.getnewaddress())
        self.sync_all()
        
        # Test vector 1: Simple peg-in
        mweb_addr = node0.getnewaddress("", "mweb")
        txid1 = node0.sendtoaddress(mweb_addr, 10.0)
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        # Verify peg-in state
        tx1 = node0.gettransaction(txid1)
        assert_equal(tx1["mweb"]["type"], "pegin")
        
        # Test vector 2: Multiple peg-ins to same address
        txid2 = node0.sendtoaddress(mweb_addr, 5.0)
        txid3 = node0.sendtoaddress(mweb_addr, 3.0)
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        # Test vector 3: Peg-out to multiple addresses
        addr1 = node0.getnewaddress()
        addr2 = node0.getnewaddress()
        
        # Create peg-out transaction
        outputs = {
            addr1: 2.0,
            addr2: 3.0
        }
        txid4 = node1.sendmany("", outputs)
        node1.generatetoaddress(1, node1.getnewaddress())
        self.sync_all()

    def test_hogex_vectors(self):
        """Test HogEx (MWEB transaction) vectors"""
        self.log.info("Testing HogEx vectors...")
        
        node0 = self.nodes[0]
        node1 = self.nodes[1]
        
        # Generate coins in MWEB
        mweb_addr0 = node0.getnewaddress("", "mweb")
        mweb_addr1 = node1.getnewaddress("", "mweb")
        
        # Peg-in some coins
        node0.sendtoaddress(mweb_addr0, 20.0)
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        # Test vector 1: Simple HogEx
        txid1 = node0.sendtoaddress(mweb_addr1, 5.0)
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        # Test vector 2: Multi-kernel HogEx
        outputs = {
            mweb_addr1: 3.0,
            node0.getnewaddress("", "mweb"): 2.0
        }
        txid2 = node0.sendmany("", outputs)
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()

    def test_kernel_vectors(self):
        """Test MWEB kernel validation vectors"""
        self.log.info("Testing kernel validation vectors...")
        
        node0 = self.nodes[0]
        
        # Generate test coins
        mweb_addr = node0.getnewaddress("", "mweb")
        node0.sendtoaddress(mweb_addr, 10.0)
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        # Test vector 1: Standard kernel
        tx1 = node0.sendtoaddress(node0.getnewaddress("", "mweb"), 1.0)
        
        # Test vector 2: Kernel with lock height
        tx2 = node0.sendtoaddress(node0.getnewaddress("", "mweb"), 1.0, "", "", False, True, 1, "LOCKHEIGHT")
        
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()

    def test_mweb_reorg(self):
        """Test MWEB behavior during chain reorganization"""
        self.log.info("Testing MWEB reorg handling...")
        
        node0 = self.nodes[0]
        node1 = self.nodes[1]
        
        # Disconnect nodes
        disconnect_nodes(node0, node1)
        
        # Create competing chains
        mweb_addr0 = node0.getnewaddress("", "mweb")
        mweb_addr1 = node1.getnewaddress("", "mweb")
        
        # Chain A: Node 0
        node0.sendtoaddress(mweb_addr0, 5.0)
        node0.generatetoaddress(5, node0.getnewaddress())
        
        # Chain B: Node 1 (longer)
        node1.sendtoaddress(mweb_addr1, 5.0)
        node1.generatetoaddress(10, node1.getnewaddress())
        
        # Reconnect and verify reorg
        connect_nodes(node0, node1)
        self.sync_all()
        
        # Verify chain B won
        assert_equal(node0.getbestblockhash(), node1.getbestblockhash())

    def test_mweb_mempool(self):
        """Test MWEB transaction mempool behavior"""
        self.log.info("Testing MWEB mempool handling...")
        
        node0 = self.nodes[0]
        
        # Generate test coins
        mweb_addr = node0.getnewaddress("", "mweb")
        node0.sendtoaddress(mweb_addr, 20.0)
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        # Create multiple MWEB transactions without mining
        for i in range(5):
            node0.sendtoaddress(node0.getnewaddress("", "mweb"), 1.0)
        
        # Verify mempool state
        mempool = node0.getrawmempool()
        assert_equal(len(mempool), 5)
        
        # Mine transactions and verify
        node0.generatetoaddress(1, node0.getnewaddress())
        self.sync_all()
        
        mempool = node0.getrawmempool()
        assert_equal(len(mempool), 0)

    def test_mweb_edge_cases(self):
        """Test MWEB edge cases and error conditions"""
        self.log.info("Testing MWEB edge cases...")
        
        node0 = self.nodes[0]
        node3 = self.nodes[3]  # No MWEB node
        
        # Test vector 1: Send to MWEB address from non-MWEB node
        mweb_addr = node0.getnewaddress("", "mweb")
        assert_raises_rpc_error(-5, "MWEB not enabled",
                              node3.sendtoaddress, mweb_addr, 1.0)
        
        # Test vector 2: Invalid MWEB address
        assert_raises_rpc_error(-5, "Invalid MWEB address",
                              node0.sendtoaddress, "invalid_mweb_addr", 1.0)
        
        # Test vector 3: Zero amount MWEB transaction
        assert_raises_rpc_error(-3, "Amount out of range",
                              node0.sendtoaddress, mweb_addr, 0.0)

if __name__ == '__main__':
    PussycoinMWEBVectorsTest().main() 