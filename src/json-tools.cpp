#include "json-tools.h"

int jansson_get_repo_count(std::string json_user_string) {
    json_t *root;
    json_error_t error;

    root = json_loads(json_user_string.c_str(), 0, &error);

    if(!root)
    {
        return 0;
    }

    if(!json_is_object(root))
    {
        json_decref(root);
        return 0;
    }

    json_t *data;

    data = json_object_get(root, "public_repos"); // we only care about public repos.

    if(!json_is_integer(data))
    {
        json_decref(root);
        return 0;
    }

    int returnVal = json_integer_value(data);
    json_decref(root);
    return returnVal;
}

int jansson_parse_repo_list(std::string json_repo_list_string, std::vector<Repo> *repoList) {
    json_t *root;
    json_error_t error;

    int count = 0;

    root = json_loads(json_repo_list_string.c_str(), 0, &error);

    if(!json_is_array(root))
    {
        json_decref(root);
        return 0;
    }

    for(size_t i = 0; i < json_array_size(root); i++) {
        json_t *data, *name, *priv, *description, *fork, *created_at, *updated_at, *homePage, *stargazers_count, *language, *license, *forks, *open_issues_count;

        std::string nameString, descriptionString, created_atString, updated_atString, homePageString, languageString, licenseString;
        bool privBool, forkBool;
        int stargazers_countInt, forksInt, open_issues_countInt;
        data = json_array_get(root, i);
        if(!json_is_object(data))
        {
            json_decref(root);
            return 0;
        }
        name = json_object_get(data, "name");
        if(json_is_string(name)) nameString = json_string_value(name);

        priv = json_object_get(data, "private");
        if(json_is_boolean(priv)) privBool = json_boolean_value(priv);
        //do owner info here
        description = json_object_get(data, "description");
        if(json_is_string(description)) descriptionString = json_string_value(description);

        fork = json_object_get(data, "fork");
        if(json_is_boolean(fork)) forkBool = json_boolean_value(fork);

        created_at = json_object_get(data, "created_at");
        if(json_is_string(created_at)) created_atString = json_string_value(created_at);

        updated_at = json_object_get(data, "updated_at");
        if(json_is_string(updated_at)) updated_atString = json_string_value(updated_at);

        homePage = json_object_get(data, "homePage");
        if(json_is_string(homePage)) homePageString = json_string_value(homePage);

        stargazers_count = json_object_get(data, "stargazers_count");
        if(json_is_integer(stargazers_count)) stargazers_countInt = json_integer_value(stargazers_count);

        language = json_object_get(data, "language");
        if(json_is_string(language)) languageString = json_string_value(language);

        license = json_object_get(data, "license");
        if(json_is_object(license)) {
            json_t *license_name = json_object_get(license, "name");
            if(json_is_string(license_name)) licenseString = json_string_value(license_name);
        }

        forks = json_object_get(data, "forks");
        if(json_is_integer(forks)) forksInt = json_integer_value(forks);

        open_issues_count = json_object_get(data, "open_issues_count");
        if(json_is_integer(open_issues_count)) open_issues_countInt = json_integer_value(open_issues_count);

        Repo newRepo(nameString,
                     descriptionString,
                     licenseString,
                     "",
                     created_atString,
                     updated_atString,
                     homePageString,
                     languageString,
                     forkBool,
                     privBool,
                     forksInt,
                     stargazers_countInt,
                     open_issues_countInt
                 );
        repoList->push_back(newRepo);

        count += 1;
    }

    json_decref(root);
    return count;

}

std::string jansson_get_avatar_url(std::string json_user_string) {
    json_t *root;
    json_error_t error;

    root = json_loads(json_user_string.c_str(), 0, &error);

    if(!root)
    {
        return "Error getting image url";
    }

    if(!json_is_object(root))
    {
        json_decref(root);
        return "Error getting image url";
    }

    json_t *data;

    data = json_object_get(root, "avatar_url");
    if(!json_is_string(data))
    {
        json_decref(root);
        return "Error getting image url";
    }

    std::string returnVal = json_string_value(data);
    json_decref(root);
    return returnVal;
}

int jansson_parse_release_list(std::string json_release_list_string, std::vector<Release> *releaseList) {
    json_t *root;
    json_error_t error;

    root = json_loads(json_release_list_string.c_str(), 0, &error);

    if(!root)
    {
        return 0;
    }

    if(!json_is_array(root))
    {
        json_decref(root);
        return 0;
    }

    int count = 0;

    for(size_t i = 0; i < json_array_size(root); i++) {
        json_t *data, *tag_name, *name, *created_at, *body, *asset_array, *draft, *prerelease;
        std::vector<Asset> assets;

        data = json_array_get(root, i);
        if(!json_is_object(data))
        {
            json_decref(root);
            return count;
        }

        tag_name = json_object_get(data, "tag_name"); // we only care about public repos.
        name = json_object_get(data, "name");
        created_at = json_object_get(data, "created_at");
        body = json_object_get(data, "body");
        draft = json_object_get(data, "draft");
        prerelease = json_object_get(data, "prerelease");

        asset_array = json_object_get(data, "assets");
        if(json_is_array(asset_array)) {
            for(size_t c = 0; c < json_array_size(asset_array); c++) {
                json_t *asset_data, *asset_name, *asset_url, *asset_size;

                asset_data = json_array_get(asset_array, c);
                if(!json_is_object(asset_data))
                    break;
                asset_name = json_object_get(asset_data, "name");
                asset_url = json_object_get(asset_data, "browser_download_url");
                asset_size = json_object_get(asset_data, "size");

                Asset new_asset;
                if(json_is_string(asset_name))
                    new_asset.name = json_string_value(asset_name);
                if(json_is_string(asset_url))
                    new_asset.url = json_string_value(asset_url);
                if(json_is_integer(asset_size))
                    new_asset.size = json_integer_value(asset_size);
                assets.push_back(new_asset);
            }
        }

        Release new_release;
        if(json_is_string(tag_name))
            new_release.tag_name = json_string_value(tag_name);
        if(json_is_string(name))
            new_release.name = json_string_value(name);
        if(json_is_string(created_at))
            new_release.created_at = json_string_value(created_at);
        if(json_is_string(body)){
            new_release.body = json_string_value(body);
            //stripUnicode(new_release.body);
        }
        if(json_is_boolean(prerelease))
            new_release.prerelease = json_boolean_value(prerelease);
        for(size_t x = 0; x < assets.size(); x++)
            new_release.assets.push_back(assets[x]);

        if(json_is_boolean(draft)) {
            if(json_boolean_value(draft) == false) {
                count += 1;
                releaseList->push_back(new_release);
            }
        }
    }

    json_decref(root);
    return count;
}
void jansson_parse_asset_list(json_t *json_array, std::vector<Asset> *assetList) {
    if(!json_is_array(json_array)) {
        return;
    }

    for(size_t i = 0; i < json_array_size(json_array); i++) {
        json_t *data, *name, *url;

        data = json_array_get(json_array, i);
        if(!json_is_object(data))
            return;
        name = json_object_get(data, "name");
        url = json_object_get(data, "browser_download_url");

        Asset new_asset;
        if(json_is_string(name))
            new_asset.name = json_string_value(name);
        if(json_is_string(url))
            new_asset.url = json_string_value(url);
        assetList->push_back(new_asset);
    }
}
