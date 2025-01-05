//
// Created by aashik on 29/12/24.
//

#include <iostream>
#include <thread>

#include <Messenger/sender.h>
#include <BitCoinCore/bitcoincore.h>

#include <Blockchain/getbestblockhash.h>
#include <Blockchain/getblock.h>
#include <Blockchain/getblockchaininfo.h>

#include <Control/help.h>

#include <RawTransactions/createrawtransaction.h>
#include <RawTransactions/decoderawtransaction.h>

#include <Mining/getblocktemplate.h>

std::unordered_map<std::string, std::unique_ptr<BitCoinCoreFunction>> BitCoinCore::bitcoinCoreFunctionMap;
std::future<void> BitCoinCore::serverFuture;
bool BitCoinCore::stop = false;
bool BitCoinCore::serverStarted = false;

void BitCoinCore::registerFunction(const std::string &function_name, std::unique_ptr<BitCoinCoreFunction> function) {
    bitcoinCoreFunctionMap[function_name] = std::move(function);
}

void BitCoinCore::call(const std::string &function_name, const Callback& callback, const std::string& params) {
    if (const auto it = bitcoinCoreFunctionMap.find(function_name); it != bitcoinCoreFunctionMap.end()) {
        it->second->setParams(params);
        it->second->execute(callback);
    }
}

void BitCoinCore::Start() {
    stop = false;
    // serverFuture = std::async(std::launch::async, StartServer);
    BitCoinCore::registerFunction("getbestblockhash", std::make_unique<GetBestBlockHash>());
    BitCoinCore::registerFunction("getblock", std::make_unique<GetBlock>());
    BitCoinCore::registerFunction("getblockchaininfo", std::make_unique<GetBlockchainInfo>());

    // Control
    BitCoinCore::registerFunction("help", std::make_unique<Help>());

    // RawTransactions
    BitCoinCore::registerFunction("createrawtransaction", std::make_unique<CreaterawTransaction>());
    BitCoinCore::registerFunction("decoderawtransaction", std::make_unique<DecodeRawTransaction>());

    // Mining
    BitCoinCore::registerFunction("getblocktemplate", std::make_unique<GetBlockTemplate>());
}

void BitCoinCore::Stop() {
    serverStarted = false;
    stop = true;
    StopServer();
    serverFuture.get();
}

void BitCoinCore::Restart() {
    Stop();
    Start();
}

bool BitCoinCore::hasInitMessage(const std::string& logLine, std::string& out_message) {
    const std::string initMessagePrefix = "init message:";

    size_t pos = logLine.find(initMessagePrefix);
    if (pos != std::string::npos) {
        std::string message = logLine.substr(pos + initMessagePrefix.length());
        size_t start = message.find_first_not_of(" ");
        if (start != std::string::npos) {
            out_message = message.substr(start);
        }
        return true;
    }
    return false;
}

void BitCoinCore::StartServer() {

    const auto messenger = std::make_unique<Messenger::Sender>("server_logs", "RPC_SERVER");
    const auto load_messenger = std::make_unique<Messenger::Sender>("status", "LOADING");
    try {
        // FILE* pipe = popen("ping google.com", "r");
        FILE* pipe = popen("bitcoin-core.daemon -testnet", "r");
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }

        char buffer[1024 * 1024];
        std::string message;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            message = std::string(buffer);
            message.pop_back();
            if (hasInitMessage(message, message)) {
                json loading;
                loading["message"] = message;
                load_messenger->putMessage(loading.dump());
                if (message.find("Done") != std::string::npos)
                    serverStarted = true;
            } else
                messenger->putMessage(message);
        }
        pclose(pipe);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void BitCoinCore::StopServer() {
    try {
        // FILE* pipe = popen("ping google.com", "r");
        FILE* pipe = popen("bitcoin-core.cli -testnet stop", "r");
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        char buffer[1024 * 1024];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::cout << buffer << std::endl;
        }
        pclose(pipe);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

long long BitCoinCore::getBlockReward(const unsigned int &height) {
    constexpr unsigned int halving_interval = 210000;
    constexpr long long initial_reward = 50 * 100000000LL;
    const unsigned int halvings = height / halving_interval;
    const long long subsidy_satoshis = initial_reward / (1LL << halvings);
    // double subsidy_btc = static_cast<double>(subsidy_satoshis) / 100000000.0;
    return subsidy_satoshis;
}
