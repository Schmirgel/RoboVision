/*
   RV02: Median und Sobel
   
   Autor: 	Marvin Herrmann	2120896
			Torben Fischer
			Ralf Prediger	2112902
   HAW-University of Applied Sciences - Hamburg,Germany

 */ 

#include "ltiObject.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <list>
#include <stdio.h>

#include "RV02.h"
#include "ltiTimer.h"
#include "ltiBMPFunctor.h"
#include "ltiViewer.h"
#include "ltiSplitImg.h"
#include "gtk.h"
#include "ltiGtkServer.h"

using std::cout;
using std::endl;


namespace lti {

  void RV02::operator()(int argc,char *argv[]) {

	/*********************************************/
	/**** has always to be started (AMei)     ****/
    /**** if program is without gtk-Widgets   ****/
	/*********************************************/

	gtkServer server;
    server.start();

	/******************************************/
	/**** instantiation of used components ****/
	/******************************************/

	/*---------------------*/
	/* loaders and viewers */
    /*---------------------*/
    loadBMP loader;                         		// object for loading .bmp-images

    viewer view("Original");                		// object for visualizing original picture
	viewer viewMedian("Median");					// object for visualizing picture modified by median-operator
	viewer viewSobelGradient("SobelGradient");		// object for visualizing picture modified by sobel-operator (gradiant)
	viewer viewSobelDirection("SobelDirection");	// object for visualizing picture modified by sobel-operator (direction)

	/*---------------------*/
	/* images & channels   */
    /*---------------------*/
    image img;                              // normalized (color) image
	channel8  src;  // source picture       // 8-bit-image (source)
    channel8  dm;   // destination picture  // 8-bit-image (dest. median) 
	channel8  dsg;  // destination picture  // 8-bit-image (dest. sobel gradiant) 
	channel8  dsd;  // destination picture  // 8-bit-image (dest. sobel direction) 


	/*-----------------------------*/
	/* Image processing components */
	/*-----------------------------*/

    // object to split image into hue, saturation and intensity
	// hue        = Farbton
	// saturation = Farbsättigung
	// intensity  = Intensität (Grauwert)
    splitImageToHSI splitter;



	/******************************************/
	/*    the program                         */
	/******************************************/

    // load the source image
    loader.load("shaft.bmp",img);
    
    // extract the intensity channel only
    splitter.getIntensity(img,src);

    // determine image size
    const int rowSize    = src.rows();
    const int columnSize = src.columns();


    // set destination size to source size 
    dm.resize(rowSize,columnSize,0,false,true);
	dsg.resize(rowSize,columnSize,0,false,true);
	dsd.resize(rowSize,columnSize,0,false,true);

    
    Median(src,dm,9,9);

	Sobel(src,dsg,dsd);



	// view pictures
    view.show(src);
    viewMedian.show(dm);
	viewSobelGradient.show(dsg);
	viewSobelDirection.show(dsd);

    getchar();

  }



  

  /***************************************************************************/
  /* Function definition: ----- Median-operator----                          */
  /***************************************************************************/
  void RV02::Median(  const	     channel8& sPic, 	// source picture 
								 channel8& dPic, 	// destination picture
	                   const int MaskSizeX,		    // mask size in x-direction
					   const int MaskSizeY		 	// mask size in y-direction
					   )
  {
	const int PicSizeY = sPic.rows();
	const int PicSizeX = sPic.columns();
	int newMaskSizeY = MaskSizeY;
	int newMaskSizeX = MaskSizeX;

	// Wir erlauben nur einen mindest Maskengroesse von 3 x 3
	if(newMaskSizeY < 3) {
		newMaskSizeY = 3;
	}

	// Wenn es eine gerade Maskengroesse gewählt 
	if(newMaskSizeY%2 == 0) {
		newMaskSizeY++;
	}

	if(newMaskSizeX < 3) {
		newMaskSizeX = 3;
	}

	if(newMaskSizeX%2 == 0){
		newMaskSizeX++;
	}

	int histogramm[256];
	unsigned char Grauwert;
	int x,y,mx,my;
	int medianIndex = ((MaskSizeY*MaskSizeX)+1)/2;

	for(y=0; y<PicSizeY-(newMaskSizeY); y++) {
		for(x=0; x<PicSizeX-(newMaskSizeY); x++) {
			for(int i = 0; i < 256; i++) {
				histogramm[i] = 0;
			}
			for(my = y; my < y+newMaskSizeY; my++) {
				for(mx = x; mx < x+newMaskSizeX; mx++) {
					Grauwert = sPic[my][mx];
					histogramm[Grauwert]++;
				}
			}
			int counter = 0;
			int zaehler = 0;
			while(counter < medianIndex) {
				counter += histogramm[zaehler];
				zaehler++;
			}
			dPic[y+((newMaskSizeY+1)/2)][x+((newMaskSizeX+1)/2)] = zaehler;
		}
	}

  }

  /***************************************************************************/
  /* Function definition: ----- Median-operator----                          */
  /***************************************************************************/
  void RV02::Sobel(  const	     channel8& sPic, 			// source picture 
								 channel8& GradientPic, 	// destination picture
								 channel8& DirectionPic 	// destination picture
					   )
  {
	  	const int PicSizeY = sPic.rows();
		const int PicSizeX = sPic.columns();
		int x,y,fx,fy;
		
		unsigned char Grauwert;

		for(y = 0; y < (PicSizeY-3); y++){
			for(x = 0; x < (PicSizeX-3); x++){
				int gxSum = 0;
				int gySum = 0;

				gxSum += sPic[y][x]*-1;
				gxSum += sPic[y+1][x]*-2;
				gxSum += sPic[y+2][x]*-1;

				gxSum += sPic[y][x+2];
				gxSum += sPic[y+1][x+2]*2;
				gxSum += sPic[y+2][x+2];

				gySum += sPic[y][x]*-1;
				gySum += sPic[y][x+1]*-2;
				gySum += sPic[y][x+2]*-1;

				gySum += sPic[y+2][x];
				gySum += sPic[y+2][x+1]*2;
				gySum += sPic[y+2][x+2];

				gxSum/=4;
				gySum/=4;

				double gxPow = (gxSum*gxSum);
				double gyPow = (gySum*gySum);

				int Gradient = sqrt(gxPow+gyPow);

				GradientPic[y+1][x+1] = Gradient;
				
				double Gradientenwinkel = 0;

				//if(gxSum!=0) {
					Gradientenwinkel = (atan2((double)gySum,(double)gxSum) * 180 / Pi);
				//}

				int Gradientenrichtung = 0;

				if(Gradientenwinkel < -22.5) {
					Gradientenwinkel += 360;
				}

				if(Gradientenwinkel >= 22.5 && Gradientenwinkel < 67.5) {
					Gradientenrichtung = 1;
				} else if(Gradientenwinkel >= 67.5 && Gradientenwinkel < 112.5) {
					Gradientenrichtung = 2;
				} else if(Gradientenwinkel >= 112.5 && Gradientenwinkel < 157.5) {
					Gradientenrichtung = 3;
				} else if(Gradientenwinkel >= 157.5 && Gradientenwinkel < 202.5) {
					Gradientenrichtung = 4;
				} else if(Gradientenwinkel >= 202.5 && Gradientenwinkel < 247.5) {
					Gradientenrichtung = 5;
				} else if(Gradientenwinkel >= 247.5 && Gradientenwinkel < 292.5) {
					Gradientenrichtung = 6;
				} else if(Gradientenwinkel >= 292.5 && Gradientenwinkel < 337.5) {
					Gradientenrichtung = 7;
				}
				DirectionPic[y+1][x+1] = Gradientenrichtung;
			}
		}
  }


};
