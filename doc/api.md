# Pussycoin API Documentation

This document describes the Pussycoin RPC API and other interfaces. Pussycoin extends the standard Bitcoin/Litecoin RPC interface with additional MWEB-specific commands and modifications for the 10-second block time and smooth emission schedule.

## RPC Interface

### General Information

- Default mainnet RPC port: 9445
- Default testnet RPC port: 19445
- Default regtest RPC port: 19446

Authentication is required and configured in `pussycoin.conf`:
```ini
rpcuser=your_username
rpcpassword=your_secure_password
```

### Standard Commands

Pussycoin supports all standard Bitcoin RPC commands. See [Bitcoin RPC API Reference](https://developer.bitcoin.org/reference/rpc/) for details.

### MWEB-Specific Commands

#### MWEB Information
```bash
getmwebinfo
```
Returns information about the MWEB state:
- Current MWEB block height
- Total MWEB supply
- Active kernels
- Hogex count

#### MWEB Wallet
```bash
getnewaddress "account" "address_type"
```
Generate new address. For MWEB:
- `address_type`: "mweb" or "mwebstealth"
- Returns: MWEB address starting with "pussymweb1"

```bash
getmwebbalance ( "dummy" minconf include_watchonly )
```
Get MWEB balance:
- `minconf`: Minimum confirmations (default: 1)
- `include_watchonly`: Include watch-only addresses (default: false)
- Returns: Balance in PUSSY

```bash
listmwebtransactions ( count skip include_watchonly)
```
List MWEB transactions:
- `count`: Number of transactions (default: 10)
- `skip`: Skip first N transactions (default: 0)
- Returns: Array of MWEB transactions

#### MWEB Transactions
```bash
sendtoaddress "address" amount ( "comment" "comment_to" subtractfeefromamount replaceable conf_target "estimate_mode" )
```
Send to address (works with both regular and MWEB addresses):
- `address`: Regular or MWEB address
- `amount`: Amount in PUSSY
- Returns: Transaction ID

```bash
sendmany "" {"address":amount,...} ( minconf "comment" ["address",...] replaceable conf_target "estimate_mode" )
```
Send to multiple addresses:
- Supports mixing regular and MWEB addresses
- Returns: Transaction ID

### Emission and Mining

#### Block Information
```bash
getblocktemplate ( TemplateRequest )
```
Get block template for mining:
- Returns current block reward based on smooth emission
- Includes MWEB kernels and outputs

```bash
getmininginfo
```
Returns mining information:
- Current block reward
- Network hashrate
- LWMA-3 difficulty

#### Emission Schedule
```bash
getblocksubsidy [height]
```
Get block subsidy at height:
- `height`: Optional block height (default: current)
- Returns:
  - `miner`: Block reward in PUSSY
  - `founders`: Founders reward (if any)
  - `tail`: true if in tail emission

### Network

#### Node Information
```bash
getnodeinfo
```
Get node information:
- Node version
- Protocol version
- Network (mainnet/testnet/regtest)
- Current block height
- Network difficulty
- Connected peers

#### Peer Information
```bash
getpeerinfo
```
Get peer information:
- Connection details
- Protocol version
- Services (including MWEB support)
- Ping time

### Wallet

#### Address Management
```bash
getnewaddress ( "label" "address_type" )
```
Get new address:
- `address_type`: "legacy", "p2sh-segwit", "bech32", "mweb", "mwebstealth"
- Returns: Address in specified format

#### Transaction Management
```bash
listtransactions ( "label" count skip include_watchonly )
```
List transactions:
- Includes both regular and MWEB transactions
- Shows peg-in/peg-out status for MWEB

### WebSocket Interface

Pussycoin provides a WebSocket interface for real-time notifications:

#### Subscriptions
```javascript
// Subscribe to new blocks
socket.send({
    "method": "blockchain.subscribe"
});

// Subscribe to address updates
socket.send({
    "method": "address.subscribe",
    "params": ["address"]
});

// Subscribe to MWEB events
socket.send({
    "method": "mweb.subscribe"
});
```

#### Notifications

New Block:
```json
{
    "method": "blockchain.headers.subscribe",
    "params": [{
        "height": 123456,
        "hex": "...",
        "mweb": {
            "kernel_count": 5,
            "output_count": 10
        }
    }]
}
```

Address Update:
```json
{
    "method": "address.update",
    "params": [
        "address",
        {
            "height": 123456,
            "tx_hash": "...",
            "value": 1.23456789
        }
    ]
}
```

MWEB Event:
```json
{
    "method": "mweb.update",
    "params": [{
        "type": "kernel_added",
        "kernel_id": "...",
        "lock_height": 123456
    }]
}
```

### Error Codes

Pussycoin uses standard Bitcoin error codes plus additional MWEB-specific codes:

| Code    | Message                  | Description                           |
|---------|--------------------------|---------------------------------------|
| -33001  | MWEB not enabled        | MWEB functionality is not enabled     |
| -33002  | Invalid MWEB address    | Malformed MWEB address               |
| -33003  | MWEB kernel not found   | Referenced kernel does not exist     |
| -33004  | MWEB peg-in failed      | Failed to peg into MWEB             |
| -33005  | MWEB peg-out failed     | Failed to peg out of MWEB           |
| -33006  | Invalid kernel          | Kernel validation failed             |
| -33007  | Hogex validation failed | HogEx transaction validation failed  |

## HTTP REST Interface

Pussycoin provides a REST interface on port 9446 (mainnet):

### Endpoints

#### Blockchain
- `GET /rest/block/<hash>.<format>`
- `GET /rest/block/notxdetails/<hash>.<format>`
- `GET /rest/headers/<count>/<hash>.<format>`

#### Transactions
- `GET /rest/tx/<txid>.<format>`
- `GET /rest/getutxos/<txid>-<n>/<txid>-<n>/.../<txid>-<n>.<format>`

#### MWEB-Specific
- `GET /rest/mweb/kernel/<kernel_id>.<format>`
- `GET /rest/mweb/output/<output_id>.<format>`
- `GET /rest/mweb/status.<format>`

### Formats
- `.json`: JSON format
- `.hex`: Raw hex format
- `.bin`: Raw binary format

## ZMQ Notifications

Pussycoin supports ZeroMQ notifications. Configure in `pussycoin.conf`:

```ini
zmqpubhashtx=tcp://127.0.0.1:29445
zmqpubhashblock=tcp://127.0.0.1:29445
zmqpubrawblock=tcp://127.0.0.1:29445
zmqpubrawtx=tcp://127.0.0.1:29445
zmqpubmwebkernel=tcp://127.0.0.1:29445
```

### Topics

Standard Topics:
- `hashtx`: Transaction hash
- `hashblock`: Block hash
- `rawblock`: Raw block data
- `rawtx`: Raw transaction data

MWEB-Specific Topics:
- `mwebkernel`: MWEB kernel updates
- `mweboutput`: MWEB output updates
- `mwebstatus`: MWEB status changes

## Libraries

### Python

```python
from pussycoin import PussycoinRPC

# Connect to Pussycoin node
rpc = PussycoinRPC('user', 'password', 'localhost', 9445)

# Get MWEB info
mweb_info = rpc.getmwebinfo()

# Create MWEB transaction
txid = rpc.sendtoaddress('pussymweb1...', 1.0)
```

### JavaScript

```javascript
const PussycoinClient = require('pussycoin-client');

// Connect to Pussycoin node
const client = new PussycoinClient({
    host: 'localhost',
    port: 9445,
    user: 'user',
    password: 'password'
});

// Get MWEB balance
client.getMWEBBalance().then(balance => {
    console.log('MWEB Balance:', balance);
});
```

## Testing

### Functional Tests
```bash
# Run all tests
test/functional/test_runner.py

# Run specific MWEB tests
test/functional/feature_pussycoin_mweb.py
test/functional/feature_pussycoin_mweb_vectors.py
```

### Integration Tests
```bash
# Build and run tests
make check
```

### Performance Benchmarks
```bash
# Run benchmarks
src/bench/bench_pussycoin

# Run stress tests
test/functional/feature_pussycoin_stress.py
``` 