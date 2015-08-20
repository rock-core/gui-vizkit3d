#ifndef VIZKIT3D_WINDOW_CAPTURE_CALLBACK
#define VIZKIT3D_WINDOW_CAPTURE_CALLBACK

#include <osg/Camera>
#include <vizkit3d/Vizkit3DWidget.hpp>

namespace vizkit3d
{
    struct CaptureOperation : public osg::Referenced
    {
        virtual ~CaptureOperation() {}
        virtual void operator()(osg::Image const& image, unsigned int contextID) = 0;
    };

    /** Draw callback that can be used to capture the render results
     *
     * This will usually not be used directly, but used instead through the
     * corresponding API on Vizkit3dWidget
     *
     * This is an almost-pristine copy of the class of the same name that can be
     * found in the OSG source code. The reason is that this class is kept
     * private, and the class that exposes this functionality
     * (ScreenCaptureHandler) does not allow to control the capture mode, thus
     * using plain old glReadPixels instead of the more modern method of using
     * PBOs, as well as hardcoding the pixel format to RGB, which (1) is slow on
     * the OpenGL side (the GPUs use BGR internally) and (2) needs conversion on
     * the Qt side (which is using BGR as well even if its pixel format is
     * called RGB ... go figure)
     */
    class WindowCaptureCallback : public osg::Camera::DrawCallback
    {
    public:
        typedef Vizkit3DWidget::GrabbingMode Mode;

        enum FramePosition
        {
            START_FRAME,
            END_FRAME
        };

        WindowCaptureCallback(int numFrames, Mode mode, FramePosition position, GLenum readBuffer, GLint pixelFormat);

        FramePosition getFramePosition() const { return _position; }

        void setCaptureOperation(CaptureOperation* operation);
        CaptureOperation* getCaptureOperation() { return _contextDataMap.begin()->second->_captureOperation.get(); }

        void setFramesToCapture(int numFrames) { _numFrames = numFrames; }
        int getFramesToCapture() const { return _numFrames; }

        virtual void operator () (osg::RenderInfo& renderInfo) const;

        struct ContextData : public osg::Referenced
        {
            ContextData(osg::GraphicsContext* gc, Mode mode, GLenum readBuffer, GLint pixelFormat);

            void getSize(osg::GraphicsContext* gc, int& width, int& height);

            void updateTimings(osg::Timer_t tick_start,
                    osg::Timer_t tick_afterReadPixels,
                    osg::Timer_t tick_afterMemCpy,
                    osg::Timer_t tick_afterCaptureOperation,
                    unsigned int dataSize);

            void read();
            void readPixels();
            void singlePBO(osg::GLBufferObject::Extensions* ext);
            void multiPBO(osg::GLBufferObject::Extensions* ext);

            typedef std::vector< osg::ref_ptr<osg::Image> >             ImageBuffer;
            typedef std::vector< GLuint > PBOBuffer;

            osg::GraphicsContext*   _gc;
            unsigned int            _index;
            Mode                    _mode;
            GLenum                  _readBuffer;

            GLenum                  _pixelFormat;
            GLenum                  _type;
            int                     _width;
            int                     _height;

            unsigned int            _currentImageIndex;
            ImageBuffer             _imageBuffer;

            unsigned int            _currentPboIndex;
            PBOBuffer               _pboBuffer;

            unsigned int            _reportTimingFrequency;
            unsigned int            _numTimeValuesRecorded;
            double                  _timeForReadPixels;
            double                  _timeForMemCpy;
            double                  _timeForCaptureOperation;
            double                  _timeForFullCopy;
            double                  _timeForFullCopyAndOperation;
            osg::Timer_t            _previousFrameTick;

            osg::ref_ptr<CaptureOperation> _captureOperation;
        };

        typedef std::map<osg::GraphicsContext*, osg::ref_ptr<ContextData> > ContextDataMap;

        ContextData* createContextData(osg::GraphicsContext* gc) const;
        ContextData* getContextData(osg::GraphicsContext* gc) const;

        Mode                        _mode;
        FramePosition               _position;
        GLenum                      _readBuffer;
        mutable OpenThreads::Mutex  _mutex;
        mutable ContextDataMap      _contextDataMap;
        mutable int                 _numFrames;
        GLint                       _pixelFormat;

        osg::ref_ptr<CaptureOperation> _defaultCaptureOperation;
    };
}

#endif
