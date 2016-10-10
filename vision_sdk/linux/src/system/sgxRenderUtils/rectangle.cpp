/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include "sgxRender3DSRV.h"
#include "rectangle.h"

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

/* VBOs */
GLuint rectangle_vbo[1];
GLuint rectangle_index_vbo[1];

/*=======================================================================
*
* Name:        rectangle_init()
*
=======================================================================*/
int rectangle_init()
{
    /*generate data & index  buffers*/
    glGenBuffers(1, rectangle_vbo);
    glGenBuffers(1, rectangle_index_vbo);

    /*predefined rectangle coordinates*/
    float rectangle[] = {
    		-RECTANGLE_HALF_WIDTH,  -RECTANGLE_HALF_HEIGHT, RECTANGLE_OFFSET_Z,
    		+RECTANGLE_HALF_WIDTH,  -RECTANGLE_HALF_HEIGHT, RECTANGLE_OFFSET_Z,
    		+RECTANGLE_HALF_WIDTH,  +RECTANGLE_HALF_HEIGHT, RECTANGLE_OFFSET_Z,
    		-RECTANGLE_HALF_WIDTH,  +RECTANGLE_HALF_HEIGHT, RECTANGLE_OFFSET_Z
    };

    /*write data into buffer*/
 	glBindBuffer(GL_ARRAY_BUFFER, rectangle_vbo[0]);
 	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);
 	
 	/*write indices into buffer*/
    GLushort rectangle_surface_indices[] = {0,1,3,2};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangle_index_vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangle_surface_indices), rectangle_surface_indices, GL_STATIC_DRAW);

    return 0;
}

/*=======================================================================
*
* Name:        rectangle_under_car_draw()
*
=======================================================================*/
void rectangle_draw()
{

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1/SV3D_STEP_SIZE, 1/SV3D_STEP_SIZE, 1/SV3D_STEP_SIZE));
	glm::mat4 mMVP       = mProjection * mView * scale;

    glUseProgram(points_program);
    GL_CHECK(glUseProgram);

    glUniformMatrix4fv(points_mvMatrixOffsetLoc, 1, GL_FALSE, &mMVP[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, rectangle_vbo[0]);
	glVertexAttribPointer(vertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexAttribPosition);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangle_index_vbo[0]);

	glUniform4f(colorLoc, RECTANGLE_R, RECTANGLE_G, RECTANGLE_B, 1.0f);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT,  (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

}

