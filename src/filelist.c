
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "filelist.h"

FileList *create_FileList(void)
{
    FileList *list = (FileList *)malloc(sizeof(*list));
    list->cur = NULL;
    list->first = NULL;
    list->last = NULL;

    return list;
}

// 删除整个链表
void destoy_list(FileList *list)
{
    if (list == NULL)
    {
        return;
    }
    FileNode *p = list->last;
    while (list->first == list->last)
    {
        list->last = list->last->prev;
        list->last->next = NULL;
        p->next = NULL;
        p->prev = NULL;
        free(p);
        p = list->last;
    }
    free(list->first);
    free(list);
}


void add_FileNode(FileList *list, FileNode *p)
{
    if (list == NULL || p == NULL)
        return;

    if (list->first == NULL)
    {
        list->first = p;
        list->last = p;
        list->cur = p;
    }
    else
    {
        list->last->next = p;
        p->prev = list->last;
        list->last = p;
        list->last->next = list->first;
        list->first->prev = list->last;
    }
}

void make_inode(FileList *list)
{
    list->cur = list->first;
    int i = 0;
    do
    {
        list->cur->i_node = i;
        i++;
        list->cur = list->cur->next;
        if (list->cur == NULL)
        {
            list->cur == list->first;
            return;
        }

    } while (list->cur != list->first);
}

void print_FistList(FileList *list)
{
    if (list == NULL || list->first == NULL)
    {
        printf("FileList IS  EMPTY!\n");
        return;
    }
    FileNode *p = list->first;
    do
    {
        printf("pic_name: %s\n", p->pic_name);
        printf("list_inode:%d\n", p->i_node);
        p = p->next;
        if (p == NULL)
        {
            return;
        }
    } while (p != list->first);
}

void change_to_next_pictures(FileList *list)
{
    if (list == NULL || list->first == NULL)
        return;

    if (list->cur->next == NULL)
    {
        list->cur = list->first;
    }
    else
    {
        list->cur = list->cur->next;
    }
}

/*
    search_pictures:在一个指定的目录下，搜索图片文件
    @path:指定的搜索目录
    返回值：
        把搜索到的图片文件名，构造一个“FileList”的链表
*/
FileList *search_pictures(const char *path)
{
    FileList *list = create_FileList();

    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("failed to opendir");
        return list;
    }

    struct dirent *dirp = NULL;

    while (dirp = readdir(dir))
    {

        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
        {
            continue;
        }

        char filename[512];
        sprintf(filename, "%s/%s", path, dirp->d_name);

        struct stat st;
        stat(filename, &st);

        int len = strlen(filename) - 4;

        if (S_ISREG(st.st_mode) &&
            (strcmp(filename + len, ".bmp") == 0 ||
             strcmp(filename + len, ".jpg") == 0))
        {
            FileNode *p = (FileNode *)malloc(sizeof(*p));
            strcpy(p->pic_name, filename);
            int i_node = 0;
            p->next = NULL;
            p->prev = NULL;

            add_FileNode(list, p);
        }
    }

    closedir(dir);

    return list;
}