#include <iostream>
#include <cstdlib>

// Minimal reproduction of the secp256k1 initialization
// Include the actual secp256k1 header for proper constants
#include "secp256k1.h"

extern "C" {
    // Forward declarations from secp256k1 
    typedef struct secp256k1_context_struct secp256k1_context;
    
    // These would normally come from secp256k1.h
    secp256k1_context* secp256k1_context_create(unsigned int flags);
    void secp256k1_context_destroy(secp256k1_context* ctx);
    int secp256k1_context_randomize(secp256k1_context* ctx, const unsigned char* seed32);
}

int main() {
    std::cout << "Starting minimal ECC test..." << std::endl;
    
    try {
        // Use the correct flag from the header instead of manually defining it
        std::cout << "About to call secp256k1_context_create with SECP256K1_CONTEXT_SIGN..." << std::endl;
        
        secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
        
        if (ctx == nullptr) {
            std::cout << "secp256k1_context_create returned NULL" << std::endl;
            return 1;
        }
        
        std::cout << "secp256k1_context_create succeeded: " << ctx << std::endl;
        
        // Try to randomize (this also fails sometimes)
        unsigned char seed[32] = {0}; // Zero seed for simplicity
        std::cout << "About to randomize context..." << std::endl;
        
        int result = secp256k1_context_randomize(ctx, seed);
        std::cout << "Randomize result: " << result << std::endl;
        
        // Clean up
        secp256k1_context_destroy(ctx);
        std::cout << "Test completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Unknown exception caught" << std::endl;
        return 1;
    }
    
    return 0;
} 