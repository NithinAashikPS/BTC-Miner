//
// Created by aashik on 29/12/24.
//

#pragma once

#include <BitCoinCore/bitcoincorefunction.h>

class GetBlockchainInfo final : public BitCoinCoreFunction {

public:
    GetBlockchainInfo();
    void execute(const Callback&) override;
};
