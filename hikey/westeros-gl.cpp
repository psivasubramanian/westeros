/*
 * SIVA: westeros-gl implementation for HiKey Board
 */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "westeros-gl.h"
#include <assert.h>
#include <gbm.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "hikey.h"
#ifdef __cplusplus
}
#endif

#if 0 // DRM
#ifdef __cplusplus
extern "C"
{
#endif
#define virtual VIRTUAL  //REMOVE HACK to fix C header issue
#include <xf86drm.h>
#include <xf86drmMode.h>
#undef virtual           //REMOVE HACK to fix C header issue
#ifdef __cplusplus
}
#endif
#endif

/*
 * WstGLNativePixmap:
 */
typedef struct _WstGLNativePixmap
{
   void *pixmap;
   int width;
   int height;
} WstNativePixmap;

typedef struct _WstGLNativeWindow
{
   public:
     operator EGLNativeWindowType() { return nw; }
     void swap() {} 
   private:
     EGLNativeWindowType nw;
} WstGLNativeWindow;

typedef struct _WstGLCtx 
{
  int displayId;
  int displayWidth;
  int displayHeight;
  EGLDisplay egl_display;
} WstGLCtx;

static int ctxCount= 0;

WstGLCtx* WstGLInit()
{
   WstGLCtx *ctx= 0;

   ctx= (WstGLCtx*)calloc( 1, sizeof(WstGLCtx) );
   if ( ctx )
   {
      if ( ctxCount == 0 )
      {
         //TODO: init         
      }

      ++ctxCount;

   }
   
   return ctx;
}

void WstGLTerm( WstGLCtx *ctx )
{
   if ( ctx )
   {
      if ( ctxCount > 0 )
      {
         --ctxCount;
      }
      if ( ctxCount == 0 )
      {
         if (!ctx->egl_display)
         {
		eglTerminate(ctx->egl_display);
         }
      }
      free( ctx );
   }
}

static int egl_ext_supports(
                     const char *extension_suffix)
{
        static const char *extensions = NULL;
        char s[64];

        if (!extensions) {
                extensions = (const char *) eglQueryString(
                        EGL_NO_DISPLAY, EGL_EXTENSIONS);

                if (!extensions)
                        return 0;

                printf("Westeros-gl: EGL client extensions %c\n",
                               extensions);
        }

        if (!strstr(extensions, "EGL_EXT_platform_base"))
                return 0;

        snprintf(s, sizeof s, "EGL_KHR_platform_%s", extension_suffix);
        if (strstr(extensions, s))
                return 1;

        snprintf(s, sizeof s, "EGL_EXT_platform_%s", extension_suffix);
        if (strstr(extensions, s))
                return 1;

        snprintf(s, sizeof s, "EGL_MESA_platform_%s", extension_suffix);
        if (strstr(extensions, s))
                return 1;

        return -1;
}

static const char *get_extension(EGLenum platform)
{
        switch (platform) {
        case EGL_PLATFORM_GBM_KHR:
                return "gbm";
        case EGL_PLATFORM_WAYLAND_KHR:
                return "wayland";
        case EGL_PLATFORM_X11_KHR:
                return "x11";
        default:
                return "gbm";
        }
}

static PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = NULL;

/*
 * WstGLCreateNativeWindow
 * Create a native window suitable for use as an EGLNativeWindow
 */
void* WstGLCreateNativeWindow( WstGLCtx *ctx, int x, int y, int width, int height )
{
    int fd = 0;
    int supports = 0;
    struct gbm_device *gbm;
        EGLenum platform = EGL_PLATFORM_GBM_KHR; 
        void *nw = NULL;
        EGLint format = GBM_FORMAT_XRGB8888;
//        drmModeConnector *connector;
//        drmModeRes *resources;
        int i = 0;
//        drmModeModeInfo mode;

        fd = open("/dev/dri/card0", O_RDWR | FD_CLOEXEC);
        if (fd < 0) {
            printf("WstGLCreateNativeWindow: failed to open DRI device\n");
            return NULL;
        }

        gbm = gbm_create_device(fd);
        if (!gbm) {
            printf("WstGLCreateNativeWindow: failed to create GBM device for DRI\n");
            return NULL;
        }

        supports = egl_ext_supports(
                        get_extension(platform));
       if (supports < 0){
                        printf("WstGLCreateNativeWindow: No EGL EXT support available\n");
                        return NULL;
       }

       if (supports) {
                if (!get_platform_display) {
                        get_platform_display = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress(
                                        "eglGetPlatformDisplayEXT");

                }

                /* eglGetProcAddress can return non-NULL and still not
                 * support the feature at runtime, so ensure the extension support checked*/
                if (get_platform_display && platform) {
                        ctx->egl_display = get_platform_display(platform,
                                                               gbm,
                                                               NULL);
                }
       }

       if (!ctx->egl_display) {
                printf("WstGLCreateNativeWindow: warning either no EGL_EXT_platform_base support or specific platform support falling back to eglGetDisplay.\n");
                ctx->egl_display = eglGetDisplay(gbm);
        }

        GetWidthHeight(fd, &width, &height);
        printf("WstGLCreateNativeWindow: drm width %d height %d\n", width, height);

#if 0 //DRM
        // Getting width & height from drm
        resources = drmModeGetResources(fd);
        if (!resources) {
                printf("drmModeGetResources failed\n");
                return NULL;
        }

        for (i = 0; i < resources->count_connectors; i++) {
                connector = drmModeGetConnector(fd,
                                                resources->connectors[i]);
                if (connector == NULL)
                        continue;

                mode = connector->modes[0];
                width = mode.hdisplay;
                height = mode.vdisplay;
                break;
                //drmModeFreeConnector(connector);
        }
#endif

        nw = (void*)gbm_surface_create(gbm, width, height, format, GBM_BO_USE_SCANOUT |
                                                           GBM_BO_USE_RENDERING);
        if(!nw)
        {
                printf("WstGLCreateNativeWindow: failed to get native surface\n");
                return NULL;
        }

	return nw;
}

/*
 * WstGLDestroyNativeWindow
 * Destroy a native window created by WstGLCreateNativeWindow
 */
void WstGLDestroyNativeWindow( WstGLCtx *ctx, void *nativeWindow )
{
   if ( ctx )
   {
        EGLNativeWindowType *nw;

        nw = (EGLNativeWindowType *)nativeWindow;
      if ( nw )
      {
         free( nw );
      }
   }
}

/*
 * WstGLGetNativePixmap
 * Given a native buffer, obtain a native pixmap
 *
 * nativeBuffer : pointer to a Nexus surface
 * nativePixmap : pointer to a pointer to a WstGLNativePixmap
 *
 * If nativePixmap points to a null pointer, a new WstGLNativePixmap will be
 * allocated.  If nativePixmap points to non-null pointer, the WstGLNativePixmap
 * will be re-used.
 *
 * The input Nexus surface contains a frame from a compositor client process.  In order
 * for its contents to be useable to the compositor in OpenGL rendering, it must be
 * copied to a Nexus surface/native pixmap pair created by the compositor process.
 */
bool WstGLGetNativePixmap( WstGLCtx *ctx, void *nativeBuffer, void **nativePixmap )
{
   bool result= false;
    
   if ( ctx )
   {
      // Not yet required
   }
   
   return result;
}

/*
 * WstGLGetNativePixmapDimensions
 * Get the dimensions of the WstGLNativePixmap
 */
void WstGLGetNativePixmapDimensions( WstGLCtx *ctx, void *nativePixmap, int *width, int *height )
{
   if ( ctx )
   {
      // Not yet required
   }
}

/*
 * WstGLReleaseNativePixmap
 * Release a WstGLNativePixmap obtained via WstGLGetNativePixmap
 */
void WstGLReleaseNativePixmap( WstGLCtx *ctx, void *nativePixmap )
{
   if ( ctx )
   {
      // Not yet required
   }
}

/*
 * WstGLGetEGLNativePixmap
 * Get the native pixmap usable as a EGL_NATIVE_PIXMAP_KHR for creating a texture
 * from the provided WstGLNativePixmap instance
 */
EGLNativePixmapType WstGLGetEGLNativePixmap( WstGLCtx *ctx, void *nativePixmap )
{
    EGLNativePixmapType eglPixmap= 0;
   
   if ( nativePixmap )
   {
      // Not yet required
   }
   
   return eglPixmap;
}
