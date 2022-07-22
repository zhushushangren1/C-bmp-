#ifndef BITMAPHEAD_H
#define BITMAPHEAD_H
#include <stdint.h>
#pragma pack(1) //按照1字节对齐
// bmp文件头 14字节
typedef struct BmpFileHeader
{
	uint16_t bfType;	  //标识 固定值就是BM(0x 4D D2)两字
	uint32_t bfSize;	  //整个BMP文件的大小
	uint16_t bfReserved1; //保留字1，没意义，设置为0
	uint16_t bfReserved2; //保留字2，没意义，设置为0
	uint32_t bfOffBits;	  //偏移数：文件头+信息头+调色板的大小
} bmpFileHeader;
// bmp信息头 40字节
typedef struct BmpInfoHeader
{
	uint32_t biSize;		  // bmp信息头的大小，40
	uint32_t biWidth;		  // bmp位图的宽度，单位为像素
	uint32_t biHeight;		  // bmp位图的高度，单位为像素
	uint16_t biPlanse;		  //平面数，固定值1
	uint16_t biBitCount;	  //每个像素的位数，1:黑白图,4:16色,8:256色,24:真彩色,32:比24多了Alpha通道
	uint32_t ComPression;	  //压缩方式 0为不压缩
	uint32_t biSizeImage;	  //位图全部像素占用的字节数
	uint32_t biXPelsPerMeter; //水平分辨率(像素/米),一般为0
	uint32_t biYPelsPerMeter; //垂直分辨率(像素/米),一般为0
	uint32_t biClrUser;		  //位图使用的颜色数,为0表示使用全部
	uint32_t biClrImportant;  //重要的颜色数,为0表示全部都重要
} bmpInfoHeader;
#endif
