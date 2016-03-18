#ifndef _AES_H
#define _AES_H

#include "stdafx.h"
#define uint32_t UINT32 

typedef struct
{
    uint32_t erk[64];   /* encryption round keys */
    uint32_t drk[64];   /* decryption round keys */
    int nr;				/* number of rounds */
}
aes_context;

int  aes_set_key( aes_context *ctx, UINT8 *key, int nbits );
void aes_encrypt( aes_context *ctx, UINT8 input[16], UINT8 output[16] );
void aes_decrypt( aes_context *ctx, UINT8 input[16], UINT8 output[16] );
void OTA_AES_Init( int aes_mode );
void Produce_RandData();

#endif /* aes.h */
