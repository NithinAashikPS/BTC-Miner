//
// Created by aashik on 29/12/24.
//

#pragma once
#include <memory>
#include <future>
#include <unordered_map>

#include <BitCoinCore/bitcoincorefunction.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class BitCoinCore {

    public:
        static void get(const std::string&, const Callback&, const std::string& params = "[]");
        static void Restart();
        static void Start();
        static void Stop();

        static bool serverStarted;

    private:
        static void registerFunction(const std::string&, std::unique_ptr<BitCoinCoreFunction>);
        static bool hasInitMessage(const std::string&, std::string&);
        static void StartServer();
        static void StopServer();

        static bool stop;
        static std::future<void> serverFuture;
        static std::unordered_map<std::string, std::unique_ptr<BitCoinCoreFunction>> bitcoinCoreFunctionMap;
};
