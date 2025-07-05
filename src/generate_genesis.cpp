#include <iostream>
#include <iomanip>
#include <chainparams.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <consensus/merkle.h>
#include <util/strencodings.h>
#include <arith_uint256.h>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Rare, Irresistible, Irreversible";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

void MineGenesis(CBlock& genesis, uint32_t nBits)
{
    arith_uint256 bnTarget;
    bnTarget.SetCompact(nBits);
    
    std::cout << "Mining genesis block..." << std::endl;
    std::cout << "Target: " << bnTarget.ToString() << std::endl;
    
    for (genesis.nNonce = 0; ; genesis.nNonce++) {
        if (genesis.nNonce % 1000000 == 0) {
            std::cout << "Nonce: " << genesis.nNonce << std::endl;
        }
        
        uint256 hash = genesis.GetHash();
        if (UintToArith256(hash) <= bnTarget) {
            std::cout << "Found genesis block!" << std::endl;
            std::cout << "Hash: " << hash.ToString() << std::endl;
            std::cout << "Nonce: " << genesis.nNonce << std::endl;
            std::cout << "Merkle: " << genesis.hashMerkleRoot.ToString() << std::endl;
            break;
        }
    }
}

int main()
{
    // Mainnet genesis
    std::cout << "=== Generating Pussycoin Mainnet Genesis ===" << std::endl;
    CBlock genesis_main = CreateGenesisBlock(1735689600, 0, 0x1e0ffff0, 1, 50 * COIN);
    MineGenesis(genesis_main, 0x1e0ffff0);
    
    std::cout << std::endl;
    
    // Testnet genesis  
    std::cout << "=== Generating Pussycoin Testnet Genesis ===" << std::endl;
    CBlock genesis_test = CreateGenesisBlock(1735689601, 0, 0x1e0ffff0, 1, 50 * COIN);
    MineGenesis(genesis_test, 0x1e0ffff0);
    
    std::cout << std::endl;
    
    // Regtest genesis
    std::cout << "=== Generating Pussycoin Regtest Genesis ===" << std::endl;
    CBlock genesis_reg = CreateGenesisBlock(1735689602, 0, 0x1e0ffff0, 1, 50 * COIN);
    MineGenesis(genesis_reg, 0x1e0ffff0);
    
    return 0;
} 