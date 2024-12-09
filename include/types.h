#ifndef _TYPES_H_
#define _TYPES_H_

#define MAX_NAME_LEN    128     

typedef int         boolean;
typedef enum newfs_file_type {
    NEWFS_REG_FILE,
    NEWFS_DIR
} NEWFS_FILE_TYPE;

/******************************************************************************
* SECTION: Macro
*******************************************************************************/
#define TRUE                    1
#define FALSE                   0
#define UINT32_BITS             32
#define UINT8_BITS              8

#define NEWFS_MAGIC_NUM           0x20110505 
#define NEWFS_SUPER_OFS           0
#define NEWFS_ROOT_INO            0

#define NEWFS_ERROR_NONE          0
#define NEWFS_ERROR_ACCESS        EACCES
#define NEWFS_ERROR_SEEK          ESPIPE
#define NEWFS_ERROR_NOTDIR        ENOTDIR
#define NEWFS_ERROR_ISDIR         EISDIR
#define NEWFS_ERROR_NOSPACE       ENOSPC
#define NEWFS_ERROR_EXISTS        EEXIST
#define NEWFS_ERROR_NOTFOUND      ENOENT
#define NEWFS_ERROR_UNSUPPORTED   ENXIO
#define NEWFS_ERROR_IO            EIO     /* Error Input/Output */
#define NEWFS_ERROR_INVAL         EINVAL  /* Invalid Args */

#define NEWFS_MAX_FILE_NAME     128
#define SUPER_BLKS              1
#define INODE_MAP_BLKS          1
#define DATA_MAP_BLKS           1
#define INODE_BLKS              37
#define MAX_INODE_PER_BLK       16
#define DATA_BLKS               4056
#define NEWFS_DATA_PER_FILE     6
#define NEWFS_DEFAULT_PERM      0777

/******************************************************************************
* SECTION: Macro Function
*******************************************************************************/
// 计算大小
#define NEWFS_IO_SZ()                     (newfs_super.sz_io)
#define NEWFS_BLK_SZ()                    (newfs_super.sz_blks)
#define NEWFS_DISK_SZ()                   (newfs_super.sz_disk)
#define NEWFS_DRIVER()                    (newfs_super.fd)
#define NEWFS_INODE_SZ()                  (sizeof(struct newfs_inode_d))
#define NEWFS_BLKS_SZ(blks)               ((blks) * NEWFS_BLK_SZ())
#define NEWFS_INODES_SZ(ino)              ((ino) * NEWFS_INODE_SZ())
#define MAX_DENTRY_PER_BLK()              (NEWFS_BLK_SZ() / sizeof(struct newfs_dentry))

// 向上取整数 向下取整
#define NEWFS_ROUND_DOWN(value, round)    ((value) % (round) == 0 ? (value) : ((value) / (round)) * (round))
#define NEWFS_ROUND_UP(value, round)      ((value) % (round) == 0 ? (value) : ((value) / (round) + 1) * (round))

// 设置文件名称
#define NEWFS_ASSIGN_FNAME(pnewfs_dentry, _fname) memcpy(pnewfs_dentry->fname, _fname, strlen(_fname))

// 计算偏移大小
#define NEWFS_INO_OFS(ino)                (newfs_super.ino_offset + NEWFS_INODES_SZ(ino))
#define NEWFS_DATA_OFS(dno)               (newfs_super.data_offset + NEWFS_BLKS_SZ(dno))

// 判断 inode 类型
#define NEWFS_IS_DIR(pinode)              (pinode->dentry->ftype == NEWFS_DIR)
#define NEWFS_IS_REG(pinode)              (pinode->dentry->ftype == NEWFS_REG_FILE)

/******************************************************************************
* SECTION: FS Specific Structure - In memory structure
*******************************************************************************/
struct newfs_dentry;
struct newfs_inode;
struct newfs_super;
struct custom_options {
	const char*        device;
};

struct newfs_super {
    uint32_t magic_num;
    int      fd;
    /* TODO: Define yourself */
    int sz_disk;          // 磁盘大小
    int sz_usage;

    /* 逻辑块 */
    int sz_io;            // io大小
    int sz_blks;          // 逻辑块大小

    /* 磁盘布局分区信息 */
    int super_blk_offset;          // 超级块于磁盘中的偏移，0
    int super_blks;            // 超级块于磁盘中的块数，1

    int map_inode_offset;     // 索引节点位图于磁盘中的偏移 1
    int map_inode_blks;       // 索引节点位图于磁盘中的块数 1
    uint8_t* map_inode;

    int map_data_offset;     // 数据块位图于磁盘中的偏移 2
    int map_data_blks;       // 数据块位图于磁盘中的块数 1
    uint8_t* map_data;

    int ino_offset;     // 索引节点区于磁盘中的偏移 3
    int ino_blks;       // 索引节点区于磁盘中的块数 256

    int data_offset;     // 数据块区于磁盘中的偏移 131
    int data_blks;       // 数据块区于磁盘中的块数 3965

    /* 支持的限制 */
    int max_ino;            // 最大支持inode数
    int max_data;           // 最大支持数据块数

    /* 根目录索引 */
    int root_ino;           // 根目录对应的inode

    /* 其他信息 */
    boolean            is_mounted;
    struct newfs_dentry* root_dentry;     // 根目录
};

struct newfs_inode {
    uint32_t ino;
    /* TODO: Define yourself */
    /* 文件的属性 */
    int                  size;               // 文件已占用空间
    int                  link;               // 链接数，默认为1
    NEWFS_FILE_TYPE      ftype;            // 文件类型（目录类型、普通文件类型）

    /* 数据块的索引 */
    int                  block_pointer[NEWFS_DATA_PER_FILE];   // 数据块块号（可固定分配）
    uint8_t*             data[NEWFS_DATA_PER_FILE];            // 指向数据块的指针

    /* 其他字段 */
    int                  dir_cnt;            // 如果是目录类型文件，下面有几个目录项
    struct newfs_dentry* dentry;             // 指向该inode的dentry
    struct newfs_dentry* dentrys;            // 所有目录项
};

struct newfs_dentry {
    /* 文件名称 */
    char     fname[MAX_NAME_LEN];

    /* inode编号 */
    uint32_t ino;

    /* TODO: Define yourself */
    /* 文件类型 */
    NEWFS_FILE_TYPE      ftype;              // 文件类型（目录类型、普通文件类型）

    struct newfs_dentry* parent;
    struct newfs_dentry* brother;
    struct newfs_inode* inode;
};

static inline struct newfs_dentry* new_dentry(char * fname, NEWFS_FILE_TYPE ftype) {
    struct newfs_dentry * dentry = (struct newfs_dentry *)malloc(sizeof(struct newfs_dentry));
    memset(dentry, 0, sizeof(struct newfs_dentry));
    NEWFS_ASSIGN_FNAME(dentry, fname);
    dentry->ftype   = ftype;
    dentry->ino     = -1;
    dentry->inode   = NULL;
    dentry->parent  = NULL;
    dentry->brother = NULL;   
    return dentry;                                         
}

/******************************************************************************
* SECTION: FS Specific Structure - Disk structure
*******************************************************************************/

struct newfs_super_d {
    uint32_t magic_num;
    int sz_usage;

    /* 磁盘布局分区信息 */
    int super_blk_offset;          // 超级块于磁盘中的偏移，0
    int super_blks;            // 超级块于磁盘中的块数，1

    int map_inode_offset;     // 索引节点位图于磁盘中的偏移 1
    int map_inode_blks;       // 索引节点位图于磁盘中的块数 1

    int map_data_offset;     // 数据块位图于磁盘中的偏移 2
    int map_data_blks;       // 数据块位图于磁盘中的块数 1

    int ino_offset;     // 索引节点区于磁盘中的偏移 3
    int ino_blks;       // 索引节点区于磁盘中的块数 37

    int data_offset;     // 数据块区于磁盘中的偏移 40
    int data_blks;       // 数据块区于磁盘中的块数 4056

    /* 支持的限制 */
    int max_ino;            // 最大支持inode数
    int max_data;           // 最大支持数据块数

    /* 根目录索引 */
    int root_ino;           // 根目录对应的inode
};

struct newfs_inode_d {
    uint32_t ino;
    /* 文件的属性 */
    int                  size;               // 文件已占用空间
    int                  link;               // 链接数，默认为1
    NEWFS_FILE_TYPE      ftype;            // 文件类型（目录类型、普通文件类型）

    /* 数据块的索引 */
    int                  block_pointer[NEWFS_DATA_PER_FILE];   // 数据块块号（可固定分配）

    /* 其他字段 */
    int                  dir_cnt;            // 如果是目录类型文件，下面有几个目录项
};

struct newfs_dentry_d {
    /* 文件名称 */
    char     fname[MAX_NAME_LEN];

    /* inode编号 */
    uint32_t ino;

    /* TODO: Define yourself */
    /* 文件类型 */
    NEWFS_FILE_TYPE      ftype;              // 文件类型（目录类型、普通文件类型）
};


#endif /* _TYPES_H_ */