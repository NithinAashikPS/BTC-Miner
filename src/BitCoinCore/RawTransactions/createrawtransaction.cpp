//
// Created by aashik on 05/01/25.
//

#include <RawTransactions/createrawtransaction.h>

CreaterawTransaction::CreaterawTransaction() : BitCoinCoreFunction() {}

void CreaterawTransaction::execute(const Callback& callback) {
    const std::string jsonData = R"({"jsonrpc": "1.0", "id": "curltest", "method": "createrawtransaction", "params": )" + params + R"( })";
    request(callback, jsonData);
}