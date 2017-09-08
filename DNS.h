//
// Created by user on 17-8-28.
//

#ifndef ZHIHUCPP_DNS_H
#define ZHIHUCPP_DNS_H

#include <iostream>
#include <map>

using namespace std;

class DNS {
public:
    static string resolve(string host);
private:
    static map<string, string> dns_cache;
};


#endif //ZHIHUCPP_DNS_H
