//
// Created by aashik on 29/12/24.
//

#pragma once

#include <BitCoinCore/bitcoincorefunction.h>

class GetBestBlockHash final : public BitCoinCoreFunction {

    public:
        GetBestBlockHash();
        void execute(const Callback&) override;
};
