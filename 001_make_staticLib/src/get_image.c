// #include "b.h"
#include "get_image.h"

int cam_close(int cam_fd)
{
    int ret = close(cam_fd); //disconnect camera

    return 0;
}


int cam_init(int IMAGE_WIDTH, int IMAGE_HEIGHT)
{
    int i;
    int ret;
    int cam_fd;
    int sel=0;
    
    struct v4l2_format format;
    struct v4l2_buffer video_buffer[BUFFER_COUNT];
    
    // Open Camera
    cam_fd = open(VIDEO_DEVICE, O_RDWR); //connect camera
    
    if (cam_fd < 0 )
        return -1;
    
    ret = ioctl(cam_fd, VIDIOC_S_INPUT, &sel); //setting video input
    if (ret < 0 )
        return -1;

    memset(&format, 0, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = SENSOR_COLORFORMAT; //V4L2_PIX_FMT_SGRBG10;//10bit raw
    format.fmt.pix.width = IMAGE_WIDTH;              //resolution
    format.fmt.pix.height = IMAGE_HEIGHT;
    ret = ioctl(cam_fd, VIDIOC_TRY_FMT, &format);
    
    if (ret != 0)
        return ret;
        
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(cam_fd, VIDIOC_S_FMT, &format);
    if (ret != 0)
        return ret;

    struct v4l2_requestbuffers req;
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(cam_fd, VIDIOC_REQBUFS, &req);
    if (ret != 0)
        return ret;
    if (req.count < BUFFER_COUNT)
        return ret;
    
    struct v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = req.type;
    buffer.memory = V4L2_MEMORY_MMAP;
    for (i = 0; i < req.count; i++)
    {
        buffer.index = i;
        ret = ioctl(cam_fd, VIDIOC_QUERYBUF, &buffer);
        if (ret != 0)
            return ret;

        video_buffer_ptr[i] = (uint8_t *)mmap(NULL, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, cam_fd, buffer.m.offset);
        if (video_buffer_ptr[i] == MAP_FAILED)
            return -1;

        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;
        ret = ioctl(cam_fd, VIDIOC_QBUF, &buffer);
        if (ret != 0)
            return ret;
    }

    int buffer_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(cam_fd, VIDIOC_STREAMON, &buffer_type);
    if (ret != 0)
        return ret;

    return cam_fd;
}

int cam_get_image(uint8_t *out_buffer, int out_buffer_size, int cam_fd)
{
    int ret;
    struct v4l2_buffer buffer;

    memset(&buffer, 0, sizeof(buffer));
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = BUFFER_COUNT;
    ret = ioctl(cam_fd, VIDIOC_DQBUF, &buffer);
    if (ret != 0)
        return ret;

    if (buffer.index < 0 || buffer.index >= BUFFER_COUNT)
        return ret;

    memcpy(out_buffer, video_buffer_ptr[buffer.index], out_buffer_size);

    ret = ioctl(cam_fd, VIDIOC_QBUF, &buffer);
    if (ret != 0)
        return ret;

    return 0;
}
