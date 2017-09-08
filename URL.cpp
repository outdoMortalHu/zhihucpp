//
// Created by user on 17-8-28.
//

#include "URL.h"
#include "DNS.h"

#define HTTPHEAD "http://"
#define HTTPSHEAD "https://"

URL::URL() {}

URL::URL(const string &s_url) {
    setUrl(s_url);
}

void URL::setUrl(const string &s_url) {
    url = s_url;
    string::size_type httpPos = url.find(HTTPHEAD);
    string::size_type httpsPos = url.find(HTTPSHEAD);
    if (httpPos != string::npos) {
        host = url.substr(strlen(HTTPHEAD));
    } else if (httpsPos != string::npos) {
        host = url.substr(strlen(HTTPSHEAD));
    } else {
        host = url;
    }
    string::size_type pos = host.find('/');
    if (pos != string::npos) {
        path = host.substr(pos);
        host = host.substr(0, pos);
    } else {
        path = "/";
    }
    ip = DNS().resolve(host);
}

const string &URL::getUrl() const {
    return url;
}

const string &URL::getHost() const {
    return host;
}

const string &URL::getIp() const {
    return ip;
}

const string &URL::getPath() const {
    return path;
}

ostream &operator<<(ostream &os, const URL &url) {
    os << "url: " << url.url << " host: " << url.host << " path: " << url.path << " ip: " << url.ip;
    return os;
}