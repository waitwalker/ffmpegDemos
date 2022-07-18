//
//  MP3Encoder.cpp
//  Runner
//
//  Created by waitwalker on 2022/7/14.
//

#include "MP3Encoder.hpp"
extern "C" {
#include <libavutil/avutil.h>
}

#pragma mark - 公有方法
MP3Encoder::MP3Encoder() {
    
}

MP3Encoder::~MP3Encoder() {
    
}

void MP3Encoder::encode() {
    unsigned version = avutil_version();
    printf("版本：%d\n", version);
    printf("开始编码\n");
    
    
}


