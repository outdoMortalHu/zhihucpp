//
// Created by user on 17-9-2.
//

#ifndef ZHIHUCPP_USERINFO_H
#define ZHIHUCPP_USERINFO_H

#include <iostream>


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

using namespace std;

class UserInfo {
public:
    UserInfo() {}

    bool ParseList(const string &response) {
        clear();
        rapidjson::Document document;
        document.Parse(response.c_str());
        if (document.IsObject()) {
            if (document.HasMember("data")) {
                rapidjson::Value &data = document["data"];
                if (data.IsArray()) {
                    for (auto &da : data.GetArray()) {
                        if (da.HasMember("url_token"));
                        rapidjson::Value &url_token = da["url_token"];
                        data_url_token.push_back(url_token.GetString());
                    }
                }
            }
        }
    }

    bool Parse(const string &response) {
        clear();
        rapidjson::Document document;
        document.Parse(response.c_str());
        if (document.IsObject()) {
            if (document.HasMember("name") && document.HasMember("url_token")) {
                if (document.HasMember("answer_count"))
                    answer_count = document["answer_count"].GetInt();
                if (document.HasMember("articles_count"))
                    articles_count = document["articles_count"].GetInt();
                if (document.HasMember("avatar_url"))
                    avatar_url = document["avatar_url"].GetString();
                if (document.HasMember("business")) {
                    rapidjson::Value &business = document["business"];
                    if (business.HasMember("name"))
                        business_name = business["name"].GetString();
                    if (business.HasMember("introduction"))
                        business_introduction = business["introduction"].GetString();
                }
                if (document.HasMember("description"))
                    description = document["description"].GetString();
                if (document.HasMember("educations")) {
                    rapidjson::Value &educations = document["educations"];
                    if (educations.IsArray()) {
                        for (auto &education : educations.GetArray()) {
                            string school_name, major_name;
                            school_name = "";
                            major_name = "";
                            if (education.HasMember("school")) {
                                rapidjson::Value &school = education["school"];
                                if (school.HasMember("name"))
                                    school_name = school["name"].GetString();
                            }
                            if (education.HasMember("major")) {
                                rapidjson::Value &major = education["major"];
                                if (major.HasMember("name"))
                                    major_name = major["name"].GetString();
                            }
                            educations_school_name = school_name;
                            educations_major_name = major_name;
                            break;
                        }
                    }
                }
                if (document.HasMember("employments")) {
                    rapidjson::Value &employments = document["employments"];
                    if (employments.IsArray()) {
                        for (auto &employment:employments.GetArray()) {
                            string company_name, job_name;
                            company_name = job_name = "";
                            if (employment.HasMember("company")) {
                                rapidjson::Value &company = employment["company"];
                                if (company.HasMember("name"))
                                    company_name = company["name"].GetString();
                            }
                            if (employment.HasMember("job")) {
                                rapidjson::Value &job = employment["job"];
                                if (job.HasMember("name"))
                                    job_name = job["name"].GetString();
                            }
                            employments_company_name = company_name;
                            employments_job_name = job_name;
                            break;
                        }
                    }
                }

                if (document.HasMember("follower_count"))
                    follower_count = document["follower_count"].GetInt();
                if (document.HasMember("following_count"))
                    following_count = document["following_count"].GetInt();
                if (document.HasMember("headline"))
                    headline = document["headline"].GetString();
                if (document.HasMember("id"))
                    id = document["id"].GetString();
                locations_name = "";
                if (document.HasMember("locations")) {
                    rapidjson::Value &locations = document["locations"];
                    if (locations.IsArray()) {
                        for (auto &location : locations.GetArray()) {
                            if (location.HasMember("name")) {
                                locations_name = location["name"].GetString();
                            }
                        }
                    }
                }
                if (document.HasMember("name"))
                    name = document["name"].GetString();
                if (document.HasMember("question_count"))
                    question_count = document["question_count"].GetInt();
                if (document.HasMember("url_token"))
                    url_token = document["url_token"].GetString();
            }
        }
    }

    friend ostream &operator<<(ostream &os, const UserInfo &info) {
        os << "answer_count: " << info.answer_count << " articles_count: " << info.articles_count << " avatar_url: "
           << info.avatar_url << " business_name: " << info.business_name << " business_introduction: "
           << info.business_introduction << " description: " << info.description << " educations_school_name: "
           << info.educations_school_name << " educations_major_name: " << info.educations_major_name
           << " employments_company_name: " << info.employments_company_name << " employments_job_name: "
           << info.employments_job_name << " follower_count: " << info.follower_count << " following_count: "
           << info.following_count << " headline: " << info.headline << " id: " << info.id << " locations_name: "
           << info.locations_name << " name: " << info.name << " question_count: " << info.question_count
           << " url_token: " << info.url_token;
        return os;
    }

    int getAnswer_count() const {
        return answer_count;
    }

    int getArticles_count() const {
        return articles_count;
    }

    const string &getAvatar_url() const {
        return avatar_url;
    }

    const string &getBusiness_name() const {
        return business_name;
    }

    const string &getBusiness_introduction() const {
        return business_introduction;
    }

    const string &getDescription() const {
        return description;
    }

    const string &getEducations_school_name() const {
        return educations_school_name;
    }

    const string &getEducations_major_name() const {
        return educations_major_name;
    }

    const string &getEmployments_company_name() const {
        return employments_company_name;
    }

    const string &getEmployments_job_name() const {
        return employments_job_name;
    }

    int getFollower_count() const {
        return follower_count;
    }

    int getFollowing_count() const {
        return following_count;
    }

    const string &getHeadline() const {
        return headline;
    }

    const string &getId() const {
        return id;
    }

    const string &getLocations_name() const {
        return locations_name;
    }

    const string &getName() const {
        return name;
    }

    int getQuestion_count() const {
        return question_count;
    }

    const string &getUrl_token() const {
        return url_token;
    }

    const vector<string> &getData_url_token() const {
        return data_url_token;
    }

    void clear() {
        answer_count = 0;
        articles_count = 0;
        avatar_url = "";
        business_name = "";
        business_introduction = "";
        description = "";
        educations_school_name = "";
        educations_major_name = "";
        employments_company_name = "";
        employments_job_name = "";
        follower_count = 0;    // 关注者
        following_count = 0;    // 关注了
        headline = "";
        id = "";
        locations_name = "";
        name = "";
        question_count = 0;
        url_token = "";
        data_url_token.clear();
    }

private:
    int answer_count;
    int articles_count;
    string avatar_url;
    string business_name;
    string business_introduction;
    string description;
    string educations_school_name;
    string educations_major_name;
    string employments_company_name;
    string employments_job_name;
    int follower_count;    // 关注者
    int following_count;    // 关注了
    string headline;
    string id;
    string locations_name;
    string name;
    int question_count;
    string url_token;
private:
    vector<string> data_url_token;
};


#endif //ZHIHUCPP_USERINFO_H
