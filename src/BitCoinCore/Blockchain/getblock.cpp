//
// Created by aashik on 29/12/24.
//
#include <Blockchain/getblock.h>

GetBlock::GetBlock() : BitCoinCoreFunction() {}

void GetBlock::execute(const Callback& callback) {
    const std::string jsonData = R"({"jsonrpc": "1.0", "id": "curltest", "method": "getblock", "params": )" + params + R"( })";
    request(callback, jsonData);
}
