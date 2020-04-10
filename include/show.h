#ifndef DISPLAY_H
#define DISPLAY_H

extern void disp_str(const char* mes);
extern void disp_pose_set(int position);
extern void disp_color_str(const char* str, int TextColor);
extern void disp_int(int num);

// 以下函数在汇编中实现
extern void  _disp_str(const char* str);
extern void  _disp_pose_set(int pose);
extern void  _disp_color_str(const char* str, int TextColor);

#endif