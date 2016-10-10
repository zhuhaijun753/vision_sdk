/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "grpxSrcLink_radar.h"

#include <math.h>

#define RGB888_TO_RGB565(r,g,b)     ((((UInt32)(r>>3) & 0x1F) << 11) | (((UInt32)(g>>2) & 0x3F) << 5) | (((UInt32)(b>>3) & 0x1F)))
#define M_PI		3.14159265358979323846
#define M_RAD		0.0174532925

GrpxRadarLink_targetInformation target1;
GrpxRadarLink_targetInformation target2;
GrpxRadarLink_targetInformation target3;

Bool bInitTargetsCalled = FALSE;
GrpxSrcLink_Obj *global_pObj;

//zz (20150616): fast sin and cos calculation
//-----------------------------------------------------------------------------
double sin_table[360];

void sin_table_fill(void)
{
	int degree;

	for (degree = 0; degree < 180; degree++)
	{
		sin_table[degree]       =  sin(degree * M_RAD );
		sin_table[degree + 180] = -sin_table[degree];
	}
}

inline double sinv_lookup(int angle) // in degrees
//double sinv_lookup(int angle)
{
   // prepare input angle to be [0, 359]
   while (angle < 0)
	   angle += 360;

   while (angle > 359)
	   angle -=360;

   UTILS_assert((angle >= 0) && (angle <= 359)); // array boundary protection

   return sin_table[angle];
}

inline double cosv_lookup(int angle) // cos(x) =  sin(x + 90)
//double cosv_lookup(int angle)
{
	return sinv_lookup(angle + 90);
}

//-----------------------------------------------------------------------------

/**
 *******************************************************************************
 *
 * \brief This function makes the GrpxSrcLink_Obj global
 *
 *******************************************************************************
 */
 
 Void GrpxRadarLink_setObj(GrpxSrcLink_Obj *pObj)
 {
	 global_pObj = pObj;
 }

/**
 *******************************************************************************
 *
 * \brief This function sets the initial values for the demo targets
 *
 *******************************************************************************
 */
Void GrpxRadarLink_setInitTargetValues(){
	
	if(! bInitTargetsCalled){
		bInitTargetsCalled = TRUE;
		
		target1.distance = 300;
		target1.angle = 0;
		target1.relSpeed = 0;
	
		target2.distance = 600;
		target2.angle = 15;
		target2.relSpeed = 20;
	
		target3.distance = 500;
		target3.angle = -30;
		target3.relSpeed = -20;
	
	}
}

/**
 *******************************************************************************
 *
 * \brief This function is updating the target values. 
 *
 *******************************************************************************
 */
Void GrpxRadarLink_targetUpdateValue(UInt32 targetNo, UInt32 value)
{
	GrpxRadarLink_clearTargets(global_pObj, 300, 400);
	if(targetNo == 0){
		if (value == 0){
			if (target2.distance != 0){
				target2.distance -= 100;
			}
		} else if (value == 1){
			if (target2.angle != -30){
				target2.angle -= 15;
			}
		} else if (value == 2){
			if (target2.angle != 30){
				target2.angle += 15;
			}
		} else {
			if (target2.distance != 700){
				target2.distance += 100;
			}
		}
	} else{
		if (value == 0){
			if (target3.distance != 0){
				target3.distance -= 100;
			}
		} else if (value == 1){
			if (target3.angle != -30){
				target3.angle -= 15;
			}
		} else if (value == 2){
			if (target3.angle != 30){
				target3.angle += 15;
			}
		} else {
			if (target3.distance != 700){
				target3.distance += 100;
			}
		}
	}
	GrpxRadarLink_setBRadarScopePrinted(FALSE);
	
}  


//zz (20150616): new calcX and calcY


//UInt32 calcX(UInt32 radius, double degree)
//{
//	return sin(degree * M_RAD ) * radius;
//}

inline UInt32 calcX(UInt32 radius, double degree)
//UInt32 calcX(UInt32 radius, double degree)
{
	return (UInt32)(sinv_lookup((int)degree) * radius);
}


//UInt32 calcY(UInt32 radius, double degree)
//{
//	return sin ((90 - degree) * M_RAD) * radius;
//}

inline UInt32 calcY(UInt32 radius, double degree)
//UInt32 calcY(UInt32 radius, double degree)
{
	return (UInt32)(cosv_lookup((int)degree) * radius);

}

Void GrpxRadarLink_drawLabel(
		GrpxSrcLink_Obj *pObj,
		UInt32 x,
		UInt32 y,
		char *text)
{
	char loadString[GRPX_RADAR_LINK_STR_SZ];
	Draw2D_FontPrm fontPrm;
	Draw2D_FontProperty fontProp;
	
	fontPrm.fontIdx = 6;

    Draw2D_getFontProperty(&fontPrm, &fontProp);
	
	snprintf(loadString, GRPX_RADAR_LINK_STR_SZ,
                          "%-4s\n",
                          text
                          );
	
	// shift the letters in both directions due to presence of the radar scope
	Draw2D_drawString(pObj->draw2DHndl,
                      x - 10,
                      y,
                      loadString,
                      &fontPrm
                      );
}

/**
 *******************************************************************************
 *
 * \brief This function clears a single target on the scope 
 *
 *******************************************************************************
 */

Void GrpxRadarLink_clearSingleTarget(GrpxSrcLink_Obj *pObj,
					UInt32 centerX,
					UInt32 centerY,
					GrpxRadarLink_targetInformation target)
{
	//clear only the litte region where the target was before
	
	int posX;
	int posY;
	
	posY = centerY - calcY(target.distance, target.angle);
	
	if (target.angle < 0)
		posX = centerX + calcX(target.distance, ( target.angle * (-1)));
	else
		posX = centerX - calcX(target.distance, target.angle);
	
	int x = posX - 20;
	int y = posY - 20;
	
	Draw2D_clearRegion(pObj->draw2DHndl,
					x,
					y,
					40,
					40);
}

/**
 *******************************************************************************
 *
 * \brief This function clears all targets
 *
 *******************************************************************************
 */
Void GrpxRadarLink_clearTargets(
					GrpxSrcLink_Obj *pObj,
					UInt32 centerX,
					UInt32 centerY)
{
	GrpxRadarLink_clearSingleTarget(
					pObj,
					centerX,
					centerY,
					target2);
					
	GrpxRadarLink_clearSingleTarget(
					pObj,
					centerX,
					centerY,
					target3);
		
	
	
}

/**
 *******************************************************************************
 *
 * \brief This function draws a circle and a cross
 * \param  pObj [IN] GrpxSrcLink_Obj
 * \param  UInt32 [IN] centerX
 * \param  UInt32 [IN] centerY
 * \param  UInt32 [IN] radius
 * \param  UInt32 [IN] color
						0: white	
						1: red
						2: green
 *
 *******************************************************************************
 */
Void GrpxRadarLink_drawTargetCircleCross(
					GrpxSrcLink_Obj *pObj,
					UInt32 centerX,
					UInt32 centerY,
					UInt32 radius,
					UInt32 colorSelection)
{
	Draw2D_LinePrm linePrm;
	UInt32 color[3];
	
	color[0] = RGB888_TO_RGB565(255, 255, 255);	//white
	color[1] = RGB888_TO_RGB565(255, 0, 0);		//red 
	color[2] = RGB888_TO_RGB565(255, 255, 0);	//yellow
	
	linePrm.lineColorFormat = SYSTEM_DF_BGR16_565;
    linePrm.lineColor = color[colorSelection];
    linePrm.lineSize = 2;

	double i;
	UInt32 x;
	UInt32 y;
	
	//draw the coloured circle
	for (i = 0; i < 90; i++){
		x = calcX(radius, i);
	    y = calcY(radius, i);

		Draw2D_drawPixel(
			pObj->draw2DHndl,
			centerX + x,
			centerY + y,
			linePrm.lineColor,
			linePrm.lineColorFormat
		);
		
		Draw2D_drawPixel(
			pObj->draw2DHndl,
			centerX + x,
			centerY - y,
			linePrm.lineColor,
			linePrm.lineColorFormat
		);
		
		Draw2D_drawPixel(
			pObj->draw2DHndl,
			centerX - x,
			centerY + y,
			linePrm.lineColor,
			linePrm.lineColorFormat
		);
		
		Draw2D_drawPixel(
			pObj->draw2DHndl,
			centerX - x,
			centerY - y,
			linePrm.lineColor,
			linePrm.lineColorFormat
		);
	}	
	
	/*
	//draw the cross in the middle
	int xDelta = calcX(radius, 45);
	int yDelta = calcY(radius, 45);
	
	Draw2D_drawLine(pObj->draw2DHndl,
			centerX - xDelta,
			centerY - yDelta,
			centerX + xDelta,
			centerY + yDelta,
			&linePrm);
			
	Draw2D_drawLine(pObj->draw2DHndl,
			centerX - xDelta,
			centerY + yDelta,
			centerX + xDelta,
			centerY - yDelta,
			&linePrm);		
	*/
}

/**
 *******************************************************************************
 *
 * \brief This function plots one single radar target
 * \param  pObj [IN] GrpxSrcLink_Obj
 * \param  UInt32 [IN] x
 * \param  UInt32 [IN] y
 * \param  UInt32 [IN] radius
 * \param  GrpxRadarLink_targetInformation [IN] target
 *
 *******************************************************************************
 */
Void GrpxRadarLink_plotTarget(GrpxSrcLink_Obj *pObj,
		Int32 x,
		Int32 y,
		Int32 radius,
		GrpxRadarLink_targetInformation *pTarget )
{
	
	//check the relative speed
	int colorSel;
	//zz (20150618): need to allow \pm 1 doppler bin error
	//               for the case of low resolution
	//zz (20150629): reverse direction
	//zz (20150629): enable dynamic threshold setting
	if(pTarget->relSpeed < (0 - pTarget->vel_threshold))
		colorSel = 2;
	else if(pTarget->relSpeed > pTarget->vel_threshold)
		colorSel = 1;
	else
		colorSel = 0;
	
	//calculate the position
	int posY;
	int posX;
	
    posY = y - calcY(pTarget->distance, pTarget->angle);

    if (pTarget->angle < 0)
        posX = x + calcX(pTarget->distance, (pTarget->angle * (-1)));
    else
        posX = x - calcX(pTarget->distance, pTarget->angle);
	
	//plot the target to the scope
	GrpxRadarLink_drawTargetCircleCross(
								pObj,
								posX,
								posY,
								6,
								colorSel);
}

/**
 *******************************************************************************
 *
 * \brief This function is responsible for displaying the radar targets
 * \param  pObj [IN] GrpxSrcLink_Obj
 * \param  UInt32 [IN] centerX
 * \param  UInt32 [IN] centerY
 * \param  UInt32 [IN] radius
 *
 *******************************************************************************
 */
 
 Void GrpxRadarLink_displayTargets(GrpxSrcLink_Obj *pObj,
					Uint32 centerX,
					UInt32 centerY,
					UInt32 radius)
{
	//Retrieve the targets that were calculcated from RSP
	
	// for a demo, plot the demo targets
    GrpxRadarLink_plotTarget(pObj, centerX, centerY, radius, &target1);
    GrpxRadarLink_plotTarget(pObj, centerX, centerY, radius, &target2);
    GrpxRadarLink_plotTarget(pObj, centerX, centerY, radius, &target3);
	
}

/**
 *******************************************************************************
 *
 * \brief This function draws the labels next to the radar scope
 * \param  pObj [IN] GrpxSrcLink_Obj
 * \param  UInt32 [IN] centerX
 * \param  UInt32 [IN] centerY
 * \param  UInt32 [IN] radius
 * \param  UInt32 [IN] degrees
 *
 *******************************************************************************
 */
Void GrpxRadarLink_drawLabels(GrpxSrcLink_Obj *pObj,
					UInt32 centerX,
					UInt32 centerY,
					UInt32 radius,
					UInt32 degrees)
{
	
	
    GrpxRadarLink_drawLabel(pObj,
			centerX,
			centerY - radius,
			"0°");
    GrpxRadarLink_drawLabel(pObj,
			centerX - calcX(radius, 15),
			centerY - calcY(radius, 15),
			"15°");
    GrpxRadarLink_drawLabel(pObj,
			centerX + calcX(radius, 15),
			centerY - calcY(radius, 15),
			"-15°");
    GrpxRadarLink_drawLabel(pObj,
			centerX - calcX(radius, 30),
			centerY - calcY(radius, 30),
			"30°");
    GrpxRadarLink_drawLabel(pObj,
			centerX + calcX(radius, 30),
			centerY - calcY(radius, 30),
			"-30°");
    GrpxRadarLink_drawLabel(pObj,
            centerX - calcX(radius, 45),
            centerY - calcY(radius, 45),
            "45°");
    GrpxRadarLink_drawLabel(pObj,
            centerX + calcX(radius, 45),
            centerY - calcY(radius, 45),
            "-45°");
    GrpxRadarLink_drawLabel(pObj,
            centerX - calcX(radius, 60),
            centerY - calcY(radius, 60),
            "60°");
    GrpxRadarLink_drawLabel(pObj,
            centerX + calcX(radius, 60),
            centerY - calcY(radius, 60),
            "-60°");

    GrpxRadarLink_drawLabel(pObj,
            centerX + calcX(590, 60) + 50,
            centerY - calcY(590, 60),
            "6m");
    GrpxRadarLink_drawLabel(pObj,
            centerX + calcX(490, 60) + 50,
            centerY - calcY(490, 60),
            "5m");
    GrpxRadarLink_drawLabel(pObj,
            centerX + calcX(390, 60) + 50,
            centerY - calcY(390, 60),
            "4m");
    GrpxRadarLink_drawLabel(pObj,
            centerX + calcX(290, 60) + 50,
            centerY - calcY(290, 60),
            "3m");
    GrpxRadarLink_drawLabel(pObj,
            centerX + calcX(190, 60) + 50,
            centerY - calcY(190, 60),
            "2m");
    GrpxRadarLink_drawLabel(pObj,
            centerX + calcX(90, 60) + 50,
            centerY - calcY(90, 60),
            "1m");

    GrpxRadarLink_drawLabel(pObj, 1540, 640, "Speed");
    GrpxRadarLink_drawTargetCircleCross(pObj, 1500, 700, 10, 0);
    GrpxRadarLink_drawLabel(pObj, 1540, 690, ":  0");
    GrpxRadarLink_drawTargetCircleCross(pObj, 1500, 750, 10, 1);
    GrpxRadarLink_drawLabel(pObj, 1540, 740, ":  -");
    GrpxRadarLink_drawTargetCircleCross(pObj, 1500, 800, 10, 2);
    GrpxRadarLink_drawLabel(pObj, 1540, 790, ":  +");
}

 /**
 *******************************************************************************
 *
 * \brief This function draws a part of a circle 
 * \param  pObj [IN] GrpxSrcLink_Obj
 * \param  UInt32 [IN] centerX
 * \param  UInt32 [IN] centerY
 * \param  UInt32 [IN] radius
 * \param  UInt32 [IN] degrees
 *
 *******************************************************************************
 */
Void GrpxRadarLink_drawPartCircle(
					GrpxSrcLink_Obj *pObj,
					UInt32 centerX,
					UInt32 centerY,
					UInt32 radius,
					UInt32 degrees
					)
{
	Draw2D_LinePrm linePrm;
	UInt32 color[2];
	
	//color[1] = RGB888_TO_RGB565(255, 255, 255);
    color[1] = RGB888_TO_RGB565(0, 128, 0);
	color[0] = RGB888_TO_RGB565(198, 198, 198);
	
	linePrm.lineColorFormat = SYSTEM_DF_BGR16_565;
    linePrm.lineColor = color[1];
    linePrm.lineSize = 2;
	
	double i;
	UInt32 x;
	UInt32 y;
	for (i = 0; i < degrees; i++){
            x = calcX(radius, i);
            y = calcY(radius, i);
			
			Draw2D_drawPixel(
				pObj->draw2DHndl,
				centerX + x,
				centerY - y,
				linePrm.lineColor,
				linePrm.lineColorFormat
				);
		
			Draw2D_drawPixel(
				pObj->draw2DHndl,
				centerX - x,
				centerY - y,
				linePrm.lineColor,
				linePrm.lineColorFormat
				);
	}
}



 /**
 *******************************************************************************
 *
 * \brief This function draws a full circle
 * \param  pObj [IN] GrpxSrcLink_Obj
 * \param  UInt32 [IN] centerX
 * \param  UInt32 [IN] centerY
 * \param  UInt32 [IN] radius
 *
 *******************************************************************************
 */
Void GrpxRadarLink_drawFullCircle(
					GrpxSrcLink_Obj *pObj,
					UInt32 centerX,
					UInt32 centerY,
					UInt32 radius
					)
{
	Draw2D_LinePrm linePrm;
	UInt32 color[2];
	
	color[1] = RGB888_TO_RGB565(255, 255, 255);
	color[0] = RGB888_TO_RGB565(198, 198, 198);
	
	linePrm.lineColorFormat = SYSTEM_DF_BGR16_565;
    linePrm.lineColor = color[1];
    linePrm.lineSize = 1;
	
	double degrees = 360;
	double i;
	UInt32 x;
	UInt32 y;
	
	for (i = 0; i < degrees; i++){
		x = calcX(radius, i / 4);
		y = calcY(radius, i / 4);
		
		Draw2D_drawPixel(
			pObj->draw2DHndl,
			centerX + x,
			centerY + y,
			linePrm.lineColor,
			linePrm.lineColorFormat
		);
		
		Draw2D_drawPixel(
			pObj->draw2DHndl,
			centerX + x,
			centerY - y,
			linePrm.lineColor,
			linePrm.lineColorFormat
		);
		
		Draw2D_drawPixel(
			pObj->draw2DHndl,
			centerX - x,
			centerY + y,
			linePrm.lineColor,
			linePrm.lineColorFormat
		);
		
		Draw2D_drawPixel(
			pObj->draw2DHndl,
			centerX - x,
			centerY - y,
			linePrm.lineColor,
			linePrm.lineColorFormat
		);
	}	
	
}

 /**
 *******************************************************************************
 *
 * \brief This function draws a monitoring grid to monitor the radar detected objects
 * \param  pObj [IN] GrpxSrcLink_Obj
 * \param  UInt32 [IN] startX
 * \param  UInt32 [IN] startY
 * \param  UInt32 [IN] stopX
 * \param  UInt32 [IN] stopY
 * \param  UInt32 [IN] gridWidth
 *
 *******************************************************************************
 */
Int32 GrpxRadarLink_drawMonitoringGrid(
					GrpxSrcLink_Obj *pObj,
					UInt32 startX,
					UInt32 startY,
					UInt32 stopX,
					UInt32 stopY,
					UInt32 gridWidth)
{
	
	Draw2D_LinePrm linePrm;
	UInt32 color[2];
	
	color[1] = RGB888_TO_RGB565(226, 226, 226);
	color[0] = RGB888_TO_RGB565(198, 198, 198);
	
	linePrm.lineColorFormat = SYSTEM_DF_BGR16_565;
    linePrm.lineColor = color[0];
    linePrm.lineSize = 1;
	
	UInt32 displayX = 0;
	UInt32 displayY = 0;
	
	for ( displayX = startX; displayX < stopX; displayX += gridWidth)
	{
		for ( displayY = startY; displayY < stopY; displayY += gridWidth)
		{
			Draw2D_drawRect(pObj->draw2DHndl,
				displayX,
				displayY,
				gridWidth,
				gridWidth,
				&linePrm
				);
		}
	}
	
	return 0;
}


/**
 *******************************************************************************
 *
 * \brief This function draws the sector lines to be able to display several angles
 * \param  pObj [IN] GrpxSrcLink_Obj
 * \param  UInt32 [IN] startX
 * \param  UInt32 [IN] startY
 * \param  UInt32 [IN] length
 *
 *******************************************************************************
 */
Int32 GrpxRadarLink_drawSectorLines(
						GrpxSrcLink_Obj *pObj,
						UInt32 startX,
						UInt32 startY,
						UInt32 length,
						UInt32 degree
						)
{
	Draw2D_LinePrm linePrm;
	UInt32 color[2];
    UInt32 deltaX, deltaY;
    UInt32 i;
	
	color[1] = RGB888_TO_RGB565(0, 128, 0);
    //color[1] = RGB888_TO_RGB565(226, 226, 226);
	color[0] = RGB888_TO_RGB565(255, 255, 255);
	
	linePrm.lineColorFormat = SYSTEM_DF_BGR16_565;
    linePrm.lineColor = color[1];
    linePrm.lineSize = 2;

    Draw2D_drawLine(pObj->draw2DHndl,
            startX,
            startY,
            startX,
            startY - length,
            &linePrm);

    deltaX = calcX(length, degree);
    deltaY = calcY(length, degree);

    Draw2D_drawLine(pObj->draw2DHndl,
            startX,
            startY,
            startX - deltaX,
            startY - deltaY,
            &linePrm);

    Draw2D_drawLine(pObj->draw2DHndl,
            startX,
            startY,
            startX + deltaX,
            startY - deltaY,
            &linePrm);

    for(i=1; degree > 15; i++)
    {
        deltaX = calcX(length, 15*i);
        deltaY = calcY(length, 15*i);

        Draw2D_drawLine(pObj->draw2DHndl,
                startX,
                startY,
                startX - deltaX,
                startY - deltaY,
                &linePrm);

        Draw2D_drawLine(pObj->draw2DHndl,
                startX,
                startY,
                startX + deltaX,
                startY - deltaY,
                &linePrm);

        degree -= 15;
    }

	return 0;
}


