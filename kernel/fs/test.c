
#include <stdlib.h>

#include "kernel.h"
#include "fs.h"
#include "fs/path.h"

extern /* static const */ struct inode rootdir_inodes[];

typedef struct vega_dirent dirent;
typedef void DIR;

DIR *opendir(const char *name);
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
int closedir(DIR *dirp);
int sys_open2(const char *pathname, int flags);

void rootdir_test(void)
{
	printk("@@ rootdir_lookup:\n");
	struct dentry dent = { .d_name = "proc" };
	struct dentry *proc_dent = vfs_lookup(&rootdir_inodes[0], &dent);
	if (proc_dent == NULL)
		printk("NULL dentry\n");
	printk("  procfs->ino = %d\n", proc_dent->d_inode->i_ino);

	printk("@@ opendir:\n");
	DIR *dir = opendir("");
	printk("@@ readdir_r:\n");
	struct dirent dirent, *dirent_res;
	readdir_r(dir, &dirent, &dirent_res);
	printk("% 6d %s\n", dirent.d_ino, dirent.d_name);
	readdir_r(dir, &dirent, &dirent_res);
	printk("% 6d %s\n", dirent.d_ino, dirent.d_name);
	readdir_r(dir, &dirent, &dirent_res);
	printk("% 6d %s\n", dirent.d_ino, dirent.d_name);
	readdir_r(dir, &dirent, &dirent_res);
	printk("% 6d %s\n", dirent.d_ino, dirent.d_name);

	closedir(dir);
	if (!readdir_r(dir, &dirent, &dirent_res))
		printk("readdir should fail!\n");
	printk("@@ rootdir test end\n");

	char buf[32];
	path_head(buf, "/home/foo");
	printk("@@ path manp: %s\n", buf);
	path_head(buf, "relative/bar");
	printk("@@ path manp: %s\n", buf);

	//sys_open2("/home/foo/bar", 0);
	sys_open("/dev", 0);

	sys_open("/proc/version", 0);

	/* dir = opendir("/proc"); */
	/* printk("@@ readdir_r:\n"); */
	/* readdir_r(dir, &dirent, &dirent_res); */
	/* printk("% 6d %s\n", dirent.d_ino, dirent.d_name); */

	printk("@@ devfs\n");
	create_dev_inode("ttyS0");
	sys_open("/dev/ttyS0", 0);
}
