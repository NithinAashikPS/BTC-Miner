//
// Created by aashik on 01/01/25.
//
#include <Control/help.h>

Help::Help() : BitCoinCoreFunction() {}

void Help::execute(const Callback& callback) {
    const std::string jsonData = R"({"jsonrpc": "1.0", "id": "curltest", "method": "help", "params": )" + params + R"( })";
    request(callback, jsonData);
}