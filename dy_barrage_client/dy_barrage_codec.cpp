//
// Created by openkt on 2018/8/5.
//

#include "dy_barrage_codec.h"
#include <string>
#include <vector>

dy_barrage_encoder::dy_barrage_encoder ()
{
}

dy_barrage_encoder::~dy_barrage_encoder ()
{
}

std::string dy_barrage_encoder::get_result ()
{
    return buf;
}

void dy_barrage_encoder::add_item (const char *key, const char *value)
{
    while (*key != '\0')
    {
        if (*key == '/')
        {
            buf.append ("@S", 2);
        }
        else if (*key == '@')
        {
            buf.append ("@A", 2);
        }
        else
        {
            buf += *key;
        }

        key++;
    }

    buf.append ("@=", 2);

    while (*value != '\0')
    {
        if (*value == '/')
        {
            buf.append ("@S", 2);
        }
        else if (*value == '@')
        {
            buf.append ("@A", 2);
        }
        else
        {
            buf += *value;
        }

        value++;
    }

    buf.append ("/", 1);
}

void dy_barrage_encoder::add_item (const char *key, const int value)
{
    char str[64] = { 0 };
    snprintf (str, sizeof (str), "%d", value);
    add_item (key, static_cast<const char *>(str));
}

dy_barrage_decoder::dy_barrage_decoder ()
{
}

dy_barrage_decoder::~dy_barrage_decoder ()
{
}

void dy_barrage_decoder::parse (const char * data)
{
    arr.clear ();

    if (*data == '\0')
    {
        return;
    }

    item kv;
    std::string buf;

    while (*data != '\0')
    {
        if (*data == '/')    //end char
        {

            kv.value = buf;
            buf.clear ();

            arr.push_back (kv);

            kv.key.clear ();
            kv.value.clear ();

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
                buf.clear ();
            }
        }
        else
        {
            buf += *data;
        }

        data++;
    }

    if (*data == '\0' && *(data - 1) != '/') //末尾漏掉斜线/的情况
    {
        kv.value = buf;
        buf.clear ();

        arr.push_back (kv);
    }
}

std::string dy_barrage_decoder::get_item_as_string (const char *key)
{
    std::string value;

    for (unsigned i = 0; i < arr.size (); i++)
    {
        if (arr[i].key == key)
        {
            value = arr[i].value;
            break;
        }
    }

    return value;
}

int dy_barrage_decoder::get_item_as_int (const char *key)
{
    int value = 0;

    for (unsigned i = 0; i < arr.size(); i++)
    {
        if (arr[i].key == key)
        {
            value = atoi (arr[i].value.c_str());
            break;
        }
    }

    return value;
}