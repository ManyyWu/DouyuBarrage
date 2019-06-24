#pragma once

#ifndef _TRANSCODING_H_
#define _TRANSCODING_H_

#include <string>

class transcoding
{
public:
    static std::string UTF8_to_GBK (const std::string & str);
    static std::string GBK_to_UTF8 (const std::string & str);
};

#endif /* _TRANSCODING_H_ */