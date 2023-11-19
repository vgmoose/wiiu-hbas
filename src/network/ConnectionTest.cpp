/****************************************************************************
 * Copyright (C) 2016 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
// #include <malloc.h>
#include <string.h>
#include <curl/curl.h>
#include "ConnectionTest.h"

ConnectionTest::ConnectionTest(const std::string & Url)
{
    if (Url!="") {
        mainUrl = Url;
        printf("ConnectionTest: Main url is %s", mainUrl.c_str());
    } else {
        mainUrl = "https://www.google.com";
        printf("ConnectionTest: No url specified; using default %s", mainUrl.c_str());
    }
}

bool ConnectionTest::Test()
{
    printf("ConnectionTest: Testing connection to %s...", mainUrl.c_str());

    CURL * curl = curl_easy_init();
    if(!curl)
        return false;

    curl_easy_setopt(curl, CURLOPT_URL, mainUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_perform(curl);

    int resp = 404;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp);

    if(resp != 200) {
        printf("ConnectionTest: response is %d; 200 expected -> connection unavailable", resp);
        curl_easy_cleanup(curl);
        return false;
    }
    printf("ConnectionTest: response is 200 -> connection available");

    // curl_easy_cleanup(curl);
    return true;
}


