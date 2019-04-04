#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>


#include "util\MathUtil.h"
#include "puppy\P_Common.h"
#include "kitten\K_Game.h"

//========================================================================
// This is needed for newer versions of Visual Studio
//========================================================================
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
//========================================================================

int main( void )
{

#ifndef DEBUG
	FreeConsole();
#endif

    int width, height, x;
    
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }
	
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	RECT monitorRect;
	GetWindowRect(GetDesktopWindow(), &monitorRect);

	int monitorX = 1280;//monitorRect.right;
	int monitorY = 720; // monitorRect.bottom;

    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( monitorX, monitorY, 0,0,0,0, 24,0, GLFW_WINDOW ) )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );
        
        glfwTerminate();
        exit( EXIT_FAILURE );
    }
    
#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

    glfwSetWindowTitle( "VG1819" );
    
    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );
	glEnable(GL_DEPTH_TEST);

    // VSYNC, 0 = Off, 1 = On, 2 = Halved, ...
    glfwSwapInterval( 0 );
    
	kitten::initGame();

    do
	{
        glfwGetMousePos( &x, NULL );
        
        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );
        
        // Special case: avoid division by zero below
        height = height > 0 ? height : 1;
        
        glViewport( 0, 0, width, height );
        
        // Clear color buffer
        glClearColor(0.0f,0.0f,0.0f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
		kitten::gameCycle();

        // Swap buffers
        glfwSwapBuffers();
    } // Check if the ESC key was pressed or the window was closed
    while( //glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
          glfwGetWindowParam( GLFW_OPENED ) );
    
	kitten::shutdownGame();
}


