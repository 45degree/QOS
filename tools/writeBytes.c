#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

typedef struct Options {
    char* input;
    char* output;
    int start;
} Options;

/**
 * 从命令行中获取数据
 */
int getOptions(int argc, char* argv[], Options* options);

/**
 * 打开指定的文件, 并将inputfile的内容写入缓冲区
 */
int OpenFile(FILE** inputFs, FILE** outputFs, Options* options);

/**
 * 关闭指定文件
 */
int CloseFile(FILE* inputFs, FILE* outputFs);

/**
 * 将input文件写入到output的指定位置
 */
int WriteByte(FILE* outputFs, int len);


char* buffer = NULL;


int main(int argc, char* argv[]) {
    Options options;
    FILE *ifs = NULL, *ofs = NULL;

#ifdef _debug
    printf("start debug\n");
#endif

    if(getOptions(argc, argv, &options) != 1) exit(-1);

#ifdef _debug
    printf("input: %s, output: %s, start: %d\n", options.input, options.output, options.start);
#endif

    int input_size;
    if((input_size =OpenFile(&ifs, &ofs, &options)) < 0) {
        CloseFile(ifs, ofs);
        exit(-1);
    }

#ifdef _debug
    printf("sizeof char: %d \t buffer len: %d\n", (int)sizeof(char), (int)strlen(buffer));
    printf("buffer: %s", buffer);
#endif

    if(WriteByte(ofs, input_size) != 1) {
        CloseFile(ifs, ofs);
        exit(-1);
    }

    CloseFile(ifs, ofs);
    return 0;
}

int getOptions(int argc, char* argv[], Options* options) {
    char* inputfile, *outputfile;
    int start;

    options->input = NULL;
    options->output = NULL;
    options->start = 0;

    int opt;
    while((opt = getopt(argc, argv, "i:o:s:"))!= -1) {

        if(optarg == NULL) {
            printf("有参数为空");
            return -1;
        }

        switch (opt) {
            case 'i':
                inputfile = optarg;
                break;
            case 'o':
                outputfile = optarg;
                break;
            case 's':
                // 存在风险
                start = atoi(optarg);
                break;
            default:
                printf("无效命令！！！！");
                return -1;
        }
    }

    options->input = inputfile;
    options->output = outputfile;
    options->start = start;

    return 1;
}

int OpenFile(FILE** inputFs, FILE** outputFs, Options* options) {
    *inputFs = fopen(options->input, "r");
    *outputFs = fopen(options->output, "r+");
    if(fseek(*outputFs, options->start, SEEK_SET) != 0) return -1;

#ifdef _debug
    printf("test input and output!\n");
    if(*inputFs == NULL) printf("input is NULL\n");
    if(*outputFs == NULL) printf("output is NULL\n");
#endif

    /*写入缓冲区*/
    if(buffer != NULL) free(buffer);
    fseek(*inputFs, 0, SEEK_END);
    int input_size = ftell(*inputFs);
    fseek(*inputFs, 0, SEEK_SET);
    buffer = (char*)malloc(input_size);

#ifdef _debug
    printf("input_size is %d\n", input_size);
#endif
    
    if ( fread(buffer, input_size, 1, *inputFs) != 1) {
        printf("buffer 读入失败\n");
        return -1;
    }
    return input_size;
}

int CloseFile(FILE* inputFs, FILE* outputFs) {
    if(fclose(inputFs) == EOF || fclose(outputFs) == EOF) {
        printf("关闭文件错误");
        return -1;
    }
    free(buffer);
    return 1;
}

int WriteByte(FILE* outputFs, int len) {
   if(fwrite(buffer, len, 1, outputFs) != 1) {
        printf("写入输出文件失败！！！\n");
        return -1;
   }
   return 1;
}
