#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "fcclient.h"

#define COLOR_MAP_LENGTH 64
#define COLOR_RANGE 256

/* Source: https://github.com/bytefish/colormaps-opencv/blob/master/src/colormap.cpp */
float r[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.9365079365079367, 0.8571428571428572, 0.7777777777777777, 0.6984126984126986, 0.6190476190476191, 0.53968253968254, 0.4603174603174605, 0.3809523809523814, 0.3015873015873018, 0.2222222222222223, 0.1428571428571432, 0.06349206349206415, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.03174603174603208, 0.08465608465608465, 0.1375661375661377, 0.1904761904761907, 0.2433862433862437, 0.2962962962962963, 0.3492063492063493, 0.4021164021164023, 0.4550264550264553, 0.5079365079365079, 0.5608465608465609, 0.6137566137566139, 0.666666666666667};

float g[] = { 0, 0.03968253968253968, 0.07936507936507936, 0.119047619047619, 0.1587301587301587, 0.1984126984126984, 0.2380952380952381, 0.2777777777777778, 0.3174603174603174, 0.3571428571428571, 0.3968253968253968, 0.4365079365079365, 0.4761904761904762, 0.5158730158730158, 0.5555555555555556, 0.5952380952380952, 0.6349206349206349, 0.6746031746031745, 0.7142857142857142, 0.753968253968254, 0.7936507936507936, 0.8333333333333333, 0.873015873015873, 0.9126984126984127, 0.9523809523809523, 0.992063492063492, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.9841269841269842, 0.9047619047619047, 0.8253968253968256, 0.7460317460317465, 0.666666666666667, 0.587301587301587, 0.5079365079365079, 0.4285714285714288, 0.3492063492063493, 0.2698412698412698, 0.1904761904761907, 0.1111111111111116, 0.03174603174603208, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float b[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.01587301587301582, 0.09523809523809534, 0.1746031746031744, 0.2539682539682535, 0.333333333333333, 0.412698412698413, 0.4920634920634921, 0.5714285714285712, 0.6507936507936507, 0.7301587301587302, 0.8095238095238093, 0.8888888888888884, 0.9682539682539679, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


void shrinkColorSpace(uint8_t* red, uint8_t* green, uint8_t* blue)
{
//	printf("input\t%d %d %d\n", *red, *green, *blue);
	int r64 = (*red);
	int g64 = (*green);
	int b64 = (*blue);
	r64 = r64 * COLOR_MAP_LENGTH / COLOR_RANGE;
	g64 = g64 * COLOR_MAP_LENGTH / COLOR_RANGE;
	b64 = b64 * COLOR_MAP_LENGTH / COLOR_RANGE;
//	printf("offset: %d ; %d ; %d\n", r64, g64, b64);
	
	r64  = (r[r64] * (COLOR_RANGE - 1));
	g64  = (g[g64] * (COLOR_RANGE - 1));
	b64  = (b[b64] * (COLOR_RANGE - 1));
	
//	printf("color: %d ; %d ; %d\n", r64, g64, b64);

	(*red)    = (uint8_t) r64;
	(*green)  = (uint8_t) g64;
	(*blue)   = (uint8_t) b64;
//	printf("output\t%d %d %d\n===========\n", *red, *green, *blue);
}

IplImage* img_resize(IplImage* src_img, int new_width,int new_height)
{
    IplImage* des_img;
    des_img=cvCreateImage(cvSize(new_width,new_height),src_img->depth,src_img->nChannels);
    cvResize(src_img,des_img,CV_INTER_NN);
    /* CV_INTER_LINEAR, CV_INTER_NN, CV_INTER_AREA, CV_INTER_CUBIC */
    return des_img;
}

int main(int argc, char *argv[])
{
	int c;
	IplImage* color_img;
	IplImage* tiny_img;

	CvCapture* cv_cap = cvCaptureFromCAM(0);
	cvNamedWindow("Video-Input",0); // create window
	
	int success, success2;
	int x, y, x1, y1;
	fcclient_t * client = fcclient_new();
	
	int gamma = 2;
	int debug = 0;
	uint8_t red, green, blue;
	uint8_t* frame = 0;
	double test = 0.0;	

	if (argc < 3)
	{
		printf("Usage %s <IP address of the wall> <gamma value>\n", argv[0]);
		printf("Usage %s debug <gamma value>\t There is no connection to the wall done, only debugging the webcam.\n", argv[0]);
		return 1;
	}

	if ( strcmp(argv[1], "debug") == 0 )
	{
		debug = 1;
	} 
	
	test = cvGetCaptureProperty(cv_cap, CV_CAP_PROP_BRIGHTNESS);
	printf("Setting of brightness is %lf\n", test);

	test = 0.1;
	//test = atof(argv[2]);
	
	int retProp = cvSetCaptureProperty(cv_cap, CV_CAP_PROP_BRIGHTNESS, test);
	printf("Setting %lf  properties returned %d\n", test, retProp);


	if (!debug)
	{
		cvNamedWindow("Video",0); // create window
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

		uint8_t f[client->width * client->height * 20]; 
		frame = f;

		printf("============= Sending Frame =============\n");
	}	
	
	for(;;) {
		if (debug == 0 && !client->connected)
		{
			printf("Network code activated\n");
			success = fcclient_processNetwork(client);
			printf("\t\tReceived data [%d]\n", success);
		}
		color_img = cvQueryFrame(cv_cap); // get frame
		
		if (!debug)
		{
		if (client->connected) {
			if(color_img != 0)
			{

				tiny_img = img_resize(color_img, client->width, client->height);
				for (y1=0; y1 < tiny_img->height; y1++) {
					for (x1=0; x1 < tiny_img->width; x1++) {
						//		printf("%d x %d\n", x1, y1);
						CvScalar pixel = cvGet2D(tiny_img, y1, x1);
						red = pixel.val[2];
						green = pixel.val[1];
						blue = pixel.val[0];

						shrinkColorSpace(&red, &green, &blue);

						fcclient_addPixel(client, frame, red, green, blue, x1, y1);
						// update the image to display in the preview
						pixel.val[2] = red;
						pixel.val[1] = green;
						pixel.val[0] = blue;
						cvSet2D(tiny_img, y1, x1, pixel);
					}
				}
				/* Now we need to send some nice frames to the wall */
				fcclient_sendFrame(client, frame);
				cvShowImage("Video", tiny_img); // show frame
				}
			}
		}
		cvShowImage("Video-Input", color_img); // show frame
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
