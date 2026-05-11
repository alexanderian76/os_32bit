#ifndef VFS_H
#define VFS_H

#include <stdint.h>

// Максимальная длина имени файла
#define MAX_FILENAME 256
#define MAX_PATH 1024
#define MAX_OPEN_FILES 64

// Режимы открытия файлов
enum file_flags {
    O_RDONLY = 0x0000,
    O_WRONLY = 0x0001,
    O_RDWR   = 0x0002,
    O_CREAT  = 0x0100,
    O_APPEND = 0x2000
};

// Типы узлов
enum vnode_type {
    VNODE_FILE = 0,
    VNODE_DIR  = 1,
    VNODE_DEV  = 2  // Для устройств (клавиатура, дисплей и т.д.)
};

// Структуры VFS (forward declarations)
struct vnode;
struct file;
struct mount;
struct filesystem;

// Операции с vnode (файлом/директорией)
typedef struct vnode_ops {
    int (*lookup)(struct vnode *dir, const char *name, struct vnode **result);
    int (*create)(struct vnode *dir, const char *name, struct vnode **result);
    int (*mkdir)(struct vnode *dir, const char *name);
    int (*unlink)(struct vnode *dir, const char *name);
} vnode_ops_t;

// Операции с открытым файлом
typedef struct file_ops {
    int (*read)(struct file *file, void *buf, uint32_t size);
    int (*write)(struct file *file, const void *buf, uint32_t size);
    int (*open)(struct vnode *node, struct file **file);
    int (*close)(struct file *file);
    int (*lseek)(struct file *file, int offset, int whence);
} file_ops_t;

// Виртуальный узел (файл или директория)
typedef struct vnode {
    struct mount *mount;           // Точка монтирования
    vnode_ops_t *v_ops;            // Операции над узлом
    file_ops_t *f_ops;             // Операции с файлом
    void *internal;                // Внутренние данные ФС
    enum vnode_type type;
    uint32_t ref_count;            // Счетчик ссылок
    char name[MAX_FILENAME];
} vnode_t;

// Дескриптор открытого файла
typedef struct file {
    vnode_t *vnode;                // Узел файла
    file_ops_t *f_ops;             // Операции с файлом
    uint32_t f_pos;                // Текущая позиция
    uint32_t flags;                // Режим открытия
    uint32_t ref_count;
} file_t;

// Файловая система
typedef struct filesystem {
    const char *name;
    int (*mount)(struct mount *mount, vnode_t *device);
} filesystem_t;

// Точка монтирования
typedef struct mount {
    vnode_t *root;                 // Корневой каталог этой ФС
    filesystem_t *fs;              // Тип ФС
    void *data;                    // Данные ФС (суперблок и т.д.)
} mount_t;

// Структура для поиска пути
typedef struct path_lookup {
    vnode_t *vnode;
    const char *filename;          // Последний компонент пути
} path_lookup_t;

// Системные вызовы VFS
int vfs_init(void);
int vfs_mount(const char *source, const char *target, const char *fs_type);
int vfs_open(const char *path, int flags, file_t **file);
int vfs_read(file_t *file, void *buf, uint32_t size);
int vfs_write(file_t *file, const void *buf, uint32_t size);
int vfs_close(file_t *file);
int vfs_lseek(file_t *file, int offset, int whence);
int vfs_mkdir(const char *path);

// Вспомогательные функции
int vfs_path_lookup(const char *path, path_lookup_t *result);
int vfs_register_filesystem(filesystem_t *fs);

#endif // VFS_H