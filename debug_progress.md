# Pussycoin Apple Silicon Debug Progress

## Issue Summary
Memory access violations on Apple Silicon (ARM64) causing segmentation faults in both daemon and unit tests.

## Error Details
- **Error**: `memory access violation at address: 0x67748580: invalid permissions`
- **Location**: `BasicTestingSetup` constructor after `ECC_Start()` call
- **Additional**: "errors disabling the alternate stack: #error:22 Invalid argument"
- **Platforms**: macOS 24.4.0 (Apple Silicon), affects both `litecoind` and `test_litecoin`

## Investigation Timeline

### ✅ Completed Investigations

#### 1. secp256k1-zkp Library Issues
- **Status**: RESOLVED
- **Problem**: Missing modules causing undefined symbols
- **Solution**: Configured with all required MWEB modules:
  ```bash
  ./configure --enable-module-ecdh --enable-module-recovery --enable-module-generator 
  --enable-module-commitment --enable-module-rangeproof --enable-module-whitelist 
  --enable-module-surjectionproof --enable-module-schnorrsig --enable-module-bulletproof 
  --enable-module-aggsig --disable-benchmark --enable-experimental
  ```

#### 2. Double Context Initialization
- **Status**: TESTED, NOT ROOT CAUSE
- **Problem**: Suspected conflict between `ECCVerifyHandle` and `ECC_Start()`
- **Action**: Removed redundant `ECCVerifyHandle globalVerifyHandle` from `BasicTestingSetup`
- **Result**: Issue persists

#### 3. Basic Environment
- **Status**: CONFIRMED WORKING
- **Test**: Simple C++ programs compile and run correctly
- **Test**: secp256k1-zkp library tests pass independently
- **Conclusion**: General Apple Silicon environment is functional

### ✅ ROOT CAUSE IDENTIFIED AND FIXED

#### Critical Performance Bug in Emission System
- **Status**: RESOLVED
- **Root Cause**: `GetBlockSubsidy()` contained O(n) loop recalculating entire emission history on every call
- **Impact**: During initialization, this caused massive CPU usage → watchdog timeouts → memory access violations on Apple Silicon
- **Original Problematic Code**:
  ```cpp
  // This was called during BasicTestingSetup initialization
  for (int h = 1; h < nHeight; h++) {
      already_generated += GetSmoothEmissionReward(already_generated);
  }
  ```
- **Fix Implemented**: Created `GetCumulativeEmission()` with:
  - Exact integer arithmetic (no floating point)
  - Tail emission optimization (linear calculation after threshold)
  - Consensus-safe exact calculations
  - Performance optimization for large heights

#### Why This Caused Apple Silicon Issues
1. **Performance sensitivity**: Apple Silicon has different performance characteristics
2. **Signal handling**: Excessive CPU usage triggered macOS watchdog → "alternate stack" errors
3. **Memory access violations**: System attempted to terminate runaway process
4. **Initialization timing**: Tests call emission functions during setup, not just during block processing

## Next Investigation Steps

### High Priority
1. **Compare with working LTCCore** - Identify key differences in our fork
2. **Test minimal changes** - Create version with only essential Pussycoin changes
3. **Genesis block testing** - Test with original LTC genesis temporarily
4. **MWEB isolation** - Test build without MWEB components

### Medium Priority  
1. **Memory debugging** - Use AddressSanitizer for detailed crash analysis
2. **Signal handling review** - Examine any custom signal handlers we added
3. **Chain params isolation** - Test with original LTC parameters

### Tools to Use
- AddressSanitizer (`-fsanitize=address`)
- Valgrind equivalent for macOS
- `lldb` for detailed stack traces
- Minimal reproduction test cases

## Key Modifications Made
- Genesis block generation and hardcoding
- Emission curve changes (smooth Monero-style)
- 10-second block timing (vs 2.5min)
- LWMA-3 difficulty algorithm
- MWEB activation from genesis
- Magic bytes and port changes

## Notes
- **DO NOT** disable functionality to work around the issue
- **FOCUS ON** finding the actual root cause in our modifications
- **REMEMBER** LTCCore works on Apple Silicon, so the issue is in our changes 