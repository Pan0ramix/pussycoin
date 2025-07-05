#include <iostream>
#include <cstdlib>

// Include the minimal required headers
#include "util/system.h"
#include "key.h"

int main() {
    std::cout << "Starting minimal initialization test..." << std::endl;
    
    try {
        // Step 1: Basic environment setup
        std::cout << "1. Setting up environment..." << std::endl;
        SetupEnvironment();
        
        // Step 2: Try ECC initialization 
        std::cout << "2. Initializing ECC..." << std::endl;
        if (!ECC_Start()) {
            std::cerr << "ECC_Start() failed!" << std::endl;
            return 1;
        }
        std::cout << "   ECC initialized successfully" << std::endl;
        
        // Step 3: Test basic ECC functionality
        std::cout << "3. Testing basic ECC operations..." << std::endl;
        CKey key;
        key.MakeNewKey(true);
        
        if (!key.IsValid()) {
            std::cerr << "Failed to create valid key!" << std::endl;
            ECC_Stop();
            return 1;
        }
        std::cout << "   Basic ECC operations work" << std::endl;
        
        // Step 4: Cleanup
        std::cout << "4. Cleaning up..." << std::endl;
        ECC_Stop();
        
        std::cout << "SUCCESS: All tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught!" << std::endl;
        return 1;
    }
} 