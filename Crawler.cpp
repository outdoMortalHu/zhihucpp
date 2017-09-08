//
// Created by user on 17-9-1.
//

#include "Crawler.h"

#define PORT 443
const char *base_url = "www.zhihu.com";

/*
 * 权值大的靠前
 * */
struct cmp {
    bool operator()(string x, string y) {
        if (x.find("/")) {
            true;
        }
        return false;
    }
};

priority_queue<string, vector<string>, cmp> unvisited_path_queue;
map<string, bool> visited_name;

Crawler::Crawler() {
    current_connect = 0;
    init();
    init_epoll();
}

void Crawler::init() {
    /*
     * 初始化priority_queue和map
     * */
    visited_name.clear();
    while (!unvisited_path_queue.empty())
        unvisited_path_queue.pop();

    mongodbHelper = MongodbHelper();

    url.setUrl(base_url);

//    unvisited_path_queue.push("cxfer");
//    unvisited_path_queue.push("kenneth-pan");
//    unvisited_path_queue.push("wang-bin-1-53");
//    unvisited_path_queue.push("haohaojun");
//    unvisited_path_queue.push("kai-yuan-ge");
//    unvisited_path_queue.push("minmin.gong");
//    unvisited_path_queue.push("rednaxelafx");
    unvisited_path_queue.push("excited-vczh");
//    unvisited_path_queue.push("spto");
}

void Crawler::init_epoll() {
    int fd = epoll_create1(0);
    if (fd <= 0) {
        log_error("epoll_create");
    }
    events = (epoll_event *) malloc(sizeof(epoll_event) * MAXEVENTS);
    epoll_fd = fd;
}

static void *run(void *arg) {
    ((Crawler *) arg)->fetch();
}

void Crawler::start() {
    pthread_t ptid;
    int ret = 0;
    ret = pthread_create(&ptid, NULL, run, this);
    if (ret < 0) {
        log_error("pthread_create error");
        exit(0);
    }
    pthread_join(ptid, NULL);
}

void Crawler::fetch() {
    /*
     *  typedef long time_t;
     *  从1970年1月1日0时0分0秒到此时的秒数
     * */
    time_t start_time, end_time;
    time(&start_time);
    while (true) {
        if ((unvisited_path_queue.size() == 0) && (current_connect == 0)) {
            break;
        }
        /*
         * 添加socket连接
         * */
        if ((current_connect < MAXCONNECTS) && (unvisited_path_queue.size() != 0)) {
            /*
             * 创建socket以及ssl连接
             * */
            cout << "currentConnect: " << current_connect << " unvisited_path_queue: " << unvisited_path_queue.size() << endl;
            usleep(SENDTIMEOUT);
            int add_socket_fd = tcpConnect();
            current_connect++;
            Connection *connection;
            connection = sslConnect(add_socket_fd);

            /*
             * 获取 mix_path_url 以及设置 epoll 的附加信息
             * */
            string name, offset, limit;
            bool is_list;
            string mix_path_url;
            char *request;
            mix_path_url = unvisited_path_queue.top();
            unvisited_path_queue.pop();

            CrawlerData *crawlerData = (CrawlerData *) malloc(sizeof(CrawlerData));
            strcpy(crawlerData->path_url, mix_path_url.c_str());
            crawlerData->connection = *connection;
            switch (OPTIONS) {
                case 1: {
                    crawlerData->type = 1;
                    cout << "mix: " << mix_path_url << endl;
                    request = http_get_followers_head(mix_path_url, name, offset, limit, is_list);
                    break;
                }
                default: {
                    break;
                }
            }
//            printf("%s\n",request);
            crawlerData->is_list = is_list;
            if (strlen(request) != 0) {
                /*
                 * 发送request
                 * */
                int ssl_write_ret = sslWrite(connection, request);
                if (ssl_write_ret <= 0) {
                    log_error("%s sslWrite %s", name.c_str(), request);
                    current_connect--;
                    sslDisconnect(connection);
                    free(crawlerData);
                    continue;
                } else {
                    /*
                     * 发送成功
                     * 添加 epoll_event
                     * */
                    struct epoll_event event;
                    event.data.ptr = (void *) crawlerData;
                    event.events = EPOLLIN | EPOLLET;
                    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, add_socket_fd, &event);
                    if (ret != 0) {
                        log_error("epoll_add");
                        return;
                    }
//                    sslDisconnect(connection);
                }
            } else {
                current_connect--;
                sslDisconnect(connection);
                free(crawlerData);
                continue;
            }
        }

        /*
         * epoll_events 监听
         * */
        int n = epoll_wait(epoll_fd, events, MAXEVENTS, EPOLLTIMEOUT);
        CrawlerData *crawlerData;
        for (int i = 0; i < n; i++) {
            /*
             * 如果是读取事件
             * */
            if (events[i].events & EPOLLIN) {
                usleep(READTIMEOUT);
                crawlerData = (CrawlerData *) events[i].data.ptr;
                switch (crawlerData->type) {
                    case 1: {
                        string response = sslRead(&crawlerData->connection);
                        string::size_type pos = response.find("\r\n\r\n");
                        if (pos != string::npos) {
                            response = response.substr(pos + 4);
                        } else {
                            current_connect--;
                            sslDisconnect(&crawlerData->connection);
                            break;
                        }

                        if (response.find("<html>") != string::npos) {
                            current_connect--;
                            sslDisconnect(&crawlerData->connection);
                            break;
                        }

                        UserInfo userInfo;
                        if (crawlerData->is_list) {
                            userInfo.ParseList(response);
                            vector<string> url_list = userInfo.getData_url_token();
                            for (vector<string>::iterator iter = url_list.begin();
                                 iter != url_list.end(); iter++) {
                                string add = *iter;
                                if (visited_name[add] == false) {
                                    unvisited_path_queue.push(add);
                                }
                            }
                            if (userInfo.getUrl_token() != "") {
                                int offset = crawlerData->offset + 20;
                                if (offset < crawlerData->total) {
                                    char in_url[128];
                                    sprintf(in_url, "%s/%d/20", userInfo.getUrl_token().c_str(), offset);
                                    unvisited_path_queue.push(string(in_url));
                                }
                            }
                            sslDisconnect(&crawlerData->connection);
                            current_connect--;
                        } else {
                            userInfo.Parse(response);
                            if (userInfo.getUrl_token() != "") {
                                mongodbHelper.mongo_insert(userInfo);
                                string url_token = userInfo.getUrl_token();
                                int followers = userInfo.getFollower_count();
                                string in_url = url_token;
                                visited_name[url_token] = true;
                                if (followers != 0) {
                                    char sss[64];
                                    sprintf(sss, "%d", 0);
                                    in_url = in_url + "/" + sss + "/20";
                                    unvisited_path_queue.push(in_url);
                                    crawlerData->offset = 0;
                                    crawlerData->total = followers;
                                }
                                current_connect--;
                                sslDisconnect(&crawlerData->connection);
                            }
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
    }
    time(&end_time);
    time_t diff = end_time - start_time;
    time_t ss = diff % 60;
    diff /= 60;
    time_t mm = diff % 60;
    diff /= 60;
    time_t hh = diff % 24;
    diff /= 24;
    time_t dd = diff;
    printf("Crawler Finished! use %ld days, %ld hours, %ld minutes, %ld seconds\n", dd, hh, mm, ss);
}


int Crawler::tcpConnect() {
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        log_error("Socket");
        return socket_fd;
    }
    if (socket_fd == 0) {
        log_warning("socket_fd %d",socket_fd);
    }
    struct sockaddr_in server;
    memset(&server, '\0', sizeof(server));
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(url.getIp().c_str());
    server.sin_family = PF_INET;

    if (connect(socket_fd, (struct sockaddr *) &server, sizeof(struct sockaddr)) < 0){
        log_error("connect");
        return 0;
    }
    return socket_fd;
}


int Crawler::make_socket_non_blocking(int socket_fd) {
    int flags, s;
    flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
        log_error("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(socket_fd, F_SETFL, flags);
    if (s == -1) {
        log_error("fcntl");
        return -1;
    }
    return 0;
}


Crawler::Connection *Crawler::sslConnect(int socket_fd) {
    Connection *c;

    c = (Connection *) malloc(sizeof(Connection));
    c->sslHandle = NULL;
    c->sslContext = NULL;

    c->socket = socket_fd;
    if (c->socket) {
        // Register the error strings for libcrypto & libssl
        SSL_load_error_strings();

        // Register the available ciphers and digests
        SSL_library_init();
        OpenSSL_add_all_algorithms();

        // New context saying we are a client, and using SSL 2 or 3
        c->sslContext = SSL_CTX_new(SSLv23_client_method());
        if (c->sslContext == NULL)
            ERR_print_errors_fp(stderr);

        // Create an SSL struct for the connection
        c->sslHandle = SSL_new(c->sslContext);
        if (c->sslHandle == NULL)
            ERR_print_errors_fp(stderr);

        // Connect the SSL struct to our connection
        if (!SSL_set_fd(c->sslHandle, c->socket))
            ERR_print_errors_fp(stderr);

        // Initiate SSL handshake
        if (SSL_connect(c->sslHandle) != 1)
            ERR_print_errors_fp(stderr);
    } else {
        log_error("Connect failed");
    }
    return c;
}

void Crawler::sslDisconnect(Crawler::Connection *c) {
    if (c->socket)
        close(c->socket);
    if (c->sslHandle) {
        SSL_shutdown(c->sslHandle);
        SSL_free(c->sslHandle);
    }
    if (c->sslContext)
        SSL_CTX_free(c->sslContext);
//    free(c);
}

string Crawler::sslRead(Crawler::Connection *c) {
    string ret = "";
    int size = 2048;
    char buf[size + 1];
    int len = 0;
    unsigned long add = 0;
    string okhead = "HTTP/1.1 200 OK";
    string cont = "Content-Length: ";
    string expi = "Expires:";
    string var = "Vary: Accept-Encoding\r\n\r\n";
    int maxsize = 0;
    len = SSL_read(c->sslHandle, buf, size);
    if (len <= 0)
        return ret;
    buf[len] = '\0';
    ret += buf;
    if (ret.find(okhead) == string::npos)
        return "";

    string::size_type pos1 = ret.find(cont);
    if (pos1 == string::npos)
        return "";
    string::size_type pos2 = ret.find("\r\n", pos1);
    if (pos2 == string::npos)
        return "";
    pos1 += cont.size();
    maxsize = atoi(ret.substr(pos1, pos2).c_str());
    if (ret.find('{') == string::npos)
        return "";
    add = ret.substr(ret.find('{')).size();
    while (add < maxsize) {
        len = SSL_read(c->sslHandle, buf, size);
        if (len <= 0)
            return "";
        buf[len] = '\0';
        ret += buf;
        add += len;
    }
    return ret;
}

int Crawler::sslWrite(Crawler::Connection *c, char *text) {
    if (c) {
        int len = SSL_write(c->sslHandle, text, strlen(text));
        if (len < 0) {
            int err = SSL_get_error(c->sslHandle, len);
            switch (err) {
                case SSL_ERROR_WANT_WRITE:
                    return 0;
                case SSL_ERROR_WANT_READ:
                    return 0;
                case SSL_ERROR_ZERO_RETURN:
                case SSL_ERROR_SYSCALL:
                case SSL_ERROR_SSL:
                default:
                    return -1;
            }
        }
    }
}

char *Crawler::http_get_followers_first_head(const string &name) {
    char *ret = (char *) malloc(MAXREQUEST);
    snprintf(ret, MAXREQUEST,
             "GET /api/v4/members/%s?include=locations,employments,gender,educations,business,voteup_count,thanked_Count,follower_count,following_count,cover_url,following_topic_count,following_question_count,following_favlists_count,following_columns_count,avatar_hue,answer_count,articles_count,pins_count,question_count,columns_count,commercial_question_count,favorite_count,favorited_count,logs_count,marked_answers_count,marked_answers_text,message_thread_token,account_status,is_active,is_bind_phone,is_force_renamed,is_bind_sina,is_privacy_protected,sina_weibo_url,sina_weibo_name,show_sina_weibo,is_blocking,is_blocked,is_following,is_followed,mutual_followees_count,vote_to_count,vote_from_count,thank_to_count,thank_from_count,thanked_count,description,hosted_live_count,participated_live_count,allow_message,industry_category,org_name,org_homepage,badge[?(type=best_answerer)].topics HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "Connection: keep-alive\r\n"
                     "accept: application/json, text/plain, */*\r\n"
                     "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36\r\n"
                     "authorization: oauth c3cef7c66a1843f8b3a9e6a1e3160e20\r\n"
                     // "Referer: https://www.zhihu.com/people/season-53-21/followers\r\n"
                     // "Accept-Encoding: gzip, deflate, br\r\n"
                     "Accept-Language: en,zh-CN;q=0.8,zh;q=0.6\r\n"
                     //"Cookie: aliyungf_tc=AQAAABRPNhoiTAsAr2P53ND54agXG2tg; _xsrf=80829eb8-9d92-43f2-ba7e-42e828eba0b8\r\n"
                     "\r\n", name.c_str(), base_url);
    return ret;
}

bool Crawler::check_http_get_followers_first_head(const string &str) {
    if (str.find('/') == string::npos) {
        return true;
    } else {
        return false;
    }
}

char *Crawler::http_get_followers_list_head(const string &name, const string &offset, const string &limit) {
    /*
     * include:data[*].answer_count,articles_count,gender,follower_count,
     * is_followed,is_following,badge[?(type=best_answerer)].topics
     * offset:1340
     * limit:20
     * */
    /*
     * include=data%5B*%5D.answer_count%2Carticles_count%2Cgender%2Cfollower_count%2C
     * is_followed%2Cis_following%2Cbadge%5B%3F(type%3Dbest_answerer)%5D.topics&offset=1340&limit=20
     * */
    char *ret = (char *) malloc(MAXREQUEST);
    int iRet = snprintf(ret, MAXREQUEST,
                        "GET /api/v4/members/%s/followers?include=data[*].answer_count,articles_count,gender,follower_count,"
                                "is_followed,is_following,badge[?(type=Dbest_answerer)].topics&offset=%s&limit=%s HTTP/1.1\r\n"
                                "Host: %s\r\n"
                                "Connection: keep-alive\r\n"
                                "accept: application/json, text/plain, */*\r\n"
                                "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36\r\n"
                                "authorization: oauth c3cef7c66a1843f8b3a9e6a1e3160e20\r\n"
                                "Accept-Language: en,zh-CN;q=0.8,zh;q=0.6\r\n"
                                "Cookie: aliyungf_tc=AQAAAGp5r3ybAgQAr2P53L0Lz+hQv6HX; q_c1=e799039c3eee48c89438119c10ebdd4b|1504101638000|1504101638000; q_c1=30232591b31343a6a0ee4f6614784b6d|1504101638000|1504101638000; _zap=427d3051-6239-4116-8ed2-17a3d8b17933; _xsrf=b99adb9c-78b0-473e-878c-af1f7f3c265b\r\n"
                                "\r\n", name.c_str(), offset.c_str(), limit.c_str(), base_url);
    if (iRet < 0) {
        log_error("snprintf");
    }
    return ret;
}

char *Crawler::http_get_first(const string &name) {
    char *ret = (char *) malloc(MAXREQUEST);
    int iRet = snprintf(ret, MAXREQUEST,
                        "GET /people/cxfer HTTP/1.1\r\n"
                                "Host: %s\r\n"
                                "Connection: keep-alive"
                                "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
                                "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36\r\n"
                                "Accept-Language: en,zh-CN;q=0.8,zh;q=0.6"
                                "authorization: oauth c3cef7c66a1843f8b3a9e6a1e3160e20\r\n"
                                "Accept-Language: en,zh-CN;q=0.8,zh;q=0.6\r\n"
                                "Cookie: aliyungf_tc=AQAAAGp5r3ybAgQAr2P53L0Lz+hQv6HX; q_c1=e799039c3eee48c89438119c10ebdd4b|1504101638000|1504101638000; q_c1=30232591b31343a6a0ee4f6614784b6d|1504101638000|1504101638000; _zap=427d3051-6239-4116-8ed2-17a3d8b17933; _xsrf=b99adb9c-78b0-473e-878c-af1f7f3c265b\r\n"
                                "\r\n", name.c_str());

    if (iRet < 0) {
        log_error("snprintf");
    }
    return ret;
}

/*
 * name/
 * */
bool Crawler::check_http_get_followers_list_head(const string &str, string &name, string &offset, string &limit) {
    string::size_type pos1 = str.find('/');
    if (pos1 == string::npos)
        return false;
    string::size_type pos2 = str.find('/', pos1 + 1);
    if (pos2 == string::npos)
        return false;
    if (pos1 == pos2)
        return false;

    name = str.substr(0, pos1);
    string temp = str.substr(0, pos2);
    offset = temp.substr(temp.find('/') + 1);
    limit = str.substr(pos2 + 1);
    return true;
}

char *Crawler::http_get_followers_head(const string &mix_path_url, string &name, string &offset, string &limit, bool &is_list) {
    /*
     * 判断 mix_path_url 类型 是第一次还是获取列表
     * */
    char *ret = (char *) malloc(MAXREQUEST);
    memset(ret, 0, sizeof(ret));
    if (mix_path_url.find('/') == string::npos) {
        is_list = false;
        name = mix_path_url;
    } else {
        is_list = true;
        string::size_type pos1, pos2;
        pos1 = mix_path_url.find('/');
        name = mix_path_url.substr(0, pos1);
        string temp = mix_path_url.substr(pos1 + 1);
        pos2 = temp.find('/');
        if (pos2 == string::npos) {
            log_error("snprintf npos");
            sprintf(ret, "");
            return ret;
        }
        offset = temp.substr(0, pos2);
        limit = temp.substr(pos2 + 1);
    }

    if (is_list) {
        int iRet = snprintf(ret, MAXREQUEST,
                            "GET /api/v4/members/%s/followers?include=data[*].answer_count,articles_count,gender,follower_count,"
                                    "is_followed,is_following,badge[?(type=Dbest_answerer)].topics&offset=%s&limit=%s HTTP/1.1\r\n"
                                    "Host: %s\r\n"
                                    "Connection: keep-alive\r\n"
                                    "accept: application/json, text/plain, */*\r\n"
                                    "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36\r\n"
                                    "authorization: oauth c3cef7c66a1843f8b3a9e6a1e3160e20\r\n"
                                    "Accept-Language: en,zh-CN;q=0.8,zh;q=0.6\r\n"
                                    "Cookie: aliyungf_tc=AQAAAGp5r3ybAgQAr2P53L0Lz+hQv6HX; q_c1=e799039c3eee48c89438119c10ebdd4b|1504101638000|1504101638000; q_c1=30232591b31343a6a0ee4f6614784b6d|1504101638000|1504101638000; _zap=427d3051-6239-4116-8ed2-17a3d8b17933; _xsrf=b99adb9c-78b0-473e-878c-af1f7f3c265b\r\n"
                                    "\r\n", name.c_str(), offset.c_str(), limit.c_str(), base_url);
        if (iRet < 0) {
            log_error("snprintf %s", mix_path_url);
            sprintf(ret, "");
        }
    } else {
        int iRet = snprintf(ret, MAXREQUEST,
                            "GET /api/v4/members/%s?include=locations,employments,gender,educations,business,voteup_count,thanked_Count,follower_count,following_count,cover_url,following_topic_count,following_question_count,following_favlists_count,following_columns_count,avatar_hue,answer_count,articles_count,pins_count,question_count,columns_count,commercial_question_count,favorite_count,favorited_count,logs_count,marked_answers_count,marked_answers_text,message_thread_token,account_status,is_active,is_bind_phone,is_force_renamed,is_bind_sina,is_privacy_protected,sina_weibo_url,sina_weibo_name,show_sina_weibo,is_blocking,is_blocked,is_following,is_followed,mutual_followees_count,vote_to_count,vote_from_count,thank_to_count,thank_from_count,thanked_count,description,hosted_live_count,participated_live_count,allow_message,industry_category,org_name,org_homepage,badge[?(type=best_answerer)].topics HTTP/1.1\r\n"
                                    "Host: %s\r\n"
                                    "Connection: keep-alive\r\n"
                                    "accept: application/json, text/plain, */*\r\n"
                                    "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36\r\n"
                                    "authorization: oauth c3cef7c66a1843f8b3a9e6a1e3160e20\r\n"
                                    // "Referer: https://www.zhihu.com/people/season-53-21/followers\r\n"
                                    // "Accept-Encoding: gzip, deflate, br\r\n"
                                    "Accept-Language: en,zh-CN;q=0.8,zh;q=0.6\r\n"
                                    //"Cookie: aliyungf_tc=AQAAABRPNhoiTAsAr2P53ND54agXG2tg; _xsrf=80829eb8-9d92-43f2-ba7e-42e828eba0b8\r\n"
                                    "\r\n", name.c_str(), base_url);
        if (iRet < 0) {
            log_error("snprintf %s", mix_path_url);
            sprintf(ret, "");

        }
    }
    return ret;
}


