#pragma once

struct _win_st;
typedef struct _win_st WINDOW;

namespace loss
{
    class Kernel;
    class FileHandle;

    class TTYRenderer
    {
        public:
            TTYRenderer();
            ~TTYRenderer();

            void kernel(Kernel *kernel);
            Kernel *kernel() const;

            void file_handle(FileHandle *file_handle);
            FileHandle *file_handle() const;

            void is_open(bool open);
            bool is_open() const;

            void render();

        private:
            bool _open;
            Kernel *_kernel;
            FileHandle *_file_handle;

            WINDOW *_window;
    };
}
