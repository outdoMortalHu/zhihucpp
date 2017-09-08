//
// Created by user on 17-9-1.
//

#include "MongodbHelper.h"


MongodbHelper::MongodbHelper() {
    uri = mongocxx::uri("mongodb://127.0.0.1:27017");
    client = mongocxx::client(uri);
    database = mongocxx::database(client["zhihu"]);
    collection = mongocxx::collection(database["users"]);
}

bool MongodbHelper::mongo_insert(const UserInfo &userinfo) {
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
            << "answer_count" << userinfo.getAnswer_count()
            << "articles_count" << userinfo.getArticles_count()
            << "avatar_url" << userinfo.getAvatar_url()
            << "business_name" << userinfo.getBusiness_name()
            << "business_introduction" << userinfo.getBusiness_introduction()
            << "description" << userinfo.getDescription()
            << "educations_school_name" << userinfo.getEducations_school_name()
            << "educations_major_name" << userinfo.getEducations_major_name()
            << "employments_company_name" << userinfo.getEmployments_company_name()
            << "employments_job_name" << userinfo.getEmployments_job_name()
            << "follower_count" << userinfo.getFollower_count()
            << "following_count" << userinfo.getFollowing_count()
            << "headline" << userinfo.getHeadline()
            << "id" << userinfo.getId()
            << "locations_name" << userinfo.getLocations_name()
            << "name" << userinfo.getName()
            << "question_count" << userinfo.getQuestion_count()
            << "url_token" << userinfo.getUrl_token()
            << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view view = doc_value.view();
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
            collection.insert_one(view);
    return false;
}

bool MongodbHelper::mongo_delete(const UserInfo &userInfo) {
    return false;
}

bool MongodbHelper::mongo_change(const UserInfo &userInfo) {
    return false;
}

UserInfo MongodbHelper::mongo_query(const UserInfo &usrinfo) {
    return UserInfo();
}

void MongodbHelper::mongo_list() {

}

int MongodbHelper::mongo_count() {
    return 0;
}
