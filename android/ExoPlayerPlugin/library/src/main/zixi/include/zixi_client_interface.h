#pragma once

#include "zixi_definitions.h"
#ifdef WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
  #if __GNUC__ >= 4
    #define DLL_EXPORT __attribute__ ((visibility ("default")))
  #else
    #define DLL_EXPORT
  #endif
#endif



#ifdef __cplusplus
extern "C"
{
#endif

/**
@mainpage  ZIXI Client Interface
@section	API for using ZIXI client interface.
*/


//--------------------------------------------------------
//			Initialization functions					
//--------------------------------------------------------

/**
@brief      initializes zixi environment.

@return		ZIXI_ERROR_OK
@return		ZIXI_ERROR_ALREADY_INITIALIZED
@return		ZIXI_ERROR_DLL_NOT_FOUND
*/
int DLL_EXPORT zixi_init(void);

/**
@brief		cleans up zixi environment.

@return		ZIXI_ERROR_OK
@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
*/
int DLL_EXPORT zixi_destroy(void);

/**
@brief		initialize a handle for zixi connection.

@param		handle[out] - returned handle upon success, destroy with zixi_delete_connection_handle function.

@return      ZIXI_ERROR_OK
@return		 ZIXI_ERROR_INVALID_PARAMETER

*/
int DLL_EXPORT zixi_init_connection_handle(void **handle);

/**
@brief		 destroys zixi connection handle.

@param		 handle - a handle to zixi connection returned by zixi_init_connection_handle

@return      ZIXI_ERROR_NOT_INITIALIZED
@return		 ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		 ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_delete_connection_handle(void *handle);


//--------------------------------------------------------
//			Configuration functions						
//--------------------------------------------------------


/**
@brief		reconfigure logging to new level or to different function.

@param		log_level	  - log detail level, 
						    [-1] to turn off , 	
						    [0] to log everything (significantly hurt performance - only for deep debugging) , 
						    [1-5] different log levels (3 recommended)
@param		log_func	  - logging function
@param		log_user_data - user data that will be returned as the first parameter in log_func

@return     ZIXI_ERROR_DLL_NOT_FOUND
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_client_configure_logging(int log_level, ZIXI_LOG_FUNC log_func, void* log_user_data);


/**
@brief		configure client id and session id.
			
			client id must be unique on a per broadcaster level.
			session id is used for authorization.

Warnings:	 Call zixi_configure_id(..) before the first zixi_connect(..)/zixi_async_connect(..)

@param		handle   - a handle to zixi connection returned by zixi_init_connection_handle
@param		guid	 - a unique identifier of the current client
@param		session	 - a session identifier that will be used for authorization

@return     ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_id(void *handle, const char guid[ZIXI_GUID_LENGTH], const char session[ZIXI_SESSION_LENGTH]);

/**
@brief		Configure a list of possible server side ports.
			 
			the client will test these ports to see which are open.
			the results are cached until zixi_destroy(..) is called.

@param		handle       - a handle to zixi connection returned by zixi_init_connection_handle
@param		ports		 - a vector of server side ports
@param		ports_number - size of the ports vector

@return     ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_ports(void *handle, unsigned short* ports, int ports_number);

/**
@brief		configure the cache that will be used when in progressive-download (PDL) mode.

Warnings: Cache size has a minimal value of several seconds of video. 
		  The minimal size is determined by the library on stream connection,
		  and depends on the individual stream and connection parameters.
		  If the provided cache size is too small, PDL mode will fail.
			 
		  You can not provide custom buffer and cache_path at the same time. 
		  At least one of the parameters must be NULL.

@param		handle        - a handle to zixi connection returned by zixi_init_connection_handle
@param		cache_size	  - the size of the cache to be used in progressive-download (PDL) mode, in bytes.
@param		cache_path	  - a path to a cache file, set to NULL to use in memory cache.
@param		custom_buffer - a pointer to preallocated buffer to be used for the cache 
							(must be at least cache_size bytes). 
							Set to NULL for internal allocation of the memory.

@return     ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_INVALID_PARAMETER
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_pdl(void *handle, int cache_size, const char* cache_path, unsigned char *custom_buffer);


/**

@brief		 Configure error correction parameters, doesn't affect connection time.
			 
@param		 handle				- a handle to zixi connection returned by zixi_init_connection_handle
@param		 max_latency		- maximum latency, higher values allow more time for error correction.
@param		 latency_mode		- select latency mode, static/increasing recomended for players, dynamic for conversion to tcp protocols
@param		 fec_mode			- one of the ZIXI_FEC_MODE enum values
@param		 fec_overhead	    - % of overhead over the original stream.
@param		 fec_block_ms	    - fec block size in milli seconds  .
@param		 fec_content_aware	- true=enable content aware / false=disable content aware 
@param		 stuff_null_on_unrecovered - true=return a null packet when a packet cannot be recovered
@param		 max_output_jitter	- cap error correction jitter in milliseconds (recommended 0 to disable)

@return      ZIXI_ERROR_NOT_INITIALIZED
@return		 ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		 ZIXI_ERROR_INVALID_PARAMETER
@return		 ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_error_correction(void *handle, unsigned int max_latency, ZIXI_LATENCY_MODE latency_mode, ZIXI_FEC_MODE fec_mode, unsigned int fec_overhead, unsigned int fec_block_ms, bool fec_content_aware, bool stuff_null_on_unrecovered, unsigned int max_output_jitter);


/**
@brief		 Configure decryption parameters for an encrypted stream.
		    
			 If the stream is not encrypted this will have no effect.

@param		 handle - a handle to zixi connection returned by zixi_init_connection_handle
@param		 type   - encryption type
@param		 key	   - decryption key

@return      ZIXI_ERROR_NOT_INITIALIZED
@return  	 ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		 ZIXI_ERROR_INVALID_PARAMETER
@return		 ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_decryption(void *handle, ZIXI_ENCRYPTION type, const char* key);

/**
@brief		connect to a URL in 'zixi://' format

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle
@param		 url				- URL in zixi:// format
@param		 low_latency		- enable low_latency mode at the cost of connection time
@param		 callbacks		- callback functions to handle different events

URL format:
zixi://[user[:session]@]server[:port]/channel

URL format for adaptive bitrate:
zixi://[user[:session]@]server[:port]/channel0?kbps=XXXX&alt=channel1,YYYY[&alt=channel2,ZZZZZ]....

for eg:
zixi://myserver.com/news
zixi://myserver.com:2077/news
zixi://guest42@myserver.com:2077/news
zixi://guest42:hashashash@myserver.com:2077/news
zixi://my@email.com@myserver.com:2077/news
zixi://my@email.com:mypassword@myserver.com:2077/news
zixi://guest42:hashashash@myserver.com:2077/news-hd?kbps=3000&alt=news-sd,1000&alt=news-low,500

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_ALREADY_CONNECTED
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_connect_url(void *handle, const char* url, bool low_latency, ZIXI_CALLBACKS callbacks);

/**
@brief		connect to a URLs in 'zixi://' format


@param		handle - a handle to zixi connection returned by zixi_init_connection_handle
@param		urls				- vector of URL's in zixi:// format
@param		num_urls         - number of URL'S
@param		low_latency		- enable low_latency mode at the cost of connection time
@param		callbacks		- callback functions to handle different events

URL format:
zixi://[user[:session]@]server[:port]/channel

URL format for adaptive bitrate:
zixi://[user[:session]@]server[:port]/channel0?kbps=XXXX&alt=channel1,YYYY[&alt=channel2,ZZZZZ]....

for eg:
zixi://myserver.com/news
zixi://myserver.com:2077/news
zixi://guest42@myserver.com:2077/news
zixi://guest42:hashashash@myserver.com:2077/news
zixi://my@email.com@myserver.com:2077/news
zixi://my@email.com:mypassword@myserver.com:2077/news
zixi://guest42:hashashash@myserver.com:2077/news-hd?kbps=3000&alt=news-sd,1000&alt=news-low,500

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_ALREADY_CONNECTED
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/ 
int DLL_EXPORT zixi_connect_urls(void *handle, char** url ,  int num_urrls, bool low_latency, ZIXI_CALLBACKS callbacks);

/**
@brief		doaload a URL in 'zixi://' format

@param		handle		- a handle to zixi connection returned by zixi_init_connection_handle
@param		url			- URL in zixi:// format. see zixi_connect_url
@param		offset		- start dowload file from this offset, in bytes
@param		callbacks	- callback functions to handle different events

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_ALREADY_CONNECTED
@return		ZIXI_ERROR_FAILED
@return	    ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_download_url(void *handle, const char* url, unsigned long long bitrate, unsigned long long offset, ZIXI_CALLBACKS callbacks);

/**
@brief		Configure the all the main and alternative streams showing a given channel

@param		handle			- a handle to zixi connection returned by zixi_init_connection_handle
@param		streams			- a vector of the stream id's
@param		bitrates		- a vector of bitrates corresponding to the given streams
@param		count			- the number of streams
@param		auto_migrate	- adaptive streaming - allow automatic switching between bitrates

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_ALREADY_CONNECTED
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_channel(void *handle, char** streams, int* bitrates, int count, bool auto_migrate);

/**
@brief		replace the internal socket communication with external read/write functions

@param		handle			- a handle to zixi connection returned by zixi_init_connection_handle
@param		read_function   - ptr for read function.
@param      read_param      - ptr for read parameters.
@param      write_function  - ptr for write function.
@param      write_param     - ptr for write parameters.

@return     ZIXI_ERROR_NOT_INITIALIZED
@return     ZIXI_ERROR_INVALID_PARAMETER
@return     ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_custom_io(void *handle, ZIXI_IO_FUNCTION read_function, void* read_param, ZIXI_IO_FUNCTION write_function, void* write_param);


/**
@brief		configure local network interface and port
			
			call before zixi_connect, zixi_async_connect, zixi_accept, zixi_connect_urs, zixi_connect_urls, zixi_download_url or zixi_download_urls

@param		handle		- a handle to zixi connection returned by zixi_init_connection_handle
@param		local_nic	- null terminated string of the local IP address to use. use null or "0.0.0.0" for any local IP
@param		local_port	- local port number. use 0 for automatic port selection

@return     ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_local_nic(void *handle, const char *local_nic, unsigned short local_port);


//--------------------------------------------------------
//			Connect/Disconnect functions
//--------------------------------------------------------

/**
@brief		Connect to a broadcaster , Returns immediately.
			
			ZIXI_CALLBACKS::zixi_status_changed will be called with connection status updates.

Warnings:	zixi_disconnect() must be called before calling this function a second time even if connection failed.

@param		handle		- a handle to zixi connection returned by zixi_init_connection_handle
@param		host		- hostname or ip of the broadcaster
@param		port		- public port of the broadcaster, set to 0 (zero) to use pre-configured ports
@param		index		- stream index from the vector of streams that was configured using zixi_configure_channel
@param		low_latency	- enable low_latency mode at the cost of connection time
@param		callbacks	- callback functions to handle different events

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_ALREADY_CONNECTED
@return		ZIXI_ERROR_TEST_FAILED
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_async_connect(void *handle, const char* host, unsigned short port, int index, bool low_latency, ZIXI_CALLBACKS callbacks);

/**
@brief			Connect to a broadcaster , Returns after connection succeeded/failed.

				ZIXI_CALLBACKS::zixi_status_changed will be called with connection status updates.

Warnings:	zixi_disconnect() must be called before calling this function a second time even if connection failed.

@param		handle		 - a handle to zixi connection returned by zixi_init_connection_handle
@param		host		 - hostname or ip of the broadcaster
@param		port		 - public port of the broadcaster, set to 0 (zero) to use pre-configured ports
@param		index		 - stream index from the vector of streams that was configured using zixi_configure_channel
@param		low_latency	 - enable low_latency mode at the cost of connection time
@param		callbacks	 - callback functions to handle different events

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_ALREADY_CONNECTED
@return		ZIXI_ERROR_RECONNECTING
@return		ZIXI_ERROR_TEST_FAILED
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_connect(void *handle, const char* host, unsigned short port, int index, bool low_latency, ZIXI_CALLBACKS callbacks);

/**
@brief			Wait for a connection from a feeder

				ZIXI_CALLBACKS::zixi_status_changed will be called with connection status updates.

Warnings:	zixi_disconnect() must be called before calling this function a second time even if connection failed.

@param		handle		 - a handle to zixi connection returned by zixi_init_connection_handle
@param		port		 - port on which the connection will be established
@param		callbacks	 - callback functions to handle different events

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_ALREADY_CONNECTED
@return		ZIXI_ERROR_RECONNECTING
@return		ZIXI_ERROR_TEST_FAILED
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_accept(void *handle, unsigned short port, ZIXI_CALLBACKS callbacks);

/**
@brief		disconnect from the current server

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_NOT_CONNECTED
@return	    ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_disconnect(void *handle);


/**
@brief			configure ts output stream

@param		handle		 - a handle to zixi connection returned by zixi_init_connection_handle
@param		host		 - hostname or ip of the broadcaster
@param		port		 - public port of the broadcaster, set to 0 (zero) to use pre-configured ports

@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_configure_ts_output_stream (void *handle, const char* host , unsigned short port);


//--------------------------------------------------------
//			Stream control functions
//--------------------------------------------------------

/**
@brief		Jump to alternative bitrate.

			when smooth = true the transition will not be noticeable in the video, but can take a few seconds
			when smooth = false, jump immediately

@param		handle			- a handle to zixi connection returned by zixi_init_connection_handle
@param		stream_index	- hostname or ip of the broadcaster
@param		smooth			- public port of the broadcaster, set to 0 (zero) to use pre-configured ports

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_INVALID_PARAMETER
@return		ZIXI_ERROR_FAILED
@return	    ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_manual_migrate(void *handle, int stream_index, bool smooth);

/**
@brief		Jump to alternative bitrate in new adaptive mode, only availabel in unicast mode

@param		handle			- a handle to zixi connection returned by zixi_init_connection_handle
@param		bitrate			- a bitrate from the available list (via zixi_query_adaptive_info) or 0 to re-enable automatic mode

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_INVALID_PARAMETER
@return		ZIXI_ERROR_FAILED
@return	    ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_adaptive_bitrate_switch(void *handle, int bitrate);

/**
@brief		pause the stream (only supported in VOD)

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle

@return		ZIXI_ERROR_NOT_INITIALIZED
@return 	ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_NOT_CONNECTED
@return		ZIXI_ERROR_OK
@return		ZIXI_ERROR_NOT_SUPPORTED
*/
int DLL_EXPORT zixi_stream_pause(void *handle);

/**
@brief		resume the stream (only supported in VOD)

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle

@return		ZIXI_ERROR_NOT_INITIALIZED
@return 	ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_NOT_CONNECTED
@return		ZIXI_ERROR_OK
@return		ZIXI_ERROR_NOT_SUPPORTED
*/
int DLL_EXPORT zixi_stream_resume(void *handle);

/**
@brief		seek to a different position in the stream , 
			any value between -1 and 100 is valid , 
			passing -1 will not actually seek, but will only test for this capability

@param		handle  - a handle to zixi connection returned by zixi_init_connection_handle
@param		percent	- the location to seek to, in percent of the total length

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_NOT_CONNECTED
@return		ZIXI_ERROR_OK
@return		ZIXI_ERROR_NOT_SUPPORTED
@return		ZIXI_ERROR_INVALID_PARAMETER
*/
int DLL_EXPORT zixi_stream_seek(void *handle, float percent);

/**
@brief		eek to a different position in the stream , 
		    any value between 0 and file size is valid	

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle
@param		bytes  - the location to seek to

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_NOT_CONNECTED
@return		ZIXI_ERROR_OK
@return 	ZIXI_ERROR_NOT_SUPPORTED
@return		ZIXI_ERROR_INVALID_PARAMETER
*/
int DLL_EXPORT zixi_stream_bytes_seek(void *handle, long long bytes);

/**
@brief		Read ready frames from zixi and fill the given buffer as much as possible

Warnings:	Next zixi_read functions will continue to read from the point in stream where 
			previous call stopped, but calls to zixi_peek_frame and zixi_get_frame 
			will not take that offset into account. 

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle
@param		buff			- preallocated buffer to hold the returning data
@param		size			- size of the preallocated buffer
@param		written_size	- the amount of bytes written to buffer
@param		eof				- flag that indicates last buffer for VOD stream. 
							  calls to 'zixi_read' past the end of file will contain 0 bytes of data with the flag set

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_NOT_CONNECTED
@return		ZIXI_ERROR_NOT_READY
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_read(void *handle, char *buff, unsigned int size, unsigned int *written_size, bool *eof, bool* discontinuity, bool full_frames);

/**
@brief		Read one ready frame (video or audio) from zixi mmt client 

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle(input)
@param		buff			- where to put a pointer to the buffer with returning data(output)
@param		written_size	- the amount of bytes written to buffer (output)
@param		discontinuity	- flag that indicates discontinuities whithin the current frame or between the last frame to the current(output)  
@param		info			- auxilary info to the frame(output)
@param		is_video		- flag to indicate if this frame is video frame (true) or audio frame (false) (output)
@param		pts				- presentation time of this frame in UTC clock (64khz clock) truncated to 32 bits(output)

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_NOT_CONNECTED
@return		ZIXI_ERROR_NOT_READY
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_mmt_read(void *handle, char **buff, unsigned int* written_size, bool* discontinuity, ZIXI_MEDIA_INFO* info, bool* is_video, unsigned long long* pts);

/**
@brief		Configure zixi mmt client 

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle(input)
@param		add_ADTS		- flag to indicate wheather the client has to add ADTS headers to the audio stream(input)
@param		reproduce_start_code	- flag to indicate wheather the client has to add NALU start codes to the video stram(input)

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_NOT_CONNECTED
@return		ZIXI_ERROR_NOT_READY
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_mmt_config(void *handle, bool add_ADTS, bool reproduce_start_code);


//--------------------------------------------------------
//			Stream status/statistics functions
//--------------------------------------------------------

/**
@brief		Returns the current stream info if available.

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle
@param		info   - pointer to a ZIXI_STREAM_INFO struct that will receive the current stream info	

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return	    ZIXI_ERROR_NOT_CONNECTED
@return	    ZIXI_ERROR_OK
@return	    ZIXI_ERROR_NOT_READY
*/
int DLL_EXPORT zixi_query_stream_info(void *handle, ZIXI_STREAM_INFO* info);

/**
@brief		Returns the list of availabe bitrates and current active bitrate if the stream is adaptive, will fail for single bitrate streams

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle
@param		bitrates - pointer to a preallocated vector of at least ZIXI_MAX_ADAPTIVE_STREAMS values
@param		size - pointer to an integer that will receive the number of actuall available bitrates
@param		current - pointer to an integer that will receive the current active stream

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return	    ZIXI_ERROR_NOT_CONNECTED
@return	    ZIXI_ERROR_OK
@return	    ZIXI_ERROR_NOT_READY
*/
int DLL_EXPORT zixi_query_adaptive_info(void *handle, unsigned int** bitrates, unsigned int* size, unsigned int* current);


/**
@brief		check the current network condition

Warnings:   this information updates once every 3 seconds, there is no use in calling this function more than once in 3 seconds.

@param		handle	- a handle to zixi connection returned by zixi_init_connection_handle
@param		latency	- an integer that will receive the current network latency value, in milliseconds.
@param		rtt		- an integer that will receive the current round trip time value, in milliseconds.

@return		ZIXI_ERROR_NOT_INITIALIZED
@return 	ZIXI_ERROR_FUNCTION_NOT_FOUND
@return 	ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_query_latency(void *handle, unsigned int* latency, unsigned int* rtt);

/**
@brief		check the status of the cache file/in memory buffer

Warnings:   You can start reading data at any stage. The fullness will never reach 100%, do not wait for that to happen.

@param		handle		- a handle to zixi connection returned by zixi_init_connection_handle
@param		in_use		- will be set to true if currently connected in progressive-download mode
@param		fullness	- percentage of cache currently used.
@param		duration	- the duration of the video currently cached in the buffer in milliseconds

@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_query_pdl(void *handle, bool* in_use , float* fullness, unsigned int* duration);

/**
@brief		query error correction statistics from the client library, this information is updated once every 3 seconds.

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle
@param		con_stats  - ZIXI_CONNECTION_STATS structure for connection statistics
@param		net_stats  - ZIXI_NETWORK_STATS structure that will receive network statistics
@param		error_correction_stats  - ZIXI_ERROR_CORRECTION_STATS structure that will receive error correction statistics

@return		ZIXI_ERROR_NOT_INITIALIZED
@return	    ZIXI_ERROR_FUNCTION_NOT_FOUND
@return	    ZIXI_ERROR_NOT_CONNECTED
@return	    ZIXI_ERROR_INVALID_PARAMETER
@return	    ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_query_statistics(void *handle, ZIXI_CONNECTION_STATS *con_stats, ZIXI_NETWORK_STATS *net_stats, ZIXI_ERROR_CORRECTION_STATS *error_correction_stats);


//--------------------------------------------------------
//			Support functions
//--------------------------------------------------------

/**
@brief		get the last error after receiving a disconnected event

Warnings: this function must not be called from the ZIXI_STATUS_FUNC callback

@param		handle - a handle to zixi connection returned by zixi_init_connection_handle

@return		ZIXI_ERROR_AUTHORIZATION_FAILED
@return  	ZIXI_ERROR_LICENSING_FAILED
@return		ZIXI_ERROR_NETWORK_ERRORS
@return		ZIXI_ERROR_FAILED
@return		ZIXI_ERROR_TIMEOUT
@return		ZIXI_ERROR_NOT_INITIALIZED
@return		ZIXI_ERROR_FUNCTION_NOT_FOUND
@return		ZIXI_ERROR_VERSION
@return		ZIXI_ERROR_OK
@return		ZIXI_ERROR_SERVER_FULL
*/
int DLL_EXPORT zixi_get_last_error(void *handle);

/**
@brief		returns the libzixiClient library version

@param      major	- major version number (will remain 0 until the first release version)
@param		minor	- minor version number (incremented each time we update our internal protocol)
@param		build	- build number  

@return		ZIXI_ERROR_NOT_INITIALIZED
@return 	ZIXI_ERROR_FUNCTION_NOT_FOUND
@return  	ZIXI_ERROR_OK
*/
int DLL_EXPORT zixi_client_version(int* major, int* minor, int* minor_minor, int* build);

/**
@brief		returns a descriptor for signaling, and the internal thread id

@param		handle			- a handle to zixi connection returned by zixi_init_connection_handle
@param		fd[out]			- pointer to a returned descriptor, fd is signalled when there is data to read
@param		thread_id[out]	- pointer to returned thread id

@return		ZIXI_ERROR_OK on success
*/
int DLL_EXPORT zixi_get_descriptors(void *handle, int* fd, int* thread_id);


int DLL_EXPORT zixi_get_ready_buffer(void *handle, unsigned char **buff, unsigned int *buff_size, bool *eof, bool* discontinuity, int* queue_size);


#ifdef __cplusplus
};	// extern "C"
#endif
