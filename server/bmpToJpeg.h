#ifndef BMPTOJPEG_H
#define BMPTOJPEG_H
#include <stdio.h>
#include "bitmapHead.h"
/**
 * @brief 输出bmp文件头信息
 *
 * @param fp bmp文件
 * @param bfHeader 文件头结构体
 * @return int
 */
int readFileHeader(FILE *fp, bmpFileHeader *bfHeader);

/**
 * @brief 输出bmp信息头信息
 *
 * @param fp bmp文件
 * @param biHeader bmp信息头结构体
 * @return int
 */
int readInfoHeader(FILE *fp, bmpInfoHeader *biHeader);

/**
 * @brief bmp文件转jpeg文件
 *
 * @param input_name bmp文件名
 * @param output_name jpeg文件名
 */
void bmp2jpeg(char *input_name, char *output_name);
#endif