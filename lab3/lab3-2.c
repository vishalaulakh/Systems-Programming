#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <limits.h> 
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

void process_directory(char *dir_path, int min_age, int max_age, int min_size, int max_size) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    time_t current_time = time(NULL);

    if (!(dir = opendir(dir_path))) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", dir_path, entry->d_name);

        if (stat(file_path, &fileStat) < 0) {
            perror("stat");
            continue;
        }

        // Check file age
        time_t file_age = current_time - fileStat.st_mtime;
        if ((min_age > 0 && file_age < min_age * 24 * 3600) || (max_age > 0 && file_age > max_age * 24 * 3600)) {
            continue;
        }

        // Check file size
        if ((min_size > 0 && fileStat.st_size <= min_size) || (max_size > 0 && fileStat.st_size >= max_size)) {
            continue;
        }

        // Print file information
        printf("%lu %s ", fileStat.st_ino, entry->d_type == DT_DIR ? "DIR" : "REG");
        print_permissions(fileStat.st_mode);
        printf(" %d %d %04d-%02d-%02d %02d:%02d:%02d %ld %s\n",
               fileStat.st_uid, fileStat.st_gid,
               1900 + localtime(&(fileStat.st_mtime))->tm_year,
               1 + localtime(&(fileStat.st_mtime))->tm_mon,
               localtime(&(fileStat.st_mtime))->tm_mday,
               localtime(&(fileStat.st_mtime))->tm_hour,
               localtime(&(fileStat.st_mtime))->tm_min,
               localtime(&(fileStat.st_mtime))->tm_sec,
               fileStat.st_size,
               entry->d_name);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    char *dir_path = ".";  // Default to current directory

    if (argc > 1 && argv[1][0] != '-') {
        dir_path = argv[1];
        argc--;
        argv++;
    }

    if (argc % 2 == 0) {
        fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int min_age = 0, max_age = 0, min_size = 0, max_size = 0;

    for (int i = 1; i < argc - 1; i += 2) {
        if (strcmp(argv[i], "-o") == 0) {
            min_age = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-y") == 0) {
            max_age = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-l") == 0) {
            min_size = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-s") == 0) {
            max_size = atoi(argv[i + 1]);
        }
    }

    process_directory(dir_path, min_age, max_age, min_size, max_size);

    return 0;
}

