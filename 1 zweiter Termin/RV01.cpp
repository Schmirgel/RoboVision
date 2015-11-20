/*
   RV01: Affine Transformation
   
   Autor: .....................
   HAW-University of Applied Sciences - Hamburg,Germany

 */ 

#include "ltiObject.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <list>
#include <stdio.h>

#include "RV01.h"
#include "ltiTimer.h"
#include "ltiBMPFunctor.h"
#include "ltiViewer.h"
#include "ltiSplitImg.h"
#include "gtk.h"
#include "ltiGtkServer.h"
#include "RV01.h"

using std::cout;
using std::endl;


namespace lti {

  void RV01::operator()(int argc,char *argv[]) {

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
    loadBMP loader;                         // object for loading .bmp-images

    viewer view("Original");                // object for visualizing images
	viewer viewAffiNNI("AffiNearestNeighborInterpolation");
	viewer viewAffiBilin("AffiBilineareInterpolation");
	viewer view4PointNNI("4PointNearestNeighborInterpolation");
	viewer view4PointBilin("4PointBilineareInterpolation");

	/*---------------------*/
	/* images & channels   */
    /*---------------------*/
    image img;													// normalized (color) image
	channel8  src;			// source picture					// 8-bit-image (source)
    channel8  affinNNI;		// affin Neares Neighbor			// 8-bit-image (destination) 
    channel8  affinBilin;	// affin bilineare Interpolation	// 8-bit-image (source) 
	channel8  fPointNNI;	// 4Point Neares Neighbor			// 8-bit-image (source) 
	channel8  fPointBilin;	// 4Point bilineare Interpolation	// 8-bit-image (source) 


	/*-----------------------------*/
	/* Image processing components */
	/*-----------------------------*/

    // object to split image into hue, saturation and intensity
	// hue        = Farbton
	// saturation = Farbs�ttigung
	// intensity  = Intensit�t (Grauwert)
    splitImageToHSI splitter;



	/******************************************/
	/*    the program                         */
	/******************************************/

    // load the source image
    loader.load("Kalib.bmp",img);
    
    // extract the intensity channel only
    splitter.getIntensity(img,src);

    // determine image size
    const int rowSize    = src.rows();
    const int columnSize = src.columns();


    // set destination size to source size 
    affinNNI.resize(rowSize,columnSize,0,false,true);
	affinBilin.resize(rowSize,columnSize,0,false,true);
	fPointNNI.resize(rowSize,columnSize,0,false,true);
	fPointBilin.resize(rowSize,columnSize,0,false,true);


  /***************************************************************************/
  /* Berrechnung der Affin Transformation									 */

	
	//transformationsparameter
	//f�r x
	const double a0 = 160.7916667;
	const double a1 = 0.441666667;
	const double a2 = -0.13;
	//f�r y
	const double b0 = 218.7416667;
	const double b1 = 0.001666667;
	const double b2 = 0.4925;

  /***************************************************************************/

  /***************************************************************************/
  /* Berrechnung der 4 Punkte Transformation								 */

	double sumPhiX, sumPhiY;
	double phiX1, phiX2, phiX3, phiX4;
	double phiY1, phiY2, phiY3, phiY4;

	const double x1 = 193,x2 = 585,x3 = 95,x4 = 667;
	const double y1 = 99,y2 = 94,y3 = 471,y4 = 477; 


 /***************************************************************************/


    //iterate over all pixels
	for(int y=0; y<rowSize; y++)
	{
		for(int x=0; x<columnSize; x++)
		{
			//calculate affin tranformation
			//affinnni is the output picture
			double sX = (a0 + a1 * x + a2 * y);
			double sY = (b0 + b1 * x + b2 * y);

			//calculate four points transormation
			//--------------begin----------------
			//TODO
			double x1Hut = x / columnSize-1;
			double x2Hut = x / columnSize-1;
			double x3Hut = x / columnSize-1;
			double x4Hut = x / columnSize-1;

			double y1Hut = y / rowSize-1;
			double y2Hut = y / rowSize-1;
			double y3Hut = y / rowSize-1;
			double y4Hut = y / rowSize-1;

			phiX1 = (1-x1Hut)*(1-y1Hut);
			phiX2 = (x2Hut)*(1-y2Hut);
			phiX3 = (x3Hut)*(y3Hut);
			phiX4 = (1-x4Hut)*(y4Hut);

			phiY1 = (1-x1Hut)*(1-y1Hut);
			phiY2 = (x2Hut)*(1-y2Hut);
			phiY3 = (x3Hut)*(y3Hut);
			phiY4 = (1-x4Hut)*(y4Hut);

			sumPhiX = phiX1 + phiX2 + phiX3 + phiX4;
			sumPhiY = phiY1 + phiY2 + phiY3 + phiY4;
			//--------------end------------------

			if(checkBorder(x,columnSize) && checkBorder(y, rowSize) && checkBorder(sX, columnSize) && checkBorder(sY, rowSize))
			{
				affinNNI[y][x] = src[(int)(sY+0.5)][(int)(sX+0.5)];
				affinBilin[y][x] = bilinInterpol(sX,sY,src);
				fPointNNI[y][x] = src[(int)(sumPhiY+0.5)][(int)(sumPhiX+0.5)];
				fPointBilin[y][x] = bilinInterpol(sumPhiX,sumPhiY,src);
			}
		}
	}


	// view pictures
    view.show(src);
	viewAffiNNI.show(affinNNI);
	viewAffiBilin.show(affinBilin);
	view4PointNNI.show(fPointNNI);
	view4PointBilin.show(fPointBilin);

    getchar();

  }

//Hilfsfunktionen
//check if given coordinate is within the given size
  bool RV01::checkBorder(int coordinate, int maxsize){
  
	  if(0 <=coordinate && coordinate < maxsize - 1 )
		  return true;

	  return false;
  }


  int RV01::bilinInterpol(const double dX, const double dY, const channel8& img){
		
		int lower, higher;

		int yx = img[(int)dY][(int)dX];
		int y1x = img[(int)dY+1][(int)dX];
		int yx1 = img[(int)dY][(int)dX+1];
		int y1x1 = img[(int)dY+1][(int)dX+1];


		(yx < yx1) ? lower = yx, higher = yx1 : lower = yx1, higher = yx; 
		
		int fx1 = yx + (dX - (int)dX) * (lower - higher);

		(y1x < y1x1) ? lower = y1x, higher = y1x1 : lower = y1x1, higher = y1x; 

		int fx2 = y1x + (dX - (int)dX) * (lower - higher);

		(fx1 < fx2) ? lower = fx1, higher = fx2 : lower = fx2, higher = fx1; 

		int fy = fx1 + (dY - (int)dY) * (lower - higher);

	  return fy;
  }


};