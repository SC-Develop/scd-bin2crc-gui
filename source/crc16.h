#ifndef _CRC16_H
#define _CRC16_H

#include <stdint.h>

/*------------------------------------------------------------
     PROTOTIPI FUNZIONI ESPORTATE 
--------------------------------------------------------------*/
void crc16Init(void);
void crc16Add(uint8_t data);
uint16_t crcGet(void);

#endif /* _CRC16_H */
