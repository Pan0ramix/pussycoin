#include <iostream>
#include <memory>

// Core Bitcoin includes
#include "random.h"
#include "key.h"
#include "chainparams.h"
#include "util/system.h"

int main() {
    std::cout << "=== Simple Apple Silicon Test ===" << std::endl;
    
    try {
        std::cout << "Step 1: Initializing ECC..." << std::endl;
        ECC_Start();
        std::cout << "  ✓ ECC_Start() completed" << std::endl;
        
        std::cout << "Step 2: Selecting chain params..." << std::endl;
        SelectParams(CBaseChainParams::MAIN);
        std::cout << "  ✓ SelectParams() completed" << std::endl;
        
        std::cout << "Step 3: Testing random number generation..." << std::endl;
        uint64_t rand_val = GetRand(1000000);
        std::cout << "  ✓ GetRand() returned: " << rand_val << std::endl;
        
        std::cout << "Step 4: Testing key generation..." << std::endl;
        CKey key;
        key.MakeNewKey(true);
        std::cout << "  ✓ Key generation completed" << std::endl;
        
        std::cout << "🎉 ALL TESTS PASSED ON APPLE SILICON!" << std::endl;
        
        ECC_Stop();
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "❌ Unknown exception" << std::endl;
        return 1;
    }
} 