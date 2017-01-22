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
#ifndef _CONNECTION_TEST_H
#define _CONNECTION_TEST_H

#include <string>
#include "fs/CFile.hpp"

class ConnectionTest
{
public:

    ConnectionTest(const std::string & Url = "");
    bool Test();
private:
    std::string mainUrl;
};

#endif // _CONNECTION_TEST_H
