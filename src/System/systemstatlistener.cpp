//
// Created by aashik on 31/12/24.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <nvml.h>
#include <rocm_smi/rocm_smi.h>

#include <System/systemstatlistener.h>

bool SystemStatListener::stop = false;
std::future<void> SystemStatListener::memoryUsage;
std::future<void> SystemStatListener::AMDGPUUsage;
std::future<void> SystemStatListener::NVIDIAGPUUsage;
std::future<void> SystemStatListener::cpuUsage;
std::future<void> SystemStatListener::dateAndTime;

void SystemStatListener::Init() {
    stop = false;
    dateAndTime = std::async(std::launch::async, listenDateAndTime);
    AMDGPUUsage = std::async(std::launch::async, listenAMDGPUUsage);
    NVIDIAGPUUsage = std::async(std::launch::async, listenNVIDIAGPUUsage);
    memoryUsage = std::async(std::launch::async, listenMemoryUsage);
    cpuUsage = std::async(std::launch::async, listenCPUUsage);
}

void SystemStatListener::Stop() {
    stop = true;
    memoryUsage.get();
    NVIDIAGPUUsage.get();
    AMDGPUUsage.get();
    cpuUsage.get();
    dateAndTime.get();
}

void SystemStatListener::listenDateAndTime() {
    auto messenger = std::make_unique<Messenger::Sender>("date_time", "SYSTEM_STATS");
    while (!stop) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_time_t);
        std::ostringstream date_oss; date_oss << std::put_time(&now_tm, "%b %d, %Y");
        std::ostringstream time_oss; time_oss << std::put_time(&now_tm, "%H:%M:%S");
        json date_and_time;
        date_and_time["date"] = date_oss.str();
        date_and_time["time"] = time_oss.str();
        messenger->putMessage(date_and_time.dump());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SystemStatListener::listenMemoryUsage() {
    auto messenger = std::make_unique<Messenger::Sender>("other", "SYSTEM_STATS");
    while (!stop) {
        if (std::ifstream memFile("/proc/meminfo"); memFile.is_open()) {
            std::string key;
            unsigned long value;
            std::string unit;
            unsigned long availableMemory = 0;
            unsigned long totalMemory = 0;
            while (memFile >> key >> value >> unit) {
                if (key == "MemTotal:") {
                    totalMemory = value;
                } else if (key == "MemAvailable:") {
                    availableMemory = value;
                    break;
                }
            }
            memFile.close();

            if (totalMemory > 0) {
                json usage_object;
                usage_object["name"] = "Memory";
                usage_object["stat"] = (static_cast<double>(totalMemory - availableMemory) / static_cast<double>(
                                            totalMemory)) * 100.0;
                messenger->putMessage(usage_object.dump());
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SystemStatListener::listenNVIDIAGPUUsage() {
    nvmlDevice_t device;
    nvmlReturn_t result = nvmlInit();
    if (result != NVML_SUCCESS) return;

    result = nvmlDeviceGetHandleByIndex(0, &device);
    if (result != NVML_SUCCESS) {
        nvmlShutdown();
        return ;
    }

    nvmlUtilization_t utilization;
    const auto messenger = std::make_unique<Messenger::Sender>("other", "SYSTEM_STATS");
    while (!stop) {
        if (nvmlDeviceGetUtilizationRates(device, &utilization) == NVML_SUCCESS) {
            json gpu;
            gpu["name"] = "NVIDIA GPU"; gpu["stat"] = utilization.gpu;
            messenger->putMessage(gpu.dump());
        }
        if (nvmlDeviceGetUtilizationRates(device, &utilization) == NVML_SUCCESS) {
            json memory;
            memory["name"] = "NVIDIA Mem"; memory["stat"] = utilization.memory;
            messenger->putMessage(memory.dump());
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } nvmlShutdown();
}

void SystemStatListener::listenAMDGPUUsage() {
    rsmi_status_t status = rsmi_init(0);
    if (status != RSMI_STATUS_SUCCESS) {
        return;
    }

    uint32_t deviceCount = 0, utilization = 0;
    rsmi_num_monitor_devices(&deviceCount);

    const auto messenger = std::make_unique<Messenger::Sender>("other", "SYSTEM_STATS");
    while (deviceCount > 0 && !stop) {
        for (uint32_t i = 0; i < deviceCount; ++i) {
            status = rsmi_dev_busy_percent_get(i, &utilization);
            if (status == RSMI_STATUS_SUCCESS) {
                json usage_object;
                usage_object["name"] = "AMD GPU";
                usage_object["stat"] = utilization;
                messenger->putMessage(usage_object.dump());
            }
        } std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    rsmi_shut_down();
}

void SystemStatListener::parseCPUStats(const std::string &line, CPUStats &stats) {
    std::istringstream iss(line);
    std::string cpu;
    iss >> cpu >> stats.user >> stats.nice >> stats.system >> stats.idle;
}

void SystemStatListener::listenCPUUsage() {
    std::vector<CPUStats> prevStats;
    std::vector<CPUStats> currentStats;
    auto messenger = std::make_unique<Messenger::Sender>("cpu", "SYSTEM_STATS");
    while (!stop) {
        std::ifstream statFile("/proc/stat");
        std::vector<std::string> lines;

        if (statFile.is_open()) {
            std::string line;
            while (std::getline(statFile, line))
                if (line.find("cpu") == 0)
                    lines.push_back(line);
            statFile.close();
        }

        if (prevStats.empty())
            prevStats.push_back({});

        currentStats.clear();
        for (const auto &l: lines) {
            CPUStats stats{};
            parseCPUStats(l, stats);
            currentStats.push_back(stats);
        }

        for (size_t i = 0; i < currentStats.size(); ++i) {
            if (i == 0) continue;
            const auto &[pre_user, pre_nice, pre_system, pre_idle] = prevStats[i];
            const auto &[cur_user, cur_nice, cur_system, cur_idle] = currentStats[i];

            unsigned long prevTotal = pre_user + pre_nice + pre_system + pre_idle;
            unsigned long currTotal = cur_user + cur_nice + cur_system + cur_idle;

            double usage = 100.0 * (1.0 - (static_cast<double>(cur_idle - pre_idle) / static_cast<double>(
                                               currTotal - prevTotal)));
            json usage_object;
            usage_object["name"] = "CPU" + std::to_string(i);
            usage_object["stat"] = usage;
            messenger->putMessage(usage_object.dump());
        }
        prevStats = currentStats;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
