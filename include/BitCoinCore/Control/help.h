//
// Created by aashik on 01/01/25.
//

#pragma once

#include <BitCoinCore/bitcoincorefunction.h>

class Help final : public BitCoinCoreFunction {

public:
    Help();
    void execute(const Callback&) override;
};
