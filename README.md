1. 准备一个容量至少是4G的空白U盘
2. 在“控制面板-〉磁盘管理”中给U盘建立一个大分区
3. 格式化该分区为FAT32，“卷标”设置为MINIME（重要！）
4. 将isolinux和LiveOS拷贝到U盘根目录下
5. 启动一个管理员权限的cmd，运行 syslinux.exe -ma -d isolinux X:，X为U盘的盘符，syslinux.exe在isolinux目录下，你可以先把它拷贝到任意位置用。
6. 完事啦！

downloads : http://pan.baidu.com/share/link?shareid=819204420&uk=3556790491
