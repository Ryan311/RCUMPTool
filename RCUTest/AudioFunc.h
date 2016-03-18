#pragma once

#define BLOCK_SIZE 4096	//8192
#define BLOCK_COUNT 40

#define MSBCFRAMESIZE 60
#define PCMFRAMESIZE 240		// mSBC -> PCM   1:4
/*
* function prototypes
*/
WAVEHDR* allocateBlocks(int size, int count);
void freeBlocks(WAVEHDR* blockArray);

int NULL_AudioDecoder(LPCTSTR raw_DecoderFilePath);
int NULL_AudioPlay();