//
// Created by Vivian Bai on 7/26/22.
//

#ifndef CAPTURE_H
#define CAPTURE_H

extern const char *dev_name;


void open_device();
int init_device();
//void start_capturing();
//void mainloop();
//void stop_capturing();
uint32_t set_format(uint32_t height, uint32_t width, uint32_t imgFormat);
void uninit_device();
void close_device();
int read_frame(void *cameraBuffer, uint32_t size, size_t *readSize);
void set_exposure_time(uint32_t exposureTime);
#endif // CAPTURE_H
