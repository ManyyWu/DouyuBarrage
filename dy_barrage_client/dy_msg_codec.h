//
// Created by openkt on 2018/8/5.
//

#ifndef DY_BARRAGE_DY_MSG_CODEC_H
#define DY_BARRAGE_DY_MSG_CODEC_H

#include "def.h"
#include <string>

// douyu message encoder 
class dy_msg_encoder {
public:
    // set header to packet
    static std::string & set_pack_header (std::string &pack);

    // create login request packet
    static std::string create_login_req ();

    // create join group request packet
    static std::string create_join_group_req (int room_id, int group_id);

    // create join group request packet
    static std::string create_keep_alive_req ();
};

// douyu message decoder 
class dy_msg_decoder {
public:
    // get login result
    static int get_login_res (std::string &pack);

    // get message type
    static int get_msg_type (std::string &pack);

    // parse barrage 
    static dy_barrage parse_barrage (std::string &pack);

    // parse live status
    static int parse_live_stat (std::string &pack);

    // parse msg that user enter into the room
    static user_enter_room_msg parse_user_enter_room_msg (std::string &pack);

private:
    static std::string parse_msg_type (const char *data);
};

#endif //DY_BARRAGE_DY_MSG_CODEC_H
