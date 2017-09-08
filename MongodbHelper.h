//
// Created by user on 17-9-1.
//

#ifndef ZHIHUCPP_MONGODBHELPER_H
#define ZHIHUCPP_MONGODBHELPER_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include "UserInfo.h"


using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

class MongodbHelper {
public:
    bsoncxx::document::value doc_value = bsoncxx::document::value(nullptr, 0, nullptr);
public:
    MongodbHelper();

    bool mongo_insert(const UserInfo &userinfo);

    bool mongo_delete(const UserInfo &userInfo);

    bool mongo_change(const UserInfo &userInfo);

    UserInfo mongo_query(const UserInfo &usrinfo);

    void mongo_list();

    int mongo_count();

private:
    mongocxx::uri uri;
    mongocxx::database database;
    mongocxx::client client;
    mongocxx::collection collection;
};


#endif //ZHIHUCPP_MONGODBHELPER_H
