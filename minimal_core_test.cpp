#include <iostream>
#include "random.h"
#include "uint256.h"
#include "key.h"

// Test each component individually to isolate the Apple Silicon crash
int main() {
    std::cout << "=== Minimal Core Component Test ===" << std::endl;
    
    try {
        std::cout << "Step 1: Basic C++ test..." << std::endl;
        int test = 42;
        std::cout << "  ✓ Basic test: " << test << std::endl;
        
        // Test FastRandomContext first since it was problematic
        std::cout << "Step 2: Testing FastRandomContext..." << std::endl;
        FastRandomContext ctx(true); // deterministic
        std::cout << "  ✓ FastRandomContext created" << std::endl;
        
        // Test uint256 operations
        std::cout << "Step 3: Testing uint256..." << std::endl;
        uint256 test_hash;
        test_hash.SetNull();
        std::cout << "  ✓ uint256 operations work" << std::endl;
        
        // Test ECC initialization (known problematic)
        std::cout << "Step 4: Testing ECC initialization..." << std::endl;
        ECC_Start();
        std::cout << "  ✓ ECC_Start() completed" << std::endl;
        
        std::cout << "🎉 ALL BASIC TESTS PASSED!" << std::endl;
        
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