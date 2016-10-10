/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#ifndef __RECTANGLE_H
#define __RECTANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*Dimensions of rectangle under car model*/
#define RECTANGLE_HALF_WIDTH (100)
#define RECTANGLE_HALF_HEIGHT (200)
#define RECTANGLE_OFFSET_Z (5)

/*RGB color of rectangle under car model*/
#define RECTANGLE_R (0.0f)
#define RECTANGLE_G (0.0f)
#define RECTANGLE_B (0.0f)

int rectangle_init(void);
void rectangle_draw(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RECTANGLE_H    */
