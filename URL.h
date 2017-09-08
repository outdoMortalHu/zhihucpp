//
// Created by user on 17-8-28.
//

#ifndef ZHIHUCPP_URL_H
#define ZHIHUCPP_URL_H

#include <iostream>
#include <cstring>

using namespace std;

class URL {
public:
    URL();

    URL(const string &s_url);

    friend ostream &operator<<(ostream &os, const URL &url);

    void setUrl(const string &s_url);

    const string &getUrl() const;

    const string &getHost() const;

    const string &getPath() const;

    const string &getIp() const;

private:
    string url;
    string host;
    string path;
    string ip;
};


#endif //ZHIHUCPP_URL_H
