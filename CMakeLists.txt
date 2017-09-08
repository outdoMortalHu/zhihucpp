cmake_minimum_required(VERSION 3.8)
project(zhihucpp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Log.h DNS.cpp DNS.h URL.cpp URL.h Crawler.cpp Crawler.h MongodbHelper.cpp MongodbHelper.h UserInfo.h)
include_directories(include)
include_directories(/usr/local/include/bsoncxx/v_noabi)
include_directories(/usr/local/include/mongocxx/v_noabi)
link_directories(/usr/local/lib)
link_libraries(/usr/local/lib/libmongocxx.so._noabi)
set(CMAKE_CXX_FLAGS_DEBUG "-lssl -lcrypto -lpthread -lmongocxx -lbsoncxx")
# -lboost_system -lboost_regex -lboost_thread
add_executable(zhihucpp ${SOURCE_FILES})