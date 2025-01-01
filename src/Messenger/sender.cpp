//
// Created by aashik on 29/12/24.
//

#include <Messenger/sender.h>

using namespace Messenger;
Sender::Sender(const std::string& memory_name, const std::string& key) : Messenger(memory_name, key){

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
        throw std::runtime_error("shm_open failed.");

    if (ftruncate(shm_fd, SHM_SIZE) == -1)
        throw std::runtime_error("ftruncate failed.");

    ptr = mmap(nullptr, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
        throw std::runtime_error("mmap failed.");

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED)
        throw std::runtime_error("sem_open failed.");
}

void Sender::putMessage(const std::string &message) {
    std::strncpy(static_cast<char*>(ptr), message.c_str(), SHM_SIZE);
    sem_post(sem);
}

Sender::~Sender() {
    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    sem_close(sem);
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);
}
