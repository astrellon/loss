/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
*/

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ram_filesystem.h>
#include <loss/fs/ram_filesystem_drive.h>

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static loss::VirtualFileSystem *vfs = nullptr;
static loss::RamFileSystem *ramfs = nullptr;

static int hello_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    // Not really used at the moment
    stbuf->st_nlink = 1;
    /*
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
    } else
        res = -ENOENT;
        */
    loss::MetadataDef metadata;
    auto read_result = vfs->entry_metadata(path, metadata);
    if (read_result != loss::SUCCESS)
    {
        res = -ENOENT;
    }
    else
    {
        if (metadata.type() == loss::FOLDER_ENTRY)
        {
            stbuf->st_mode = S_IFDIR | 0755;
        }
        else if (metadata.type() == loss::FILE_ENTRY)
        {
            stbuf->st_mode = S_IFREG | 0444;
            uint32_t size;
            vfs->entry_size(path, size);
            stbuf->st_size = size;
        }
    }

    return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    loss::FolderEntry folder;
    auto read_result = vfs->read_folder(path, folder);

    if (read_result == loss::SUCCESS)
    {
        for (auto &iter : folder)
        {
            filler(buf, iter.first.c_str(), NULL, 0);
        }
    }
    //filler(buf, hello_path + 1, NULL, 0);

    return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    len = strlen(hello_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, hello_str + offset, size);
    } else
        size = 0;

    return size;
}

static struct fuse_operations hello_oper = {
    .getattr    = hello_getattr,
    .readdir    = hello_readdir,
    .open       = hello_open,
    .read       = hello_read,
};

int main(int argc, char *argv[])
{
    int i;
	for(i = 1; i < argc && (argv[i][0] == '-'); i++)
    {
		if(i == argc)
        {
			return (-1);
		}
	}

    vfs = new loss::VirtualFileSystem();
    const char *file_to_open = argv[i];
    {
        std::ifstream input(file_to_open);
        loss::RamFileSystemDeserialise deserialise(input, ramfs);
        deserialise.load();
    }
    vfs->root_filesystem(ramfs);

	for(; i < argc; i++)
    {
		argv[i] = argv[i+1];
	}
	argc--;
    auto result = fuse_main(argc, argv, &hello_oper, NULL);

    delete vfs;

    return result;
}
