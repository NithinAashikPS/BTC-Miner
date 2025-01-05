//
// Created by aashik on 29/12/24.
//

#pragma once

#include <curl/curl.h>

#include <functional>
#include <string>

using Callback = std::function<void(const std::string &)>;

class BitCoinCoreFunction {
public:
    BitCoinCoreFunction();

    virtual ~BitCoinCoreFunction();

    virtual void execute(const Callback &) = 0;

    void setParams(const std::string &);

protected:
    void request(const Callback &, const std::string &);

    std::string params;

private:
    void init();
    std::string url, username, password;
    std::string response;

    CURL *curl;
    curl_slist *headers = nullptr;
};
