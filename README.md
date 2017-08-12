# WFPFirewall
基于WFP(Windows Filter Platform)的个人防火墙系统

# 摘要
该工程为专业综合实验课程设计，用以实现一个基于WFP(Windows Filter Platform)的个人防火墙系统，主要分为基于规则的过滤模块以及进程网络访问的监控模块（另外还有一些简易的功能模块）。
其中，进程网络访问的监控模块 使用 Microsoft的msnmntr样例，详情请见[msnmntr](https://github.com/Microsoft/Windows-driver-samples/tree/master/network/trans/msnmntr)，对于msnmntr中的EXE项目，本工程进行了代码嵌入，已实现其功能；对于msnmntr中的驱动项目sys，本工程直接将其加入使用，编译生成.sys文件，再安装驱动即可。

# 流程
* 生成项目msnmntr，获得msnmntr的驱动安装包(.sys，.inf，.cat等文件)，安装驱动。</br>
* 生成项目PFirewall，获得PFirewall.exe  </br>
对于进程网络访问的监控模块，详见[msnmntr](https://github.com/Microsoft/Windows-driver-samples/tree/master/network/trans/msnmntr)，本工程对其只是实现了一个GUI接口。

# 程序主界面
![Image text](https://github.com/raymon-tian/WFPFirewall/blob/master/ImageFolder/main.png)
# 过滤功能演示
* 过滤出防火墙的源IP分别为 [180.97.33.108](http://180.97.33.108/) 和 [115.159.241.25](http://115.159.241.95/)的IP数据包和TCP数据包</br>
![Image text](https://github.com/raymon-tian/WFPFirewall/blob/master/ImageFolder/filter.png)</br>
![Image text](https://github.com/raymon-tian/WFPFirewall/blob/master/ImageFolder/r1.png)</br>
![Image text](https://github.com/raymon-tian/WFPFirewall/blob/master/ImageFolder/r2.png)</br>
