//
// Created by aashik on 05/01/25.
//

#include <RawTransactions/decoderawtransaction.h>

DecodeRawTransaction::DecodeRawTransaction() : BitCoinCoreFunction() {}

void DecodeRawTransaction::execute(const Callback& callback) {
    const std::string jsonData = R"({"jsonrpc": "1.0", "id": "curltest", "method": "decoderawtransaction", "params": )" + params + R"( })";
    request(callback, jsonData);
}
