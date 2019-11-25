#include <string.h>
#include "Logger.h"
#if defined(USE_RTTHREAD)
#include <entry.h>
#include <dfs_posix.h>

#define DBG_TAG               "LOG"
#define DBG_LVL               DBG_INFO
#include <rtdbg.h>
#endif /* #if defined(USE_RTTHREAD) */

#define LOG_FILE_NAME "/log.bin"

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
};

uint8_t Write_Format(const struct LogStructure *s);
	
uint8_t WriteBlock(const void *pBuffer, uint16_t size)
{
  int fd;

  /* Open file to write */
  fd = open(LOG_FILE_NAME, O_WRONLY | O_APPEND | O_BINARY);
  if (fd>= 0)
  {
    write(fd, pBuffer, size);
    close(fd);
    return 0;
  }

  return 1;
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
  int     fd;
 
  LOG_I("Log_Init");
  rt_thread_mdelay(500);
  if (dfs_mount("sd0", "/", "elm", 0, 0) == RT_EOK)
  {
    LOG_I("sd card mount to '/'");
  }
  else
  {
    LOG_W("sd card mount to '/' failed!");
  }
  
  fd = open(LOG_FILE_NAME, O_RDONLY | O_BINARY);
  if (fd>=0)
  {
    close(fd);
    unlink(LOG_FILE_NAME);
  }
  
  fd = open(LOG_FILE_NAME, O_WRONLY | O_BINARY | O_CREAT);
  if (fd>= 0)
  {
    for(i = 0; i < ARRAY_SIZE(log_structure); i++) 
    {
      Write_Format(&log_structure[i]);
    }
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
