#pragma once

#include <time.h>

#ifndef __cplusplus
	#ifndef bool
		#define bool char
		#define true 1
		#define false 0
	#endif
#endif

// zixi warning values
#define ZIXI_WARNING_OVER_LIMIT			100

// zixi error values
// recoverable errors
#define ZIXI_ERROR_RECONNECTING			7
#define ZIXI_ERROR_BUFFER_TO_SMALL		6
#define ZIXI_ERROR_NOT_READY			5
#define ZIXI_ERROR_ALREADY_CONNECTED	4
#define ZIXI_ERROR_ALREADY_INITIALIZED	3
#define ZIXI_ERROR_STREAM_CHANGED		2
#define ZIXI_ERROR_INVALID_PARAMETER	1

// no error
#define ZIXI_ERROR_OK					0

// non-recoverable errors
#define	ZIXI_ERROR_FAILED				-1
#define ZIXI_ERROR_TIMEOUT				-2
#define ZIXI_ERROR_NOT_INITIALIZED		-3
#define ZIXI_ERROR_NOT_CONNECTED		-4
#define ZIXI_ERROR_DLL_NOT_FOUND		-5
#define ZIXI_ERROR_FUNCTION_NOT_FOUND	-6
#define ZIXI_ERROR_AUTHORIZATION_FAILED	-7
#define ZIXI_ERROR_LICENSING_FAILED		-8
#define ZIXI_ERROR_NETWORK_ERRORS		-9
#define ZIXI_ERROR_VERSION				-10
#define ZIXI_ERROR_SERVER_FULL			-11
#define ZIXI_ERROR_EOF					-12
#define ZIXI_ERROR_NOT_SUPPORTED		-13
#define ZIXI_ERROR_TEST_FAILED			-14
#define ZIXI_ERROR_RESOLVING_FAILED		-15
#define ZIXI_ERROR_CACHE_TO_SMALL		-16
#define ZIXI_ERROR_NETWORK				-17
#define ZIXI_ERROR_NOT_FOUND			-18
#define ZIXI_ERROR_BAD_URL				-19
#define ZIXI_ERROR_FILE_LOCAL			-20
#define ZIXI_ERROR_FILE_REMOTE			-21		
#define ZIXI_WARNING_REMOTE_FILE_EXISTS -22		//	File cannot be uploaded without 'overwrite' permission.
#define ZIXI_ERROR_DECRYPTION			-23	

// zixi latency settings
#define ZIXI_LOW_LATENCY		500
#define ZIXI_MEDIUM_LATENCY		4000
#define ZIXI_HIGH_LATENCY		8000

// zixi encryption types
typedef enum
{
	ZIXI_AES_128,
	ZIXI_AES_192,
	ZIXI_AES_256,
	ZIXI_NO_ENCRYPTION,
	ZIXI_AUTO_ENCRYPTION
}ZIXI_ENCRYPTION;

// zixi connection status
typedef enum 
{	
	ZIXI_DISCONNECTED,
	ZIXI_CONNECTING,
	ZIXI_CONNECTED,
	ZIXI_DISCONNECTING,
	ZIXI_RECONNECTING
}ZIXI_STATUS;

// zixi log levels
typedef enum
{
	ZIXI_LOG_NONE = -1,
	ZIXI_LOG_ALL = 0,
	ZIXI_LOG_DEBUG = 1,
	ZIXI_LOG_INFO = 2,
	ZIXI_LOG_WARNINGS = 3,
	ZIXI_LOG_ERRORS = 4,
	ZIXI_LOG_FATAL = 5
}ZIXI_LOG_LEVELS;

typedef enum
{
	ZIXI_LATENCY_STATIC = 0,
	ZIXI_LATENCY_INCREASING = 1,
	ZIXI_LATENCY_DYNAMIC = 2,
}ZIXI_LATENCY_MODE;

typedef enum
{
	ZIXI_ADAPTIVE_NONE = 0,
	ZIXI_ADAPTIVE_ENCODER = 1,
	ZIXI_ADAPTIVE_FEC = 2,
}ZIXI_ADAPTIVE_MODE;

typedef enum
{
	ZIXI_FEC_OFF = 0,
	ZIXI_FEC_ON = 1,
	ZIXI_FEC_ADAPTIVE = 2,
}ZIXI_FEC_MODE;

// zixi audio/video parameters
typedef struct 
{
	unsigned long long file_size;
	int	length;
	bool mpegts_stream;
	bool vod_stream;
	const char* stream_name;
	int stream_name_length;
	const char* password;
	int password_length;
	const char* remote_address;
	int remote_address_length;

	bool	valid_info;
} ZIXI_STREAM_INFO;

typedef struct
{
	unsigned long long	packets;
	unsigned long long	bytes;

	unsigned long long	out_of_order;
	unsigned long long	dropped;
	unsigned long long	duplicates;
	unsigned long long	overflow;

	unsigned int		bit_rate;
	unsigned int		packet_rate;

	unsigned int		jitter;
	unsigned int		rtt;
	unsigned int		latency;

	unsigned int		available_bitrate;
	char				congested;
} ZIXI_NETWORK_STATS;

typedef struct
{
	unsigned long long	arq_packets;
	unsigned long long	fec_packets;

	unsigned long long	arq_recovered;
	unsigned long long	fec_recovered;
	unsigned long long	not_recovered;

	unsigned long long	duplicates;
	unsigned long long	requests;
	unsigned long long	overflow;

	unsigned int	fec_bit_rate;
	unsigned int	fec_packet_rate;

	unsigned long long	nulls_stuffed;

	unsigned long long	late_dropped;		// packets dropped because of max_output_jitter limit

} ZIXI_ERROR_CORRECTION_STATS;

typedef struct
{
	unsigned long long	reconnections;
	unsigned long long	up_time;

	int					last_error;

	ZIXI_STATUS			status;
	time_t				last_status_change; // time of last status change as returned by the time(0) function

	char				host_ip[16];

} ZIXI_CONNECTION_STATS;

// zixi callback function types
typedef void (*ZIXI_BITRATE_CHANGED_FUNC)(void *zixi_handler, int stream_index, int bitrate, char* stream_name, void *user_data);
typedef void (*ZIXI_STATUS_FUNC)(void *zixi_handler, ZIXI_STATUS status, void *user_data);
typedef void (*ZIXI_NEW_STREAM_FUNC)(void *zixi_handler, ZIXI_STREAM_INFO info, void *user_data);
typedef void (*ZIXI_LOG_FUNC)(void *user_data , int level, const char *msg);
typedef int (*ZIXI_IO_FUNCTION)(void *handler, char *buffer, int length, struct sockaddr_in* addr, void *user_data);
typedef void (*ZIXI_RELEASE_FUNCTION)(char *buffer, int length, void *user_data);

// zixi callbacks struct
typedef struct 
{
	ZIXI_STATUS_FUNC			zixi_status_changed;
	ZIXI_NEW_STREAM_FUNC		zixi_new_stream;
	ZIXI_BITRATE_CHANGED_FUNC	zixi_bitrate_changed;
	void						*user_data;
} ZIXI_CALLBACKS;

#define ZIXI_GUID_LENGTH	128
#define ZIXI_SESSION_LENGTH	128
#define ZIXI_CHANNEL_LENGTH	64

#define ZIXI_MAX_ADAPTIVE_STREAMS	16


// zixi auxiliary information for demuxe'd frames
typedef struct {
	// video only
	void * sps;
	void * pps;
	unsigned int	sps_len;
	unsigned int    pps_len;
	unsigned char compatibilty;
	unsigned char level;
	unsigned short width;
	unsigned short height;
	
	unsigned short sarH;
	unsigned short sarW;
	float		   fps;
	bool		   interlaced;
	
	// audio only
	unsigned char channel_config; // or unsigned short channel_count
	unsigned int sample_rate;
	unsigned char sample_rate_index;
	unsigned int ticks_per_frame_90K;
	
	// both
	unsigned char profile;
	int			   timescale;
} ZIXI_MEDIA_INFO;
