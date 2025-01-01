//
// Created by aashik on 29/12/24.
//

#include <iostream>
#include <Blockchain/getbestblockhash.h>

GetBestBlockHash::GetBestBlockHash() : BitCoinCoreFunction() {}

void GetBestBlockHash::execute(const Callback& callback) {
    const std::string jsonData = R"({"jsonrpc": "1.0", "id": "curltest", "method": "getbestblockhash", "params": )" + params + R"( })";
    request(callback, jsonData);
}
