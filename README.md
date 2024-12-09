# 基于FUSE的青春版EXT2文件系统

### 磁盘布局设计
- 磁盘容量 4MB，逻辑块大小 1024B
- 逻辑块：4096块
- 超级块、索引节点位图、数据块位图：各1块
- 索引节点区：一个索引节点大小：44B
    - 1024 / 44 = 23个索引节点使用一个逻辑块
    - 一共需要 4096 / (6 + 1/23) = 677个索引节点
    - 一共需要 677/23 = 30块
- 数据块区 4096-3-30 = 4063 块

