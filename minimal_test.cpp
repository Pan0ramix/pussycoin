#include <iostream>
#include <cassert>

// Test basic functionality without Boost framework
int main() {
    std::cout << "Starting minimal Apple Silicon test..." << std::endl;
    
    // Test 1: Basic arithmetic
    std::cout << "Test 1: Basic arithmetic..." << std::endl;
    assert(1 + 1 == 2);
    std::cout << "  ✓ Basic arithmetic works" << std::endl;
    
    // Test 2: Memory allocation
    std::cout << "Test 2: Memory allocation..." << std::endl;
    int* ptr = new int(42);
    assert(*ptr == 42);
    delete ptr;
    std::cout << "  ✓ Memory allocation works" << std::endl;
    
    // Test 3: Standard library
    std::cout << "Test 3: Standard library..." << std::endl;
    std::string test_str = "Hello Apple Silicon";
    assert(test_str.length() == 19);
    std::cout << "  ✓ Standard library works" << std::endl;
    
    std::cout << "All minimal tests passed! ✓" << std::endl;
    std::cout << "This suggests the issue is in Boost test framework or static initialization." << std::endl;
    
    return 0;
} 