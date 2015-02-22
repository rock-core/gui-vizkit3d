#include "EnableGLDebugOperation.hpp"
#include <osg/GL>
#include <osg/GLExtensions>
#include <osg/GL2Extensions>
#include <osg/Notify>

using namespace vizkit3d;

typedef void (GL_APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (GL_APIENTRY *GLDebugMessageControlPROC)(GLenum , GLenum , GLenum , GLsizei , const GLuint *, GLboolean );
typedef void (GL_APIENTRY *GLDebugMessageCallbackPROC)(GLDEBUGPROC , const void *) ;
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION 0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM 0x8245
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH 0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES 0x9144
#define GL_DEBUG_LOGGED_MESSAGES 0x9145
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
#define GL_DEBUG_TYPE_POP_GROUP 0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH 0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH 0x826D
#define GL_BUFFER 0x82E0
#define GL_SHADER 0x82E1
#define GL_PROGRAM 0x82E2
#define GL_QUERY 0x82E3
#define GL_PROGRAM_PIPELINE 0x82E4
#define GL_SAMPLER 0x82E6
#define GL_DISPLAY_LIST 0x82E7
#define GL_MAX_LABEL_LENGTH 0x82E8
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002



static void  debugCallback(GLenum source, GLenum type, GLuint , GLenum severity,
                    GLsizei , const GLchar *message, const void *)
{ 
    std::string srcStr = "UNDEFINED";
    switch(source)
    {
        case GL_DEBUG_SOURCE_API:             srcStr = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   srcStr = "WINDOW_SYSTEM"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: srcStr = "SHADER_COMPILER"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     srcStr = "THIRD_PARTY"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     srcStr = "APPLICATION"; break;
        case GL_DEBUG_SOURCE_OTHER:           srcStr = "OTHER"; break;
    }

    std::string typeStr = "UNDEFINED";

    osg::NotifySeverity osgSeverity = osg::DEBUG_INFO;
    switch(type)
    {
        case GL_DEBUG_TYPE_ERROR:
            typeStr = "ERROR";
            osgSeverity = osg::FATAL;
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "UNDEFINED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "PERFORMANCE"; break;
        case GL_DEBUG_TYPE_OTHER:               typeStr = "OTHER"; break;
    }


    osg::notify(osgSeverity)<< "OpenGL " << typeStr <<  " [" << srcStr <<"]: " << message <<std::endl;
}


void vizkit3d::enableGLDebugExtension(int context_id)
{
    //create the extensions
    GLDebugMessageControlPROC glDebugMessageControl = NULL;
    GLDebugMessageCallbackPROC glDebugMessageCallback = NULL;
    if(osg::isGLExtensionSupported(context_id, "GL_KHR_debug"))
    {
        osg::setGLExtensionFuncPtr(glDebugMessageCallback, "glDebugMessageCallback");
        osg::setGLExtensionFuncPtr(glDebugMessageControl, "glDebugMessageControl");

    }else if(osg::isGLExtensionSupported(context_id, "GL_ARB_debug_output"))
    {
        osg::setGLExtensionFuncPtr(glDebugMessageCallback, "glDebugMessageCallbackARB");
        osg::setGLExtensionFuncPtr(glDebugMessageControl, "glDebugMessageControlARB");
    }else if(osg::isGLExtensionSupported(context_id, "GL_AMD_debug_output"))
    {
        osg::setGLExtensionFuncPtr(glDebugMessageCallback, "glDebugMessageCallbackAMD");
        osg::setGLExtensionFuncPtr(glDebugMessageControl, "glDebugMessageControlAMD");
    }

    if(glDebugMessageCallback != NULL && glDebugMessageControl != NULL)
    {
        OSG_NOTICE << "enabling GL debug" << std::endl;
        glGetError();
        glEnable(GL_DEBUG_OUTPUT);
        if (glGetError() != GL_NO_ERROR)
            OSG_WARN << "failed to turn debugging output" << std::endl;

        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        if (glGetError() != GL_NO_ERROR)
            OSG_WARN << "failed to turn debugging output" << std::endl;

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        if (glGetError() != GL_NO_ERROR)
            OSG_WARN << "failed to turn debugging output" << std::endl;

        glDebugMessageCallback(debugCallback, NULL);
        if (glGetError() != GL_NO_ERROR)
            OSG_WARN << "failed to turn debugging output" << std::endl;
    }
    else
    {
        OSG_WARN << "GL debug requested, but cannot find a supported debug extension" << std::endl;
    }
}
