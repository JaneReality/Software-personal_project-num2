#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long charNum(const char *filePath)
{
	if (filePath == NULL)
		return 0;
	FILE *fp = fopen(filePath, "r");
	if (fp == NULL)
		return 0;
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fclose(fp);
	return size;
}

long wordNum(const char *filePath)
{
	if (filePath == NULL)
		return 0;
	FILE *fp = fopen(filePath, "r");
	if (fp == NULL)
		return 0;
	static char non_word[] = " ,.!\n";
	static long size = strlen(non_word);
	char flag = 1;
	char ch = fgetc(fp);
	int index;
	long nWord = 0;
	while (!feof(fp))
	{
		for (index = 0; index < size;++index)
		if (ch == non_word[index])
			break;
		if (index < size)
			flag = 1;
		else if (flag)
		{
			++nWord;
			flag = 0;
		}
		ch = fgetc(fp);
	}
	return nWord;
}

long lineNum(const char *filePath)
{
	if (filePath == NULL)
		return 0;
	FILE *fp = fopen(filePath, "r");
	if (fp == NULL)
		return 0;
	long nLine = 0;
	char ch = fgetc(fp);
	while (!feof(fp))
	{
		if (ch == '\n')
			++nLine;
		ch = fgetc(fp);
	}
	fclose(fp);
	return nLine + 1;
}

#include <io.h>

long browseDir(char argv[], const char *path, long(*numCount)(const char*))
{
	if (strcmp(argv, "-c") != 0
		&& strcmp(argv, "-w") != 0
		&& strcmp(argv, "-l") != 0)
		return 0;
	if (path == NULL || numCount == NULL)
		return 0;
	long number = 0;
	static char drive[_MAX_DRIVE];
	static char dir[_MAX_DIR];
	static char fname[_MAX_FNAME];
	static char ext[_MAX_EXT];
	char subPath[_MAX_PATH];	//子路径
	_splitpath(path, drive, dir, fname, ext);
	_makepath(subPath, drive, dir, fname, NULL);
	_finddata_t fileInfo;	//文件信息结构体
	memset(&fileInfo, 0, sizeof(fileInfo));	//清零
	//搜索指定路径，第一个与表达式匹配的文件信息
	long hFile = _findfirst(subPath, &fileInfo);
	if (hFile == -1L)	//文件句柄不为-1，表示找到匹配文件
		return 0;
	do
	{
		//忽略"."上级目录和".."当前目录
		if (strncmp(fileInfo.name, ".", 2) == 0 || strncmp(fileInfo.name, "..", 3) == 0)
			continue;
		//合成当前文件的绝对路径
		_splitpath(path, drive, dir, fname, ext);
		if (fileInfo.attrib & _A_SUBDIR)	//目录
		{
			sprintf(subPath, "%s%s%s\\%s%s", drive, dir, fileInfo.name, fname, ext);
			number += browseDir(argv, subPath, numCount);	//调用自身搜索子目录中的文件
		}
		else if (strstr(fileInfo.name, ext) == fileInfo.name + strlen(fileInfo.name) - strlen(ext))	//文件
		{
			sprintf(subPath, "%s%s%s", drive, dir, fileInfo.name);
			if (strcmp(argv, "-c") == 0)
				number += charNum(subPath);	//调用访问方法访问当前文件
			else if (strcmp(argv, "-w") == 0)
				number += wordNum(subPath);
			else if (strcmp(argv, "-l") == 0)
				number += lineNum(subPath);
		}
	} while (_findnext(hFile, &fileInfo) == 0);	//搜索下一个文件
	_findclose(hFile);	//关闭文件句柄
	return number;
}

void multiNum(char *argv[])
{
	if (argv == NULL)
		return;
	if (strcmp(argv[2], "-c") == 0)
		printf("总字符数：%ld", browseDir(argv[2], argv[3], charNum));
	else if (strcmp(argv[2], "-w") == 0)
		printf("总单词数：%ld", browseDir(argv[2], argv[3], wordNum));
	else if (strcmp(argv[2], "-l") == 0)
		printf("总行数：%ld", browseDir(argv[2], argv[3], lineNum));
}

int main(int argc, char *argv[])
{
	if (argc < 3)
		return -1;
	if (strcmp(argv[1], "-c") == 0)
		printf("字符数：%ld", charNum(argv[2]));
	else if (strcmp(argv[1], "-w") == 0)
		printf("单词数：%ld", wordNum(argv[2]));
	else if (strcmp(argv[1], "-l") == 0)
		printf("行数：%ld", lineNum(argv[2]));
	else if (strcmp(argv[1], "-s") == 0)
		multiNum(argv);
	return 0;
}