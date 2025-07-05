# Pussycoin MWEB Privacy Guide

This guide explains how to use Pussycoin's MWEB (Mimblewimble Extension Block) privacy features effectively.

## Table of Contents
1. [Introduction](#introduction)
2. [MWEB Addresses](#mweb-addresses)
3. [Transaction Types](#transaction-types)
4. [Privacy Best Practices](#privacy-best-practices)
5. [Common Operations](#common-operations)
6. [Troubleshooting](#troubleshooting)

## Introduction

MWEB (Mimblewimble Extension Block) is a privacy-enhancing technology that allows for confidential transactions in Pussycoin. Key features include:

- **Confidential Transactions**: All MWEB amounts are hidden
- **Transaction Aggregation**: Multiple transactions are merged, obscuring the transaction graph
- **Cut-Through**: Spent outputs are removed, reducing blockchain size
- **Optional Privacy**: Choose when to use MWEB features

## MWEB Addresses

### Address Types

1. **Standard MWEB Address**
   ```bash
   pussycoin-cli getnewaddress "label" "mweb"
   ```
   - Starts with "pussymweb1"
   - Used for regular MWEB transactions
   - Reusable address

2. **Stealth MWEB Address**
   ```bash
   pussycoin-cli getnewaddress "label" "mwebstealth"
   ```
   - Also starts with "pussymweb1"
   - Single-use address for enhanced privacy
   - Generates new stealth address for each transaction

### Address Management

```bash
# List all MWEB addresses
pussycoin-cli getaddressesbylabel "mweb-wallet"

# Validate MWEB address
pussycoin-cli validateaddress "pussymweb1..."

# Get MWEB address info
pussycoin-cli getaddressinfo "pussymweb1..."
```

## Transaction Types

### 1. Peg-In (Regular → MWEB)
Converting regular PUSSY to private MWEB PUSSY:

```bash
# Simple peg-in
pussycoin-cli sendtoaddress "pussymweb1..." 10.0

# Advanced peg-in with options
pussycoin-cli sendtoaddress "pussymweb1..." 10.0 "" "" false true 1 "UNSET"
```

### 2. MWEB-to-MWEB Transfer
Private transfers between MWEB addresses:

```bash
# Single recipient
pussycoin-cli sendtoaddress "pussymweb1..." 5.0

# Multiple recipients
pussycoin-cli sendmany "" {
    "pussymweb1...": 3.0,
    "pussymweb1...": 2.0
}
```

### 3. Peg-Out (MWEB → Regular)
Converting private MWEB PUSSY back to regular PUSSY:

```bash
# Simple peg-out
pussycoin-cli sendtoaddress "regular_address" 1.0

# Peg-out with minimum confirmations
pussycoin-cli sendtoaddress "regular_address" 1.0 "" "" false false 6
```

## Privacy Best Practices

### 1. Transaction Timing
- **Avoid Patterns**: Don't transact at regular intervals
- **Wait Between Operations**: Allow time between peg-in and peg-out
- **Use Peak Hours**: Transact when network activity is high

### 2. Amount Management
- **Break Up Large Amounts**: Split into smaller transactions
- **Avoid Round Numbers**: Use irregular amounts
- **Mix With Other Transactions**: Let your transaction get mixed with others

### 3. Address Usage
- **Use Stealth Addresses**: For highest privacy
- **Never Reuse Addresses**: Generate new addresses frequently
- **Separate Wallets**: Use different wallets for different purposes

### 4. Network Privacy
- **Use Tor**: Configure Pussycoin with Tor
- **Connect to Multiple Nodes**: Increase network diversity
- **Avoid Revealing IP**: Don't run a public node with MWEB transactions

## Common Operations

### Checking Balances
```bash
# Get total MWEB balance
pussycoin-cli getmwebbalance

# Get balance by address
pussycoin-cli getaddressbalance "pussymweb1..."
```

### Transaction History
```bash
# List MWEB transactions
pussycoin-cli listmwebtransactions

# Get specific transaction
pussycoin-cli gettransaction "txid"
```

### Coin Control
```bash
# List unspent MWEB outputs
pussycoin-cli listmwebunspent

# Select specific outputs
pussycoin-cli sendtoaddress "pussymweb1..." 1.0 "" "" false true 1 "UNSET" '["output_id1",...]'
```

### Backup and Recovery
```bash
# Backup MWEB wallet
pussycoin-cli backupwallet "path/to/backup"

# Dump MWEB private keys
pussycoin-cli dumpmwebprivkey "pussymweb1..."

# Import MWEB private key
pussycoin-cli importmwebprivkey "key"
```

## Troubleshooting

### Common Issues

1. **Transaction Stuck**
   ```bash
   # Rescan MWEB state
   pussycoin-cli rescanmwebchain

   # Rebroadcast transaction
   pussycoin-cli resendmwebtransaction "txid"
   ```

2. **Sync Issues**
   ```bash
   # Check MWEB sync status
   pussycoin-cli getmwebinfo

   # Reset MWEB state
   pussycoin-cli resetmwebchain
   ```

3. **Balance Issues**
   ```bash
   # Rescan wallet
   pussycoin-cli rescanblockchain

   # Rebuild MWEB database
   pussycoin-cli rebuildmwebdb
   ```

### Error Messages

| Error                    | Solution                                    |
|-------------------------|---------------------------------------------|
| "MWEB not synced"       | Wait for full sync or check network        |
| "Invalid kernel"        | Transaction invalid, create new one         |
| "Output not found"      | Wait for confirmation or rescan            |
| "Kernel reorg"          | Wait for chain to stabilize                |

### Debug Commands
```bash
# Enable MWEB debug logging
pussycoin-cli logging "["mweb"]"

# Get detailed MWEB status
pussycoin-cli getmwebdebuginfo

# Check MWEB mempool
pussycoin-cli getmwebmempoolinfo
```

## Advanced Topics

### 1. Multi-Party Transactions
MWEB supports creating transactions with multiple participants:

```bash
# Create multi-party transaction
pussycoin-cli createmwebmultisig 2 ["pussymweb1...", "pussymweb1..."]
```

### 2. Time-Locked Transactions
Create transactions that can't be spent until a certain time:

```bash
# Create time-locked transaction
pussycoin-cli sendtoaddress "pussymweb1..." 1.0 "" "" false true 1 "LOCKTIME" 1234567
```

### 3. Scripting Support
MWEB supports limited scripting capabilities:

```bash
# Create hash-locked transaction
pussycoin-cli createmwebhtlc "hash" 100 "pussymweb1..."
```

### 4. Monitoring
Set up monitoring for MWEB transactions:

```bash
# Subscribe to MWEB notifications
pussycoin-cli mwebnotify '["kernel", "output"]'

# Monitor MWEB mempool
watch -n 10 'pussycoin-cli getmwebmempoolinfo'
```

## Security Considerations

1. **Network Security**
   - Use encrypted connections
   - Configure firewall rules
   - Monitor for suspicious activity

2. **Wallet Security**
   - Encrypt MWEB wallet
   - Regular backups
   - Use hardware wallets when possible

3. **Transaction Security**
   - Verify recipient addresses
   - Check transaction details
   - Wait for confirmations

4. **Privacy Enhancements**
   - Use VPN/Tor
   - Avoid linking identities
   - Regular wallet rotation 