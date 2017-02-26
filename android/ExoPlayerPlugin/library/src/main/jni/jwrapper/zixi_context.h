//
// Created by roy on 21-Feb-16.
//

#ifndef PLAYERTESTMERGEIT_ZIXI_CONTEXT_H
#define PLAYERTESTMERGEIT_ZIXI_CONTEXT_H

#ifdef __cplusplus
class ZixiNativeContext {
public:
    ZixiNativeContext();
    ~ZixiNativeContext();
    int connect(const char * url, const char * device_id, const char * dec_key, unsigned int latency);
    int disconnect();
    int read(char * out_buffer, int offset, int read_size,int*);
    int getStatistics(long long * out_data);

    void onSourceUnplugged();
private:

    void * mZixiHandle;
    void * mLeftover;
    bool    mHaveSource;
    unsigned int mLeftoverSize;
    unsigned int mLeftoverRead;
    unsigned int mLeftoverFilled;

};
#endif //__cplusplus

#ifdef __cplusplus
extern "C"{
#endif

void *  createContext();
void    deleteContext(void* context);
int     connectContext(void* context, const char * url, const char * device_id, const char * dec_key, unsigned int latency);
int     disconnectContext(void * context);
int     readContext(void* context, char* out_buffer, int offset, int read_size, int*);
int     getContextStatistics(void * context, long long * data);
int     getStatisticsArraySize();
#ifdef __cplusplus
}
#endif
#endif //PLAYERTESTMERGEIT_ZIXI_CONTEXT_H
