//
// Created by aashik on 29/12/24.
//

#pragma once

#include <BitCoinCore/bitcoincorefunction.h>

class GetBlock final : public BitCoinCoreFunction {

public:
    GetBlock();
    void execute(const Callback&) override;
};
