#ifndef __LOGGER_H
#define __LOGGER_H

#ifdef __cplusplus
 extern "C" {
#endif
   
#include <stdint.h>

/*
Format characters in the format string for binary log messages
  a   : int16_t[32]
  b   : int8_t
  B   : uint8_t
  h   : int16_t
  H   : uint16_t
  i   : int32_t
  I   : uint32_t
  f   : float
  d   : double
  n   : char[4]
  N   : char[16]
  Z   : char[64]
  c   : int16_t * 100
  C   : uint16_t * 100
  e   : int32_t * 100
  E   : uint32_t * 100
  L   : int32_t latitude/longitude
  M   : uint8_t flight mode
  q   : int64_t
  Q   : uint64_t
 */
#define PACKED __attribute__((packed, aligned(1)))
#define ARRAY_SIZE(_arr) (sizeof(_arr) / sizeof(_arr[0]))

/*
 unfortunately these need to be macros because of a limitation of
 named member structure initialisation in g++
*/
#define LOG_PACKET_HEADER	       uint8_t head1, head2, msgid;
#define LOG_PACKET_HEADER_INIT(id) HEAD_BYTE1, HEAD_BYTE2, id
#define LOG_PACKET_HEADER_LEN 3 // bytes required for LOG_PACKET_HEADER

// once the logging code is all converted we will remove these from
// this header
#define HEAD_BYTE1  0xA3    // Decimal 163
#define HEAD_BYTE2  0x95    // Decimal 149

// structure used to define logging format
struct LogStructure {
  uint8_t msg_type;
  uint8_t msg_len;
  const char *name;
  const char *format;
  const char *labels;
};

/*
  log structures common to all vehicle types
 */
struct PACKED log_Format {
  LOG_PACKET_HEADER
  uint8_t type;
  uint8_t length;
  char name[4];
  char format[16];
  char labels[64];
};

struct PACKED log_TEST {
  LOG_PACKET_HEADER
  uint64_t time_us;
  uint16_t value;
};

struct PACKED log_PID {
  LOG_PACKET_HEADER
  uint64_t time_us;
  float   target;
  float   actual;
  float   error;
  float   P;
  float   I;
  float   D;
  float   FF;
  float   DR;
  float   ER;
  float   TR;
};

struct PACKED log_ENC {
  LOG_PACKET_HEADER
  uint64_t time_us;
  int32_t  delta_tick;
  int32_t  tick;
  double   delta_min;
  uint16_t delta_ms;
};

struct PACKED log_PWM {
  LOG_PACKET_HEADER
  uint64_t time_us;
  int16_t  pwm_1;
  int16_t  pwm_2;
  int16_t  pwm_3;
  int16_t  pwm_4;
};

enum LogMessages{
  LOG_TEST_MSG = 0,
  LOG_PIDW1_MSG,
  LOG_PIDW2_MSG,
  LOG_PIDW3_MSG,
  LOG_PIDW4_MSG,
  LOG_ENC1_MSG,
  LOG_ENC2_MSG,
  LOG_ENC3_MSG,
  LOG_ENC4_MSG,
  LOG_PWM_MSG,

  LOG_FORMAT_MSG = 128, // this must remain #128

  _LOG_LAST_MSG_
};

typedef struct PID_Info {
  float target;
  float actual;
  float error;
  float P;
  float I;
  float D;
  float FF;
  float DR;
  float ER;
  float TR;
}PID_Info;

void     Log_Init(void);
void     Write_Test(uint64_t time_us, uint16_t value);
void     Write_PID(uint8_t msg_type, const PID_Info *info);
void     Write_Encoder(uint8_t msg_type, int32_t delta_tick, int32_t tick, double delta_min, uint16_t delta_ms);
void     Write_PWM(int16_t pwm_1, int16_t pwm_2, int16_t pwm_3, int16_t pwm_4);

#ifdef __cplusplus
}
#endif
   
#endif /*__LOGGER_H*/
