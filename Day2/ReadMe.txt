helloos0.nas为初始汇编程序
为了方便制作磁盘映像，以后都采用制作512字节启动区的方式，剩下的部分用磁盘映像管理工具制作
因此舍去helloos0.nas后半部分得到了ipl.nas，并且改造asm.bat将输出文件名改为ipl.nas。
即把asm.bat改为：
..\z_tools\nask.exe ipl.nas ipl.bin ipl.lst
顺便输出列表文件ipl.lst（确认指令怎么翻译成机器语言的）
增加makeimg.bat:
..\z_tools\edimg.exe   imgin:../z_tools/fdimg0at.tek   wbinimg src:ipl.bin len:512 from:0 to:0   imgout:helloos.img
以ipl.bin为基础制作磁盘映像文件hellos.img
在命令行依次键入asm makeimg run即可
===========================================================================================
为了方便，加入Makefile文件（文本编辑器可打开查看）与make.bat文件
这样只要执行make run即可运行
make clean清除其他生成文件（更多指令在Makefile中查看）


===========================================================================================



注：部分工具使用川合秀实（Hidemi Kawai  http://hrb.osask.jp/）编写工具
    make.exe为GNU项目组开发工具


By沈津生
http://blog.csdn.net/scusjs
http://falcon_s.byethost15.com/