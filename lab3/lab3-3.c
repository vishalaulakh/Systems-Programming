#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <limits.h> 
#include <pwd.h>
#include <grp.h>
 
#ifndef PATH_MAX
#define PATH_MAX 4096 
#endif

void print_permissions(mode_t mode) {
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

void printFileDetails(const char *filename, const struct stat *fileStat) {
    struct passwd *pwd = getpwuid(fileStat->st_uid);
    struct group *grp = getgrgid(fileStat->st_gid);

    char timeString[80];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localtime(&fileStat->st_mtime));

    printf("%lu ", (unsigned long)fileStat->st_ino);
    printf(S_ISDIR(fileStat->st_mode) ? "DIR  " : "REG  ");
    print_permissions(fileStat->st_mode);
    printf(" %d %d %s %s %s %lld %s\n",
           fileStat->st_uid, fileStat->st_gid,
           pwd ? pwd->pw_name : "",
           grp ? grp->gr_name : "",
           timeString,
           (long long)fileStat->st_size, filename);
}

void listFiles(const char *path, int minDaysOld, int maxDaysOld, long long minSize, long long maxSize) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    printf("inode Type Permission UID GID Modification date Size Filename\n");

    while ((entry = readdir(dir)) != NULL) {
        char filePath[PATH_MAX];
        snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);

        if (stat(filePath, &fileStat) == -1) {
            perror("Error getting file status");
            exit(EXIT_FAILURE);
        }

        // Filter based on modification time and size
        time_t currentTime = time(NULL);
        time_t modifiedTime = fileStat.st_mtime;
        int daysOld = (int)((currentTime - modifiedTime) / (24 * 3600));

        if ((minDaysOld == -1 || daysOld >= minDaysOld) &&
            (maxDaysOld == -1 || daysOld <= maxDaysOld) &&
            (minSize == -1 || fileStat.st_size >= minSize) &&
            (maxSize == -1 || fileStat.st_size <= maxSize)) {
            printFileDetails(entry->d_name, &fileStat);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *path;
    int minDaysOld = -1, maxDaysOld = -1;
    long long minSize = -1, maxSize = -1;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-o daysOld] [-y daysYounger] [-l minSize] [-s maxSize] [directory]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse command line arguments
    for (int i = 1; i < argc - 1; i += 2) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'o':
                    minDaysOld = atoi(argv[i + 1]);
                    break;
                case 'y':
                    maxDaysOld = atoi(argv[i + 1]);
                    break;
                case 'l':
                    minSize = atoll(argv[i + 1]);
                    break;
                case 's':
                    maxSize = atoll(argv[i + 1]);
                    break;
                default:
                    fprintf(stderr, "Invalid option: %s\n", argv[i]);
                    exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Invalid option: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    path = (argc % 2 == 0) ? argv[argc - 1] : ".";  // Default to current directory if no directory is specified

    listFiles(path, minDaysOld, maxDaysOld, minSize, maxSize);

    return 0;
}

