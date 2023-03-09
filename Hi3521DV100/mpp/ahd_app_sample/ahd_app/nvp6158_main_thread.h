#ifndef __NVP6158_MAIN_THREAD_H__
#define __NVP6158_MAIN_THREAD_H__

void nvp6158_start(void);
void nvp6158_stop(void);
void nvp6158_control_coax_enter(int channel);
void nvp6158_control_coax_up(int channel);
void nvp6158_control_coax_down(int channel);
void nvp6158_control_coax_right(int channel);
void nvp6158_control_coax_left(int channel);
void nvp6158_getStatus(int len, int *ret);
#endif
