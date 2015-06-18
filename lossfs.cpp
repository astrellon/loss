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
    std::cout << "Getattr: " << path << "\n";
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    // Not really used at the moment
    stbuf->st_nlink = 1;
    
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
            stbuf->st_mode = S_IFREG | 0666;
            uint32_t size;
            vfs->entry_size(path, size);
            stbuf->st_size = size;
        }
    }

    return res;
}
static int hello_fgetattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
    std::cout << "Fgetattr: " << path << "\n";
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    // Not really used at the moment
    stbuf->st_nlink = 1;
    
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
            stbuf->st_mode = S_IFREG | 0666;
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
    std::cout << "Readdir: " << path << "\n";
    (void) offset;
    (void) fi;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    loss::FolderEntry folder;
    auto read_result = vfs->read_folder(path, folder);

    if (read_result == loss::SUCCESS)
    {
        for (auto &iter : folder)
        {
            loss::MetadataDef metadata;
            auto read_result = vfs->entry_metadata(path, metadata);

            struct stat st;
            if (metadata.type() == loss::FOLDER_ENTRY)
            {
                st.st_mode = S_IFDIR | 0755;
            }
            else if (metadata.type() == loss::FILE_ENTRY)
            {
                st.st_mode = S_IFREG | 0666;
                uint32_t size;
                vfs->entry_size(path, size);
                st.st_size = size;
            }
            filler(buf, iter.first.c_str(), &st, 0);
        }
    }
    else
    {
        return -ENOENT;
    }

    return 0;
}

static int hello_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    std::cout << "Create: " << path << ", " << mode << "\n";
    auto result = vfs->create_file(path);
    loss::FileHandle *handle = nullptr;
    vfs->open(1, path, loss::FileHandle::READ | loss::FileHandle::WRITE, handle);
    fi->fh = (intptr_t)handle;
    return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
    std::cout << "Open: " << path << "\n";
    loss::FileHandle *handle = nullptr;
    auto open_result = vfs->open(1, path, loss::FileHandle::READ | loss::FileHandle::WRITE, handle);
    if (open_result != loss::SUCCESS)
    {
        return -ENOENT;
    }
    fi->fh = (intptr_t)handle;

    return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
    std::cout << "Read: " << path << ": " << size << ": " << offset << "\n";
    loss::FileHandle *handle = (loss::FileHandle *)fi->fh;
    handle->read_position(offset);
    auto read_result = vfs->read(handle, size, (uint8_t*)buf);

    if (read_result.status() != loss::SUCCESS)
    {
        return -ENOENT;
    }

    return read_result.bytes();
    /*
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
        */
}

static int hello_write(const char *path, const char *buf, size_t size, off_t off, struct fuse_file_info *fi)
{
    std::cout << "Write: " << path << ": " << size << ": " << off << "\n";
    auto write_result = vfs->write(path, off, size, (const uint8_t *)buf);
    return write_result.bytes();
}

static void hello_destroy(void *user_data)
{
    std::cout << "Destroyed\n";
}

static struct fuse_operations hello_oper = {
    .getattr    = hello_getattr,
    .fgetattr   = hello_fgetattr,
    .readdir    = hello_readdir,
    .open       = hello_open,
    .read       = hello_read,
    .create     = hello_create,
    .write      = hello_write,
    .destroy    = hello_destroy,
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
    ramfs = new loss::RamFileSystem();
    vfs->root_filesystem(ramfs);
    const char *file_to_open = argv[i];
    {
        std::ifstream input(file_to_open);
        loss::RamFileSystemDeserialise deserialise(input, ramfs);
        deserialise.load();
    }

	for(; i < argc; i++)
    {
		argv[i] = argv[i+1];
	}
	argc--;
    auto result = fuse_main(argc, argv, &hello_oper, NULL);

    delete vfs;

    return result;
}
