#include <string.h>
#include <entry.h>
#include <stm32f4xx_hal.h>
#include "Logger.h"
#include "AP_Buffer.h"

extern AP_Buffer *buffer;

const struct LogStructure log_structure[] = {
  {
    LOG_FORMAT_MSG, sizeof(struct log_Format),
    "FMT", "BBnNZ",       "Type,Length,Name,Format,Columns"
  },
  {
    LOG_TEST_MSG, sizeof(struct log_TEST),
    "TEST", "QH",         "TimeUS,value"
  },
  { LOG_PIDW1_MSG, sizeof(struct log_PID), 
    "PID1", "Qffffffffff",   "TimeUS,Tar,Act,Err,P,I,D,FF,DR,ER,TR"
  }, 
  { LOG_PIDW2_MSG, sizeof(struct log_PID), 
    "PID2", "Qffffffffff",   "TimeUS,Tar,Act,Err,P,I,D,FF,DR,ER,TR"
  },
  { LOG_PIDW3_MSG, sizeof(struct log_PID), 
    "PID3", "Qffffffffff",   "TimeUS,Tar,Act,Err,P,I,D,FF,DR,ER,TR"
  },
  { LOG_PIDW4_MSG, sizeof(struct log_PID), 
    "PID4", "Qffffffffff",   "TimeUS,Tar,Act,Err,P,I,D,FF,DR,ER,TR"
  },
  { LOG_ENC1_MSG, sizeof(struct log_ENC), 
    "ENC1", "QiidH",   "TimeUS,DLT,TK,DT,DMS"
  },
  { LOG_ENC2_MSG, sizeof(struct log_ENC), 
    "ENC2", "QiidH",   "TimeUS,DLT,TK,DT,DMS"
  },
  { LOG_ENC3_MSG, sizeof(struct log_ENC), 
    "ENC3", "QiidH",   "TimeUS,DLT,TK,DT,DMS"
  },
  { LOG_ENC4_MSG, sizeof(struct log_ENC), 
    "ENC4", "QiidH",   "TimeUS,DLT,TK,DT,DMS"
  },
  { LOG_PWM_MSG, sizeof(struct log_PWM), 
    "PWM", "Qhhhh",   "TimeUS,m1,m2,m3,m4"
  },
  { LOG_KF_MSG, sizeof(struct log_KF), 
    "KF", "Qffffffffffff",   "TimeUS,RAcc,PAcc,Gx,Gy,RF,PF,GxF,GyF,VR,VP,VX,VY"
  },
  { LOG_RNG_MSG, sizeof(struct log_RNG), 
    "RNG", "QH",   "TimeUS,dist"
  },
};

uint8_t Write_Format(const struct LogStructure *s);
	
uint8_t WriteBlock(const void *pBuffer, uint16_t size)
{
  buffer->write(pBuffer, size);
  
//  char buf[100];
//  sprintf(buf, "buf cnt: %d\r\n", buffer->buf_len());
//  rt_kputs(buf);
  
  return 0;
}

void Fill_Format(const struct LogStructure *s, struct log_Format *pkt)
{
  memset(pkt, 0, sizeof(*pkt));
  pkt->head1 = HEAD_BYTE1;
  pkt->head2 = HEAD_BYTE2;
  pkt->msgid = LOG_FORMAT_MSG;
  pkt->type = s->msg_type;
  pkt->length = s->msg_len;
  strncpy(pkt->name, s->name, sizeof(pkt->name));
  strncpy(pkt->format, s->format, sizeof(pkt->format));
  strncpy(pkt->labels, s->labels, sizeof(pkt->labels));
}

void Log_Init(void)
{
  uint8_t i;

  for(i = 0; i < ARRAY_SIZE(log_structure); i++) 
  {
    Write_Format(&log_structure[i]);
  }
}

/*
  write a structure format to the log
 */
uint8_t Write_Format(const struct LogStructure *s)
{
  struct log_Format pkt;

  Fill_Format(s, &pkt);

  return WriteBlock(&pkt, sizeof(pkt));
}

void Write_Test(void)
{
  for(uint8_t i=0;i<10;i++){
    struct log_TEST pkt = {
      LOG_PACKET_HEADER_INIT(LOG_TEST_MSG),
      i,
      i,
    };
    WriteBlock(&pkt, sizeof(pkt));
  }
}

void Write_PID(uint8_t msg_type, const PID_Info *info)
{
  struct log_PID pkt = {
    LOG_PACKET_HEADER_INIT(msg_type),
    HAL_GetTick(),
    info->target,
    info->actual,
    info->error,
    info->P,
    info->I,
    info->D,
    info->FF,
    info->DR,
    info->ER,
    info->TR
  };
  WriteBlock(&pkt, sizeof(pkt));
}

void Write_Encoder(uint8_t msg_type, int32_t delta_tick, int32_t tick, double delta_min, uint16_t delta_ms)
{
  struct log_ENC pkt = {
    LOG_PACKET_HEADER_INIT(msg_type),
    HAL_GetTick(),
    delta_tick,
    tick,
    delta_min,
    delta_ms
  };
  WriteBlock(&pkt, sizeof(pkt));
}

void Write_PWM(int16_t pwm_1, int16_t pwm_2, int16_t pwm_3, int16_t pwm_4)
{
  struct log_PWM pkt = {
    LOG_PACKET_HEADER_INIT(LOG_PWM_MSG),
    HAL_GetTick(),
    pwm_1,
    pwm_2,
    pwm_3,
    pwm_4
  };
  WriteBlock(&pkt, sizeof(pkt));
}

void Write_Attitude(float roll_acc, 
                    float pitch_acc, 
                    float gx, 
                    float gy, 
                    float roll_flt, 
                    float pitch_flt, 
                    float gx_flt, 
                    float gy_flt,
                    float var_r,
                    float var_p,
                    float var_gx,
                    float var_gy)
{
  struct log_KF pkt = {
    LOG_PACKET_HEADER_INIT(LOG_KF_MSG),
    HAL_GetTick(),
    roll_acc,
    pitch_acc,
    gx,
    gy,
    roll_flt,
    pitch_flt,
    gx_flt,
    gy_flt,
    var_r,
    var_p,
    var_gx,
    var_gy
  };
  WriteBlock(&pkt, sizeof(pkt));
}

void Write_RangeFinder(uint16_t dist)
{
  struct log_RNG pkt = {
    LOG_PACKET_HEADER_INIT(LOG_RNG_MSG),
    HAL_GetTick(),
    dist
  };
  WriteBlock(&pkt, sizeof(pkt));
}