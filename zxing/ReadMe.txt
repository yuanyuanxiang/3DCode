Zxing项目的源码在：https://github.com/glassechidna/zxing-cpp

（1）ZXing 修改说明：

	2016-12-18进行了一次比对，仅对下述文件进行了修改。

	PerspectiveTransform

	Decoder

	Detector

	QRCodeReader

	DecodeHints.cpp（仅注释掉第33行）

（2）ZXing 例子修改说明：

	ImageReaderSource（修改create函数）