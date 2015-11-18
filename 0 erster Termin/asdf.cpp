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
	viewer viewTransformed("Transformed");

	/*---------------------*/
	/* images & channels   */
    /*---------------------*/
    image img;                              // normalized (color) image
	channel8  src;  // source picture       // 8-bit-image (source)
    channel8  dst;  // destination picture  // 8-bit-image (source) 


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
    loader.load("Kalib.bmp",img);
    
    // extract the intensity channel only
    splitter.getIntensity(img,src);

    // determine image size
    const int rowSize    = src.rows();
    const int columnSize = src.columns();


    // set destination size to source size 
    dst.resize(rowSize,columnSize,0,false,true);


	//transformationsparameter
	//für x
	const double a0 = 160.7916667;
	const double a1 = 0.441666667;
	const double a2 = -0.13;
	//für y
	const double b0 = 218.7416667;
	const double b1 = 0.001666667;
	const double b2 = 0.4925;


    // copy source pixels to destination pixels
	for(int y=0; y<rowSize; y++){
		for(int x=0; x<columnSize; x++){
		
			int sX = a0 + a1 * x + a2 * y;
			int sY = b0 + b1 * x + b2 * y;
			if(checkBorder(sX, columnSize) && checkBorder(sY, rowSize)) 
				dst[x][y] = src[sX][sY];
		}
	}

	// view pictures
    view.show(src);
    viewTransformed.show(dst);

    getchar();

  }

  bool RV01::checkBorder(int coordinate, int maxsize){
  
	  if(0 <=coordinate && coordinate < maxsize - 1 )
		  return true;

	  return false;
  }

/*
  int RV01::bilinInterpol(const double dX, const double dY, const channel8& img){

	  return NULL;
  }
*/

};