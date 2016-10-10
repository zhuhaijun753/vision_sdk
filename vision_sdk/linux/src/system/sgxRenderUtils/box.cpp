/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include "sgxRender3DSRV.h"
#include "box.h"

#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#define degreesToRadians(x) x*(3.141592f/180.0f)

/*Program & Shaders*/
extern int points_program;
extern GLint points_mvMatrixOffsetLoc;
extern GLuint vertexAttribPosition;
extern GLint colorLoc;

/* Projection & Camera matrix*/
extern glm::mat4 mProjection;
extern glm::mat4 mView;

/*VBOs*/
GLuint boxes_vbo[1];
GLuint boxes_index_vbo[2];
GLuint boxes_legend_vbo[1];
GLuint boxes_legend_index_vbo[2];

/*Parameters (global variables)*/
extern int inFramesWidth;
extern int inFramesHeight;

/*Defines*/
#define BOXES_RENDER_SURFACE_NUM_VERTICES (14)
#define BOXES_RENDER_EDGE_NUM_VERTICES (18)
#define BOXES_NUM_VERTICES (8)


int boxes_init()
{
	UInt32 ii;

    /*generate data buffers*/
    glGenBuffers(1, boxes_vbo);
    glGenBuffers(2, boxes_index_vbo);


    glGenBuffers(1, boxes_legend_vbo);
    glGenBuffers(2, boxes_legend_index_vbo);


    /*box surface indices*/
    GLushort boxes_surface_indices_single[] =
    	{0, 1, 3, 2, 6, 1, 5, 4, 6, 7, 3, 4, 0, 1};

	GLushort boxes_surface_indices[BOXES_RENDER_SURFACE_NUM_VERTICES*MAX_BOXES];
 	for (ii=0; ii<BOXES_RENDER_SURFACE_NUM_VERTICES*MAX_BOXES; ii++)
 	{
 		boxes_surface_indices[ii] = BOXES_NUM_VERTICES*(ii/BOXES_RENDER_SURFACE_NUM_VERTICES) +
 				boxes_surface_indices_single[ii%BOXES_RENDER_SURFACE_NUM_VERTICES];
 	}

 	/*box edge indices*/
 	GLushort boxes_edge_indices_single[] =
 		{0,1,5,4,0,3,7,4,5,6,7,3,2,6,5,1,2,1};

    GLushort boxes_edge_indices[BOXES_RENDER_EDGE_NUM_VERTICES*MAX_BOXES];
 	for (ii=0; ii<BOXES_RENDER_EDGE_NUM_VERTICES*MAX_BOXES; ii++)
 	{
 		boxes_edge_indices[ii] = BOXES_NUM_VERTICES*(ii/BOXES_RENDER_EDGE_NUM_VERTICES) +
 				boxes_edge_indices_single[ii%BOXES_RENDER_EDGE_NUM_VERTICES];
 	}

 	glBindBuffer(GL_ARRAY_BUFFER, boxes_vbo[0]);
 	glBufferData(GL_ARRAY_BUFFER, MAX_BOXES*3*BOXES_NUM_VERTICES*sizeof(float), (void *)0, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_index_vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxes_surface_indices), boxes_surface_indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_index_vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxes_edge_indices), boxes_edge_indices, GL_STATIC_DRAW);

    /*Legend*/
    float boxes_legend_vertices[] = {
    		SFM_DISP_LEGEND_OFFSETX1-SFM_DISP_LEGEND_SQUARE_WIDTH,  -SFM_DISP_LEGEND_SQUARE_WIDTH+SFM_DISP_LEGEND_OFFSETY, 0.0f,
    		SFM_DISP_LEGEND_OFFSETX1+SFM_DISP_LEGEND_SQUARE_WIDTH,  -SFM_DISP_LEGEND_SQUARE_WIDTH+SFM_DISP_LEGEND_OFFSETY, 0.0f,
    		SFM_DISP_LEGEND_OFFSETX1+SFM_DISP_LEGEND_SQUARE_WIDTH,   SFM_DISP_LEGEND_SQUARE_WIDTH+SFM_DISP_LEGEND_OFFSETY, 0.0f,
    		SFM_DISP_LEGEND_OFFSETX1-SFM_DISP_LEGEND_SQUARE_WIDTH,   SFM_DISP_LEGEND_SQUARE_WIDTH+SFM_DISP_LEGEND_OFFSETY, 0.0f,
    		SFM_DISP_LEGEND_OFFSETX2-SFM_DISP_LEGEND_SQUARE_WIDTH,  -SFM_DISP_LEGEND_SQUARE_WIDTH+SFM_DISP_LEGEND_OFFSETY, 0.0f,
    		SFM_DISP_LEGEND_OFFSETX2+SFM_DISP_LEGEND_SQUARE_WIDTH,  -SFM_DISP_LEGEND_SQUARE_WIDTH+SFM_DISP_LEGEND_OFFSETY, 0.0f,
    		SFM_DISP_LEGEND_OFFSETX2+SFM_DISP_LEGEND_SQUARE_WIDTH,   SFM_DISP_LEGEND_SQUARE_WIDTH+SFM_DISP_LEGEND_OFFSETY, 0.0f,
    		SFM_DISP_LEGEND_OFFSETX2-SFM_DISP_LEGEND_SQUARE_WIDTH,   SFM_DISP_LEGEND_SQUARE_WIDTH+SFM_DISP_LEGEND_OFFSETY, 0.0f};
 	glBindBuffer(GL_ARRAY_BUFFER, boxes_legend_vbo[0]);
 	glBufferData(GL_ARRAY_BUFFER, sizeof(boxes_legend_vertices), boxes_legend_vertices, GL_STATIC_DRAW);

 	GLushort boxes_legend_indices[] = {0,1,3,2,4,5,7,6};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_legend_index_vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxes_legend_indices), boxes_legend_indices, GL_STATIC_DRAW);

    GLushort boxes_legend_edge_indices[] = {4,5,6,7,4};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_legend_index_vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxes_legend_edge_indices), boxes_legend_edge_indices, GL_STATIC_DRAW);

    return 0;
}

/*=======================================================================
*
* Name:        void boxes_draw(float boxes_local[][24], UInt32 numBoxes,
* 						Pose3D_f *pose, int viewID)
*
* Description: draw boxes in ego and world view
*
=======================================================================*/
void boxes_draw(float boxes_local[][24], UInt32 numBoxes,
					Pose3D_f *pose, int viewID)
{

	UInt32 ii;
	glm::mat4 mMVP_box;

	if (numBoxes==0)
		return;

	if (viewID==0)
	{
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1/SV3D_STEP_SIZE, 1/SV3D_STEP_SIZE, 1/SV3D_STEP_SIZE));
		/*copy pose into local variable*/
		float m[16] = {(float)pose->R.xx, (float)pose->R.yx, (float)pose->R.zx, 0,
					(float)pose->R.xy, (float)pose->R.yy, (float)pose->R.zy, 0,
					(float)pose->R.xz, (float)pose->R.yz, (float)pose->R.zz, 0,
					(float)pose->t.x, (float)pose->t.y, (float)pose->t.z, 1};

		glm::mat4 mModel_box = glm::make_mat4(m);

		mMVP_box        = mProjection * mView * scale * mModel_box;
	}
	else
	{
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(SFM_DISP_WORLDMAP_SCALE, SFM_DISP_WORLDMAP_SCALE, SFM_DISP_WORLDMAP_SCALE));
		glm::mat4 mProjection2 =glm::ortho(-2000.0f*SFM_DISP_WORDLMAP_ASPECT_RATIO, 2000.0f*SFM_DISP_WORDLMAP_ASPECT_RATIO, -2000.0f, 2000.0f, 1.0f, 1000.0f);
		glm::mat4 mView2 = glm::lookAt(
								glm::vec3(0, 0, SFM_DISP_WORLDMAP_VIEW_DISTANCE),
							    glm::vec3(0, 0, 0),
								glm::vec3(0, 1, 0)
									  );
		mMVP_box   =  mProjection2 * mView2 * scale;
	}

    glUseProgram(points_program);
    GL_CHECK(glUseProgram);

    glUniformMatrix4fv(points_mvMatrixOffsetLoc, 1, GL_FALSE, &mMVP_box[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, boxes_vbo[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBoxes*24*sizeof(float), (void *)boxes_local[0]);

	glVertexAttribPointer(vertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexAttribPosition);

	 /*surfaces*/
	 /****************/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_index_vbo[0]);

	glUniform4f(colorLoc, BOXES_COLOR_R, BOXES_COLOR_G, BOXES_COLOR_B, 0.5f);
    for (ii = 0; ii < numBoxes; ii++)
    {
  		glDrawElements(GL_TRIANGLE_STRIP, BOXES_RENDER_SURFACE_NUM_VERTICES, GL_UNSIGNED_SHORT,  (void *)(BOXES_RENDER_SURFACE_NUM_VERTICES * ii * sizeof(GLushort)));
    }

	 /*edges*/
	 /****************/
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_index_vbo[1]);

    glUniform4f(colorLoc, BOXES_COLOR_R, BOXES_COLOR_G, BOXES_COLOR_B, 1.0f);
    glLineWidth(2.0f);
    for (ii = 0; ii < numBoxes; ii++)
    {
  		glDrawElements(GL_LINE_LOOP, BOXES_RENDER_EDGE_NUM_VERTICES, GL_UNSIGNED_SHORT,  (void *)(BOXES_RENDER_EDGE_NUM_VERTICES * ii * sizeof(GLushort)));
    }

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

}

/*=======================================================================
*
* Name:        void boxes_inputImage_draw(float boxes_local[][24],
* 						UInt32 numBoxes, Pose3D_f *poses, UInt32 camNo)
*
* Description: draw boxes in input image views
*
=======================================================================*/
void boxes_inputImage_draw(float boxes_local[][24], UInt32 numBoxes,
								Pose3D_f *poses, UInt32 camNo)
{

	float boxes_projected[MAX_BOXES][24];
	UInt32 numBoxes_projected;
	boxes_projectToInputImage(boxes_local, numBoxes, poses, camNo, boxes_projected, &numBoxes_projected);

	if (numBoxes_projected==0)
		return;

	 UInt32 ii;

	 glm::mat4 mMVP_box = glm::ortho(0.0f, (float)inFramesWidth, 0.0f, (float)inFramesHeight, -1.0f, 1.0f);

	 glUseProgram(points_program);
	 GL_CHECK(glUseProgram);

	 glUniformMatrix4fv(points_mvMatrixOffsetLoc, 1, GL_FALSE, &mMVP_box[0][0]);

	 glEnable(GL_BLEND);
	 glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	 glBindBuffer(GL_ARRAY_BUFFER, boxes_vbo[0]);
	 glBufferSubData(GL_ARRAY_BUFFER, 0, 24*numBoxes_projected*sizeof(float), (void *)boxes_projected);

	 glVertexAttribPointer(vertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	 glEnableVertexAttribArray(vertexAttribPosition);

	 /*surfaces*/
	 /****************/
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_index_vbo[0]);

	 glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 0.3f);
	 for (ii = 0; ii < numBoxes_projected; ii++)
	 {
		glDrawElements(GL_TRIANGLE_STRIP, BOXES_RENDER_SURFACE_NUM_VERTICES, GL_UNSIGNED_SHORT,
				(void *)(BOXES_RENDER_SURFACE_NUM_VERTICES * ii * sizeof(GLushort)));
	 }

	 glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	 /*edges*/
	 /****************/
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_index_vbo[1]);

	 glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f);
	 glLineWidth(2.0f);
	 for (ii = 0; ii < numBoxes_projected; ii++)
	 {
		glDrawElements(GL_LINE_LOOP, BOXES_RENDER_EDGE_NUM_VERTICES, GL_UNSIGNED_SHORT,
				(void *)(BOXES_RENDER_EDGE_NUM_VERTICES * ii * sizeof(GLushort)));
	 }

	 glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	 glDisable(GL_BLEND);

	return;
}


/*=======================================================================
*
* Name: void boxes_projectToInputImage(float boxes[][24], UInt32 numBoxes,
* 				Pose3D_f *poses, UInt32 camNo, float boxes_projected[][24],
* 				UInt32 *numBoxes_projected)
*
* Description: for each box, check if it visible in camera camNo, and
* 			project to input image if so
*
=======================================================================*/
void boxes_projectToInputImage(float boxes[][24], UInt32 numBoxes,
		Pose3D_f *poses, UInt32 camNo, float boxes_projected[][24],
		UInt32 *numBoxes_projected)
{
	UInt32 bb,vv, numBoxesKeep;
	UInt32 bAllVerticesInView;
	Point3D_f xyzc;
	Point3D_f xyzw;
	Point3D_f xyd;
	UInt32 status;

	numBoxesKeep = 0;
	for (bb=0; bb<numBoxes; bb++)
	{
		bAllVerticesInView = 1;
		for (vv=0; vv<BOXES_NUM_VERTICES; vv++)
		{
			xyzw.x = boxes[bb][3*vv];
			xyzw.y = boxes[bb][3*vv+1];
			xyzw.z = boxes[bb][3*vv+2];
			pose3Dtransform(&poses[camNo], &xyzw, &xyzc);
			status = project3DPointToImage(&xyzc, &xyd);
			if (status==0)
			{
				boxes_projected[numBoxesKeep][3*vv] =   xyd.x;
				boxes_projected[numBoxesKeep][3*vv+1] = (float)inFramesHeight-xyd.y;
				boxes_projected[numBoxesKeep][3*vv+2] = xyd.z;
			}
			else
			{
				bAllVerticesInView = 0;
				break;
			}
		}
		if (bAllVerticesInView)
		{
			numBoxesKeep++;
		}
	}

	*numBoxes_projected = numBoxesKeep;

	return;
}

/*=======================================================================
*
* Name: void boxes_legend_draw();
*
* Description: draw legend
*
=======================================================================*/
void boxes_legend_draw()
{
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 mProjection2 =glm::ortho(-500.0f, 500.f, -80.0f, 80.0f, 1.0f, 1000.0f);
	glm::mat4 mView2 = glm::lookAt(
							glm::vec3(0, 0, 10),
						    glm::vec3(0, 0, 0),
							glm::vec3(0, 1, 0)
								  );
	glm::mat4 mMVP_box   =  mProjection2 * mView2 * scale;

	glUseProgram(points_program);
	GL_CHECK(glUseProgram);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniformMatrix4fv(points_mvMatrixOffsetLoc, 1, GL_FALSE, &mMVP_box[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, boxes_legend_vbo[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_legend_index_vbo[0]);

	glVertexAttribPointer(vertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexAttribPosition);

	/*free space square*/
	glUniform4f(colorLoc, FREE_SPACE_COLOR_R, FREE_SPACE_COLOR_G, FREE_SPACE_COLOR_B, 0.3f);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT,  (void *)0);

	/*occupied space square*/
    glUniform4f(colorLoc, BOXES_COLOR_R, BOXES_COLOR_G, BOXES_COLOR_B, 0.5f);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT,  (void *)(4*sizeof(GLushort)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_legend_index_vbo[1]);
    glUniform4f(colorLoc, BOXES_COLOR_R, BOXES_COLOR_G, BOXES_COLOR_B, 1.0f);
    glLineWidth(2.0f);
    glDrawElements(GL_LINE_LOOP, 5, GL_UNSIGNED_SHORT,  (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);


	return;
}


