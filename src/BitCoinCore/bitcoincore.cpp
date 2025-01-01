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

std::unordered_map<std::string, std::unique_ptr<BitCoinCoreFunction>> BitCoinCore::bitcoinCoreFunctionMap;
std::future<void> BitCoinCore::serverFuture;
bool BitCoinCore::stop = false;
bool BitCoinCore::serverStarted = false;

void BitCoinCore::registerFunction(const std::string &function_name, std::unique_ptr<BitCoinCoreFunction> function) {
    bitcoinCoreFunctionMap[function_name] = std::move(function);
}

void BitCoinCore::get(const std::string &function_name, const Callback& callback, const std::string& params) {
    if (const auto it = bitcoinCoreFunctionMap.find(function_name); it != bitcoinCoreFunctionMap.end()) {
        it->second->setParams(params);
        it->second->execute(callback);
    }
}

void BitCoinCore::Start() {
    stop = false;
    serverFuture = std::async(std::launch::async, StartServer);
    BitCoinCore::registerFunction("getbestblockhash", std::make_unique<GetBestBlockHash>());
    BitCoinCore::registerFunction("getblock", std::make_unique<GetBlock>());
    BitCoinCore::registerFunction("getblockchaininfo", std::make_unique<GetBlockchainInfo>());

    // Control
    BitCoinCore::registerFunction("help", std::make_unique<Help>());
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
