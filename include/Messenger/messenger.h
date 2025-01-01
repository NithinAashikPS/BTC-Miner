//
// Created by aashik on 29/12/24.
//

#pragma once
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <cstring>
#include <functional>
#include <semaphore.h>

using MessageListener = std::function<void(const std::string&)>;
namespace Messenger {
    class Messenger {

        public:
            Messenger(const std::string&, const std::string&);
            virtual ~Messenger() = default;

            // virtual void closeMessenger() = 0;

            virtual void putMessage(const std::string&) {}
            virtual void getMessage(const MessageListener&) {}

        protected:
            char *SHM_NAME;
            char *SEM_NAME;
            const int SHM_SIZE = 1024 * 1024;

            void* ptr;
            sem_t* sem;
            int shm_fd;
    };
}

