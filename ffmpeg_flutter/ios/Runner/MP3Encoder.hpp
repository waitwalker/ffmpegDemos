//
//  MP3Encoder.hpp
//  Runner
//
//  Created by waitwalker on 2022/7/14.
//

#ifndef MP3Encoder_hpp
#define MP3Encoder_hpp

#include <stdio.h>

#pragma mark - MP3编码类
class MP3Encoder {
public:
    MP3Encoder();
    ~MP3Encoder();
    // 编码
    void encode();
};

#endif /* MP3Encoder_hpp */
