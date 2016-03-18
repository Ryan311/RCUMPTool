// wmpTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "AudioFunc.h"

WAVEHDR* allocateBlocks(int size, int count)
{
	LPSTR buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;
	/*
	* allocate memory for the entire set in one go
	*/
	if ((buffer = (LPSTR)HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		totalBufferSize
		)) == NULL)
	{
		fprintf(stderr, "Memory allocation error\n");
		ExitProcess(1);
	}
	/*
	* and set up the pointers to each bit
	*/
	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;
	for (i = 0; i < count; i++) {

		blocks[i].dwBufferLength = size;
		blocks[i].lpData = (LPSTR)buffer;
		buffer += size;

	}
	return blocks;
}

void freeBlocks(WAVEHDR* blockArray)
{
	/*
	* and this is why allocateBlocks works the way it does
	*/
	HeapFree(GetProcessHeap(), 0, blockArray);
}


int NULL_AudioDecoder(LPCTSTR raw_DecoderFilePath)
{
	return 0;
}

int NULL_AudioPlay()
{
	return 0;
}
