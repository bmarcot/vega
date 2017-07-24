#ifndef _DIRENT_H
#define _DIRENT_H

typedef void DIR;
struct dirent;

DIR *opendir(const char *dirname);
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
int closedir(DIR *dirp);

#endif /* !_DIRENT_H */
