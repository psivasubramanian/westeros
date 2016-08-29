/*
 * SIVA: westeros-gl implementation for HiKey Board
 */
#ifndef _WESTEROS_GL_H
#define _WESTEROS_GL_H

#ifdef ENABLE_EGL

#include <EGL/egl.h>
#include <EGL/eglext.h>

/*#else

typedef int EGLint;
typedef int EGLenum;
typedef void *EGLDisplay;
typedef void *EGLSurface;
typedef void *EGLConfig;
typedef int *EGLNativeDisplayType;
typedef int *EGLNativeWindowType;
#define EGL_DEFAULT_DISPLAY ((EGLNativeDisplayType)0)
*/
#endif /* ENABLE_EGL */

#ifndef EGL_EXT_platform_base
typedef EGLDisplay (*PFNEGLGETPLATFORMDISPLAYEXTPROC) (EGLenum platform, void *native_display, const EGLint *attrib_list);
typedef EGLSurface (*PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC) (EGLDisplay dpy, EGLConfig config, void *native_window, const EGLint *attrib_list);
#endif

#ifndef EGL_PLATFORM_GBM_KHR
#define EGL_PLATFORM_GBM_KHR 0x31D7
#endif

#ifndef EGL_PLATFORM_WAYLAND_KHR
#define EGL_PLATFORM_WAYLAND_KHR 0x31D8
#endif

#ifndef EGL_PLATFORM_X11_KHR
#define EGL_PLATFORM_X11_KHR 0x31D5
#endif

#define NO_EGL_PLATFORM 0


typedef struct _WstGLCtx WstGLCtx;

WstGLCtx* WstGLInit();
void WstGLTerm( WstGLCtx *ctx );
void* WstGLCreateNativeWindow( WstGLCtx *ctx, int x, int y, int width, int height );
void WstGLDestroyNativeWindow( WstGLCtx *ctx, void *nativeWindow );
bool WstGLGetNativePixmap( WstGLCtx *ctx, void *nativeBuffer, void **nativePixmap );
void WstGLGetNativePixmapDimensions( WstGLCtx *ctx, void *nativePixmap, int *width, int *height );
void WstGLReleaseNativePixmap( WstGLCtx *ctx, void *nativePixmap );
EGLNativePixmapType WstGLGetEGLNativePixmap( WstGLCtx *ctx, void *nativePixmap);

#endif
