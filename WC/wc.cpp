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
	char subPath[_MAX_PATH];	//��·��
	_splitpath(path, drive, dir, fname, ext);
	_makepath(subPath, drive, dir, fname, NULL);
	_finddata_t fileInfo;	//�ļ���Ϣ�ṹ��
	memset(&fileInfo, 0, sizeof(fileInfo));	//����
	//����ָ��·������һ������ʽƥ����ļ���Ϣ
	long hFile = _findfirst(subPath, &fileInfo);
	if (hFile == -1L)	//�ļ������Ϊ-1����ʾ�ҵ�ƥ���ļ�
		return 0;
	do
	{
		//����"."�ϼ�Ŀ¼��".."��ǰĿ¼
		if (strncmp(fileInfo.name, ".", 2) == 0 || strncmp(fileInfo.name, "..", 3) == 0)
			continue;
		//�ϳɵ�ǰ�ļ��ľ���·��
		_splitpath(path, drive, dir, fname, ext);
		if (fileInfo.attrib & _A_SUBDIR)	//Ŀ¼
		{
			sprintf(subPath, "%s%s%s\\%s%s", drive, dir, fileInfo.name, fname, ext);
			number += browseDir(argv, subPath, numCount);	//��������������Ŀ¼�е��ļ�
		}
		else if (strstr(fileInfo.name, ext) == fileInfo.name + strlen(fileInfo.name) - strlen(ext))	//�ļ�
		{
			sprintf(subPath, "%s%s%s", drive, dir, fileInfo.name);
			if (strcmp(argv, "-c") == 0)
				number += charNum(subPath);	//���÷��ʷ������ʵ�ǰ�ļ�
			else if (strcmp(argv, "-w") == 0)
				number += wordNum(subPath);
			else if (strcmp(argv, "-l") == 0)
				number += lineNum(subPath);
		}
	} while (_findnext(hFile, &fileInfo) == 0);	//������һ���ļ�
	_findclose(hFile);	//�ر��ļ����
	return number;
}

void multiNum(char *argv[])
{
	if (argv == NULL)
		return;
	if (strcmp(argv[2], "-c") == 0)
		printf("���ַ�����%ld", browseDir(argv[2], argv[3], charNum));
	else if (strcmp(argv[2], "-w") == 0)
		printf("�ܵ�������%ld", browseDir(argv[2], argv[3], wordNum));
	else if (strcmp(argv[2], "-l") == 0)
		printf("��������%ld", browseDir(argv[2], argv[3], lineNum));
}

int main(int argc, char *argv[])
{
	if (argc < 3)
		return -1;
	if (strcmp(argv[1], "-c") == 0)
		printf("�ַ�����%ld", charNum(argv[2]));
	else if (strcmp(argv[1], "-w") == 0)
		printf("��������%ld", wordNum(argv[2]));
	else if (strcmp(argv[1], "-l") == 0)
		printf("������%ld", lineNum(argv[2]));
	else if (strcmp(argv[1], "-s") == 0)
		multiNum(argv);
	return 0;
}