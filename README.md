介绍:

	此demo的功能是用opencv的dnn模块，来运行yolov3的进行目标检测和进行OCR的，OCR模型采用的是改进版的crnn，CNN+FC。
	来对检测到的目标进行识别。

检测：

	检测的目标是***银行的票据。检测的目标共有4类，分别为主键、流水号、附件章（只检测不需要识别）及附件标题。

识别：

	对检测到的目标进行识别，其中需要识别的目标有主键、流水号和附件标题。此识别是对水平从左到有的输入有效。

模型训练说明：
	 yolov3模型是在darknet下训练的得到weights文件。
	 ocr模型是在chineseOCR提供的脚本及模型ocr_dense.pth基础上，增加自己的数据得到的。
	 ocr用的是pytorch进行训练的，训练得到的pth文件，需要经过pytorch转keras，然后keras转darknet，两个转换过程，
	 在原chineseOCR中提供了参考的脚本。本人实在这些脚本基础上，修改得到的。具体的训练代码及转换脚本，我整理好后
	 会在另一个repo中上传。
	 我这里的ocr.cfg文件，与原始chineseOCR的ocr.cfg文件存在一些小的不同，我这里用到了batch_norm。
	 
	 chineseOCR的链接地址：https://github.com/chineseocr/chineseocr.git
	 
	 
	
cmake进行构建：

	对于自己写的.cpp .h文件拷贝到src文件夹下，然后在CmakeLists.txt文件中的add_executable
	(detect_objects src/demo.cpp src/dataStructures.h src/***.cpp src/***.h) 后面依次添加，
	然后cmake构建就可以


文件夹说明：

	3rdparty:存放的是第三方库 opencv4.1.0的库,从下载链接中，下载后将库拷贝到该文件夹下

	build：是已经cmake构建好的vs2015的project，具体的cmake过程，见图片cmake截图.png 



	data： 
		yolo文件夹：存放的是yolov3的cfg文件和weights文件，以及names文件
		ocr文件夹：存放的是ocr模型的cfg、dict和weights文件
		ocr的模型文件是在chineseOCR的ocr_dense.pth(字典大小是5530)的基础上，用实际数据进行微调得到的
	test： 存放的是几张测试图片
	src:   存放的是源文件


注意：
	
	在下载此文件后，直接运行build目录下的OpencvDarknet.sln会报错，原因是路劲错误，找不到相应文件，
	需要在本地重新cmake构建一下参见cmake截图.png
	

版本说明：
opencv4.1.0

测试速度:
	识别速度：FPS = 19.48 
	检测速度：FPS = 1.989
	检测+识别速度：FPS = 1.459

补充：
	
	此次用到的yolov3模型文件，yolov3.weights,及ocr模型ocr_5530.weights，
	以及编译好的opencv4.1.0的库文件链接下载地址如下：


opencv4.1.0的库文件下载链接：
链接：https://pan.baidu.com/s/1b4IpmKwc6YQZ1jzRs64y6g  提取码：impz

yolov3及ocr的模型文件下载链接：
链接：https://pan.baidu.com/s/169cpsBcHAKDog1GRtEKHqg 
提取码：ujyd


致谢：
	  在此研究过程中，得到了chineseOCR作者的大力支持与帮助，在此向他表示衷心的感谢！
	  另外，也感谢实验室提供的资源，也感谢颜姐给予的帮助和指导！

Author:BigPanda
E-mail:wangxiong@founder.com
State Key Laboratory of Digital Publishing Technology
Date:2020-9-9

