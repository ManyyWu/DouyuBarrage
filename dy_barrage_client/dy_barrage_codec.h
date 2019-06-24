#pragma once
//
// Created by openkt on 2018/8/5.
//

#ifndef DY_BARRAGE_DY_BARRAGE_CODEC_H
#define DY_BARRAGE_DY_BARRAGE_CODEC_H

#include <string>
#include <vector>

// 键
typedef struct item
{
    std::string key;
    std::string value;
}item;

typedef std::vector<item> item_array;

/* 斗鱼弹幕编码器 */
class dy_barrage_encoder
{
private:
    std::string buf;

public:
    dy_barrage_encoder ();
    ~dy_barrage_encoder ();

    /* 获取编码后的字节串 */
    std::string get_result ();

    /* 向缓冲区添加元素 */
    void add_item (const char *key, const char *value);
    void add_item (const char *key, const int value);
};

/* 斗鱼弹幕解码器 */
class dy_barrage_decoder
{
private:
    item_array arr;

public:
    dy_barrage_decoder ();
    ~dy_barrage_decoder ();

    /* 解析弹幕 */
    void parse (const char * data);

    /* 从解析后的弹幕中获取元素值 */
    std::string get_item_as_string (const char *key);
    int get_item_as_int (const char *key);
};

#endif //DY_BARRAGE_DY_BARRAGE_CODEC_H
