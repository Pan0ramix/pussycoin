#!/usr/bin/env python3
# Copyright (c) 2024 The Pussycoin Core developers
# Distributed under the MIT software license

"""
Generate a new genesis block for Pussycoin networks
"""

import hashlib
import struct
import time
import sys
from io import BytesIO

def sha256(data):
    return hashlib.sha256(data).digest()

def double_sha256(data):
    return sha256(sha256(data))

def calculate_merkle_root(txids):
    """Calculate merkle root from list of transaction IDs"""
    if len(txids) == 0:
        return b'\x00' * 32
    
    if len(txids) == 1:
        return txids[0]
    
    next_level = []
    for i in range(0, len(txids), 2):
        left = txids[i]
        right = txids[i + 1] if i + 1 < len(txids) else left
        next_level.append(double_sha256(left + right))
    
    return calculate_merkle_root(next_level)

def create_coinbase_tx():
    """Create the genesis coinbase transaction"""
    # Version: 1
    tx = struct.pack("<I", 1)
    
    # Number of inputs: 1
    tx += struct.pack("<B", 1)
    
    # Input: Previous output hash (null)
    tx += b'\x00' * 32
    
    # Input: Previous output index (0xffffffff for coinbase)
    tx += struct.pack("<I", 0xffffffff)
    
    # Input script: height (0) + arbitrary data
    script_data = b'\x00' + b'Rare, Irresistible, Irreversible'
    script_len = len(script_data)
    tx += struct.pack("<B", script_len) + script_data
    
    # Input sequence: 0xffffffff
    tx += struct.pack("<I", 0xffffffff)
    
    # Number of outputs: 1
    tx += struct.pack("<B", 1)
    
    # Output value: 50 PUSSY = 5000000000 satoshis
    tx += struct.pack("<Q", 5000000000)
    
    # Output script: P2PKH to a standard test key
    pubkey_script = bytes.fromhex('76a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2615109d88ac')
    script_len = len(pubkey_script)
    tx += struct.pack("<B", script_len) + pubkey_script
    
    # Lock time: 0
    tx += struct.pack("<I", 0)
    
    return tx

def create_block_header(version, prev_hash, merkle_root, timestamp, bits, nonce):
    """Create block header"""
    header = struct.pack("<I", version)           # Version
    header += prev_hash                           # Previous block hash
    header += merkle_root                         # Merkle root
    header += struct.pack("<I", timestamp)       # Timestamp
    header += struct.pack("<I", bits)            # Target bits
    header += struct.pack("<I", nonce)           # Nonce
    return header

def mine_genesis_block(network="mainnet", target_bits=0x1e0ffff0):
    """Mine a genesis block by finding valid nonce"""
    # Network-specific timestamps
    timestamps = {
        "mainnet": 1735689600,  # Jan 1, 2025 00:00:00 UTC
        "testnet": 1735689601,  # Jan 1, 2025 00:00:01 UTC  
        "regtest": 1735689602   # Jan 1, 2025 00:00:02 UTC
    }
    
    timestamp = timestamps.get(network, timestamps["mainnet"])
    
    print(f"Creating Pussycoin {network} genesis block...")
    
    # Create coinbase transaction
    coinbase_tx = create_coinbase_tx()
    coinbase_hash = double_sha256(coinbase_tx)
    
    print(f"Coinbase TX: {coinbase_tx.hex()}")
    print(f"Coinbase Hash: {coinbase_hash[::-1].hex()}")  # Reverse for display
    
    # Calculate merkle root (just coinbase for genesis)
    merkle_root = coinbase_hash
    print(f"Merkle Root: {merkle_root[::-1].hex()}")
    
    # Genesis block parameters
    version = 1
    prev_hash = b'\x00' * 32
    
    # Calculate target from bits
    target = 0x0ffff * (2 ** (8 * (0x1e - 3)))
    print(f"Target: {target:064x}")
    
    print(f"Mining {network} genesis block...")
    nonce = 0
    while True:
        header = create_block_header(version, prev_hash, merkle_root, timestamp, target_bits, nonce)
        hash_result = double_sha256(header)
        hash_int = int.from_bytes(hash_result[::-1], 'big')
        
        if hash_int < target:
            print(f"Found {network} genesis block!")
            print(f"Nonce: {nonce}")
            print(f"Hash: {hash_result[::-1].hex()}")
            print(f"Header: {header.hex()}")
            
            # Print C++ code for chainparams.cpp
            print(f"\n// Add to {network} section in chainparams.cpp:")
            print(f"genesis = CreateGenesisBlock({timestamp}, {nonce}, 0x{target_bits:08x}, 1, 50 * COIN);")
            print(f"consensus.hashGenesisBlock = genesis.GetHash();")
            print(f"assert(consensus.hashGenesisBlock == uint256S(\"0x{hash_result[::-1].hex()}\"));")
            print(f"assert(genesis.hashMerkleRoot == uint256S(\"0x{merkle_root[::-1].hex()}\"));")
            
            return {
                'network': network,
                'timestamp': timestamp,
                'nonce': nonce,
                'bits': target_bits,
                'hash': hash_result[::-1].hex(),
                'merkle_root': merkle_root[::-1].hex()
            }
        
        nonce += 1
        if nonce % 100000 == 0:
            print(f"Tried {nonce} nonces...")

if __name__ == "__main__":
    network = sys.argv[1] if len(sys.argv) > 1 else "mainnet"
    if network not in ["mainnet", "testnet", "regtest"]:
        print("Usage: python3 generate_genesis.py [mainnet|testnet|regtest]")
        sys.exit(1)
    
    mine_genesis_block(network)
 