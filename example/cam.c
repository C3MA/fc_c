#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "fcclient.h"

IplImage* img_resize(IplImage* src_img, int new_width,int new_height)
{
    IplImage* des_img;
    des_img=cvCreateImage(cvSize(new_width,new_height),src_img->depth,src_img->nChannels);
    cvResize(src_img,des_img,CV_INTER_LINEAR);
    return des_img;
}

int main(int argc, char *argv[])
{
	int c;
	IplImage* color_img;
	IplImage* tiny_img;

	CvCapture* cv_cap = cvCaptureFromCAM(0);
	cvNamedWindow("Video",0); // create window
	
	int success, success2;
	int x, y, x1, y1;
	fcclient_t * client = fcclient_new();
	
	if (argc < 2)
	{
		printf("Usage %s <IP address of the wall>\n", argv[0]);
		return 1;
	}
	
	printf("Connecting to %s...\n", argv[1]);
	success = fcclient_open(client, argv[1]);
	printf("Connection returned %d\n", success);
	
	if (success < 0) {
		printf("Cannot connect to wall\n");
		return 1;
	}

	do {
		/* call this function until we were successfull in receiving something */
		success = fcclient_processNetwork(client);
		printf("Network: %d\n", success);
		/*FIXME update the function using "select()" with an timeout */
	} while (!success);
	/* the server has answered */
	
	success2 = fcclient_start(client);
	printf("Start: %d\n", success2);
	
	uint8_t frame[client->width * client->height * 20]; 
	
	
	printf("============= Sending Frame =============\n");
		
	for(;;) {
		if (!client->connected)
		{
			success = fcclient_processNetwork(client);
			printf("\t\tReceived data [%d]\n", success);
		}
		color_img = cvQueryFrame(cv_cap); // get frame
		
		if (client->connected) {
			if(color_img != 0)
			{
				tiny_img = img_resize(color_img, client->width, client->height);
				cvShowImage("Video", tiny_img); // show frame
				for (y1=0; y1 < tiny_img->height; y1++) {
					for (x1=0; x1 < tiny_img->width; x1++) {
				//		printf("%d x %d\n", x1, y1);
						CvScalar pixel = cvGet2D(tiny_img, y1, x1); 
						fcclient_addPixel(client, frame, pixel.val[2], pixel.val[1], pixel.val[0] , x1, y1);
					}
				}
				/* Now we need to send some nice frames to the wall */
				fcclient_sendFrame(client, frame);
			//printf("Width %d, height %d\n", tiny_img->width, tiny_img->height);
			}
		}
		c = cvWaitKey(50); // wait 50 ms or for key stroke
		if(c == 27)
		{
			break; // if ESC, break and quit
		}
	}
	/* clean up */
	cvReleaseCapture( &cv_cap );
	cvDestroyWindow("Video");
}
