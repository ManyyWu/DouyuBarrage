//
// Created by openkt on 2018/8/5.
//

#include "dy_msg_codec.h"
#include "dy_barrage_codec.h"
#include "def.h"
#include <string>
#include <ctime>
#ifdef _MSC_VER
#include <windows.h>
#endif

std::string & dy_msg_encoder::set_pack_header (std::string &pack)
{
    int data_len   = (int)pack.length() + 8;
    short msg_type = 689;                                           // client message type is 689
    char encrypt   = 0;
    char reserve   = 0;

    pack.insert(0, (const char *)&reserve, sizeof(reserve));        // 1 bytes is reserve
    pack.insert(0, (const char *)&encrypt, sizeof(encrypt));        // 1 bytes is encrypt
    pack.insert(0, (const char *)&msg_type, sizeof(msg_type));      // 2 bytes is message type
    pack.insert(0, (const char *)&data_len, sizeof(data_len));      // 4 bytes is len
    pack.insert(0, (const char *)&data_len, sizeof(data_len));      // 4 bytes is len

    return pack;
}

std::string dy_msg_encoder::create_login_req ()
{
    dy_barrage_encoder enc;
    enc.add_item("type", "loginreq");

    std::string login_req = enc.get_result();
    char c = '\0';
    login_req.append((const char *)&c, 1);       //should end with '\0'

    return set_pack_header(login_req);
}

std::string dy_msg_encoder::create_join_group_req (int room_id, int group_id)
{
    dy_barrage_encoder enc;
    enc.add_item("type", "joingroup");
    enc.add_item("rid", room_id);
    enc.add_item("gid", group_id);

    std::string enc_str = enc.get_result();
    char c = '\0';
    enc_str.append((const char *)&c, 1);         //should end with '\0'

    return set_pack_header(enc_str);
}

std::string dy_msg_encoder::create_keep_alive_req ()
{
    dy_barrage_encoder enc;
    enc.add_item("type", "keeplive");
#ifdef _MSC_VER
    enc.add_item("tick", (int)GetTickCount());
#else
    enc.add_item("tick", (int)time(NULL));
#endif

    std::string enc_str = enc.get_result();
    char c = '\0';
    enc_str.append((const char *)&c, 1);        //should end with '\0'

    return set_pack_header(enc_str);
}

std::string dy_msg_decoder::parse_msg_type (const char *data)
{
    if (*data == '\0')
    {
        return "";
    }

    item kv;
    std::string buf;

    while (*data != '\0')
    {
        if (*data == '/')    //end char
        {
            kv.value = buf;
            break;
        }
        else if (*data == '@')
        {
            data++;

            if (*data == 'A')
            {
                buf += '@';
            }
            else if (*data == 'S')  // char '/'
            {
                buf += '/';
            }
            else if (*data == '=')  // key value separator
            {
                kv.key = buf;
                buf.clear();
            }
        }
        else
        {
            buf += *data;
        }

        data++;
    }

    if (kv.key == "type")
        if (kv.value != "")
            return kv.value;
    return "";
}

int dy_msg_decoder::get_login_res (std::string &pack)
{
    int ret = 0;

    std::string dec_str(pack.data() + 8, pack.size() - 8);

    dy_barrage_decoder dec;
    dec.parse(dec_str.c_str());

    std::string type_str = dec.get_item_as_string("type");
    if(type_str != "loginres")
        ret = -1;

    return ret;
}

int dy_msg_decoder::get_msg_type (std::string &pack)
{
    if (pack.size() <= 8)
        return -1;

    std::string type_str = parse_msg_type(pack.c_str() + 8);
    if ("" == type_str)
        return -1;

    int msg_type;
    if(type_str == "chatmsg")
    {
        msg_type = DY_MSG_TYPE_CHATMSG;
    }
    else if (type_str == "loginres")
    {
        msg_type = DY_MSG_TYPE_LOGIN_RESPONSE;
    }
    else if (type_str == "rss")
    {
        msg_type = DY_MSG_TYPE_LIVE_STAT;
    }
    else if (type_str == "uenter")
    {
        msg_type = DY_MSG_TYPE_USER_ENTER;
    }
    else
    {
        msg_type = DY_MSG_TYPE_OTHERS;
    }
    // other...

    return msg_type;
}

 dy_barrage dy_msg_decoder::parse_barrage (std::string &pack)
{
    std::string dec_str(pack.data() + 8, pack.size() - 8);

    dy_barrage_decoder dec;
    dec.parse(dec_str.c_str());

    dy_barrage barr;
    barr.login_platform = dec.get_item_as_int("ct");
    barr.content = dec.get_item_as_string("txt");
    barr.room_group = dec.get_item_as_int("rg");
    barr.platform_group = dec.get_item_as_int("pg");
    barr.sender_name = dec.get_item_as_string("nn");
    barr.user_level = dec.get_item_as_int("level");
    barr.barr_color = dec.get_item_as_int("col");
    barr.is_noble = dec.get_item_as_int("nc");
    barr.is_fans_barr = dec.get_item_as_int("ifs");
    barr.badge_room_id = dec.get_item_as_int("brid");

    return barr;
}

int dy_msg_decoder::parse_live_stat (std::string &pack)
{
    std::string dec_str(pack.data() + 8, pack.size() - 8);

    dy_barrage_decoder dec;
    dec.parse(dec_str.c_str());

    return dec.get_item_as_int("ss");
}

user_enter_room_msg dy_msg_decoder::parse_user_enter_room_msg (std::string &pack)
{
    std::string dec_str(pack.data() + 8, pack.size() - 8);

    dy_barrage_decoder dec;
    dec.parse(dec_str.c_str());

    user_enter_room_msg msg;
    msg.room_group = dec.get_item_as_int("rid");
    msg.platform_group = dec.get_item_as_int("pg");
    msg.user_name = dec.get_item_as_string("nn");

    return msg;

}
