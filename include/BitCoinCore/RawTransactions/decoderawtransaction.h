//
// Created by aashik on 05/01/25.
//

#pragma once

#include <BitCoinCore/bitcoincorefunction.h>

class DecodeRawTransaction : public BitCoinCoreFunction {
public:
    DecodeRawTransaction();
    void execute(const Callback&) override;
};