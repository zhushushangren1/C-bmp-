#ifndef JPEGTOBMP_H
#define JPEGTOBMP_H
#include <stdio.h>
/**
 * @brief jpeg转为bmp格式
 *
 * @param input_file 输入的jpeg的文件名
 * @param output_file 转换后的bmp的文件名
 */
void analyse_jpeg(FILE *input_file, FILE *output_file);
#endif