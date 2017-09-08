//
// Created by user on 17-8-28.
//

#include "DNS.h"
#include <netdb.h>
#include <arpa/inet.h>
#include "Log.h"

string DNS::resolve(string host) {
    map<string, string>::iterator iter;
    iter = dns_cache.find(host);
    if (iter != dns_cache.end()) {
        return iter->second;
    } else {
        struct hostent *hostentPtr;
        hostentPtr = gethostbyname(host.c_str());
        if (hostentPtr == NULL) {
            log_error("can not resolve Host: %s" ,host.c_str());
        } else {
            switch (hostentPtr->h_addrtype) {
                case AF_INET:
                    char str[INET_ADDRSTRLEN];
                    inet_ntop(hostentPtr->h_addrtype, hostentPtr->h_addr, str, sizeof(str));
                    dns_cache.insert(map<string, string>::value_type(host, str));
                    return str;
                default:
                    log_error("Not AF_INET Host: %s",host.c_str());
            }
        }
        return "";
    }
}

map<string,string> DNS::dns_cache;