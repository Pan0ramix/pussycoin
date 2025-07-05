#include <iostream>
#include "src/key.h"

int main() {
    std::cout << "Starting ECC test..." << std::endl;
    try {
        ECC_Start();
        std::cout << "ECC_Start completed successfully" << std::endl;
        
        bool result = ECC_InitSanityCheck();
        std::cout << "ECC_InitSanityCheck result: " << result << std::endl;
        
        ECC_Stop();
        std::cout << "ECC_Stop completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 