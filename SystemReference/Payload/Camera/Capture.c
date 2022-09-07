/*
*  V4L2 video capture example
*
*  This program can be used and distributed without restrictions.
*
*      This program is provided with the V4L2 API
* see https://linuxtv.org/docs.php for more information
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

//enum io_method {
// IO_METHOD_READ,
// IO_METHOD_MMAP,
// IO_METHOD_USERPTR,
//};

struct buffer {
 void   *start;
 size_t  length;
};

static char            *dev_name;
static int              fd = -1;
struct buffer          *buffers;
static unsigned int     n_buffers;
static int              out_buf;
static int              force_format;
static int              frame_count = 70;

static void errno_exit(const char *s)
{
 fprintf(stderr, "%s error %d, %s\\n", s, errno, strerror(errno));
 exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
 int r;

 do {
   r = ioctl(fh, request, arg);
 } while (-1 == r && EINTR == errno);

 return r;
}

static void process_image(const void *p, int size)
{
 if (out_buf)
   fwrite(p, size, 1, stdout);

 fflush(stderr);
 fprintf(stderr, ".");
 fflush(stdout);
}

int read_frame(void *cameraBuffer, uint32_t size, size_t *readSize)
{
  *readSize = read(fd, cameraBuffer, size);
   if (-1 == *readSize && errno != EAGAIN) {
      return -1;
   }
 return 0;
}

static void uninit_device(void)
{
 unsigned int i;
 free(buffers[0].start);
 free(buffers);
}

static void init_read(unsigned int buffer_size)
{
 buffers = calloc(1, sizeof(*buffers));

 if (!buffers) {
   fprintf(stderr, "Out of memory\\n");
   exit(EXIT_FAILURE);
 }

 buffers[0].length = buffer_size;
 buffers[0].start = malloc(buffer_size);

 if (!buffers[0].start) {
   fprintf(stderr, "Out of memory\\n");
   exit(EXIT_FAILURE);
 }
}

static void init_mmap(void)
{
 struct v4l2_requestbuffers req;

 CLEAR(req);

 req.count = 4;
 req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 req.memory = V4L2_MEMORY_MMAP;

 if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
   if (EINVAL == errno) {
     fprintf(stderr, "%s does not support "
                     "memory mappingn", dev_name);
     exit(EXIT_FAILURE);
   } else {
     errno_exit("VIDIOC_REQBUFS");
   }
 }

 if (req.count < 2) {
   fprintf(stderr, "Insufficient buffer memory on %s\\n",
           dev_name);
   exit(EXIT_FAILURE);
 }

 buffers = calloc(req.count, sizeof(*buffers));

 if (!buffers) {
   fprintf(stderr, "Out of memory\\n");
   exit(EXIT_FAILURE);
 }

 for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
   struct v4l2_buffer buf;

   CLEAR(buf);

   buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   buf.memory      = V4L2_MEMORY_MMAP;
   buf.index       = n_buffers;

   if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
     errno_exit("VIDIOC_QUERYBUF");

   buffers[n_buffers].length = buf.length;
   buffers[n_buffers].start =
       mmap(NULL /* start anywhere */,
            buf.length,
            PROT_READ | PROT_WRITE /* required */,
            MAP_SHARED /* recommended */,
            fd, buf.m.offset);

   if (MAP_FAILED == buffers[n_buffers].start)
     errno_exit("mmap");
 }
}

static void init_userp(unsigned int buffer_size)
{
 struct v4l2_requestbuffers req;

 CLEAR(req);

 req.count  = 4;
 req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 req.memory = V4L2_MEMORY_USERPTR;

 if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
   if (EINVAL == errno) {
     fprintf(stderr, "%s does not support "
                     "user pointer i/on", dev_name);
     exit(EXIT_FAILURE);
   } else {
     errno_exit("VIDIOC_REQBUFS");
   }
 }

 buffers = calloc(4, sizeof(*buffers));

 if (!buffers) {
   fprintf(stderr, "Out of memory\\n");
   exit(EXIT_FAILURE);
 }

 for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
   buffers[n_buffers].length = buffer_size;
   buffers[n_buffers].start = malloc(buffer_size);

   if (!buffers[n_buffers].start) {
     fprintf(stderr, "Out of memory\\n");
     exit(EXIT_FAILURE);
   }
 }
}

static int init_device(void)
{
 struct v4l2_capability cap;
 struct v4l2_cropcap cropcap;
 struct v4l2_crop crop;
 struct v4l2_format fmt;
 unsigned int min;

 if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
   if (EINVAL == errno) {
     fprintf(stderr, "%s is no V4L2 device\\n",
             dev_name);
     exit(EXIT_FAILURE);
   } else {
     errno_exit("VIDIOC_QUERYCAP");
   }
 }

 if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
   fprintf(stderr, "%s is no video capture device\\n",
           dev_name);
   exit(EXIT_FAILURE);
 }

   if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
     fprintf(stderr, "%s does not support read i/o\\n",
             dev_name);
     exit(EXIT_FAILURE);
   }

 /* Select video input, video standard and tune here. */


 CLEAR(cropcap);

 cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

 if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
   crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   crop.c = cropcap.defrect; /* reset to default */

   if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
     switch (errno) {
     case EINVAL:
       /* Cropping not supported. */
       break;
     default:
       /* Errors ignored. */
       break;
     }
   }
 } else {
   /* Errors ignored. */
 }
}

uint32_t set_format(uint32_t height, uint32_t width, uint32_t imgFormat)
{
  struct v4l2_format fmt;
  unsigned int min;

  CLEAR(fmt);

  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (force_format) {
    fmt.fmt.pix.width       = width;
    fmt.fmt.pix.height      = height;
    if (imgFormat == V4L2_PIX_FMT_YUYV){
      fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    } else {
      fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    }
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
      errno_exit("VIDIOC_S_FMT");

    /* Note VIDIOC_S_FMT may change width and height. */
  } else {
    /* Preserve original settings as set by v4l2-ctl for example */
    if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
      errno_exit("VIDIOC_G_FMT");
  }

  /* Buggy driver paranoia. */
  min = fmt.fmt.pix.width * 2;
  if (fmt.fmt.pix.bytesperline < min) {
    fmt.fmt.pix.bytesperline = min;
  }
  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
  if (fmt.fmt.pix.sizeimage < min) {
    fmt.fmt.pix.sizeimage = min;
  }
  return fmt.fmt.pix.sizeimage;
}

// https://stackoverflow.com/questions/61581125/v4l2-absolute-exposure-setting-has-almost-not-effect
// https://forums.raspberrypi.com/viewtopic.php?t=281994
// https://linuxtv.org/downloads/v4l-dvb-apis-new/userspace-api/v4l/ext-ctrls-camera.html?highlight=exposure
void set_exposure_time(uint32_t exposureTime){
  struct v4l2_control control;
  control.id = V4L2_CID_EXPOSURE_ABSOLUTE;
  control.value = exposureTime;
  xioctl(fd, VIDIOC_S_CTRL, &control);
}

static void close_device(void)
{
 if (-1 == close(fd))
   errno_exit("close");

 fd = -1;
}

static void open_device(void)
{
 struct stat st;

 if (-1 == stat(dev_name, &st)) {
   fprintf(stderr, "Cannot identify '%s': %d, %s\\n",
           dev_name, errno, strerror(errno));
   exit(EXIT_FAILURE);
 }

 if (!S_ISCHR(st.st_mode)) {
   fprintf(stderr, "%s is no devicen", dev_name);
   exit(EXIT_FAILURE);
 }

 fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

 if (-1 == fd) {
   fprintf(stderr, "Cannot open '%s': %d, %s\\n",
           dev_name, errno, strerror(errno));
   exit(EXIT_FAILURE);
 }
}
