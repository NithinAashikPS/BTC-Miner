//
// Created by aashik on 01/01/25.
//

#pragma once
#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <future>
#include <CL/opencl.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

typedef unsigned char BYTE;

class BTCMiner {
    struct MiningDevice {
        cl::Platform platform;
        cl::Device device;
        cl::Context context;
        cl::CommandQueue queue;
        cl::Program program;
        cl::Kernel kernel;
    };

    typedef struct {
        BYTE VERSION[04]{};
        BYTE PREV_HASH[32]{};
        BYTE MERKLE_ROOT[32]{};
        BYTE TIMESTAMP[04]{};
        BYTE BITS_DIFF[04]{};
        BYTE DIFF_TARGET[32]{};
        unsigned int x = 0;
    } BLOCK_TEMPLATE;

    typedef struct {
        unsigned int start;
        unsigned int end;
    } NONCE_RANGE;

    typedef struct {
        std::string data;
        std::string hash;
    } Transaction;

public:
    static void Start();

    static void Stop();

private:
    static void Run(MiningDevice, const std::vector<NONCE_RANGE> &);

    static void hexStringToBytes(const std::string &, BYTE *);

    static std::string hash256(const std::string &hex_str);

    static void reverse(BYTE *, const int &);

    static void toLittleEndian(const long int &, BYTE *);

    static void getDifficultyHex(const long int &, BYTE *);

    static void updateMerkleRoot();

    static bool getBlockchainInfo();

    static bool getBlockTemplate();

    static std::string createRawTransaction();

    static std::atomic<bool> stop;
    static std::vector<Transaction> transactions;
    static json coreTransaction;

    static unsigned int currentHeight;
    static unsigned int height;
    static unsigned int fees;
    static long int version;
    static std::string prevHash;
    static std::string merkleRoot;
    static long int time;
    static std::string bits;
    static std::string target;
};
