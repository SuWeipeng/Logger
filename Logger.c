#include <string.h>
#include "Logger.h"
#include "fatfs.h"

#define LOG_FILE_NAME "log.bin"

FATFS fs;
FATFS *pfs;
FIL fil;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
FRESULT fres;
UINT br, bw;

const struct LogStructure log_structure[] = {
  {
    LOG_FORMAT_MSG, sizeof(struct log_Format),
    "FMT", "BBnNZ",      "Type,Length,Name,Format,Columns"
  },
  {
    LOG_TEST_MSG, sizeof(struct log_TEST),
    "TEST", "QH",         "TimeUS,value"
  },
};

uint8_t Write_Format(const struct LogStructure *s);
	
uint8_t WriteBlock(const void *pBuffer, uint16_t size)
{
  /* Mount SD Card */
  if(f_mount(&fs, "", 0) != FR_OK)
    return 1;

  /* Open file to write */
  if(f_open(&fil, LOG_FILE_NAME, FA_OPEN_APPEND | FA_WRITE) != FR_OK)
    return 2;

  /* Check freeSpace space */
  if(f_getfree("", &fre_clust, &pfs) != FR_OK)
    return 3;

  totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
  freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);

  /* free space is less than 1kb */
  if(freeSpace < 1)
    return 4;

  /* Writing*/
  f_write (&fil, pBuffer, size, &bw);

  /* Close file */
  if(f_close(&fil) != FR_OK)
    return 5;

  /* Unmount SDCARD */
  if(f_mount(NULL, "", 1) != FR_OK)
    return 6;

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
  FRESULT res;
 
  if(f_mount(&fs, "", 0) != FR_OK)
    return;
  res = f_open(&fil, LOG_FILE_NAME, FA_CREATE_NEW | FA_WRITE); 
  if(res == FR_OK){
    f_close(&fil);
  } else if(res == FR_EXIST){
    f_unlink(LOG_FILE_NAME);
  }
  if(f_mount(NULL, "", 1) != FR_OK)
    return;
  
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

void Write_Test(uint64_t time_us, uint16_t value)
{
  struct log_TEST pkt = {
    LOG_PACKET_HEADER_INIT(LOG_TEST_MSG),
    time_us,
    value,
  };
  WriteBlock(&pkt, sizeof(pkt));
}
