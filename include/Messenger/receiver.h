//
// Created by aashik on 29/12/24.
//

#pragma once

#include <Messenger/messenger.h>

namespace Messenger {
    class Receiver final : public Messenger {

        public:
            Receiver(const std::string&, const std::string&);
            ~Receiver() override;

            void getMessage(const MessageListener &) override;
    };
}
