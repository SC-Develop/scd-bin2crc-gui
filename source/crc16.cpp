/*------------------------------------------------------------
    FILE INCLUSI
--------------------------------------------------------------*/
#include "crc16.h"
#include <stdint.h>

/*------------------------------------------------------------
    VARIABILI DI MODULO 
--------------------------------------------------------------*/
static uint16_t crc;

/*------------------------------------------------------------
    FUNZIONI 
--------------------------------------------------------------*/
void crc16Init(void)
{
  crc = 0xFFFF;
}

void crc16Add(uint8_t data)
{
  uint8_t x;

  x = crc >> 8 ^ data;
  x ^= x>>4;
  crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
}

uint16_t crcGet(void)
{
  return crc;
}