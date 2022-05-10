//
// Created by Victor Alicino on 10/05/22.
//

#include "WebConnection.h"

WebConnection::WebConnection() {
    //Create HTTP Stream to read data
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    this->http_stream_reader = http_stream_init(&http_cfg);
}

http_stream_reader WebConnection::get_http_stream_reader() {
    return this->http_stream_reader;
}
