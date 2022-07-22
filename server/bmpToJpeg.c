#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <jpeglib.h>
#include "bitmapHead.h"
#define JPEG_QUALITY 90 //图片压缩质量
//读取文件头
int readFileHeader(FILE *fp, bmpFileHeader *bfHeader)
{
	fseek(fp, 0, SEEK_SET);
	fread(bfHeader, sizeof(bmpFileHeader), 1, fp);
	int i;
	printf("读入文件头：");
	char *p = (char *)bfHeader;
	for (i = 0; i < sizeof(bmpFileHeader); i++, p++)
	{
		printf("%02x ", *p);
	}
	printf("\n");
	printf("文件头信息如下：\n");
	printf("文件大小: %d 字节\n", bfHeader->bfSize);
	printf("位图数据偏移：%d\n", bfHeader->bfOffBits);
	return 0;
}

//读取信息头
int readInfoHeader(FILE *fp, bmpInfoHeader *biHeader)
{
	fseek(fp, 14, SEEK_SET);
	fread(biHeader, sizeof(bmpInfoHeader), 1, fp);
	int i;
	printf("读入位图信息头：\n");
	char *p = (char *)biHeader;
	for (i = 0; i < sizeof(bmpInfoHeader); i++, p++)
	{
		printf("%02x ", *p);
		if (i % 20 == 0 && i != 0)
			printf("\n");
	}
	printf("\n");
	printf("位图信息头信息如下：\n");
	printf("bmp图像宽度：%d\n", biHeader->biWidth);
	printf("bmp图像高度：%d\n", biHeader->biHeight);
	printf("bmp图像颜色位数: %d\n", biHeader->biBitCount);
	printf("bmp图像实际数据占用字节：%d\n", biHeader->biSizeImage);
	return 0;
}
// bmp转jpeg(bmpName,jpegName,bmpWidth,bmpHeight)
int BmpToJpeg(char *bmp_file, char *jpeg_file, int width, int height, int bitCount)
{

	FILE *fd;						   // bmp文件
	struct jpeg_compress_struct cinfo; //创建代表压缩的结构体
	struct jpeg_error_mgr jerr;		   // jpeg默认错误处理结构体
	FILE *outfile;					   //输出文件

	// BMP数据默认是（biHeight>0)
	//但偶尔也会出现biHeight<0，图片数据倒置的情况。
	int flag = 1; //是否逆序
	if (height < 0)
	{
		height = -height;
		flag = -1; //逆序存储
	}
	long sizeImage = width * height * bitCount / 8; //文件大小

	unsigned char *data; //数据空间
	data = (unsigned char *)malloc(sizeImage);
	fd = fopen(bmp_file, "rb");
	if (!fd)
	{
		printf("ERROR1:无法打开bmp文件，请检查路径是否正确!\n");
		free(data);
		exit(0);
		return -1;
	}
	fseek(fd, 54, SEEK_SET); //跳过文件头和信息头

	int ret = fread(data, sizeImage, 1, fd);
	if (ret == 0 && ferror(fd))
	{
		printf("ERROT2:无法读取bmp文件!\n");
		free(data);
		fclose(fd);
		return -1;
	}

	//初始化jpeg压缩对象
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	if ((outfile = fopen(jpeg_file, "wb")) == NULL)
	{
		//错误信息输出到文件中
		fprintf(stderr, "无法打开(创建) %s\n", jpeg_file);
		return -1;
	}
	jpeg_stdio_dest(&cinfo, outfile); //指定压缩后的图像存放的目标文件

	cinfo.image_height = height; //压缩后的宽度高度，像素为单位
	cinfo.image_width = width;
	cinfo.input_components = 3;		  //颜色分量
	cinfo.in_color_space = JCS_YCbCr; //设置色彩空间

	//设置压缩参数
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE); //设置图片压缩质量
	jpeg_start_compress(&cinfo, TRUE);			  //设置压缩入口

	JSAMPROW *row_pointer; //图像像素的每一行
	row_pointer = malloc(height * width * 3);

	if (flag == -1) //逆序
	{
		long rgb_index = 0; //定义bmp像素访问索引
		for (int i = 0; i < height; i++)
		{
			unsigned char *lineData = NULL; //定义这一行的像素
			lineData = malloc(width * 3);	//开空间
			for (int j = 0; j < width; j++)
			{
				// YCrCb色彩空间
				lineData[j * 3 + 2] = 0.5 * data[rgb_index + 2] - 0.4178 * data[rgb_index + 1] - 0.0831 * data[rgb_index] + 128;
				lineData[j * 3 + 1] = -0.1687 * data[rgb_index + 2] - 0.3313 * data[rgb_index + 1] + 0.5 * data[rgb_index] + 128;
				lineData[j * 3 + 0] = 0.299 * data[rgb_index + 2] + 0.587 * data[rgb_index + 1] + 0.114 * data[rgb_index];
				rgb_index += 3;
				if (bitCount == 32)
				{
					rgb_index++; //跳过Alpha通道
				}
			}
			//如果是24位真彩图而且宽度不是4的倍数，需要跳过最后补了的无用像素
			if ((width * bitCount / 8) % 4)
			{
				rgb_index += width % 4;
			}
			row_pointer[i] = lineData; //设置第i行像素
		}
	}
	else if (flag == 1) //不逆序
	{
		long rgb_index = 0; //定义bmp像素访问索引
		for (int i = height - 1; i >= 0; i--)
		{
			unsigned char *lineData = NULL; //定义这一行的像素
			lineData = malloc(width * 3);	//开空间
			for (int j = 0; j < width; j++)
			{
				// YCrCb色彩空间
				lineData[j * 3 + 2] = 0.5 * data[rgb_index + 2] - 0.4178 * data[rgb_index + 1] - 0.0831 * data[rgb_index] + 128;
				lineData[j * 3 + 1] = -0.1687 * data[rgb_index + 2] - 0.3313 * data[rgb_index + 1] + 0.5 * data[rgb_index] + 128;
				lineData[j * 3 + 0] = 0.299 * data[rgb_index + 2] + 0.587 * data[rgb_index + 1] + 0.114 * data[rgb_index];
				rgb_index += 3;
				if (bitCount == 32)
				{
					rgb_index++; //跳过Alpha通道
				}
			}
			//如果是24位真彩图而且宽度不是4的倍数，需要跳过最后补了的无用像素
			if ((width * bitCount / 8) % 4)
			{
				rgb_index += width % 4;
			}
			row_pointer[i] = lineData; //设置第i行像素
		}
	}
	jpeg_write_scanlines(&cinfo, row_pointer, height); //输出每一行像素
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	//释放空间
	free(row_pointer);
	free(data);
	fclose(fd);
	fclose(outfile);
	return 0;
}
void bmp2jpeg(char *input_name, char *output_name)
{
	FILE *fp;
	fp = fopen(input_name, "rb");
	if (fp == NULL)
	{
		printf("打开bmp文件失败\n");
		exit(0);
	}
	//输出信息
	// bmpFileHeader bf;
	// bmpInfoHeader bi;
	// readFileHeader(fp,&bf);
	// readInfoHeader(fp,&bi);

	bmpFileHeader bfHeader; //文件头
	fseek(fp, 0, SEEK_SET);
	fread(&bfHeader, sizeof(bmpFileHeader), 1, fp);
	bmpInfoHeader biHeader; //信息头
	fseek(fp, 14, SEEK_SET);
	fread(&biHeader, sizeof(bmpInfoHeader), 1, fp);
	fseek(fp, 0, SEEK_SET);
	BmpToJpeg(input_name, output_name, biHeader.biWidth, biHeader.biHeight, biHeader.biBitCount);
}