//
// Created by aashik on 29/12/24.
//
#include <Blockchain/getblockchaininfo.h>

GetBlockchainInfo::GetBlockchainInfo() : BitCoinCoreFunction() {}

void GetBlockchainInfo::execute(const Callback& callback) {
    const std::string jsonData = R"({"jsonrpc": "1.0", "id": "curltest", "method": "getblockchaininfo", "params": )" + params + R"( })";
    request(callback, jsonData);
}
