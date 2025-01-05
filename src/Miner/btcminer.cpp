//
// Created by aashik on 01/01/25.
//

#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <gmp.h>
#include <Miner/btcminer.h>
#include <Miner/kernal.h>
#include <BitCoinCore/bitcoincore.h>

#include <openssl/sha.h>

std::atomic<bool> BTCMiner::stop = false;
std::vector<BTCMiner::Transaction> BTCMiner::transactions;
unsigned int BTCMiner::currentHeight = 0;
unsigned int BTCMiner::fees = 0;

json BTCMiner::coreTransaction = json::array({
    {{"txid", "0000000000000000000000000000000000000000000000000000000000000000"}, {"vout", 0}}
});

unsigned int BTCMiner::height;
long int BTCMiner::version;
long int BTCMiner::time;

std::string BTCMiner::bits;
std::string BTCMiner::target;
std::string BTCMiner::prevHash;
std::string BTCMiner::merkleRoot;

void BTCMiner::Start() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) return;

    std::vector<MiningDevice> mining_devices;
    std::vector<NONCE_RANGE> nonces;

    // std::ifstream file("/home/aashik/Downloads/test_block.json", std::ios::in | std::ios::binary);
    // if (!file) {
    //     throw std::runtime_error("Could not open file: ");
    // }

    // std::ostringstream content;
    // content << file.rdbuf();
    // json data = json::parse(content.str());

    // merkleRoot = data["mrkl_root"].get<std::string>();

    // for (const auto &transaction: data["tx"]) {
    //     transactions.push_back(Transaction{
    //         "",
    //         transaction["hash"].get<std::string>()
    //     });
    // }

    for (const cl::Platform &platform: platforms) {
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if (devices.empty()) continue;
        for (const cl::Device &device: devices) {
            std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
            cl::Context context(device);
            const cl::CommandQueue queue(context, device);
            cl::Program program(context, kernel_code);
            if (program.build({device}) != CL_SUCCESS) {
                // Retrieve and print the build log
                std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
                std::cout << "Build failed for device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
                std::cout << "Error Log: " << buildLog << std::endl;
                continue;
            }
            const cl::Kernel kernel(program, "StartMining");

            MiningDevice mining_device{};
            mining_device.platform = platform;
            mining_device.device = device;
            mining_device.context = context;
            mining_device.queue = queue;
            mining_device.kernel = kernel;
            mining_devices.push_back(mining_device);
        }
    }

    unsigned int max_nonce = (1U << 32) - 1;
    unsigned int core_count = 1024;
    unsigned int nonce_range_size = max_nonce / core_count;

    for (unsigned int i = 0; i < core_count; ++i) {
        NONCE_RANGE nonce_range;
        nonce_range.start = i * nonce_range_size;
        nonce_range.end = nonce_range.start + nonce_range_size;
        nonces.emplace_back(nonce_range);
    }

    // for (MiningDevice &mining_device: mining_devices) {
    std::thread miner([&](const MiningDevice &dev, const std::vector<NONCE_RANGE> &ncs) {
        Run(dev, ncs);
    }, mining_devices.at(1), nonces);
    miner.detach();
    // }
}

void BTCMiner::Stop() {
    stop = true;
    // for (MiningDevice& mining_device: mining_devices) {
    //     minerFutures.emplace(mining_device, std::async(std::launch::async, Run, mining_device));
    // }
    // minerFuture.get();
}

void BTCMiner::Run(MiningDevice device, const std::vector<NONCE_RANGE> &nonces) {
    while (!stop) {
        if (getBlockchainInfo()) {
            unsigned int nonceIdx = 0;
            int breakFlag = 0;
            if (currentHeight == height || breakFlag == 0) {
                fees = 0;
                transactions.clear();
                merkleRoot.clear();
                if (getBlockTemplate()) {
                    if (std::string coinbaseTransaction = createRawTransaction(); !coinbaseTransaction.empty()) {
                        BitCoinCore::call("decoderawtransaction", [&](const std::string &result) {
                            try {
                                if (const json result_json = json::parse(result); !result_json["result"].
                                    is_null()) {
                                    transactions.insert(transactions.begin(), {
                                                            coinbaseTransaction,
                                                            result_json["result"]["hash"].get<std::string>()
                                                        });
                                }
                            } catch (const nlohmann::json::parse_error &e) {
                                std::cerr << "1 Parse error: " << e.what() << std::endl;
                            }
                        }, "[" + coinbaseTransaction + "]");
                        BLOCK_TEMPLATE block_template;
                        updateMerkleRoot();
                        hexStringToBytes(prevHash, block_template.PREV_HASH);
                        hexStringToBytes(merkleRoot, block_template.MERKLE_ROOT);
                        hexStringToBytes(target, block_template.DIFF_TARGET);
                        hexStringToBytes(bits, block_template.BITS_DIFF);
                        reverse(block_template.PREV_HASH, 32);
                        reverse(block_template.MERKLE_ROOT, 32);
                        reverse(block_template.DIFF_TARGET, 32);
                        reverse(block_template.BITS_DIFF, 4);

                        toLittleEndian(version, block_template.VERSION);
                        toLittleEndian(time, block_template.TIMESTAMP);

                        // printf("Hash : ");
                        // for (int i = 0; i < 32; i++) {
                        //     printf("%02X", block_template.PREV_HASH[i]);
                        // }
                        // printf("\n");
                        // std::cout << bits << " " << block_template.PREV_HASH << std::endl;
                        std::cout << device.device.getInfo<CL_DEVICE_NAME>() << " Mining : "
                        << height << ", Tx Count : " << transactions.size() << ", Merkle Root : " << merkleRoot << std::endl;

                        const cl::Buffer blockTemplateBuffer(device.context, CL_MEM_READ_WRITE,
                                                             sizeof(BLOCK_TEMPLATE));
                        const cl::Buffer nonceRangeBuffer(device.context, CL_MEM_WRITE_ONLY,
                                                          sizeof(NONCE_RANGE) * nonces.size());
                        const cl::Buffer nonceIdxBuffer(device.context, CL_MEM_READ_WRITE, sizeof(unsigned int));
                        const cl::Buffer breakFlagBuffer(device.context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                                         sizeof(int), &breakFlag);
                        device.kernel.setArg(0, blockTemplateBuffer);
                        device.kernel.setArg(1, nonceRangeBuffer);
                        device.kernel.setArg(2, nonceIdxBuffer);
                        device.kernel.setArg(3, breakFlagBuffer);

                        device.queue.enqueueWriteBuffer(blockTemplateBuffer, CL_TRUE, 0, sizeof(BLOCK_TEMPLATE),
                                                        &block_template);
                        device.queue.enqueueWriteBuffer(nonceRangeBuffer, CL_TRUE, 0,
                                                        sizeof(NONCE_RANGE) * nonces.size(), nonces.data());
                        device.queue.
                                enqueueWriteBuffer(nonceIdxBuffer, CL_TRUE, 0, sizeof(unsigned int), &nonceIdx);
                        cl::Event event;
                        const auto start = std::chrono::high_resolution_clock::now();
                        device.queue.enqueueNDRangeKernel(device.kernel, cl::NullRange, cl::NDRange(32, 32),
                                                          cl::NullRange, nullptr,
                                                          &event);
                        auto res = event.wait();
                        const auto end = std::chrono::high_resolution_clock::now();

                        device.queue.enqueueReadBuffer(blockTemplateBuffer, CL_TRUE, 0, sizeof(BLOCK_TEMPLATE),
                                                       &block_template);
                        device.queue.enqueueReadBuffer(nonceIdxBuffer, CL_TRUE, 0, sizeof(unsigned int), &nonceIdx);
                        device.queue.enqueueReadBuffer(breakFlagBuffer, CL_TRUE, 0, sizeof(int), &breakFlag);

                        const std::chrono::duration<float> duration = end - start;

                        // if (breakFlag) {
                            std::cout << device.device.getInfo<CL_DEVICE_NAME>() << " Kernel execution time: " <<
                                    duration.count() * 1000 <<
                                    " ms, Nonce: " << block_template.x << std::endl;
                            // std::cout << device.device.getInfo<CL_DEVICE_NAME>() << " " << breakFlag << std::endl;
                        // }
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void BTCMiner::updateMerkleRoot() {
    std::vector<Transaction> hashes;
    for (auto &[data, hash]: transactions) {
        std::string reversed_txid;
        for (size_t i = 0; i < hash.size(); i += 2) {
            reversed_txid = hash.substr(i, 2) + reversed_txid;
        }
        hashes.push_back(Transaction{
            data,
            reversed_txid
        });
    }
    while (hashes.size() > 1) {
        if (hashes.size() % 2 != 0) {
            hashes.push_back(hashes.back());
        }
        std::vector<Transaction> nextLevel;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            std::string concatenated = hashes[i].hash + hashes[i + 1].hash;
            nextLevel.push_back(Transaction{
                "",
                hash256(concatenated)
            });
        }
        hashes = std::move(nextLevel);
    }

    const std::string final_hash = hashes.at(0).hash;
    for (size_t i = 0; i < final_hash.size(); i += 2) {
        merkleRoot = final_hash.substr(i, 2) + merkleRoot;
    }
}

bool BTCMiner::getBlockchainInfo() {
    bool ret = false;
    BitCoinCore::call("getblockchaininfo", [&](const std::string &result) {
        try {
            if (const json result_json = json::parse(result); !result_json["result"].is_null()) {
                currentHeight = result_json["result"]["headers"].get<unsigned int>();
                ret = true;
            }
        } catch (const nlohmann::json::parse_error &e) {
            std::cerr << "getblockchaininfo Parse error: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ret = false;
        }
    });
    return ret;
}

bool BTCMiner::getBlockTemplate() {
    bool ret = false;
    BitCoinCore::call("getblocktemplate", [&](const std::string &result) {
        try {
            if (const json result_json = json::parse(result); !result_json["result"].is_null()) {
                height = result_json["result"]["height"].get<unsigned int>();
                version = result_json["result"]["version"].get<long int>();
                prevHash = result_json["result"]["previousblockhash"].get<std::string>();
                time = result_json["result"]["curtime"].get<long int>();
                bits = result_json["result"]["bits"].get<std::string>();
                target = result_json["result"]["target"].get<std::string>();
                for (const auto &transaction:
                     result_json["result"]["transactions"]) {
                    fees += transaction["fee"].get<unsigned int>();
                    transactions.emplace_back(Transaction{
                        transaction["data"].get<std::string>(),
                        transaction["hash"].get<std::string>()
                    });
                }
                ret = true;
            }
        } catch (const nlohmann::json::parse_error &e) {
            std::cerr << "getBlockTemplate Parse error: " << e.what() << std::endl;
            ret = false;
        }
    }, "[{\"rules\": [\"segwit\"]}]");

    return ret;
}

std::string BTCMiner::createRawTransaction() {
    std::string coinbaseTransaction;
    const json params = json::array({
        coreTransaction,
        json::array({
            {{"tb1qgs7vfsksnlzzs3plt92e284es6lteftjhx75nt", BitCoinCore::getBlockReward(height) + fees}}
        })
    });
    BitCoinCore::call("createrawtransaction", [&](const std::string &result) {
        const json result_json = json::parse(result)["result"];
        coinbaseTransaction = result_json.dump();
    }, params.dump());
    return coinbaseTransaction;
}

void BTCMiner::toLittleEndian(const long int &num, BYTE *hex) {
    for (int i = 0; i < sizeof(int); i++) {
        hex[i] = (num >> (i * 8)) & 0xFF;
    }
}

void BTCMiner::reverse(BYTE *arr, const int &size) {
    int start = 0;
    int end = size - 1;
    while (start < end) {
        const BYTE temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

void BTCMiner::hexStringToBytes(const std::string &hexString, BYTE *bytes) {
    const char *hexChars = hexString.c_str();
    const size_t str_len = strlen(hexChars);
    for (size_t i = 0; i < (str_len / 2); i++) {
        sscanf(hexChars + 2 * i, "%2hhx", &bytes[i]);
    }
}

std::string BTCMiner::hash256(const std::string &hex_str) {
    std::vector<unsigned char> binary;
    for (size_t i = 0; i < hex_str.length(); i += 2) {
        unsigned int byte;
        std::stringstream ss;
        ss << std::hex << hex_str.substr(i, 2);
        ss >> byte;
        binary.push_back(static_cast<unsigned char>(byte));
    }

    unsigned char hash1[SHA256_DIGEST_LENGTH];
    SHA256(binary.data(), binary.size(), hash1);

    unsigned char hash2[SHA256_DIGEST_LENGTH];
    SHA256(hash1, SHA256_DIGEST_LENGTH, hash2);

    std::stringstream result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        result << std::hex << std::setfill('0') << std::setw(2) << (int) hash2[i];
    }

    return result.str();
}

void BTCMiner::getDifficultyHex(const long int &bits, BYTE *target) {
    long int exp = 8 * ((bits >> 24) - 3);
    long int mant = bits & 0xffffff;

    mpz_t result, multiplied;
    mpz_inits(result, multiplied, NULL);
    mpz_ui_pow_ui(result, 2, exp);

    mpz_set_ui(multiplied, mant);
    mpz_mul(multiplied, multiplied, result);
    size_t count;
    const auto hex_bytes = static_cast<BYTE *>(mpz_export(nullptr, &count, 1, sizeof(BYTE), 1, 0, multiplied));

    for (size_t i = 0; i < (32 - count); ++i) {
        target[i] = 0x00;
    }
    for (size_t i = (32 - count); i < 32; ++i) {
        target[i] = hex_bytes[i - (32 - count)];
    }
    mpz_clears(result, multiplied, NULL);
    free(hex_bytes);
}
