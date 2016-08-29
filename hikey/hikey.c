/*
 * SIVA: westeros-gl implementation for HiKey Board
 */

#ifdef __cplusplus
extern "C"
{
#endif
#include <xf86drm.h>
#include <xf86drmMode.h>
#ifdef __cplusplus
}
#endif

/*
 * Function to get width & height from connected drm device with its fd
 */
void GetWidthHeight(int fd, int *width, int *height)
{
        drmModeConnector *connector;
        drmModeRes *resources;
        int i = 0;
        drmModeModeInfo mode;

        // Getting width & height from drm
        resources = drmModeGetResources(fd);
        if (!resources) {
                printf("drmModeGetResources failed\n");
                return;
        }

        for (i = 0; i < resources->count_connectors; i++) {
                connector = drmModeGetConnector(fd,
                                                resources->connectors[i]);
                if (!connector)
                        continue;

                mode = connector->modes[0];
                *width = mode.hdisplay;
                *height = mode.vdisplay;
                break;
                //drmModeFreeConnector(connector);
        }
}
