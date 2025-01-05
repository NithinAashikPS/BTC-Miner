//
// Created by aashik on 05/01/25.
//

#pragma once

#include <BitCoinCore/bitcoincorefunction.h>

class GetBlockTemplate : public BitCoinCoreFunction {
public:
    GetBlockTemplate();
    void execute(const Callback&) override;
};
