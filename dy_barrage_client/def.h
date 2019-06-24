#pragma once
//
// Created by openkt on 2018/8/4.
//

#ifndef DY_BARRAGE_DEF_H
#define DY_BARRAGE_DEF_H

#include <string>

// message type
enum DY_MSG_TYPE
{
    DY_MSG_TYPE_OTHERS = -1,         // others
    DY_MSG_TYPE_CHATMSG = 0,         // barrage
    DY_MSG_TYPE_LOGIN_RESPONSE = 1,  // login response
    DY_MSG_TYPE_LIVE_STAT = 2,       // live status changed
    DY_MSG_TYPE_USER_ENTER = 3,      // enter into the room
    // other...
};

// barrage color
enum BARR_COLOR
{
    BARR_COLOR_WHITE = 0,
    BARR_COLOR_BLUE = 2,
    BARR_COLOR_GREEN = 3,
    BARR_COLOR_PINK = 6,
    BARR_COLOR_ORANGE  = 4,
    BARR_COLOR_PURPLE = 5,
    BARR_COLOR_RED = 1
};

// UI message type
enum MSG_TYPE
{
    MSG_TYPE_BARR = 0,               // barrage
    MSG_TYPE_MSG = 1,                // connection message
    MSG_TYPE_ERR = 2,                // error message
    MSG_TYPE_LIVE_BEGIN = 3,         // live begin
    MSG_TYPE_LIVE_END = 4,           // live end
    MSG_TYPE_USER_ENTER = 5,         // user enter the room
    // other...
};

// barrage
typedef struct dy_barrage {
    /*
    * platform_type
    * 0-web
    * 1-android
    * 2-ios
    * */
    int login_platform;

    /*
    * room permissions group
    * 0-temporary visitor (default)
    * 1-room visitor
    * 2-room temporary administrator
    * 3-room administrator
    * 4-room super administrator
    * 5-room owner
    * */
    int room_group;

    /*
    * platform permissions group
    * 0-temporary visitor (default)
    * 1-normal user
    * 2-honor administrator
    * 3-tour administrator
    * 4-director
    * 5-super administrator
    * */
    int platform_group;

    /*
    * barrage content
    * */
    std::string content;

    /*
    * username
    * */
    std::string sender_name;

    /*
    * user level
    * */
    int user_level;

    /*
    * barrage color
    * 0-white
    * 2-blue
    * 3-green
    * 6-pink
    * 4-orange
    * 5-purple
    * 1-red
    * */
    int barr_color;

    /*
    * 0-is not noble barrage
    * 1-is noble barrage
    * */
    int is_noble;

    /*
    * 0-is not fans barrage
    * 1-is fans barrage
    * */
    int is_fans_barr;

    /*
    * the room number of the badge worn
    * */
    int badge_room_id;

    // other...
}dy_barrage;

typedef struct user_enter_room_msg {
    /*
    * room group
    * */
    int room_group;

    /*
    * platform group
    * */
    int platform_group;

    /*
    * user name
    * */
    std::string user_name;
}user_enter_room_msg;

typedef struct user_leave_room_msg {
    /*
    * room group
    * */
    int room_group;

    /*
    * platform group
    * */
    int platform_group;

    /*
    * user name
    * */
    std::string user_name;
}user_leave_room_msg;

#endif //DY_BARRAGE_DEF_H
