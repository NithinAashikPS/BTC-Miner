//
// Created by aashik on 29/12/24.
//

#include <iostream>
#include <list>
#include <BitCoinCore/bitcoincorefunction.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

BitCoinCoreFunction::BitCoinCoreFunction() {

    curl_global_init(CURL_GLOBAL_DEFAULT);
    url = "http://127.0.0.1:8332/";
    username = "Aashik0808";
    password = "Aashik@0808";
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

        headers = curl_slist_append(headers, "Content-Type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    }

}

BitCoinCoreFunction::~BitCoinCoreFunction() {

    if (curl) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
};

void BitCoinCoreFunction::setParams(const std::string &params) {
    this->params = params;
}

void BitCoinCoreFunction::request(const Callback &callback, const std::string &jsonData) {

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
            response = R"({"result":null,"error":{"code":-28,"message":")";
            response += curl_easy_strerror(res);
            response += R"("}})";
            callback(response);
        } else {
            callback(response);
        }
        response = "";
    }
}

