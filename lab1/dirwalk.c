#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <locale.h>

int show_links = 0;
int show_dirs = 1;
int show_files = 1;
int sort_entries = 0;

#define MAX_ENTRIES 1024

char *entries[MAX_ENTRIES];
int count = 0;

int compare(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

void list_directory(const char *path)
{
    struct dirent *entry;
    struct stat file_stat;
    DIR *dp = opendir(path);

    if (dp == NULL)
    {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (lstat(fullpath, &file_stat) == -1)
        {
            perror("lstat");
            continue;
        }

        if ((S_ISLNK(file_stat.st_mode) && show_links) ||
            (S_ISDIR(file_stat.st_mode) && show_dirs) ||
            (S_ISREG(file_stat.st_mode) && show_files))
        {
            if (count < MAX_ENTRIES)
            {
                entries[count++] = strdup(fullpath); // Копируем строку
            }
        }
    }

    closedir(dp);

    if (sort_entries)
    {
        qsort(entries, count, sizeof(char *), compare);
    }

    for (int i = 0; i < count; i++)
    {
        printf("%s\n", entries[i]);
        free(entries[i]);
    }
}

void parse_options(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "ldfs")) != -1)
    {
        switch (opt)
        {
        case 'l':
            show_links = 1;
            show_files = 0;
            break;
        case 'd':
            show_dirs = 1;
            show_files = 0;
            show_links = 0;
            break;
        case 'f':
            show_files = 1;
            show_dirs = 0;
            show_links = 0;
            break;
        case 's':
            sort_entries = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-l] [-d] [-f] [-s] [dir]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    // Устанавливаем локаль
    setlocale(LC_ALL, "C");

    parse_options(argc, argv);

    char *dir = (optind < argc) ? argv[optind] : ".";

    list_directory(dir);

    return 0;
}
