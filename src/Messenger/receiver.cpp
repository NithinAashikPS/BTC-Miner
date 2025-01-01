//
// Created by aashik on 29/12/24.
//

#include <thread>
#include <Messenger/receiver.h>

using namespace Messenger;
Receiver::Receiver(const std::string &memory_name, const std::string &key) : Messenger(memory_name, key) {

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDONLY, 0666);
    if (shm_fd == -1)
        throw std::runtime_error("shm_open failed.");

    ptr = mmap(nullptr, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
        throw std::runtime_error("mmap failed.");

    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED)
        throw std::runtime_error("sem_open failed.");
}

Receiver::~Receiver() {
    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    sem_close(sem);
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);
}

void Receiver::getMessage(const MessageListener &callback) {

    std::thread receiverThread([&](const MessageListener &callback) {
        while (true) {
            sem_wait(sem);
            char buffer[SHM_SIZE];
            std::strncpy(buffer, static_cast<char*>(ptr), SHM_SIZE);
            // if (std::string(buffer) == "exit") {
            //     break;
            // }
            callback(std::string(buffer));
        }
    }, callback);
    receiverThread.detach();
}
