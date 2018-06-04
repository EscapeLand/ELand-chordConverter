# ELand-chordConverter
将一张图片形式的吉他谱转换成XML格式的MIDI文件，便于用Guitar Pro之类的打谱软件读取修改。
-
实际上更常用的应该是，拿来一张六线谱，扫出来给Guitar Pro听个响 :D

文件：

	Dodo.cpp 收集了图像处理函数
	frmain.cpp 过时文件，原本用作GUI，现在GUI已停止开发. 六月份的时候可能重写GUI
	
	myheader.h 头文件及函数声明
	eagle.cpp 机器学习部分
	maincpp.cpp 作为一个命令行程序的主程序部分
	GUI.h 过时文件，自写自用的GUI库. 六月份左右会更新
	tData.csv 机器学习保存的数据，做备份之用
	

本项目将于2018年七月停止开发，之后视情况可能由管理员接管.
