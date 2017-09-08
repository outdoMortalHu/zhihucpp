//
// Created by user on 17-9-1.
//

#ifndef ZHIHUCPP_CRAWLER_H
#define ZHIHUCPP_CRAWLER_H

#include <iostream>
#include <cstdlib>
#include <zconf.h>
#include <pthread.h>
#include <fcntl.h>
#include <queue>
#include <map>
#include <iconv.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>
#include "URL.h"
#include "Log.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#include "MongodbHelper.h"

#include <poll.h>

#include "UserInfo.h"

using namespace std;

#define MAXEVENTS    1024
#define MAXCONNECTS  2
#define MAXREQUEST   3072
#define EPOLLTIMEOUT 1000
#define OPTIONS      1
#define SENDTIMEOUT  1000000
#define READTIMEOUT  1000
#define MAXQUEUESIZE 100000


#define log(...)             \
    do {                     \
        printf(__VA_ARGS__); \
        fflush(stdout);      \
    } while (0)

#define check0(x, ...)        \
    if (x)                    \
        do {                  \
            log(__VA_ARGS__); \
            exit(1);          \
    } while (0)

#define check1(x, ...)        \
    if (!(x))                 \
        do {                  \
            log(__VA_ARGS__); \
            exit(1);          \
    } while (0)

class Crawler {
public:
    Crawler();

    void start();

    void fetch();

private:
    URL url;
    int epoll_fd;
    epoll_event *events;
    int current_connect;
    struct Connection {
        int socket;
        SSL *sslHandle;
        SSL_CTX *sslContext;
    };
    struct CrawlerData {
        char path_url[64];
        bool is_list;
        int type;
        int offset;
        int total;
        Connection connection;
    };
    MongodbHelper mongodbHelper;
private:
    void init();

    void init_epoll();

    int tcpConnect();

    int make_socket_non_blocking(int socket_fd);

    Connection *sslConnect(int socket_fd);

    void sslDisconnect(Connection *c);

    string sslRead(Connection *c);

    int sslWrite(Connection *c, char *text);

    char *http_get_followers_head(const string &mix_path_url, string &name, string &offset, string &limit, bool &is_list);

    char *http_get_followers_first_head(const string &name);

    bool check_http_get_followers_first_head(const string &str);

    char *http_get_followers_list_head(const string &name, const string &offset, const string &limit);

    char *http_get_first(const string &name);

    bool check_http_get_followers_list_head(const string &str, string &name, string &offset, string &limit);

};


#endif //ZHIHUCPP_CRAWLER_H
