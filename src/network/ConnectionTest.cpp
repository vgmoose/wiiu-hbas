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
#include <malloc.h>
#include <string.h>
#include "ConnectionTest.h"
#include "dynamic_libs/curl_functions.h"
#include "utils/logger.h"

#include "dynamic_libs/os_functions.h"

ConnectionTest::ConnectionTest(const std::string & Url)
{
    if (Url!="") {
        mainUrl = Url;
        log_printf("ConnectionTest: Main url is %s", mainUrl.c_str());
    } else {
        mainUrl = "http://www.google.com";
        log_printf("ConnectionTest: No url specified; using default %s", mainUrl.c_str());
    }
}

bool ConnectionTest::Test()
{
    log_printf("ConnectionTest: Testing connection to %s...", mainUrl.c_str());

    CURL * curl = n_curl_easy_init();
    if(!curl)
        return false;

    n_curl_easy_setopt(curl, CURLOPT_URL, mainUrl.c_str());
    n_curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    n_curl_easy_perform(curl);

    int resp = 404;
    n_curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp);

    if(resp != 200) {
        log_printf("ConnectionTest: response is %d; 200 expected -> connection unavailable", resp);
        n_curl_easy_cleanup(curl);
        return false;
    }
    log_printf("ConnectionTest: response is 200 -> connection available");

    n_curl_easy_cleanup(curl);
    return true;
}


