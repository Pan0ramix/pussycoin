# Pussycoin (PUSSY)

Pussycoin is a privacy-focused cryptocurrency forked from Litecoin Core, featuring MWEB (Mimblewimble Extension Block) privacy technology, 10-second block times, and a smooth emission schedule.

## Key Features

- **Fast Blocks**: 10-second block time for rapid transactions
- **MWEB Privacy**: Built-in Mimblewimble privacy features
- **Smooth Emission**: Monero-style smooth emission with tail emission
- **Modern Consensus**: LWMA-3 difficulty adjustment algorithm
- **Advanced Features**: Taproot, SegWit, and all modern Bitcoin improvements active from genesis

## Emission Schedule

Pussycoin features a smooth emission schedule inspired by Monero:

- Pre-tail supply: ~9.2M PUSSY
- Tail emission: 0.02 PUSSY per block (perpetual)
- Block reward: Smoothly decreasing until tail emission
- Block time: 10 seconds
- Total supply: Infinite (with decreasing inflation)

## Building Pussycoin

### Prerequisites

- GCC/Clang compiler (C++17 support required)
- CMake 3.16 or newer
- Boost 1.74.0 or newer
- OpenSSL 1.1 or newer
- Berkeley DB 4.8 or newer
- ZeroMQ 4.3.1 or newer
- Qt 5.15 or newer (for GUI)

### Build Instructions

#### Linux/macOS

```bash
# Install dependencies (Ubuntu example)
sudo apt-get install build-essential libtool autotools-dev automake pkg-config bsdmainutils python3
sudo apt-get install libssl-dev libevent-dev libboost-system-dev libboost-filesystem-dev libboost-chrono-dev
sudo apt-get install libboost-program-options-dev libboost-test-dev libboost-thread-dev
sudo apt-get install libminiupnpc-dev libzmq3-dev
sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools
sudo apt-get install libqrencode-dev

# Clone repository
git clone https://github.com/pussycoin/pussycoin.git
cd pussycoin

# Build
./autogen.sh
./configure
make -j$(nproc)
```

#### Windows (using MSVC)

1. Install Visual Studio 2019 or newer
2. Install vcpkg and required dependencies
3. Open `build_msvc/pussycoin.sln`
4. Build the solution

### Running Tests

```bash
# Functional tests
python3 test/functional/test_runner.py

# Unit tests
src/test/test_pussycoin

# Bench tests
src/bench/bench_pussycoin
```

## Using MWEB Privacy Features

MWEB (Mimblewimble Extension Block) provides optional privacy features:

### MWEB Addresses

- Generate MWEB address: `pussycoin-cli getnewaddress "label" "mweb"`
- MWEB addresses start with "pussymweb1"

### MWEB Transactions

1. **Peg-In (Regular → MWEB)**
   ```bash
   pussycoin-cli sendtoaddress "mweb_address" amount
   ```

2. **MWEB-to-MWEB Transfer**
   ```bash
   pussycoin-cli sendtoaddress "mweb_address" amount
   ```

3. **Peg-Out (MWEB → Regular)**
   ```bash
   pussycoin-cli sendtoaddress "regular_address" amount
   ```

### Privacy Best Practices

- Use MWEB addresses for sensitive transactions
- Allow time between peg-in and peg-out
- Avoid pattern-forming transaction amounts
- Use coin control to manage MWEB coins

## API Documentation

### RPC Commands

#### MWEB-Specific Commands

- `getmwebbalance`: Get MWEB balance
- `listmwebtransactions`: List MWEB transactions
- `getmwebinfo`: Get MWEB status and statistics

#### Standard Commands

See [Bitcoin RPC API Reference](https://developer.bitcoin.org/reference/rpc/) for common commands.

### Network Ports

- Mainnet: 9444 (RPC: 9445)
- Testnet: 19444 (RPC: 19445)
- Regtest: 19445 (RPC: 19446)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

Pussycoin is released under the terms of the MIT license. See [COPYING](COPYING) for more information.
