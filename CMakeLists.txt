cmake_minimum_required(VERSION 3.25)
project(BTC-Mining VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DAPPLICATION_NAME="${PROJECT_NAME}")

set(CMAKE_PREFIX_PATH /home/aashik/Qt/6.8.1/Src/install/lib/cmake /opt/rocm-6.2.3/lib/cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(PkgConfig REQUIRED)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 Network Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 Network Concurrent)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rocm_smi REQUIRED)
find_package(NVML REQUIRED)
find_package(OpenCL REQUIRED)
find_package(OpenSSL REQUIRED)

pkg_check_modules(gmpxx REQUIRED gmpxx)

set(
        BLOCKCHAIN
        src/BitCoinCore/Blockchain/getbestblockhash.cpp
        src/BitCoinCore/Blockchain/getblock.cpp
        src/BitCoinCore/Blockchain/getblockchaininfo.cpp
        include/BitCoinCore/Blockchain/getbestblockhash.h
        include/BitCoinCore/Blockchain/getblock.h
        include/BitCoinCore/Blockchain/getblockchaininfo.h
)

set(
        CONTROL
        src/BitCoinCore/Control/help.cpp
        include/BitCoinCore/Control/help.h
)

set(
        MINING
        src/BitCoinCore/Mining/getblocktemplate.cpp
        include/BitCoinCore/Mining/getblocktemplate.h
)

set(
        RAW_TRANSACTIONS
        src/BitCoinCore/RawTransactions/createrawtransaction.cpp
        src/BitCoinCore/RawTransactions/decoderawtransaction.cpp
        include/BitCoinCore/RawTransactions/createrawtransaction.h
        include/BitCoinCore/RawTransactions/decoderawtransaction.h
)

set(
        MESSENGER
        src/Messenger/sender.cpp
        src/Messenger/receiver.cpp
        include/Messenger/sender.h
        include/Messenger/receiver.h
        src/Messenger/messenger.cpp
        include/Messenger/messenger.h
)

set(
        SYSTEM
        src/System/systemstatlistener.cpp
        include/System/systemstatlistener.h
)

set(
        UI
        src/UI/messagereceivermodel.cpp
        src/UI/controller.cpp
        src/UI/systemstats.cpp
        src/UI/bitcoincore_ui.cpp
        src/UI/messagereceiver.cpp
        include/UI/messagereceivermodel.h
        include/UI/controller.h
        include/UI/systemstats.h
        include/UI/bitcoincore_ui.h
        include/UI/messagereceiver.h
)

set(
        BITCOIN_CORE
        ${BLOCKCHAIN}
        ${CONTROL}
        ${MINING}
        ${RAW_TRANSACTIONS}
        src/BitCoinCore/bitcoincorefunction.cpp
        src/BitCoinCore/bitcoincore.cpp
        include/BitCoinCore/bitcoincorefunction.h
        include/BitCoinCore/bitcoincore.h
)

set(
        MINER
        src/Miner/btcminer.cpp
        include/Miner/btcminer.h
        include/Miner/kernal.h
)

set(PROJECT_SOURCES
    ${BITCOIN_CORE}
    ${MESSENGER}
    ${MINER}
    ${SYSTEM}
    ${UI}
    src/main.cpp
    ui/ui.qrc
    font/font.qrc
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

include_directories(${gmpxx_INCLUDE_DIRS})
link_directories(${gmpxx_LIBRARY_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Quick Qt6::QuickControls2 Qt6::Network Qt6::Concurrent)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} PRIVATE rocm_smi64)
target_link_libraries(${PROJECT_NAME} PRIVATE ${NVML_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${gmpxx_LIBRARIES} gmpxx)

target_include_directories(${PROJECT_NAME} PRIVATE ${NVML_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE include/BitCoinCore)

get_target_property(QT_TARGET_TYPE Qt6::Core TYPE)
if(${QT_TARGET_TYPE} STREQUAL "STATIC_LIBRARY")
    find_package(Qt6QmlImportScanner REQUIRED)
    qt6_import_qml_plugins(${CMAKE_PROJECT_NAME})
endif()
