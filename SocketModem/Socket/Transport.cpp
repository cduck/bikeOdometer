/* Universal Socket Modem Interface Library
* Copyright (c) 2013 Multi-Tech Systems
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "Transport.h"
#include "Cellular.h"
#include "Wifi.h"

Transport::TransportType Transport::_type = Transport::NONE;

IPStack* Transport::customType = NULL;

void Transport::setTransport(TransportType type)
{
    if (type == CUSTOM) {
        printf("[ERROR] Transport not set, use other setTransport method for setting custom type.\n\r");
        return;
    }
    _type = type;
}

void Transport::setTransport(IPStack* type)
{
    customType = type;
    _type = CUSTOM;
}

IPStack* Transport::getInstance()
{
    switch (_type) {
        case CELLULAR:
            return (IPStack*) Cellular::getInstance();
        case WIFI:
            return (IPStack*) Wifi::getInstance();
        case CUSTOM:
            return customType;
        default:
            printf("[ERROR] Transport not set, use setTransport method.\n\r");
            return NULL;
    }
}

