cmake_minimum_required(VERSION 3.10.2)
#设置生成的so动态库最后输出的路径
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})
add_library( # 库名
             sample

             # 该库生成动态函数
             SHARED

             # 依赖的文件
        src/main/cpp/samplelib.cpp
        src/main/cpp/MP3Encoder.h
        src/main/cpp/MP3Encoder.cpp)


find_library( # 设置Path变量的名称
              log-lib

              # 指定要查询的库名字
              # 在ndk中查找 liblog.so 函数库
              log )


target_link_libraries( # 目标库
                       sample

                       # 要链接的库
                       android
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )