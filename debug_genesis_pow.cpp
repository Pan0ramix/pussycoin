#include <iostream>
#include <iomanip>
#include <chainparams.h>
#include <primitives/block.h>
#include <pow.h>
#include <util/strencodings.h>
#include <arith_uint256.h>

int main()
{
    try {
        SelectParams(CBaseChainParams::REGTEST);
        const CChainParams& params = Params();
        const CBlock& genesis = params.GenesisBlock();
        
        std::cout << "=== Genesis Block Debug Info ===" << std::endl;
        std::cout << "Genesis Hash: " << genesis.GetHash().ToString() << std::endl;
        std::cout << "Genesis PoWHash: " << genesis.GetPoWHash().ToString() << std::endl;
        std::cout << "Genesis nBits: 0x" << std::hex << genesis.nBits << std::dec << std::endl;
        std::cout << "Genesis nNonce: " << genesis.nNonce << std::endl;
        std::cout << "Genesis nTime: " << genesis.nTime << std::endl;
        std::cout << "Genesis nVersion: " << genesis.nVersion << std::endl;
        
        // Test proof of work validation
        bool pow_valid = CheckProofOfWork(genesis.GetPoWHash(), genesis.nBits, params.GetConsensus());
        std::cout << "PoW Valid: " << (pow_valid ? "YES" : "NO") << std::endl;
        
        // Show target calculation
        arith_uint256 target;
        bool fNegative, fOverflow;
        target.SetCompact(genesis.nBits, &fNegative, &fOverflow);
        std::cout << "Target: " << target.ToString() << std::endl;
        std::cout << "PoWHash as arith: " << UintToArith256(genesis.GetPoWHash()).ToString() << std::endl;
        std::cout << "PoWHash <= Target: " << (UintToArith256(genesis.GetPoWHash()) <= target) << std::endl;
        
        // Check consensus params
        std::cout << "powLimit: " << params.GetConsensus().powLimit.ToString() << std::endl;
        std::cout << "fPowNoRetargeting: " << params.GetConsensus().fPowNoRetargeting << std::endl;
        std::cout << "fPowAllowMinDifficultyBlocks: " << params.GetConsensus().fPowAllowMinDifficultyBlocks << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 