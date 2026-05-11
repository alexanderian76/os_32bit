#include "vfs.h"
#include "kern.h"  // Ваши функции print и т.д.
#include "string.h"
#include <stdlib.h>

// Глобальные переменные VFS
static vnode_t *root_vnode = NULL;           // Корневой узел (/)
static filesystem_t *filesystems[16];
static int num_filesystems = 0;
static file_t *open_files[MAX_OPEN_FILES];

// Простая tmpfs (временная ФС в памяти)
// Внутренние структуры tmpfs
typedef struct tmpfs_inode {
    enum vnode_type type;
    uint32_t size;
    void *data;                    // Для файлов - данные
    struct tmpfs_inode *parent;
    struct tmpfs_inode *children[64];  // Для директорий - записи
    char name[MAX_FILENAME];
    int child_count;
} tmpfs_inode_t;

// Прототипы операций tmpfs
static int tmpfs_lookup(vnode_t *dir, const char *name, vnode_t **result);
static int tmpfs_create(vnode_t *dir, const char *name, vnode_t **result);
static int tmpfs_mkdir(vnode_t *dir, const char *name);
static int tmpfs_read(file_t *file, void *buf, uint32_t size);
static int tmpfs_write(file_t *file, const void *buf, uint32_t size);
static int tmpfs_open(vnode_t *node, file_t **file);
static int tmpfs_close(file_t *file);
static int tmpfs_lseek(file_t *file, int offset, int whence);

// Операции tmpfs
static vnode_ops_t tmpfs_vnode_ops = {
    .lookup = tmpfs_lookup,
    .create = tmpfs_create,
    .mkdir = tmpfs_mkdir,
    .unlink = NULL,
};

static file_ops_t tmpfs_file_ops = {
    .read = tmpfs_read,
    .write = tmpfs_write,
    .open = tmpfs_open,
    .close = tmpfs_close,
    .lseek = tmpfs_lseek,
};

// Функция монтирования tmpfs
static int tmpfs_mount(mount_t *mount, vnode_t *device) {
    (void)device;  // tmpfs не использует устройство
    
    // Создаем корневой inode для tmpfs
    tmpfs_inode_t *root_inode = (tmpfs_inode_t*)malloc(sizeof(tmpfs_inode_t));
    if (!root_inode) return -1;
    
    memset(root_inode, 0, sizeof(tmpfs_inode_t));
    root_inode->type = VNODE_DIR;
    root_inode->child_count = 0;
    strcpy(root_inode->name, "/");
    
    // Создаем vnode
    vnode_t *root_node = (vnode_t*)malloc(sizeof(vnode_t));
    if (!root_node) {
        free(root_inode);
        return -1;
    }
    
    root_node->mount = mount;
    root_node->v_ops = &tmpfs_vnode_ops;
    root_node->f_ops = &tmpfs_file_ops;
    root_node->internal = root_inode;
    root_node->type = VNODE_DIR;
    root_node->ref_count = 1;
    strcpy(root_node->name, "/");
    
    mount->root = root_node;
    mount->data = NULL;
    
    print("tmpfs mounted successfully\n");
    return 0;
}

// Регистрация tmpfs
static filesystem_t tmpfs_fs = {
    .name = "tmpfs",
    .mount = tmpfs_mount,
};

// Реализация tmpfs операций
static tmpfs_inode_t* vnode_to_inode(vnode_t *vnode) {
    return (tmpfs_inode_t*)vnode->internal;
}

static int tmpfs_lookup(vnode_t *dir, const char *name, vnode_t **result) {
    tmpfs_inode_t *dir_inode = vnode_to_inode(dir);
    
    if (dir_inode->type != VNODE_DIR) {
      //  print("NOT_DIR");
        return -1;  // Не директория
    }
    
    // Ищем в детях
    for (int i = 0; i < dir_inode->child_count; i++) {
        tmpfs_inode_t *child = dir_inode->children[i];
     //   print(child->name);
      //      print_char('\n');
      //  print(name);
        if (strcmp(child->name, name) == 0) {
            // Создаем vnode для найденного узла
            vnode_t *child_node = (vnode_t*)malloc(sizeof(vnode_t));
            if (!child_node) return -1;
        //    print("CHILD_NODE");
            child_node->mount = dir->mount;
            child_node->v_ops = dir->v_ops;
            child_node->f_ops = dir->f_ops;
            child_node->internal = child;
            child_node->type = child->type;
            child_node->ref_count = 1;
            strcpy(child_node->name, name);
            
            *result = child_node;
            return 0;
        }
    }
    
    return -1;  // Не найдено
}

static int tmpfs_create(vnode_t *dir, const char *name, vnode_t **result) {
    tmpfs_inode_t *dir_inode = vnode_to_inode(dir);
    
    if (dir_inode->type != VNODE_DIR) {
        return -1;
    }
    
    // Проверяем, не существует ли уже
    for (int i = 0; i < dir_inode->child_count; i++) {
        if (strcmp(dir_inode->children[i]->name, name) == 0) {
            return -1;  // Уже существует
        }
    }
    
    // Создаем новый inode
    tmpfs_inode_t *new_inode = (tmpfs_inode_t*)malloc(sizeof(tmpfs_inode_t));
    if (!new_inode) return -1;
    
    memset(new_inode, 0, sizeof(tmpfs_inode_t));
    new_inode->type = VNODE_FILE;
    new_inode->size = 0;
    new_inode->data = NULL;
    new_inode->parent = dir_inode;
    strcpy(new_inode->name, name);
    
    // Добавляем в родительскую директорию
    dir_inode->children[dir_inode->child_count++] = new_inode;
    
    // Создаем vnode
    vnode_t *new_node = (vnode_t*)malloc(sizeof(vnode_t));
    if (!new_node) {
        free(new_inode);
        return -1;
    }
    
    new_node->mount = dir->mount;
    new_node->v_ops = dir->v_ops;
    new_node->f_ops = dir->f_ops;
    new_node->internal = new_inode;
    new_node->type = VNODE_FILE;
    new_node->ref_count = 1;
    strcpy(new_node->name, name);
    
    *result = new_node;
    return 0;
}

static int tmpfs_mkdir(vnode_t *dir, const char *name) {
    tmpfs_inode_t *dir_inode = vnode_to_inode(dir);
    
    if (dir_inode->type != VNODE_DIR) {
        return -1;
    }
    
    // Проверяем, не существует ли уже
    for (int i = 0; i < dir_inode->child_count; i++) {
        if (strcmp(dir_inode->children[i]->name, name) == 0) {
            return -1;
        }
    }
    
    // Создаем новый inode для директории
    tmpfs_inode_t *new_inode = (tmpfs_inode_t*)malloc(sizeof(tmpfs_inode_t));
    if (!new_inode) return -1;
    
    memset(new_inode, 0, sizeof(tmpfs_inode_t));
    new_inode->type = VNODE_DIR;
    new_inode->size = 0;
    new_inode->data = NULL;
    new_inode->parent = dir_inode;
    strcpy(new_inode->name, name);
    new_inode->child_count = 0;
    
    // Добавляем в родительскую директорию
    dir_inode->children[dir_inode->child_count++] = new_inode;
    
    return 0;
}

static int tmpfs_read(file_t *file, void *buf, uint32_t size) {
    tmpfs_inode_t *inode = vnode_to_inode(file->vnode);
    
    if (inode->type != VNODE_FILE) {
        return -1;
    }
    
    // Проверяем границы
    if (file->f_pos >= inode->size) {
        return 0;  // EOF
    }
    
    uint32_t bytes_to_read = size;
    if (file->f_pos + bytes_to_read > inode->size) {
        bytes_to_read = inode->size - file->f_pos;
    }
    
    // Копируем данные
    memcpy(buf, (char*)inode->data + file->f_pos, bytes_to_read);
    file->f_pos += bytes_to_read;
    
    return bytes_to_read;
}

static int tmpfs_write(file_t *file, const void *buf, uint32_t size) {
    tmpfs_inode_t *inode = vnode_to_inode(file->vnode);
    
    if (inode->type != VNODE_FILE) {
        return -1;
    }
    
    // Расширяем файл если нужно
    uint32_t new_size = file->f_pos + size;
    if (new_size > inode->size) {
        void *new_data = malloc(new_size);
        if (!new_data) return -1;
        
        if (inode->data) {
            memcpy(new_data, inode->data, inode->size);
            free(inode->data);
        }
        inode->data = new_data;
        inode->size = new_size;
    }
    
    // Записываем данные
    memcpy((char*)inode->data + file->f_pos, buf, size);
    file->f_pos += size;
    
    return size;
}

static int tmpfs_open(vnode_t *node, file_t **file) {
    file_t *new_file = (file_t*)malloc(sizeof(file_t));
    if (!new_file) return -1;
    
    new_file->vnode = node;
    new_file->f_ops = node->f_ops;
    new_file->f_pos = 0;
    new_file->ref_count = 1;
    node->ref_count++;
    
    *file = new_file;
    return 0;
}

static int tmpfs_close(file_t *file) {
    if (file->vnode) {
        file->vnode->ref_count--;
        // Здесь можно удалить vnode если ref_count == 0
    }
    free(file);
    return 0;
}

static int tmpfs_lseek(file_t *file, int offset, int whence) {
    tmpfs_inode_t *inode = vnode_to_inode(file->vnode);
    
    switch (whence) {
        case 0: // SEEK_SET
            file->f_pos = offset;
            break;
        case 1: // SEEK_CUR
            file->f_pos += offset;
            break;
        case 2: // SEEK_END
            file->f_pos = inode->size + offset;
            break;
        default:
            return -1;
    }
    
    return file->f_pos;
}

// Регистрация файловой системы
int vfs_register_filesystem(filesystem_t *fs) {
    if (num_filesystems >= 16) return -1;
    filesystems[num_filesystems++] = fs;
    return 0;
}

// Поиск файловой системы по имени
static filesystem_t* find_filesystem(const char *name) {
    for (int i = 0; i < num_filesystems; i++) {
        if (strcmp(filesystems[i]->name, name) == 0) {
            return filesystems[i];
        }
    }
    return NULL;
}

// Инициализация VFS
int vfs_init(void) {
    print("Initializing VFS...\n");
    
    // Регистрируем tmpfs
    vfs_register_filesystem(&tmpfs_fs);
    
    // Монтируем корневую файловую систему
    if (vfs_mount(NULL, "/", "tmpfs") != 0) {
        print("Failed to mount root filesystem!\n");
        return -1;
    }
    
    print("VFS initialized successfully\n");
    return 0;
}

// Монтирование файловой системы
int vfs_mount(const char *source, const char *target, const char *fs_type) {
    (void)source;  // В простой реализации игнорируем
    
    print("Mounting ");
    print(fs_type);
    print(" at ");
    print(target);
    print("\n");
    
    // Находим файловую систему
    filesystem_t *fs = find_filesystem(fs_type);
    if (!fs) {
        print("Filesystem type not found\n");
        return -1;
    }
    
    // Создаем точку монтирования
    mount_t *mount = (mount_t*)malloc(sizeof(mount_t));
    if (!mount) return -1;
    
    mount->fs = fs;
    mount->data = NULL;
    
    // Вызываем mount для ФС
    if (fs->mount(mount, NULL) != 0) {
        free(mount);
        return -1;
    }
    
    // Если монтируем в корень
    if (strcmp(target, "/") == 0) {
        root_vnode = mount->root;
    }
    
    return 0;
}

// Разбор пути на компоненты
static int parse_path(const char *path, char components[][MAX_FILENAME], int *num_components) {
    *num_components = 0;
    
    // Пропускаем начальные слэши
    while (*path == '/') path++;
    
    if (*path == '\0') {
        return 0;  // Корневой путь
    }
    
    // Разбиваем на компоненты
    char buffer[MAX_PATH];
    strcpy(buffer, path);
    
    char *token = strtok(buffer, "/");
    while (token && *num_components < 16) {
        strcpy(components[*num_components], token);
        (*num_components)++;
        token = strtok(NULL, "/");
    }
    
    return 0;
}

// Поиск пути
int vfs_path_lookup(const char *path, path_lookup_t *result) {
    if (!root_vnode) return -1;
    
    char components[16][MAX_FILENAME];
    int num_components;
    parse_path(path, components, &num_components);
    
    vnode_t *current = root_vnode;
  //  print("CURRENT: ");
   // print(current->name);
  //  if(current->type == VNODE_DIR)
   //     print("VNODE_DIR");
   // print_char('\n');
    // Идем по всем компонентам пути
    for (int i = 0; i < num_components; i++) {
        vnode_t *next = NULL;
      //  print("COMPONENT i: ");
      //  print(components[i]);
      //  print_char('\n');
        // Если текущий узел не директория - ошибка
        if (current->type != VNODE_DIR) {
          //  print("NOT_DIR");
            return -1;
        }
        
        // Ищем следующий компонент
        if (current->v_ops->lookup(current, components[i], &next) != 0) {
            // Если это последний компонент и файл не найден
            
            if (i == num_components - 1) {
                result->vnode = current;
                result->filename = components[i];
                //print("DIR_NO_FILE");
                return -2;  // Файл не найден, но путь существует
            }
            return -1;  // Ошибка поиска
        }
        
        current = next;
    }
    
    result->vnode = current;
    result->filename = "";
    return 0;  // Путь найден
}

// Открытие файла
int vfs_open(const char *path, int flags, file_t **file) {
    path_lookup_t lookup_result;
    vnode_t *node = NULL;
    
    int result = vfs_path_lookup(path, &lookup_result);
    
    if (result == 0) {
        // Файл существует
        node = lookup_result.vnode;
    } else if (result == -2 && (flags & O_CREAT)) {
        // Файл не существует, но нужно создать
        vnode_t *dir = lookup_result.vnode;
        const char *name = lookup_result.filename;
        
        if (dir->v_ops->create(dir, name, &node) != 0) {
            return -1;
        }
    } else {
     //   print("WRONG_PATH");
        return -1;  // Путь не найден
    }
    
    // Нельзя открыть директорию
    if (node->type == VNODE_DIR) {
        return -1;
    }
    
    // Создаем file структуру
    file_t *new_file;
    if (node->f_ops->open(node, &new_file) != 0) {
        return -1;
    }
    
    new_file->flags = flags;
    
    // Если флаг O_APPEND, переходим в конец файла
    if (flags & O_APPEND) {
        tmpfs_inode_t *inode = vnode_to_inode(node);
        new_file->f_pos = inode->size;
    }
    
    // Находим свободный слот в таблице открытых файлов
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i] == NULL) {
            open_files[i] = new_file;
            break;
        }
    }
    
    *file = new_file;
    return 0;
}

// Чтение из файла
int vfs_read(file_t *file, void *buf, uint32_t size) {
    if (!file || !file->f_ops->read) return -1;
    
    // Проверяем права доступа
    if ((file->flags & O_RDONLY) || (file->flags & O_RDWR)) {
        return file->f_ops->read(file, buf, size);
    }
    
    return -1;  // Нет прав на чтение
}

// Запись в файл
int vfs_write(file_t *file, const void *buf, uint32_t size) {
    if (!file || !file->f_ops->write) return -1;
    
    // Проверяем права доступа
    if ((file->flags & O_WRONLY) || (file->flags & O_RDWR)) {
        return file->f_ops->write(file, buf, size);
    }
    
    return -1;  // Нет прав на запись
}

// Закрытие файла
int vfs_close(file_t *file) {
    if (!file) return -1;
    
    int result = file->f_ops->close(file);
    
    // Удаляем из таблицы открытых файлов
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i] == file) {
            open_files[i] = NULL;
            break;
        }
    }
   // print("FILE_CLOSED");
    return result;
}

// Поиск позиции в файле
int vfs_lseek(file_t *file, int offset, int whence) {
    if (!file || !file->f_ops->lseek) return -1;
    return file->f_ops->lseek(file, offset, whence);
}

// Создание директории
int vfs_mkdir(const char *path) {
    path_lookup_t lookup_result;
    
    int result = vfs_path_lookup(path, &lookup_result);
    if (result != -2) return -1;  // Должен не существовать
    
    vnode_t *dir = lookup_result.vnode;
    const char *name = lookup_result.filename;
    
    if (dir->type != VNODE_DIR) return -1;
    
    return dir->v_ops->mkdir(dir, name);
}