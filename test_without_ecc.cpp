#include <iostream>
#include <string>

// Minimal test to check if the issue is specifically with ECC initialization
// This replicates BasicTestingSetup constructor without ECC_Start()

class MockSetup {
public:
    MockSetup() {
        std::cout << "MockSetup constructor starting..." << std::endl;
        
        // Skip ECC initialization - this is what we suspect is causing the crash
        // ECC_Start();  // COMMENTED OUT
        
        std::cout << "MockSetup constructor completed (without ECC)" << std::endl;
    }
    
    ~MockSetup() {
        std::cout << "MockSetup destructor starting..." << std::endl;
        // ECC_Stop(); // COMMENTED OUT
        std::cout << "MockSetup destructor completed" << std::endl;
    }
};

void test_basic_sanity() {
    std::cout << "Starting basic_sanity test..." << std::endl;
    
    // This mimics what the sanity test does but without ECC
    bool success = true;
    
    std::cout << "Basic sanity test completed: " << (success ? "PASS" : "FAIL") << std::endl;
}

int main() {
    std::cout << "=== Test Without ECC ===" << std::endl;
    
    try {
        MockSetup setup;
        test_basic_sanity();
        
        std::cout << "Test completed successfully - no memory access violation!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Unknown exception" << std::endl;
        return 1;
    }
} 