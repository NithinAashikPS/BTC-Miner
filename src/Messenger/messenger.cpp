//
// Created by aashik on 29/12/24.
//

#include <iostream>
#include <Messenger/messenger.h>

::Messenger::Messenger::Messenger(const std::string& memory_name, const std::string& key): ptr(nullptr), sem(nullptr),
    shm_fd(0) {

    const std::string shm_name = "/" + memory_name;
    const std::string sem_name = "/" + key;

    SHM_NAME = new char[shm_name.size() + 1];
    std::strcpy(SHM_NAME, shm_name.c_str());
    SEM_NAME = new char[sem_name.size() + 1];
    std::strcpy(SEM_NAME, sem_name.c_str());
}
