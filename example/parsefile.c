/*
 *  parsefile.c
 *
 *  Created by ollo on 21.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <stdio.h>
#include "fcseq.h"

int main(int argc, char** argv)
{
	fcsequence_t seq;
	fcseq_ret_t ret;
	int x, y, ypos;

	if (argc != 2)
	{
		fprintf(stderr, "usage %s <sequence file>\n", argv[0]);
		return 1;
	}
	
	ret = fcseq_load(argv[1], &seq);
	
	if (ret != FCSEQ_RET_OK)
	{
		fprintf(stderr, "Error code is %d\n", ret);
		fprintf(stderr, "Unable to load sequnce (%s:%d)", __FILE__, __LINE__);
		return 1;
	}
		
	printf("=== Meta information ===\nfps: %d, width: %d, height: %d\n",seq.fps,seq.width,seq.height);		
	
	/* use the extract resolution to define the necessary memory to extract one frame */
	uint8_t rgb24[seq.width * seq.height * 3];
	
	do {
		/* parse */
		ret = fcseq_nextFrame(&seq, rgb24);
		
		printf("==============================\n");
		for (y=0; y < seq.height; y++)
		{
			ypos = y * seq.width * 3;
			for(x=0; x < seq.width; x++)
			{
				printf("%.2X", rgb24[(ypos+x*3) + 0]);
				printf("%.2X", rgb24[(ypos+x*3) + 1]);
				printf("%.2X", rgb24[(ypos+x*3) + 2]);
			}
			printf("\n");
		}
		
	} while (ret != FCSEQ_RET_EOF);
	
	/* clean people now tidy up */
	fcseq_close(&seq);

	return 0;
}
