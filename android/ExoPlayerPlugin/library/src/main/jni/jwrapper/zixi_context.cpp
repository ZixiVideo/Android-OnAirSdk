//
// Created by roy on 21-Feb-16.
//

#include "zixi_context.h"

//#include "../../core/common/version.h"
//#include "../../core/common/logging.h"
#include "../../zixi/include/zixi_definitions.h"
#include "../../zixi/include/zixi_client_interface.h"
#include <algorithm>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>


#define STATISTICS_ARRAY_SIZE 24

long long currentTimeInMilliseconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

extern "C" {
void *  createContext(){
    return (void *) new ZixiNativeContext;
}
void    deleteContext(void* context){
    delete ((ZixiNativeContext*)context);
}
int     connectContext(void* context,const char * url, const char * device_id, const char * dec_key, unsigned int latency){
    return ((ZixiNativeContext*)context)->connect(url,device_id, dec_key, latency);
}
int     disconnectContext(void * context){
    return ((ZixiNativeContext*)context)->disconnect();
}
int     readContext(void* context, char* out_buffer, int offset, int read_size, int* out_read){
   return ((ZixiNativeContext*)context)->read(out_buffer,offset,read_size,out_read);
}
int     getContextStatistics(void * context, long long * data) {
    return ((ZixiNativeContext*)context)->getStatistics(data);
}
int     getStatisticsArraySize() {
    return STATISTICS_ARRAY_SIZE;
}
}

void new_stream_handler(void * zixi_handle, ZIXI_STREAM_INFO info, void * zixi_connection_handle){
    if (!info.valid_info) {
        // source dc
        ((ZixiNativeContext*)zixi_connection_handle)->onSourceUnplugged();
    }
}

void status_changed_handler(void * zixi_handle, ZIXI_STATUS status, void * zixi_connection_handle) {
    /*L_(L_ERROR) << "status_changed_handler " << (int)status;*/
}

ZixiNativeContext::ZixiNativeContext(){
    mZixiHandle = 0;
}

ZixiNativeContext::~ZixiNativeContext() {
}

void ZixiNativeContext::onSourceUnplugged() {
    mHaveSource = false;
}
int ZixiNativeContext::connect(const char * url, const char * device_id, const char * dec_key, unsigned int latency) {
    int ret = ZIXI_ERROR_ALREADY_INITIALIZED;
    if (!mZixiHandle) {
        /*L_(L_ERROR) << "ZixiNativeContext::connect zixi_init()";*/
        ret = zixi_init();
        if (ret == ZIXI_ERROR_OK) {
          /*  L_(L_ERROR) << "ZixiNativeContext::connect zixi_init_connection";*/
            ret = zixi_init_connection_handle(&mZixiHandle);
            if (ret == ZIXI_ERROR_OK) {
                /*L_(L_ERROR) << "ZixiNativeContext::connect zixi_configure_id";*/
                ret = zixi_configure_id(mZixiHandle, (char*)device_id,"");
                if (ret == ZIXI_ERROR_OK) {
                    /*L_(L_ERROR) << "ZixiNativeContext::connect zixi_configure_error_correction";*/
                    ret = zixi_configure_error_correction(mZixiHandle, latency, ZIXI_LATENCY_STATIC,ZIXI_FEC_OFF,30,50,false,false,0);
                    if (ret == ZIXI_ERROR_OK) {
                        ZIXI_CALLBACKS callbacks = {0};
                        mHaveSource = true;
                        callbacks.zixi_new_stream = new_stream_handler;
                        callbacks.zixi_status_changed = status_changed_handler;
                        callbacks.user_data = (void*)this;
                        /*L_(L_ERROR) << "ZixiNativeContext::connect zixi_connect_url";*/
                        ret = zixi_connect_url(mZixiHandle, (char*)url, true, callbacks);
                        /*L_(L_ERROR) << "ZixiNativeContext::connect zixi_connect_url returned " << ret;*/
                        if (ret != ZIXI_ERROR_OK) {
                            /*L_(L_ERROR) << "ZixiNativeContext::connect -> failed to connect";*/
                            zixi_delete_connection_handle(mZixiHandle);
                            mZixiHandle = NULL;
                            zixi_destroy();
                        } else {
                            /*L_(L_ERROR) << "ZixiNativeContext::connect -> connected";*/
                        }
                    }
                }
            }

        }
    }
    return ret;
}

int ZixiNativeContext::disconnect() {
    if (mZixiHandle) {
        /*L_(L_ERROR) << "ZixiNativeContext::disconnect";*/
        zixi_disconnect(mZixiHandle);
        zixi_delete_connection_handle(mZixiHandle);
        /*L_(L_ERROR) << "ZixiNativeContext::done";*/
        mZixiHandle = NULL;
        zixi_destroy();
    }
    return 0;
}

long long ts_mark = 0;
void plotStats(void * p) {
    ZIXI_CONNECTION_STATS conn_stats = {0};
    ZIXI_NETWORK_STATS net_stats = {0};
    ZIXI_ERROR_CORRECTION_STATS ecs = {0};
    if (zixi_query_statistics(p,&conn_stats, &net_stats, &ecs) == 0) {
        /*L_(L_ERROR) << "zixi_query_statistics -> bytes : " << net_stats.bytes << " dropped: " << net_stats.dropped << " latency: " << net_stats.latency <<
        " rtt: " << net_stats.rtt << " jitter: " << net_stats.jitter;*/
    }
}

int ZixiNativeContext::read(char * out_buffer, int offset, int read_size, int * out_read) {
    unsigned int read_now;
    bool eof,disc;
    int orig_read_size = read_size;
    int orig_offset = offset;
    if (out_read) {
        *out_read = 0;
    }
    if (currentTimeInMilliseconds() - ts_mark > 1000) {
      //  plotStats(mZixiHandle);
        ts_mark = currentTimeInMilliseconds();
    }

    int zixi_ret = 0;//zixi_read(mZixiHandle, out_buffer + offset, read_size, &read_now,&eof,&disc,true);

    //if (out_read) {
    //   *out_read += read_now;
    //}


    while ((read_size > 0 && (zixi_ret == 0) || (zixi_ret ==5)) && (mHaveSource)) {
        zixi_ret = zixi_read(mZixiHandle, out_buffer + offset, read_size, &read_now,&eof,&disc,false);
        if (zixi_ret == 0) {
            if (out_read) {
                *out_read += read_now;
            }
            read_size -= read_now;
            offset+= read_now;
        } else if (zixi_ret == 5) {
            usleep(10);
        } else {
            /*L_(L_ERROR) << "zixi_read returned " << zixi_ret;*/
        }
    }

    if (!mHaveSource) {
        zixi_ret = -23;
    }
    return zixi_ret;
}

int ZixiNativeContext::getStatistics(long long * out_data){
    ZIXI_NETWORK_STATS net_stats = {0};
    ZIXI_CONNECTION_STATS conn_stats = {0};
    ZIXI_ERROR_CORRECTION_STATS ecs = {0};
    int zixi_ret = zixi_query_statistics(mZixiHandle,&conn_stats , &net_stats, &ecs);
    if (zixi_ret == ZIXI_ERROR_OK) {
        out_data[0] = net_stats.packets;
        out_data[1] = net_stats.bytes;
        out_data[2] = net_stats.out_of_order;
        out_data[3] = net_stats.dropped;
        out_data[4] = net_stats.duplicates;
        out_data[5] = net_stats.overflow;
        out_data[6] = net_stats.bit_rate;
        out_data[7] = net_stats.packet_rate;
        out_data[8] = net_stats.jitter;
        out_data[9] = net_stats.rtt;
        out_data[10] = net_stats.latency;
        out_data[11] = net_stats.available_bitrate;
        out_data[12] = net_stats.congested;

        out_data[13] = ecs.arq_packets;
        out_data[14] = ecs.fec_packets;
        out_data[15] = ecs.arq_recovered;
        out_data[16] = ecs.fec_recovered;
        out_data[17] = ecs.not_recovered;
        out_data[18] = ecs.duplicates;
        out_data[19] = ecs.requests;
        out_data[20] = ecs.overflow;
        out_data[21] = ecs.fec_bit_rate;
        out_data[22] = ecs.fec_packet_rate;
        out_data[23] = ecs.nulls_stuffed;
    }
    return zixi_ret;
}