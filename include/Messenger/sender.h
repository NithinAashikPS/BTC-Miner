//
// Created by aashik on 29/12/24.
//

#pragma once

#include <Messenger/messenger.h>

namespace Messenger {
    class Sender final : public Messenger {

    public:
        Sender(const std::string&, const std::string&);
        ~Sender() override;

        void putMessage(const std::string &) override;
    };
}
