#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include "fb.h"

// crudely ripped off from http://raspberrycompote.blogspot.com/2013/01/low-level-graphics-on-raspberry-pi-part_22.html


uint32_t *FB_Init(int *width, int *height) {
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    uint32_t *fbp = NULL;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return NULL;
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        return NULL;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        return NULL;
    }
    if (vinfo.bits_per_pixel != 32) {
        printf("Please implement !32bpp rendering ;)\n");
        return NULL;
    }
    printf("%dx%d, %d bpp\n", vinfo.xres, vinfo.yres, 
           vinfo.bits_per_pixel );

    // map framebuffer to user memory 
    screensize = finfo.smem_len;

    fbp = (uint32_t*)mmap(0, 
                      screensize, 
                      PROT_READ | PROT_WRITE, 
                      MAP_SHARED, 
                      fbfd,
                      0);

    if ((int)fbp == -1) {
        printf("Failed to mmap.\n");
        return NULL;
    }
    *width = vinfo.xres;
    *height = vinfo.yres;
    return fbp;
}
