ELand-chordConverter
=
本项目将于2018-6-15停止维护，之后由@YuanWangZhe开发后续功能.
-
将一张图片形式的吉他谱转换成XML格式的MIDI文件，便于用Guitar Pro之类的打谱软件读取修改。
-
实际上更常用的应该是，拿来一张六线谱，扫出来给Guitar Pro听个响 :D

#文件：

	Dodo.cpp 收集了图像处理函数
	frmain.cpp 过时文件，原本用作GUI，现在GUI已停止开发. 六月份的时候可能重写GUI
	eagle.cpp 机器学习部分
	maincpp.cpp 作为一个命令行程序的主程序部分
	GUI.h 过时文件，自写自用的GUI库. 六月份左右会更新
	tData.csv 机器学习保存的数据，做备份之用
	Cuckoo.cpp 识别小节的类
	type.h 自定义类型和类声明
	myheader.h 函数声明汇总
	swan.cpp 用以输出至XML

#引用的项目：

	OpenCV
	tinyXML2

最真诚的感谢:)<br>
最后，这是哈尔滨工业大学2017大一年度项目. 感谢我的导师和学长，还有三个给我`端茶送水`的室友:(
