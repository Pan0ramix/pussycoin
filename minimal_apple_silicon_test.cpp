#include <iostream>
#include <util/system.h>
#include <logging.h>
#include <chainparams.h>
#include <key.h>

// Step by step testing to isolate Apple Silicon crash
int main() {
    std::cout << "=== Minimal Apple Silicon Test ===" << std::endl;
    
    try {
        std::cout << "Step 1: Basic C++ initialization..." << std::endl;
        int test = 42;
        std::cout << "  Basic variable: " << test << std::endl;
        
        std::cout << "Step 2: Headers included successfully" << std::endl;
        
        std::cout << "Step 3: Testing ECC_Start()..." << std::endl;
        ECC_Start();
        std::cout << "  ✓ ECC_Start() completed" << std::endl;
        
        std::cout << "Step 4: Testing SelectParams()..." << std::endl;
        SelectParams(CBaseChainParams::MAIN);
        std::cout << "  ✓ SelectParams() completed" << std::endl;
        
        std::cout << "🎉 ALL STEPS PASSED!" << std::endl;
        
        ECC_Stop();
        
    } catch (const std::exception& e) {
        std::cout << "❌ Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "❌ Unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
} 