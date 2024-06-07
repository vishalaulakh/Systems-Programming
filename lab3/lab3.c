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

//Function that prints the permissions in a rwxrwxrwx format
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
// Function to print the Details of a file (inode, type, permissions, UID, GID, modification date, size, filename)
void printFileDetails(const char *filename, const struct stat *fileStat) {
    struct passwd *pwd = getpwuid(fileStat->st_uid); //get the user info based on UID
    struct group *grp = getgrgid(fileStat->st_gid); // get the group information based on GID

   // Formating the modification time as a string
    char timeString[80];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localtime(&fileStat->st_mtime));
   // Printing the Inode number
    printf("%lu ", (unsigned long)fileStat->st_ino);
  
    // Printing "DIR" if it is a directory, otherwise "REG" for a regular file
    printf(S_ISDIR(fileStat->st_mode) ? "DIR  " : "REG  ");

    // Printing the file permissions
    print_permissions(fileStat->st_mode);

   // Printing UID, GID, UserName, GroupName, modification time, file Size and File Name.
    printf(" %-4d %-4d  %-10s  %-10s %-19s %-8lld %s\n",
           fileStat->st_uid, fileStat->st_gid,
           pwd ? pwd->pw_name : "", // if pwd is not NULL, use the user name; otherwise use an empty string
           grp ? grp->gr_name : "", // if grp is not NULL, using the group name; otherwise an empty string
           timeString,
           (long long)fileStat->st_size, filename);
}
// Function to list the files in a directory based on the specified criteria
void listFiles(const char *path, int minDaysOld, int maxDaysOld, long long minSize, long long maxSize) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    printf("Inode   Type Permission%-4s %-4s  %-10s  %-10s %-19s %-8s %s\n", " UID", " GID", " UName","GName","Modification date","Size","Filename");

    while ((entry = readdir(dir)) != NULL) {

	// Skip entries with names "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char filePath[PATH_MAX];
        snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);

        if (stat(filePath, &fileStat) == -1) {
            perror("Error getting file status");
            exit(EXIT_FAILURE);
        }

        // Filter on the  modification time and size of files
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

//Main function to parse the command line arguments and call the listFiles function
int main(int argc, char *argv[]) {
    const char *path=".";
    int minDaysOld = -1, maxDaysOld = -1;
    long long minSize = -1, maxSize = -1;

// if no directory path is specified, set the current directory as the directory to run the commands
//    if (argc < 2) {
//	path = ".";
//    }

    // Parse the command line arguments
    for (int i = 1; i < argc - 1; i += 2) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'o':
                    minDaysOld = atoi(argv[i + 1]); //Converting the string to an integer
                    break;
                case 'y':
                    maxDaysOld = atoi(argv[i + 1]); // Converting the string to an Integer
                    break;
                case 'l':
                    minSize = atoll(argv[i + 1]); //Converting the  String to a long long
                    break;
                case 's':
                    maxSize = atoll(argv[i + 1]); //Converting the String to a long long as well
                    break;
                default:
                    fprintf(stderr, "Invalid option: %s\n", argv[i]); //If some other Character is Provided
                    exit(EXIT_FAILURE); //Exit 
            }
        } else {
            fprintf(stderr, "Invalid option: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    // the last argument is the directory path
  //  path = (argc == 1 || (argc > 1 && strlen(argv[argc - 1]) == 0)) ? "." : argv[argc - 1];     // Default to the current directory if no directory is specified
	if (argc > 1 && argv[argc - 1][0] != '-') {
        	path = argv[argc - 1];
    	}	

    // Call the function to list files based on specified criteria
    listFiles(path, minDaysOld, maxDaysOld, minSize, maxSize);

    return 0;
}

