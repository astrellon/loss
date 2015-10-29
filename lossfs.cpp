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

static loss::VirtualFileSystem *vfs = nullptr;
static loss::RamFileSystem *ramfs = nullptr;
static const char *file_to_open = nullptr;
std::ofstream log_file;

static void save_hdd()
{
    if (file_to_open != nullptr)
    {
        std::ofstream output(file_to_open);
        if (output.is_open())
        {
            log_file << "Writing back to: " << file_to_open << "\n";
        }
        else
        {
            log_file << "Unable to save back to: " << file_to_open << "\n";
        }
        loss::RamFileSystemSerialise serialise(output, ramfs);
        serialise.save();
    }
    else
    {
        log_file << "Input file is null!\n";
    }
}


static int hello_getattr(const char *path, struct stat *stbuf)
{
    log_file << "Getattr: " << path << "\n";
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
    log_file << "Fgetattr: " << path << "\n";
    return hello_getattr(path, stbuf);
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    log_file << "Readdir: " << path << "\n";
    (void) offset;
    (void) fi;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    auto context = fuse_get_context();

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
            st.st_uid = context->uid;
            st.st_gid = context->gid;

            log_file << "- " << iter.first << "\n";
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
    log_file << "Create: " << path << ", " << mode << "\n";
    auto result = vfs->create_file(path);
    loss::FileHandle *handle = nullptr;
    vfs->open(1, path, loss::FileHandle::READ | loss::FileHandle::WRITE, handle);
    fi->fh = (intptr_t)handle;
    
    return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
    log_file << "Open: " << path << "\n";
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
    log_file << "Read: " << path << ": " << size << ": " << offset << "\n";
    loss::FileHandle *handle = (loss::FileHandle *)fi->fh;
    handle->read_position(offset);
    auto read_result = vfs->read(handle, size, (uint8_t*)buf);

    if (read_result.status() != loss::SUCCESS)
    {
        return -ENOENT;
    }

    return read_result.bytes();
}

static int hello_write(const char *path, const char *buf, size_t size, off_t off, struct fuse_file_info *fi)
{
    log_file << "Write: " << path << ": " << size << ": " << off << "\n";
    auto write_result = vfs->write(path, off, size, (const uint8_t *)buf);

    return write_result.bytes();
}

static void hello_destroy(void *user_data)
{
    save_hdd();
    log_file << "Destroyed\n";
}

static int hello_truncate(const char *path, off_t offset)
{
    log_file << "Truncate: " << path << "\n";
    return 0;
}
static int hello_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
    log_file << "Truncate: " << path << ", " << offset << "\n";
    return 0;
}

static int hello_flush(const char *path, struct fuse_file_info *fi)
{
    log_file << "Flush: " << path << "\n";
    return 0;
}
static int hello_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
    log_file << "Fsync: " << path << ", " << datasync << "\n";
    return 0;
}

static int hello_link(const char *path, const char *newpath)
{
    log_file << "Link: " << path << ", " << newpath << "\n";
    return 0;
}
static int hello_unlink(const char *path)
{
    log_file << "Unlink: " << path << "\n";
    auto remove_result = vfs->remove_entry(path);
    if (remove_result != loss::SUCCESS)
    {
        return -ENOENT;
    }
    return 0;
}
static int hello_rename(const char *path, const char *newpath)
{
    log_file << "Rename: " << path << ", " << newpath << "\n";
    auto rename_result = vfs->rename(path, newpath);
    if (rename_result == loss::ENTRY_NOT_FOUND)
    {
        return -ENOENT;
    }
    else if (rename_result == loss::WRONG_ENTRY_TYPE)
    {
        return -ENOTDIR;
    }
    else if (rename_result == loss::ENTRY_ALREADY_EXITS)
    {
        return -EEXIST;
    }
    else if (rename_result != loss::SUCCESS)
    {
        return -EACCES;
    }

    return 0;
}
static int hello_release(const char *path, struct fuse_file_info *fi)
{
    log_file << "Release: " << path << "\n";
    vfs->close((loss::FileHandle*)fi->fh);
    return 0;
}

static int hello_mkdir(const char *path, mode_t mode)
{
    auto create_result = vfs->create_folder(path);
    if (create_result == loss::ENTRY_NOT_FOUND)
    {
        return -ENOENT;
    }
    else if (create_result == loss::WRONG_ENTRY_TYPE)
    {
        return -ENOTDIR;
    }
    else if (create_result == loss::ENTRY_ALREADY_EXITS)
    {
        return -EEXIST;
    }
    else if (create_result != loss::SUCCESS)
    {
        return -EACCES;
    }

    return 0;
}
static int hello_rmdir(const char *path)
{
    loss::MetadataDef metadata;
    auto meta_result = vfs->entry_metadata(path, metadata);
    if (meta_result == loss::ENTRY_NOT_FOUND)
    {
        return -ENOENT;
    }
    if (metadata.type() != loss::FOLDER_ENTRY)
    {
        return -ENOTDIR;
    }

    auto remove_result = vfs->remove_entry(path);
    if (remove_result == loss::FOLDER_NOT_EMPTY)
    {
        return -ENOTEMPTY;
    }
    else if (remove_result == loss::ENTRY_NOT_FOUND)
    {
        return -ENOENT;
    }
    else if (remove_result != loss::SUCCESS)
    {
        return -EACCES;
    }

    return 0;
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
    .ftruncate  = hello_ftruncate,
    .truncate   = hello_truncate,
    .flush      = hello_flush,
    .fsync      = hello_fsync,
    .link       = hello_link,
    .unlink     = hello_unlink,
    .mkdir      = hello_mkdir,
    .rmdir      = hello_rmdir,
    .rename     = hello_rename,
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
    file_to_open = argv[i];

    log_file.open("./fuse.log");
    log_file << "Attempting to open: " << file_to_open << " as hdd\n";

    {
        std::ifstream input(file_to_open);
        loss::RamFileSystemDeserialise deserialise(input, ramfs);
        deserialise.load();
        input.close();
    }

	for(; i < argc; i++)
    {
		argv[i] = argv[i+1];
	}
	argc--;

    log_file << "Starting FUSE main\n";
    auto result = fuse_main(argc, argv, &hello_oper, NULL);
    
    log_file << "Shutting down FUSE main\n";

    delete vfs;

    return result;
}
