//
// Created by openkt on 2018/8/5.
//

#ifndef DY_BARRAGE_DY_BARRAGE_CLIENT_H
#define DY_BARRAGE_DY_BARRAGE_CLIENT_H

#include "kqueue.h"
#include "def.h"
#include "dy_msg_codec.h"
#include <string>

extern "C" {
#ifdef _MSC_VER
#include <process.h>
#else
#include <pthread.h>
#endif
};

#define MAX_DATA_SIZE 40960   //maximum length(bytes) of each reception

struct queue_msg {
    std::string msg_str;  // message string

    int color;            // barrage color, color is set only when the message is a barrage

    int msg_type;         // message type

    queue_msg ();

    queue_msg (const char *msg_str, int color, int msg_type);
};

class dy_barrage_client {
private:
    int m_server_sock_fd; // server socket

    int m_room_id;        // room id

    int m_group_id;       // barrage group_id;

private:
    bool m_is_stopped;

    bool m_to_stop;

#ifdef _MSC_VER
    uintptr_t m_pthr_keep_alive;

    uintptr_t m_pthr_get_dy_msg;
#else
    pthread_t m_thr_keep_alive;
    pthread_t m_thr_get_dy_msg;
#endif

    k::queue<queue_msg> m_barr_queue; // barrage queue

public:
    int start (const char *host, int port, int room_id, int group_id = -9999);

    void stop ();

    bool is_stopped ();

    k::queue<queue_msg> & get_barr_queue ();

private:
    static void thr_keep_alive (void *args);

    static void thr_get_dy_msg (void *args);

private:
    // connect to douyu barrage server
    int connect_dy_server (const char *host, int port);

    // login douyu live room
    int login_dy_room ();

    // join room group
    int join_dy_room_group ();

    // get douyu server message
    int get_dy_server_msg ();

private:
    // do something when receive a barrage
    void on_barrage (std::string &pack);

    // do something when receive a login response
    int on_login_response (std::string &pack);

    // do something when receive a login response
    void on_live_stat_changed (std::string &pack);

    // do something when one person enter into the room
    void on_user_enter_room (std::string &pack);

    // do something when one person leave the room
    void on_user_leave_room (std::string &pack);

private:
    // release resources
    void release_res ();
};


#endif //DY_BARRAGE_DY_BARRAGE_CLIENT_H
