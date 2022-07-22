#include "jpegToBmp.h"
#include <jpeglib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "bitmapHead.h"
void write_bmp_header(j_decompress_ptr cinfo, FILE *output_file)
{
    bmpFileHeader bfh;
    bmpInfoHeader bih;

    unsigned long width;
    unsigned long height;
    unsigned short depth;
    unsigned long headersize;
    unsigned long filesize;

    width = cinfo->output_width;
    height = cinfo->output_height;
    depth = cinfo->output_components;
    //计算偏移量和文件大小
    headersize = 14 + 40;
    filesize = headersize + width * height * depth;
    //给其他参数设0
    memset(&bfh, 0, 14);
    memset(&bih, 0, 40);
    //写入比较关键的几个bmp头参数
    bfh.bfType = 0x4D42;
    bfh.bfSize = filesize;      //整个文件大小
    bfh.bfOffBits = headersize; //偏移量54
    //写入信息头
    bih.biSize = 40;                            //信息头大小
    bih.biWidth = width;                        //图片宽度
    bih.biHeight = height;                      //图片高度
    bih.biPlanse = 1;                           //平面数1
    bih.biBitCount = (unsigned short)depth * 8; //位深度
    bih.biSizeImage = width * height * depth;   //图片信息大小

    fwrite(&bfh, 14, 1, output_file);
    fwrite(&bih, 40, 1, output_file);
}

void write_bmp_data(j_decompress_ptr cinfo, unsigned char *src_buff, FILE *output_file)
{
    unsigned char *dst_width_buff;
    unsigned char *point;

    unsigned long width;
    unsigned long height;
    unsigned short depth;

    width = cinfo->output_width;
    height = cinfo->output_height;
    depth = cinfo->output_components;

    dst_width_buff = malloc(width * depth + 4 - ((width * depth - 1) % 4 + 1)); //补齐4字节
    memset(dst_width_buff, 0, sizeof(unsigned char) * width * depth);
    point = src_buff + width * depth * (height - 1); //倒着写数据，bmp格式是倒的，jpg是正的
    for (unsigned long i = 0; i < height; i++)
    {
        for (unsigned long j = 0; j < width * depth; j += depth)
        {
            //
            dst_width_buff[j + 2] = point[j + 0];
            dst_width_buff[j + 1] = point[j + 1];
            dst_width_buff[j + 0] = point[j + 2];
        }
        point -= width * depth;                                                                    //指针偏移
        fwrite(dst_width_buff, width * depth + 4 - ((width * depth - 1) % 4 + 1), 1, output_file); //一次写一行
    }
}

void analyse_jpeg(FILE *input_file, FILE *output_file)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    unsigned char *src_buff;
    unsigned char *point;

    cinfo.err = jpeg_std_error(&jerr); //一下为libjpeg函数，具体参看相关文档
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, input_file);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    unsigned long width = cinfo.output_width;
    unsigned long height = cinfo.output_height;
    unsigned short depth = cinfo.output_components;

    // printf("%ld\n%ld\n%d\n",width,height,depth);
    src_buff = malloc(width * height * depth);
    memset(src_buff, 0, sizeof(unsigned char) * width * height * depth);

    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, width * depth, 1);

    point = src_buff;
    while (cinfo.output_scanline < height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1); //读取一行jpg图像数据到buffer
        memcpy(point, *buffer, width * depth);  //将buffer中的数据逐行给src_buff
        point += width * depth;                 //一次改变一行
    }

    write_bmp_header(&cinfo, output_file);         //写bmp文件头
    write_bmp_data(&cinfo, src_buff, output_file); //写bmp像素数据

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
}