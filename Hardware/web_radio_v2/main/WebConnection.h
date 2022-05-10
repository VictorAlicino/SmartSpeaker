//
// Created by Victor Alicino on 10/05/22.
//

#ifndef WEB_RADIO_V2_WEBCONNECTION_H
#define WEB_RADIO_V2_WEBCONNECTION_H


class WebConnection {
private:
    audio_element_handle_t http_stream_reader;

    //Singleton constructor
    WebConnection();

public:
    static WebConnection& get_instance();

    http_stream_reader get_http_stream_reader() const;
};


#endif //WEB_RADIO_V2_WEBCONNECTION_H
