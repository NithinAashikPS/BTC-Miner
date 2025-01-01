//
// Created by aashik on 31/12/24.
//

#pragma once

#include <memory>
#include <future>
#include <Messenger/sender.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct CPUStats {
    unsigned long user, nice, system, idle;
};

class SystemStatListener {

public:
    static void Init();
    static void Stop();

private:
    static void parseCPUStats(const std::string&, CPUStats&);
    static void listenCPUUsage();
    static void listenMemoryUsage();
    static void listenAMDGPUUsage();
    static void listenNVIDIAGPUUsage();
    static void listenDateAndTime();

    static std::future<void> memoryUsage, AMDGPUUsage, NVIDIAGPUUsage, cpuUsage, dateAndTime;
    static bool stop;
};
