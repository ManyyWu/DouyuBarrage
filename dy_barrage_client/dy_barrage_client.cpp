//
// Created by openkt on 2018/8/5.
//
#include "dy_barrage_client.h"
#include "dy_msg_codec.h"
#include "def.h"
#include <string>
#include <vector>
#include <thread>

extern "C" {
#ifdef _MSC_VER
#include "transcoding.h"
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
};

queue_msg::queue_msg ()
{
}

queue_msg::queue_msg (const char *msg_str, int color, int msg_type)
{
    this->msg_str = msg_str;
    this->color = color;
    this->msg_type = msg_type;
}

int dy_barrage_client::start (const char *host, int port, int room_id, int group_id)
{
    m_to_stop = false;
    m_is_stopped = false;
    m_room_id = room_id;
    m_group_id = group_id;

    int ret;

    // connect to douyu barrage server
    ret = connect_dy_server(host, port);
    if (ret < 0)
    {
        queue_msg err("Failed to conect to server!", 0, MSG_TYPE_ERR);
        m_barr_queue.push(err);
        release_res();
        return -1;
    }

    queue_msg err("Conect to server sucessfuly!", 0, MSG_TYPE_MSG);
    m_barr_queue.push(err);

    // login douyu server
    ret = login_dy_room();
    if (ret < 0)
    {
        queue_msg err("Failed to login room!", 0, MSG_TYPE_ERR);
        m_barr_queue.push(err);
        release_res();
        return -1;
    }

    // join room group
    ret = join_dy_room_group();
    if (ret < 0)
    {
        queue_msg err("Failed to join barrage group!", 0, MSG_TYPE_ERR);
        m_barr_queue.push(err);
        release_res();
        return -1;
    }

    // create a thread to keep alive
    std::thread thr1(thr_keep_alive, this);
    thr1.detach();

    // create a thread to get dy barrage
    std::thread thr2(thr_get_dy_msg, this);
    thr2.detach();

    return 0;
}

void dy_barrage_client::stop ()
{
    m_to_stop = true;
}

bool dy_barrage_client::is_stopped ()
{
    return m_is_stopped;
}

k::queue<queue_msg> &dy_barrage_client::get_barr_queue ()
{
    return m_barr_queue;
}

void dy_barrage_client::thr_keep_alive (void *args)
{
    dy_barrage_client *dy_bclient = (dy_barrage_client *) args;

    while (!dy_bclient->m_to_stop)
    {
        std::string data = dy_msg_encoder::create_keep_alive_req();

        int ret = send(dy_bclient->m_server_sock_fd, data.data(), data.size(), 0);
        if (ret < 0)
        {
            queue_msg err("Send alive packet error!", 0, MSG_TYPE_ERR);
            dy_bclient->m_barr_queue.push(err);
            dy_bclient->m_to_stop = true;
            break;
        }

        int i = 0;
        while (i <= 400 && !dy_bclient->m_to_stop)
        { // keep live once per 45 seconds
#ifdef _MSC_VER
            Sleep (100);     // 100ms
#else
            usleep(100000); // 100ms
#endif
            i++;
        }
    }
}

void dy_barrage_client::thr_get_dy_msg (void *args)
{
    dy_barrage_client *dy_bclient = (dy_barrage_client *) args;

    while (!dy_bclient->m_to_stop)
    {
        int ret = dy_bclient->get_dy_server_msg();
        if (ret == -1)
        {
            queue_msg err("Get barrage error!", 0, MSG_TYPE_ERR);
            ((dy_barrage_client *) args)->m_barr_queue.push(err);
            dy_bclient->m_to_stop = true;
            break;
        }
    }

    // release resources
    dy_bclient->release_res();
}

int dy_barrage_client::connect_dy_server (const char *name, int port)
{
    struct hostent *host;

#ifdef _MSC_VER
    unsigned long lgIP = inet_addr (name);
    //输入的IP字符串
    if (lgIP != INADDR_NONE)
    {
        WSACleanup ();
        return -1;
    }
    WSADATA wsaData;
    WSAStartup (MAKEWORD (1, 1), &wsaData);
#endif

    host = gethostbyname(name);
    if (!host)
        return -1;

    m_server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_sock_fd < 0)
        return -1;

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons (port);
    serv_addr.sin_addr = *((struct in_addr *) host->h_addr);
    memset(serv_addr.sin_zero, 0, 8);

    int con_ret = connect(m_server_sock_fd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
    if (con_ret < 0)
        return -1;

    return 0;
}

int dy_barrage_client::login_dy_room ()
{
    int ret;
    dy_barrage login_req;
    std::string data = dy_msg_encoder::create_login_req();

    ret = send(m_server_sock_fd, data.data(), data.size(), 0);
    if (ret < 0)
        return -1;

    // receive login response
    ret = get_dy_server_msg();
    if (ret < 0)
        return -1;

    return 0;
}

int dy_barrage_client::join_dy_room_group ()
{
    int ret = 0;

    std::string join_group_req = dy_msg_encoder::create_join_group_req(m_room_id, m_group_id);
    ret = send(m_server_sock_fd, join_group_req.data(), join_group_req.size(), 0);
    if (ret < 0)
        return -1;

    return 0;
}

int dy_barrage_client::get_dy_server_msg ()
{
    int recv_bytes;

    int packet_length = 0;
    // get the packet length 4 bytes
    recv_bytes = recv(m_server_sock_fd, reinterpret_cast<char *>(&packet_length), 4, 0);
    if (recv_bytes < 0)
        return -1;

    if (packet_length <= 8)
        return -2;

    // get the packet body packet_length bytes
    char buf[MAX_DATA_SIZE] = {0};
    recv_bytes = recv(m_server_sock_fd, buf, packet_length, 0);
    if (recv_bytes < 0)
    {
        return -1;
    }
    else
    {
        // you should better paste the packet as douyu packet header length, if not something unpredictable may happen
        std::string pack(buf, recv_bytes);
        int msg_type = dy_msg_decoder::get_msg_type(pack);

        switch (msg_type)
        {
        case DY_MSG_TYPE_LOGIN_RESPONSE:
            return on_login_response(pack);
        case DY_MSG_TYPE_CHATMSG:
            on_barrage(pack);
            break;
        case DY_MSG_TYPE_LIVE_STAT:
            on_live_stat_changed(pack);
            break;
        case DY_MSG_TYPE_USER_ENTER:
            on_user_enter_room(pack);
            break;
        default:
            break;
        }
    }

    return 0;
}

void dy_barrage_client::on_barrage (std::string &pack)
{
    dy_barrage barr = dy_msg_decoder::parse_barrage(pack);

    // 将符合要求的弹幕push至队列
    if (// mask_check(barr)                    // 关键字屏蔽
        // vip_check(barr)                     // vip检查
            barr.badge_room_id == m_room_id        // 佩戴本房间徽章
            || barr.barr_color > 0                 // 彩色弹幕
            || barr.is_fans_barr > 0               // 粉丝弹幕
            || barr.is_noble > 0                   // 贵族弹幕
            || barr.room_group > 1                 // 房管弹幕
            || barr.platform_group > 1)            // 超管弹幕
    {
        queue_msg msg;
#ifdef _MSC_VER
        if (barr.room_group > 1) { // 房管弹幕
            msg.color = barr.barr_color;
            msg.msg_str = "[房管] "
                     + transcoding::UTF8_to_GBK (barr.sender_name) // 用户名
                     + ": "
                     + transcoding::UTF8_to_GBK (barr.content); // 内容
        } else if (barr.platform_group > 1) { // 超管弹幕
            msg.color = barr.barr_color;
            msg.msg_str = "[超管] "
                     + transcoding::UTF8_to_GBK (barr.sender_name) // 用户名
                     + ": "
                     + transcoding::UTF8_to_GBK (barr.content); // 内容
        } else {
            msg.color = barr.barr_color;
            msg.msg_str = transcoding::UTF8_to_GBK (barr.sender_name) // 用户名
                     + ": "
                     + transcoding::UTF8_to_GBK (barr.content); // 内容
        }
#else
        if (barr.room_group > 1)
        { // 房管弹幕
            //barr.color = BARR_COLOR_ROOM_ADMIN;
            msg.color = barr.barr_color;
            msg.msg_str = "[房管] "
                          + barr.sender_name // 用户名
                          + ": "
                          + barr.content; // 内容
        }
        else if (barr.platform_group > 1)
        { // 超管弹幕
            msg.color = barr.barr_color;
            msg.msg_str = "[超管] "
                          + barr.sender_name // 用户名
                          + ": "
                          + barr.content; // 内容
        }
        else
        {
            msg.color = barr.barr_color;
            msg.msg_str = barr.sender_name // 用户名
                          + ": "
                          + barr.content; // 内容
        }
#endif
        m_barr_queue.push(msg);
    }
}

int dy_barrage_client::on_login_response (std::string &pack)
{
    int ret = dy_msg_decoder::get_msg_type(pack);
    if (DY_MSG_TYPE_LOGIN_RESPONSE != ret)
        return -1;

    return 0;
}

void dy_barrage_client::on_live_stat_changed (std::string &pack)
{
    int stat = dy_msg_decoder::parse_live_stat(pack);
    if (stat) // living
    {
        queue_msg msg("Live is started", 0, MSG_TYPE_LIVE_BEGIN);
        m_barr_queue.push(msg);
    }
    else     // ended
    {
        queue_msg msg("Live is ended", 0, MSG_TYPE_LIVE_END);
        m_barr_queue.push(msg);
    }
}

void dy_barrage_client::on_user_enter_room (std::string &pack)
{
    user_enter_room_msg enter_msg = dy_msg_decoder::parse_user_enter_room_msg(pack);

    queue_msg msg;
    msg.msg_type = MSG_TYPE_USER_ENTER;
    if (enter_msg.room_group > 1)
    { // 房管弹幕
        //barr.color = BARR_COLOR_ROOM_ADMIN;
        msg.color = BARR_COLOR_BLUE;
        msg.msg_str = "[房管] "
                      + enter_msg.user_name // 用户名
                      + " 进入直播间";
    }
    else if (enter_msg.platform_group > 1)
    { // 超管弹幕
        msg.color = BARR_COLOR_RED;
        msg.msg_str = "[超管] "
                      + enter_msg.user_name // 用户名
                      + " 进入直播间";
    }
    else
    {
        msg.color = BARR_COLOR_WHITE;
        msg.msg_str = enter_msg.user_name // 用户名
                      + " 进入直播间";
    }

    m_barr_queue.push(msg);
}

void dy_barrage_client::release_res ()
{
    m_to_stop = true;

#ifdef _MSC_VER
    if (m_server_sock_fd != -1)
        closesocket(m_server_sock_fd);
    WSACleanup();
#else
    if (m_server_sock_fd != -1)
        close(m_server_sock_fd);
#endif
    m_server_sock_fd = -1;

    queue_msg msg("Connection is closed!", 0, MSG_TYPE_MSG);
    m_barr_queue.push(msg);

    m_is_stopped = true;
}