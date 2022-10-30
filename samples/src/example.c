
/*
<https://github.com/rafagafe/tiny-json>

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.
  SPDX-License-Identifier: MIT
  Copyright (c) 2018 Rafa Garcia <rafagarcia77@gmail.com>.
  Permission is hereby  granted, free of charge, to any  person obtaining a copy
  of this software and associated  documentation files (the "Software"), to deal
  in the Software  without restriction, including without  limitation the rights
  to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
  copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
  IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
  FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
  AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
  LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include <stdio.h>
#include <stdlib.h>
#include "json-maker/json-maker.h"

struct weather {
    int temp;
    int hum;
};

struct time {
    int hour;
    int minute;
};

struct measure {
    struct weather weather;
    struct time time;
};

struct data {
    char const* city;
    char const* street;
    struct measure measure;
    int samples[ 4 ];
};

/* Add a time object property in a JSON string.
  "name":{"temp":-5,"hum":48}, */
char* json_weather( char* dest, char const* name, struct weather const* weather, size_t* remlen ) {
    dest = json_objOpen( dest, name, remlen );              // --> "name":{\0
    dest = json_int( dest, "temp", weather->temp, remlen ); // --> "name":{"temp":22,\0
    dest = json_int( dest, "hum", weather->hum, remlen );   // --> "name":{"temp":22,"hum":45,\0
    dest = json_objClose( dest, remlen );                   // --> "name":{"temp":22,"hum":45},\0
    return dest;
}

/* Add a time object property in a JSON string.
  "name":{"hour":18,"minute":32}, */
char* json_time( char* dest, char const* name, struct time const* time, size_t* remlen ) {
    dest = json_objOpen( dest, name, remlen );
    dest = json_int( dest, "hour",   time->hour, remlen );
    dest = json_int( dest, "minute", time->minute, remlen );
    dest = json_objClose( dest, remlen );
    return dest;
}

/* Add a measure object property in a JSON string.
 "name":{"weather":{"temp":-5,"hum":48},"time":{"hour":18,"minute":32}}, */
char* json_measure( char* dest, char const* name, struct measure const* measure, size_t* remlen ) {
    dest = json_objOpen( dest, name, remlen );
    dest = json_weather( dest, "weather", &measure->weather, remlen );
    dest = json_time( dest, "time", &measure->time, remlen );
    dest = json_objClose( dest, remlen );
    return dest;
}

/* Add a data object property in a JSON string. */
char* json_data( char* dest, char const* name, struct data const* data, size_t* remlen ) {
    dest = json_objOpen( dest, NULL, remlen );
    dest = json_str( dest, "city",   data->city, remlen );
    dest = json_str( dest, "street", data->street, remlen );
    dest = json_measure( dest, "measure", &data->measure, remlen );
    dest = json_arrOpen( dest, "samples", remlen );
    for( int i = 0; i < 4; ++i )
        dest = json_int( dest, NULL, data->samples[i], remlen );
    dest = json_arrClose( dest, remlen );
    dest = json_objClose( des, remlen  );
    return dest;
}

/** Convert a data structure to a root JSON object.
  * @param dest Destination memory block.
  * @param data Source data structure.
  * @return  The JSON string length. */
int data_to_json( char* dest, struct data const* data, size_t* remlen ) {
    char* p = json_data( dest, NULL, data, remlen );
    p = json_end( p, remlen );
    return p - dest;
}

/*
 * {
 *     "city": "liverpool",
 *     "street": "mathew",
 *     "measure": {
 *         "weather": {
 *             "temp": 25,
 *             "hum": 65
 *         },
 *         "time": {
 *             "hour": 14,
 *             "minute": 31
 *         }
 *     },
 *     "samples": [
 *         25,
 *         65,
 *         -37,
 *         512
 *     ]
 * }
 *
 */

int main(int argc, char** argv) {
    static struct data const data = {
        .city    = "liverpool",
        .street  = "mathew",
        .measure = {
            .weather = {
                .hum  = 65,
                .temp = 25
            },
            .time = {
                .hour   = 14,
                .minute = 31
            }
        },
        .samples = {
             25,
             65,
            -37,
            512
        }
    };
    enum {
        bufflen = 512
    };
    char buff[bufflen];
    size_t remlen = bufflen;
    int len = data_to_json( buff, &data, &remlen );
    if( len >= sizeof buff ) {
        fprintf( stderr, "%s%d%s%d\n", "Error. Len: ", len, " Max: ", (int)sizeof buff - 1 );
        return EXIT_FAILURE;
    }
    puts( buff );
    return EXIT_SUCCESS;
}
