//
// Created by aashik on 05/01/25.
//

#include <Mining/getblocktemplate.h>

GetBlockTemplate::GetBlockTemplate() : BitCoinCoreFunction() {}

void GetBlockTemplate::execute(const Callback& callback) {
    const std::string jsonData = R"({"jsonrpc": "1.0", "id": "curltest", "method": "getblocktemplate", "params": )" + params + R"( })";
    request(callback, jsonData);
}
