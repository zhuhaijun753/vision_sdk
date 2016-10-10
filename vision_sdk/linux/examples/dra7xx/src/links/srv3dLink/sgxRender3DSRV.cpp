/*
 *******************************************************************************
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include "sgxRender3DSRV.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <math.h>

static int stride = 3+2+2+1+1;
static void * prevLUT=(void *)0xdead;
static void * prevblendLUT=(void *)0xbeef;


#define MAX_INDEX_BUFFERS 2

typedef struct {
	unsigned short *buffer;
	unsigned int length;
} t_index_buffer;

t_index_buffer index_buffers[MAX_INDEX_BUFFERS];
unsigned int active_index_buffer = 1;
bool index_buffer_changed = 0;

#define QUADRANTS 4
#define QUADRANT_WIDTH ((POINTS_WIDTH/2)+1)
#define QUADRANT_HEIGHT ((POINTS_HEIGHT/2)+1)

pthread_t scan_thread;
int scan_thread_data;


// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 mProjection;
// Camera matrix
glm::mat4 mView;
// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 mModel_bowl;  // Changes for each model !
glm::mat4 mModel_car;  // Changes for each model !
// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 mMVP_bowl;
glm::mat4 mMVP_car;


srv_coords_t srv_coords[] = {
		{0.000000, 0.000000, 640.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
		{0.000000, 0.000000, 440.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
//		{0.000000, -360.000000, 180.000000, 0.000000, 1248.000000, -30.000000, -0.300000, 0.000000, 0.000000}
		{0.000000, 200.000000, 240.000000, -1.000000, 80.000000, 35.000000, -1.500000, 0.000000, 0.000000}
		//{0.000000, 20.000000, 340.000000, 0.000000, 125.000000, 0.000000, -1.500000, 0.000000, 0.000000}
		//{0.000000, -280.000000, 160.000000, 0.000000, 205.000000, 0.000000, 0.000000, 0.000000, 0.000000},
		//{0.000000, 0.000000, 1020.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
		//{0.000000, 0.000000, 500.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
		//{0.000000, -20.000000, 460.000000, 5.000000, 124.000000, 10.000000, -1.200000, 0.000000, -3.200001}

};

//#define ENABLE_CAR_SIDE_PANES 1
//#define ENABLE_GLOBAL_BLENDING 1
#define ENABLE_BOWL_ROTATION 1
#define ENABLE_SGX_RENDERED_PREVIEW 1

//Layout
#define LAYOUT_NUM_SINGLEVIEWS_HORIZ 2
#define LAYOUT_NUM_SINGLEVIEWS_VERT 2
//Index generation logic
#define MAX_POINTS_WIDTH 440
#define MAX_POINTS_HEIGHT 540

//Mesh splitting logic
#define MAX_VBO_MESH_SPLIT 8
static GLuint vboId[MAX_VBO_MESH_SPLIT*3];

//int points_init_vertices_vbo();
//void points_draw_vbo();
static const char gSgxRender3DSRV_vertexShader_1VBO[] =
        "  attribute vec3 aVertexPosition;\n "
        "  attribute vec2 aTextureCoord1;\n "
        "  attribute vec2 aTextureCoord2;\n "
        "  attribute vec2 blendVals;\n "
        "  uniform mat4 uMVMatrix;\n "
        "  varying vec2 outNormTexture;\n "
        "  varying vec2 outNormTexture1;\n "
        "  varying vec2 outBlendVals;\n "
        "  mediump float RANGE_X = 220.0; \n "
        "  mediump float RANGE_Y = 270.0; \n "
        "  mediump float TEXTURE_X = 1280.0*16.0; \n "
        "  mediump float TEXTURE_Y = 720.0*16.0;\n "

 " void main(void) {\n "
 "     gl_Position = uMVMatrix * vec4((-RANGE_X+aVertexPosition.x), (RANGE_Y-aVertexPosition.y), aVertexPosition.z, 1.0);\n "
		"     outNormTexture.x = aTextureCoord1.t/TEXTURE_X;\n"
		"     outNormTexture.y = aTextureCoord1.s/TEXTURE_Y;\n"
		"     outNormTexture1.x = aTextureCoord2.t/TEXTURE_X;\n"
		"     outNormTexture1.y = aTextureCoord2.s/TEXTURE_Y;\n"
		"     outBlendVals = blendVals;\n"
 "}\n"
;

static const char gSgxRender3DSRV_fragmentShader_1VBO[] =
#ifndef STANDALONE
        " #extension GL_OES_EGL_image_external : require \n"
#endif
        " precision mediump float;\n "
#ifndef STANDALONE
        " uniform samplerExternalOES uSampler[2];\n "
#else
        " uniform sampler2D uSampler[2];\n "
#endif
        " varying vec2 outNormTexture;\n "
        " varying vec2 outNormTexture1;\n "
        " varying vec2 outBlendVals;\n "
		" vec4 iFragColor1; \n "
        " vec4 iFragColor2; \n "
        " void main(){\n"
		"     iFragColor1 = texture2D(uSampler[0], outNormTexture);\n "
		"     iFragColor2 = texture2D(uSampler[1], outNormTexture1);\n "
        "     gl_FragColor = (outBlendVals.x)*iFragColor1 + (outBlendVals.y)*iFragColor2;\n "
        " }\n"
   ;

#ifdef STANDALONE
void System_eglCheckEglError(const char* op, EGLBoolean returnVal) {
   EGLint error;

   if (returnVal != EGL_TRUE) {
       printf(" EGL: %s() returned %d\n", op, returnVal);
   }

   for (error = eglGetError(); error != EGL_SUCCESS; error = eglGetError()) {
       printf(" EGL: after %s() eglError (0x%x)\n", op, error);
   }
}

void System_eglCheckGlError(const char* op) {
   GLint error;

   for (error = glGetError(); error; error = glGetError()) {
       printf("GL: after %s() glError (0x%x)\n", op, error);
   }
}
#endif


static struct termios oldt;

void restore_terminal_settings(void)
{
    tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
}

void disable_waiting_for_enter(void)
{
    struct termios newt;

    /* Make terminal read 1 char at a time */
    tcgetattr(0, &oldt);  /* Save terminal settings */
    printf("Terminal setting flags:0x%x", oldt.c_lflag);
    newt = oldt;  /* Init new settings */
    newt.c_lflag &= ~(ICANON | ECHO);  /* Change settings */
    tcsetattr(0, TCSANOW, &newt);  /* Apply settings */
    atexit(restore_terminal_settings); /* Make sure settings will be restored when program ends  */
}

enum e_mode {
	cam,
	target,
	angle
} mode;
float camx = 0.0;
float camy = 0.0;
float camz = 800.0f;
float targetx = 0.0;
float targety = 0.0;
float targetz = 0.0;
float anglex = 0.0f;
float angley = 0.0f;
float anglez = 0.0f;
float cam_delta = 20.0f;
float target_delta = 5.0f;
float angle_delta = 0.5f;
float cam_delta_inc = 20.0f;
float target_delta_inc = 1.0f;
float angle_delta_inc = 0.1f;
bool animate = false;
GLenum render_mode = GL_TRIANGLE_STRIP;

#define COORD_TRANSITION(x, i, j) \
  x = x + j * ((srv_coords[i].x - x)/num_iterations)


static int gap = 50;
static int num_iterations = 450;
static int current_index = 0;
static int step = 0;
static int num_coordsets;
bool animatengap = false;

void animate_to_coords(int index, unsigned int usec_between_frames)
{
	int j;
	for(j = 0; j < num_iterations; j++)
	{
		COORD_TRANSITION(camx, index, j);
		COORD_TRANSITION(camy, index, j);
		COORD_TRANSITION(camz, index, j);
		COORD_TRANSITION(targetx, index, j);
		COORD_TRANSITION(targety, index, j);
		COORD_TRANSITION(targetz, index, j);
		COORD_TRANSITION(anglex, index, j);
		COORD_TRANSITION(angley, index, j);
		COORD_TRANSITION(anglez, index, j);
		SgxRender3DSRV_updateView();
		//car_x = srv_coords[i].car_x + j * ((srv_coords[(i+1)%num_coordsets].car_x - srv_coords[i].car_x)/num_iterations);
		usleep(usec_between_frames);
	}
}
#if 0
void *scan_thread_function(void *ptr)
{
//	char input;
	int i, j;
//	disable_waiting_for_enter();
	num_coordsets = (int)(sizeof(srv_coords)/sizeof(srv_coords_t));
	//printf("Number of coordinate sets: %d", num_coords);
	while(1)
	{
		for (i = 0; i < num_coordsets; i++)
		{
			animate_to_coords(i);
			sleep(2);
			if (i >= num_coordsets)
				i = 0;
		}
	}
}
#else
void *scan_thread_function(void *ptr)
{
	char input;
	float *delta = &cam_delta;
	float *delta_inc = &cam_delta_inc;
	float *x = &camx, *y = &camy, *z = &camz;
	disable_waiting_for_enter();
	while(1)
	{
		input = getchar();
		switch(input)
		{
		case 'c':
			mode = cam;
			delta = &cam_delta;
			delta_inc = &cam_delta_inc;
			x = &camx;
			y = &camy;
			z = &camz;
			break;
		case 't':
			mode = target;
			delta = &target_delta;
			delta_inc = &target_delta_inc;
			x = &targetx;
			y = &targety;
			z = &targetz;
			break;
		case 'a':
			mode = angle;
			delta = &angle_delta;
			delta_inc = &angle_delta_inc;
			x = &anglex;
			y = &angley;
			z = &anglez;
			break;
		case 'i':
			if(*delta < 0)
				*delta = -*delta;
			break;
		case 'd':
			if(*delta > 0)
				*delta = -*delta;
			break;
		case 'x':
				*x += *delta;
			break;
		case 'y':
				*y += *delta;
			break;
		case 'z':
				*z += *delta;
			break;
		case 'm':
			if(*delta >=0)
				*delta += *delta_inc;
			else
				*delta -= *delta_inc;
			printf("Delta: %f\n", *delta);
			break;
		case 'l' :
			if(*delta >=0)
				*delta -= *delta_inc;
			else
				*delta += *delta_inc;
			break;
		case 'g' :
			if(active_index_buffer == 0)
				active_index_buffer = 1;
			else
				active_index_buffer = 0;
			index_buffer_changed = true;
			break;
		case '1':
			animate = false;
			animate_to_coords(0, 6000);
			break;
		case '2':
			animate = false;
			animate_to_coords(1, 6000);
			break;
		case '3':
			animate = false;
			animate_to_coords(2, 6000);
			break;
		case 'j':
			animate = true;
			break;
		case 'k':
			animate = false;
			break;
		case 'q':
			camx = srv_coords[0].camx;
			camy = srv_coords[0].camy;
			camz = srv_coords[0].camz;
			targetx = srv_coords[0].targetx;
			targety = srv_coords[0].targety;
			targetz = srv_coords[0].targetz;
			anglex = srv_coords[0].anglex;
			angley = srv_coords[0].angley;
			anglez = srv_coords[0].anglez;
			break;
		case 'n':
			if(render_mode == GL_LINE_STRIP)
				render_mode = GL_TRIANGLE_STRIP;
			else
				render_mode = GL_LINE_STRIP;
			break;
		default:
			break;
		}
		SgxRender3DSRV_updateView();
		printf("%f, %f, %f, %f, %f, %f, %f, %f, %f\n",
				camx,
				camy,
				camz,
				targetx,
				targety,
				targetz,
				anglex,
				angley,
				anglez);
	}
}
#endif
#define degreesToRadians(x) x*(3.141592f/180.0f)

void  SgxRender3DSRV_updateView()
{
	mProjection = glm::perspective(degreesToRadians(40), 880.0f / 1080.0f, 1.0f, 5000.0f);
	mView       = glm::lookAt(
			glm::vec3(camx, camy, camz), // Camera is at (4,3,3), in World Space
			glm::vec3(targetx,targety,targetz), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	mView = glm::rotate(mView, anglex, glm::vec3(1.0, 0.0, 0.0));
	mView = glm::rotate(mView, angley, glm::vec3(0.0, 1.0, 0.0));
	mView = glm::rotate(mView, anglez, glm::vec3(0.0, 0.0, 1.0));
	mModel_bowl     = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, (80.0f/540.0f)));  // Changes for each model !
	mModel_car      = glm::scale(
			glm::mat4(1.0f),
			glm::vec3(0.02f));  // Changes for each model !
	mModel_car = glm::rotate(mModel_car, degreesToRadians(90), glm::vec3(1.0, 0.0, 0.0));
	mModel_car = glm::rotate(mModel_car, degreesToRadians(-90), glm::vec3(0.0, 1.0, 0.0));
	//mModel_car = glm::translate(mModel_car, glm::vec3(0.0, 0.0, 800.0));
	mMVP_bowl       = mProjection * mView * mModel_bowl;
	mMVP_car        = mProjection * mView * mModel_car;
}

void generate_indices(t_index_buffer *index_buffer, int xlength, int ylength, int gap)
{
	   unsigned short *buffer = index_buffer->buffer;
       unsigned int x, y, k=0;
       for (y=0; y<ylength-gap; y+=gap)
       {
               if(y>0)
                       buffer[k++]=(unsigned short) (y*xlength);
               for (x=0; x<xlength; x+=gap)
               {
                       buffer[k++]=(unsigned short) (y*xlength + x);
                       buffer[k++]=(unsigned short) ((y+gap)*xlength + x);
               }
               if(y < ylength - 1 - gap)
                       buffer[k++]=(unsigned short) ((y+gap)*xlength + (xlength -1));
       }
       index_buffer->length = k;
}



int generate_short_indices(int width, int height, unsigned short* out)
{
    int i, j;
    unsigned int indexlen = 0;
    if(width > MAX_POINTS_WIDTH) return -1;
    if(height > MAX_POINTS_HEIGHT) return -1;
    for(i = 0;i < height-1;i ++)
    {
        int inc = i*width;
        for(j = 0;j < width-1;j ++)
        {
            out[indexlen + 0] = 0+inc;
            out[indexlen + 1] = width+inc;
            out[indexlen + 2] = width+1+inc;
            out[indexlen + 3] = 0+inc;
            out[indexlen + 4] = width+1+inc;
            out[indexlen + 5] = 1+inc;
            inc ++;
            indexlen += 6;
        }
    }
    return 0;
}   
   
GLuint SgxRender3DSRV_loadShader(GLenum shaderType, const char* pSource) {
   GLuint shader = glCreateShader(shaderType);
   if (shader) {
       glShaderSource(shader, 1, &pSource, NULL);
       glCompileShader(shader);
       GLint compiled = 0;
       glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
       if (!compiled) {
           GLint infoLen = 0;
           glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
           if (infoLen) {
               char* buf = (char*) malloc(infoLen);
               if (buf) {
                   glGetShaderInfoLog(shader, infoLen, NULL, buf);
                   D_PRINTF(" GL: Could not compile shader %d:\n%s\n",
                       shaderType, buf);
                   free(buf);
               }
           } else {
               D_PRINTF(" GL: Guessing at GL_INFO_LOG_LENGTH size\n");
               char* buf = (char*) malloc(0x1000);
               if (buf) {
                   glGetShaderInfoLog(shader, 0x1000, NULL, buf);
                   D_PRINTF(" GL: Could not compile shader %d:\n%s\n",
                   shaderType, buf);
                   free(buf);
               }
           }
           glDeleteShader(shader);
           shader = 0;
       }
   }
   return shader;
}

GLuint SgxRender3DSRV_createProgram(const char* pVertexSource, const char* pFragmentSource) {
   GLuint vertexShader = SgxRender3DSRV_loadShader(GL_VERTEX_SHADER, pVertexSource);
   if (!vertexShader) {
       return 0;
   }

   GLuint pixelShader = SgxRender3DSRV_loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
   if (!pixelShader) {
       return 0;
   }

   GLuint program = glCreateProgram();
   if (program) {
       glAttachShader(program, vertexShader);
       //System_eglCheckGlError("glAttachShader");
       glAttachShader(program, pixelShader);
       //System_eglCheckGlError("glAttachShader");
       glLinkProgram(program);
       GLint linkStatus = GL_FALSE;
       glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
       if (linkStatus != GL_TRUE) {
           GLint bufLength = 0;
           glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
           if (bufLength) {
               char* buf = (char*) malloc(bufLength);
               if (buf) {
                   glGetProgramInfoLog(program, bufLength, NULL, buf);
                   D_PRINTF(" GL: Could not link program:\n%s\n", buf);
                   free(buf);
               }
           }
           glDeleteProgram(program);
           program = 0;
       }
   }
   if(vertexShader && pixelShader && program)
   {
     glDeleteShader(vertexShader);
     glDeleteShader(pixelShader);
    }
   return program;
}

//Vertices init for surround view (VBO approach)
static int surroundview_init_vertices_vbo(SgxRender3DSRV_Obj *pObj, GLuint vertexId, GLuint blendId, GLuint indexId, 
                                          void* vertexBuff, void* blendBuff, void* indexBuff,
                                          int vertexBuffSize, int blendBuffSize, int indexBuffSize
                                          )
{  
     //upload the vertex and texture and image index interleaved array
     //Bowl LUT - Interleaved data (5 data)
     glBindBuffer(GL_ARRAY_BUFFER, vertexId);

     glBufferData(GL_ARRAY_BUFFER, vertexBuffSize, vertexBuff, GL_STATIC_DRAW);
     glVertexAttribPointer(pObj->vertexPositionAttribLoc, 3, GL_LUT_DATATYPE, GL_FALSE, stride*sizeof(LUT_DATATYPE), 0);     
     
     glVertexAttribPointer(pObj->vertexTexCoord1AttribLoc, 2, GL_LUT_DATATYPE, GL_FALSE, (stride)*sizeof(LUT_DATATYPE), (GLvoid*)(3*sizeof(LUT_DATATYPE)));
     glVertexAttribPointer(pObj->vertexTexCoord2AttribLoc, 2, GL_LUT_DATATYPE, GL_FALSE, (stride)*sizeof(LUT_DATATYPE), (GLvoid*)(5*sizeof(LUT_DATATYPE)));
     GL_CHECK(glVertexAttribPointer);

     //blend LUT
     glBindBuffer(GL_ARRAY_BUFFER, blendId);
     glBufferData(GL_ARRAY_BUFFER, blendBuffSize, blendBuff, GL_STATIC_DRAW);
     glVertexAttribPointer(pObj->blendAttribLoc, 2, GL_BLENDLUT_DATATYPE, GL_TRUE, 2*sizeof(char), 0);
     GL_CHECK(glVertexAttribPointer);
                                                 
     //Index buffer
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffSize, indexBuff, GL_STATIC_DRAW);
     GL_CHECK(glBufferData);

     //Enable for the rendering
     glEnableVertexAttribArray(pObj->vertexPositionAttribLoc);
     glEnableVertexAttribArray(pObj->vertexTexCoord1AttribLoc);
     glEnableVertexAttribArray(pObj->vertexTexCoord2AttribLoc);

     glEnableVertexAttribArray(pObj->blendAttribLoc);     

     return 0;
}

void surroundview_init_vertices_vbo_wrap(SgxRender3DSRV_Obj *pObj)
{
 int i;
 int vertexoffset = 0, blendoffset = 0;
 
	glGenBuffers(QUADRANTS*3, vboId);
	for(i = 0;i < QUADRANTS;i ++)
	{
		vertexoffset = i * (sizeof(LUT_DATATYPE)*stride*QUADRANT_WIDTH*QUADRANT_HEIGHT);
		blendoffset = i * (sizeof(BLENDLUT_DATATYPE)*2*QUADRANT_WIDTH*QUADRANT_HEIGHT);

		surroundview_init_vertices_vbo(
     pObj,
			vboId[i*3], vboId[i*3+1], vboId[i*3+2],
			(char*)pObj->LUT3D + vertexoffset,
			(char*)pObj->blendLUT3D + blendoffset, 
			(char*)(index_buffers[active_index_buffer].buffer),
			sizeof(LUT_DATATYPE)*stride*QUADRANT_WIDTH*QUADRANT_HEIGHT,
			sizeof(BLENDLUT_DATATYPE)*2*QUADRANT_WIDTH*QUADRANT_HEIGHT,
			sizeof(short)*(index_buffers[active_index_buffer].length)
			);
	}
	index_buffer_changed = false;
}

#define RANGE_X_MESH 220.0
#define RANGE_Y_MESH 270.0
#define RANGE_Z_MESH 430.0
void onscreen_mesh_state_restore_program_textures_attribs(SgxRender3DSRV_Obj *pObj, GLuint *texYuv, int tex1, int tex2)
{
     //set the program we need
     glUseProgram(pObj->uiProgramObject);

     glUniform1i(pObj->samplerLocation0, 0);
     glActiveTexture(GL_TEXTURE0);

#ifndef STANDALONE
     glBindTexture(GL_TEXTURE_EXTERNAL_OES, texYuv[tex1]);
     glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
     glBindTexture(GL_TEXTURE_2D, texYuv[tex1]);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
     GL_CHECK(glBindTexture);

     glUniform1i(pObj->samplerLocation1, 1);  
     glActiveTexture(GL_TEXTURE1);

#ifndef STANDALONE
     glBindTexture(GL_TEXTURE_EXTERNAL_OES, texYuv[tex2]);
     glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
     glBindTexture(GL_TEXTURE_2D, texYuv[tex2]);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
     GL_CHECK(glBindTexture);

     //Enable the attributes
     glEnableVertexAttribArray(pObj->vertexPositionAttribLoc);
     glEnableVertexAttribArray(pObj->vertexTexCoord1AttribLoc);
     glEnableVertexAttribArray(pObj->vertexTexCoord2AttribLoc);

     glEnableVertexAttribArray(pObj->blendAttribLoc);

#ifdef ENABLE_BOWL_ROTATION
    glUniformMatrix4fv(pObj->mvMatrixLocation, 1, GL_FALSE, &mMVP_bowl[0][0]);
    GL_CHECK(glUniformMatrix4fv);     	 
#endif	
}

void onscreen_mesh_state_restore_vbo(SgxRender3DSRV_Obj *pObj,
                                        GLuint vertexId, GLuint blendId, GLuint indexId)
{
           
     //restore the vertices and indices
     glBindBuffer(GL_ARRAY_BUFFER, vertexId);
     glVertexAttribPointer(pObj->vertexPositionAttribLoc, 3, GL_LUT_DATATYPE, GL_FALSE, stride*sizeof(LUT_DATATYPE), 0);
     glVertexAttribPointer(pObj->vertexTexCoord1AttribLoc, 2, GL_LUT_DATATYPE, GL_FALSE, (stride)*sizeof(LUT_DATATYPE), (GLvoid*)(3*sizeof(LUT_DATATYPE)));
     glVertexAttribPointer(pObj->vertexTexCoord2AttribLoc, 2, GL_LUT_DATATYPE, GL_FALSE, (stride)*sizeof(LUT_DATATYPE), (GLvoid*)(5*sizeof(LUT_DATATYPE)));

     glBindBuffer(GL_ARRAY_BUFFER, blendId);
     glVertexAttribPointer(pObj->blendAttribLoc, 2, GL_BLENDLUT_DATATYPE, GL_TRUE, 2*sizeof(char), 0);

     //Index buffer
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
}


int SgxRender3DSRV_setup(SgxRender3DSRV_Obj *pObj)
{
    //STEP1 - shader setup
    pObj->uiProgramObject = SgxRender3DSRV_createProgram(
                        gSgxRender3DSRV_vertexShader_1VBO,
                        gSgxRender3DSRV_fragmentShader_1VBO
                     );
    if (pObj->uiProgramObject==0)
    {
       return -1;
    }

    glUseProgram(pObj->uiProgramObject);
    //System_eglCheckGlError("glUseProgram");

    //locate sampler uniforms
    pObj->samplerLocation0 = glGetUniformLocation(pObj->uiProgramObject, "uSampler[0]");
    glUniform1i(pObj->samplerLocation0, 0);
    GL_CHECK(glUniform1i);
    pObj->samplerLocation1 = glGetUniformLocation(pObj->uiProgramObject, "uSampler[1]");
    glUniform1i(pObj->samplerLocation1, 1);
    GL_CHECK(glUniform1i);
    pObj->mvMatrixLocation = glGetUniformLocation(pObj->uiProgramObject, "uMVMatrix");
    GL_CHECK(glGetAttribLocation);
    pObj->vertexPositionAttribLoc = glGetAttribLocation(pObj->uiProgramObject, "aVertexPosition");
    GL_CHECK(glGetAttribLocation);
    pObj->blendAttribLoc = glGetAttribLocation(pObj->uiProgramObject, "blendVals");
    GL_CHECK(glGetAttribLocation);
    pObj->vertexTexCoord1AttribLoc = glGetAttribLocation(pObj->uiProgramObject, "aTextureCoord1");
    GL_CHECK(glGetAttribLocation);
    pObj->vertexTexCoord2AttribLoc = glGetAttribLocation(pObj->uiProgramObject, "aTextureCoord2");
    GL_CHECK(glGetAttribLocation);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //System_eglCheckGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //System_eglCheckGlError("glClear");

    glDisable(GL_DEPTH_TEST);
#ifdef ENABLE_GLOBAL_BLENDING    
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif    

    pObj->mouse = 0.5; 
    pObj->delta = 0.0025; //0.00125;

    //STEP2 - initialise the vertices
    car_init_vertices_vbo(&pObj->car_gl1);
    GL_CHECK(car_init_vertices_vbo);
    
    //STEP3 - initialise the individual views
    screen1_init_vbo();
    GL_CHECK(screen1_init_vbo);
 
    //Generate indices
    for (int i = 0; i < MAX_INDEX_BUFFERS; i++)
    {
    	index_buffers[i].buffer = (unsigned short *)malloc(QUADRANT_WIDTH * QUADRANT_HEIGHT * 3 * sizeof(unsigned short));
    	generate_indices((t_index_buffer *)&index_buffers[i], QUADRANT_WIDTH, QUADRANT_HEIGHT, pow(2,i));
    }

    //points_init_vertices_vbo();
    SgxRender3DSRV_updateView();
    //cull
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    num_coordsets = (int)(sizeof(srv_coords)/sizeof(srv_coords_t));

    pthread_create(&scan_thread, NULL, scan_thread_function, (void *)&scan_thread_data);
 
    return 0;
}

int SgxRender3DSRV_setup_offscreen(SgxRender3DSRV_Obj *pObj)
{
    //STEP1 - shader setup
    pObj->uiProgramObject = SgxRender3DSRV_createProgram(
                        gSgxRender3DSRV_vertexShader_1VBO,
                        gSgxRender3DSRV_fragmentShader_1VBO
                     );
    if (pObj->uiProgramObject==0)
    {
       return -1;
    }

    glUseProgram(pObj->uiProgramObject);
    //System_eglCheckGlError("glUseProgram");

    //locate sampler uniforms
    pObj->samplerLocation0 = glGetUniformLocation(pObj->uiProgramObject, "uSampler[0]");
    glUniform1i(pObj->samplerLocation0, 0);
    GL_CHECK(glUniform1i);
    pObj->samplerLocation1 = glGetUniformLocation(pObj->uiProgramObject, "uSampler[1]");
    glUniform1i(pObj->samplerLocation1, 1);
    GL_CHECK(glUniform1i);
    pObj->mvMatrixLocation = glGetUniformLocation(pObj->uiProgramObject, "uMVMatrix");
    GL_CHECK(glGetAttribLocation);
    pObj->vertexPositionAttribLoc = glGetAttribLocation(pObj->uiProgramObject, "aVertexPosition");
    GL_CHECK(glGetAttribLocation);
    pObj->blendAttribLoc = glGetAttribLocation(pObj->uiProgramObject, "blendVals");
    GL_CHECK(glGetAttribLocation);
    pObj->vertexTexCoord1AttribLoc = glGetAttribLocation(pObj->uiProgramObject, "aTextureCoord1");
    GL_CHECK(glGetAttribLocation);
    pObj->vertexTexCoord2AttribLoc = glGetAttribLocation(pObj->uiProgramObject, "aTextureCoord2");
    GL_CHECK(glGetAttribLocation);


    pObj->mouse = 0.5;
    pObj->delta = 0.0025; //0.00125;

    //STEP2 - initialise the vertices
    car_init_vertices_vbo(&pObj->car_gl1);
    GL_CHECK(car_init_vertices_vbo);

    //STEP3 - initialise the individual views
    screen1_init_vbo();
    GL_CHECK(screen1_init_vbo);

    //Generate indices
    for (int i = 0; i < MAX_INDEX_BUFFERS; i++)
    {
        index_buffers[i].buffer = (unsigned short *)malloc(QUADRANT_WIDTH * QUADRANT_HEIGHT * 3 * sizeof(unsigned short));
        generate_indices((t_index_buffer *)&index_buffers[i], QUADRANT_WIDTH, QUADRANT_HEIGHT, pow(2,i));
    }

    //points_init_vertices_vbo();
    SgxRender3DSRV_updateView();

    num_coordsets = (int)(sizeof(srv_coords)/sizeof(srv_coords_t));

    return 0;
}

void SgxRender3DSRV_drawWrapper(SgxRender3DSRV_Obj *pObj, GLuint *texYuv)
{
	int i;
	if(prevLUT != pObj->LUT3D || prevblendLUT != pObj->blendLUT3D || index_buffer_changed == true)
	{
		prevLUT = pObj->LUT3D;
		prevblendLUT = pObj->blendLUT3D;
		surroundview_init_vertices_vbo_wrap(pObj);
	}

 //First setup the program once
 glUseProgram(pObj->uiProgramObject);
    //then change the meshes and draw
    for(i = 0;i < QUADRANTS;i ++)
    {
		onscreen_mesh_state_restore_program_textures_attribs(
				pObj, texYuv, (0+i)%4, (3+i)%4);
        onscreen_mesh_state_restore_vbo(
            pObj, vboId[i*3], vboId[i*3+1], vboId[i*3+2]);
        GL_CHECK(onscreen_mesh_state_restore_vbo);
		glDrawElements(render_mode, index_buffers[active_index_buffer].length, GL_UNSIGNED_SHORT,  0);
        GL_CHECK(glDrawElements);
    }
}

void SgxRender3DSRV_renderFrame3DSRV(SgxRender3DSRV_Obj *pObj, GLuint *texYuv)
{
    glClear(GL_COLOR_BUFFER_BIT);

     {
        glViewport(520, 0, 880, 1080);
        SgxRender3DSRV_drawWrapper(pObj, texYuv);
        car_draw_vbo(&pObj->car_gl1, 4);
        //points_draw_vbo();
     }

#if ENABLE_SGX_RENDERED_PREVIEW
     // Draw the other panes
     {
        glViewport(0, 1080-(200+440*1),520,440);
        screen1_draw_vbo(texYuv[0]);
#ifdef ENABLE_CAR_SIDE_PANES
        car_draw_vbo(&pObj->car_gl1, 0);
#endif
     }
     {
        glViewport(0, 1080-(200+440*2),520,440);
        screen1_draw_vbo(texYuv[1]);
#ifdef ENABLE_CAR_SIDE_PANES
        car_draw_vbo(&pObj->car_gl1, 1);
#endif
     }
     {
        glViewport(520+880, 1080-(200+440*1),520,440);
        screen1_draw_vbo(texYuv[2]);
#ifdef ENABLE_CAR_SIDE_PANES
        car_draw_vbo(&pObj->car_gl1, 2);
#endif
     }
     {
        glViewport(520+880, 1080-(200+440*2),520,440);
        screen1_draw_vbo(texYuv[3]);
#ifdef ENABLE_CAR_SIDE_PANES
        car_draw_vbo(&pObj->car_gl1, 3);
#endif
     }
#endif
}

void SgxRender3DSRV_renderFrame(SgxRender3DSRV_Obj *pObj, GLuint *texYuv)
{
	if(animate == true)
	{
		if(animatengap == false)
		{
			/* stay here for gap number of frames;
			 *
			 */
			step++;
			if(step >= gap)
			{
				step = 0;
				animatengap = true;
			}
		}
		else
		{
			unsigned int next_index=(current_index+1)%num_coordsets;
			COORD_TRANSITION(camx, next_index, step);
			COORD_TRANSITION(camy, next_index, step);
			COORD_TRANSITION(camz, next_index, step);
			COORD_TRANSITION(targetx, next_index, step);
			COORD_TRANSITION(targety, next_index, step);
			COORD_TRANSITION(targetz, next_index, step);
			COORD_TRANSITION(anglex, next_index, step);
			COORD_TRANSITION(angley, next_index, step);
			COORD_TRANSITION(anglez, next_index, step);
			SgxRender3DSRV_updateView();
			step++;
			if(step >= num_iterations)
			{
				step = 0;
				animatengap = false;
				current_index = next_index;
			}
		}
	}

	SgxRender3DSRV_renderFrame3DSRV(pObj, texYuv);
}


void render3dFrame(SgxRender3DSRV_Obj *pObj, GLuint *texYuv)
{
    glClear(GL_COLOR_BUFFER_BIT);

     {
        glViewport(0, 0, 880, 1080);
        SgxRender3DSRV_drawWrapper(pObj, texYuv);
        car_draw_vbo(&pObj->car_gl1, 4);
     }
}

void render3dFrame_offscreen(SgxRender3DSRV_Obj *pObj, GLuint *texYuv)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //System_eglCheckGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //System_eglCheckGlError("glClear");

    glDisable(GL_DEPTH_TEST);
#ifdef ENABLE_GLOBAL_BLENDING
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

    //cull
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

        glViewport(0, 0, 880, 1080);
        SgxRender3DSRV_updateView();
        SgxRender3DSRV_drawWrapper(pObj, texYuv);
        car_draw_vbo(&pObj->car_gl1, 4);
}
