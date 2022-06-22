#include "ffmpegs.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret,errbuf,sizeof(errbuf));

FFmpegs::FFmpegs()
{

}

void FFmpegs::aacEncode(AudioEncodeSpec &in, const char *outFilename) {

}
