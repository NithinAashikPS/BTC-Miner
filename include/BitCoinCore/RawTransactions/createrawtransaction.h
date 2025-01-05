//
// Created by aashik on 05/01/25.
//

#pragma once

#include <BitCoinCore/bitcoincorefunction.h>

class CreaterawTransaction : public BitCoinCoreFunction {
public:
    CreaterawTransaction();
    void execute(const Callback&) override;
};
