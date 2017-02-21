2006年4月16完成

用于连接两个图形客户端用于自动下棋
原来是用BCB5.0编译的，改用BCB6.0后出现以下问题：
“unable to find package import”

解决办法：
1、打开工程文件 project --option-- package 在 runtime package 下面 （这个是按照过第三方控件 然后卸载 第三方控件之后残留的垃圾）
或者
2、用记事本 打开 bpr工程文件，找到 <PACKAGES value="vcl.bpi rtl.bpi ...把没用的bpi删掉。











