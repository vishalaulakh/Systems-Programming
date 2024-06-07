#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void printFileDetails(const char *filename, const struct stat *fileStat) {
    struct passwd *pwd = getpwuid(fileStat->st_uid);
    struct group *grp = getgrgid(fileStat->st_gid);

    char timeString[80];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localtime(&fileStat->st_mtime));

    printf("%lu ", (unsigned long)fileStat->st_ino);
    printf(S_ISDIR(fileStat->st_mode) ? "DIR  " : "REG  ");
    printf((fileStat->st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat->st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat->st_mode & S_IXUSR) ? "x" : "-");
    printf("     ");
    printf("%s    ", pwd ? pwd->pw_name : "");
    printf("%s    ", grp ? grp->gr_name : "");
    printf("%s    ", timeString);
    printf("%lld    %s\n", (long long)fileStat->st_size, filename);
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
        char filePath[1024];
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

