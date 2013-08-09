#include <cv.h>
#include <highgui.h>
#include <stdio.h>

IplImage* img_resize(IplImage* src_img, int new_width,int new_height)
{
    IplImage* des_img;
    des_img=cvCreateImage(cvSize(new_width,new_height),src_img->depth,src_img->nChannels);
    cvResize(src_img,des_img,CV_INTER_LINEAR);
    return des_img;
}

void main(int argc,char *argv[])
{
	int c;
	IplImage* color_img;
	IplImage* tiny_img;
	CvCapture* cv_cap = cvCaptureFromCAM(0);
	cvNamedWindow("Video",0); // create window
	for(;;) {
		color_img = cvQueryFrame(cv_cap); // get frame
		if(color_img != 0)
		{
			tiny_img = img_resize(color_img, 10, 7);
			cvShowImage("Video", tiny_img); // show frame
			
			printf("Width %d, height %d\n", tiny_img->width, tiny_img->height);
			
			CvScalar pixel = cvGet2D(tiny_img, 0, 0); // upper left pixel (hopefully)
			printf("0x0 : %lf,%lf,%lf\n", pixel.val[2], pixel.val[1], pixel.val[0] );
			
		}
		c = cvWaitKey(10); // wait 10 ms or for key stroke
		if(c == 27)
		{
			break; // if ESC, break and quit
		}
	}
	/* clean up */
	cvReleaseCapture( &cv_cap );
	cvDestroyWindow("Video");
}
