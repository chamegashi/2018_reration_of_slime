#include <iostream>
#include <sstream>
#include <windows.h>

#include <Kinect.h>

#include <opencv2\opencv.hpp>
#include <atlbase.h>


#define ERROR_CHECK(ret)      \
	if ((ret) != S_OK) {      \
		std::stringstream ss; \
		ss << "failed " #ret " " << std::hex << ret << std::endl; \
		throw std::runtime_error(ss.str().c_str());               \
	}

class KinectApp {

private:
	CComPtr<IKinectSensor> kinect = nullptr;
	CComPtr<IDepthFrameReader> depthFrameReader = nullptr;

	// 表示
	int depthWidth;
	int depthHeight;

	std::vector<unsigned short> depthBuffer;

	// window 用

	const char* DepthWindowName = "depth image";

	// check depth 用

	int rightHighPoint;
	int rightLowPoint;
	int leftHighPoint;
	int leftLowPoint;

	// check depth index 用

	int rightHighIndex;
	int rightLowIndex;
	int leftHighIndex;
	int leftLowIndex;


	// 手の判断の depth

	int changeDistance = 200;

	// マウス用
	struct mouseParam {
		int x;
		int y;
		int event;
		int flags;
	};

public:

	// kincet の初期化

	void init() {

		//kinect の取得
		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
		ERROR_CHECK(kinect->Open());

		//Depth リーダ取得
		CComPtr<IDepthFrameSource> depthFrameSource;
		ERROR_CHECK(kinect->get_DepthFrameSource(&depthFrameSource));
		ERROR_CHECK(depthFrameSource->OpenReader(&depthFrameReader));

		// Depth size 取得
		CComPtr<IFrameDescription> depthFrameDescripton;
		ERROR_CHECK(depthFrameSource->get_FrameDescription(&depthFrameDescripton));
		ERROR_CHECK(depthFrameDescripton->get_Width(&depthWidth));
		ERROR_CHECK(depthFrameDescripton->get_Height(&depthHeight));

		// Depth の最大、最小取得
		unsigned short minDeptheliableDistance;
		unsigned short maxDeptheliableDistance;

		ERROR_CHECK(depthFrameSource->get_DepthMinReliableDistance(&minDeptheliableDistance));
		ERROR_CHECK(depthFrameSource->get_DepthMaxReliableDistance(&maxDeptheliableDistance));

		std::cout << "depth min  : " << minDeptheliableDistance << std::endl;
		std::cout << "depth max  : " << maxDeptheliableDistance << std::endl;
		std::cout << "depth width : " << depthWidth << std::endl;
		std::cout << "depth height : " << depthHeight << std::endl;

		// バッファー作成

		depthBuffer.resize(depthWidth * depthHeight);
		std::cout << "depthBuffer size : " <<  depthBuffer.size() << std::endl;

		// index 作成

		rightHighIndex = (depthWidth * depthHeight / 4) + (depthWidth / 4);
		rightLowIndex = (depthWidth * depthHeight / 4 * 3) + (depthWidth / 4);
		leftHighIndex = (depthWidth * depthHeight / 4) + (depthWidth / 4 * 3);
		leftLowIndex = (depthWidth * depthHeight / 4 * 3) + (depthWidth / 4 * 3);
		}

	void run() {
		while (1) {
			updateDepthFrame();
			drawFourPoint();
			checkDepth();

			auto key = cv::waitKey(10);
			if (key == 'q') {
				cv::destroyWindow(DepthWindowName);
				std::cout << "\nコンソールで操作できる状態 now" << std::endl;
				break;
			}
		}
	}

	// 基準点の初期化

	void pointCalibration() {

		std::cout << "\n基準点の初期化の実行" << std::endl;

		// データの取得
		for (int i = 0; i < 100; ++i) {
			updateDepthFrame();
		}

		rightHighPoint = depthBuffer[rightHighIndex];
		rightLowPoint = depthBuffer[rightLowIndex];
		leftHighPoint = depthBuffer[leftHighIndex];
		leftLowPoint = depthBuffer[leftLowIndex];

		std::cout << "rightHighPoint : " << rightHighPoint << std::endl;
		std::cout << "rightLowPoint : " << rightLowPoint << std::endl;
		std::cout << "leftHighPoint : " << leftHighPoint << std::endl;
		std::cout << "leftLowPoint : " << leftLowPoint << std::endl;

		std::cout << "\n基準点の初期化完了" << std::endl;
	}

	// キャリブレーション

	void calibration() {

		std::cout << "キャリブレーションの実行" << std::endl;
		std::cout << "q : キャリブレーションの終了" << std::endl;
		std::cout << "左クリック : 位置の取得" << std::endl;

		// 変数の宣言
		mouseParam mouseEvent;

		// window の生成
		while (1) {
			
			updateDepthFrame();

			// depth データを表示する
			cv::Mat depthImage(depthHeight, depthWidth, CV_8UC1);

			// Depth data -> gray data
			for (int i = 0; i < depthImage.total(); ++i) {
				depthImage.data[i] = depthBuffer[i] % 255;
			}

			cv::imshow(DepthWindowName, depthImage);

			// コールバック設定
			cv::setMouseCallback(DepthWindowName, calibCallBack, &mouseEvent);

			// キーコンフィグ
			auto key = cv::waitKey(10);
			
			// 終了
			if (key == 'q') {
				cv::destroyWindow(DepthWindowName);
				std::cout << "\nコンソールで操作できる状態 now" << std::endl;
				break;
			}
			
			// 座標の取得
			else if (mouseEvent.event == cv::EVENT_LBUTTONDOWN) {
				
				std::cout << "取得した値" << std::endl;
				std::cout << "x 座標 : " << mouseEvent.x << std::endl;
				std::cout << "y 座標 : " << mouseEvent.y << std::endl;
				std::cout << "配列の位置 : " << depthWidth * mouseEvent.y + mouseEvent.x	<< std::endl;
			}
		}
	}

private:
	
	// update depth frame
	void updateDepthFrame() {

		//Depthフレーム取得
		CComPtr<IDepthFrame> depthFrame;
		auto ret = depthFrameReader->AcquireLatestFrame(&depthFrame);
		if (ret != S_OK) {
			return;
		}

		// データ取得
		ERROR_CHECK(depthFrame->CopyFrameDataToArray(depthBuffer.size(), &depthBuffer[0]));
	}

	// depth の表示

	void drawDepthFrame() {

		// depth データを表示する
		cv::Mat depthImage(depthHeight, depthWidth, CV_8UC1);

		// Depth data -> gray data
		for (int i = 0; i < depthImage.total(); ++i) {
			depthImage.data[i] = depthBuffer[i] % 255;
		}

		cv::imshow(DepthWindowName, depthImage);
	}

	// 4点の表示

	void drawFourPoint() {
		
		// depth 画面表示
		cv::Mat depthImage(depthHeight, depthWidth, CV_8UC1);

		// Depth data -> gray data
		for (int i = 0; i < depthImage.total(); ++i) {
			depthImage.data[i] = depthBuffer[i] % 255;
		}

		// データのインデックスを取得、表示
		cv::circle(depthImage, cv::Point(depthWidth / 4 * 3, depthHeight / 4 * 3), 10, cv::Scalar(0, 0, 255), 2);
		cv::circle(depthImage, cv::Point(depthWidth / 4 * 3, depthHeight / 4), 10, cv::Scalar(0, 0, 255), 2);
		cv::circle(depthImage, cv::Point(depthWidth / 4, depthHeight / 4 * 3), 10, cv::Scalar(0, 0, 255), 2);
		cv::circle(depthImage, cv::Point(depthWidth / 4, depthHeight / 4), 10, cv::Scalar(0, 0, 255), 2);

		cv::imshow(DepthWindowName, depthImage);
	}

	// depth の変化の取得

	void checkDepth() {

		// データの取得

		if (rightHighPoint == NULL || rightLowPoint == NULL || leftHighPoint == NULL || leftLowPoint == NULL) {
			std::cout << "初期化されていません。初期化を行えぃ！！" << std::endl;
		}

		if (std::abs(rightHighPoint - depthBuffer[rightHighIndex]) > changeDistance) {
			std::cout << "catch " << changeDistance << " rightHigh" << std::endl;
		}
		if (std::abs(rightLowPoint - depthBuffer[rightLowIndex]) > changeDistance) {
			std::cout << "catch " << changeDistance << " rightLow" << std::endl;
		}
		if (std::abs(leftHighPoint - depthBuffer[leftHighIndex]) > changeDistance) {
			std::cout << "catch " << changeDistance << " leftHigh" << std::endl;
		}
		if (std::abs(leftLowPoint - depthBuffer[leftLowIndex]) > changeDistance) {
			std::cout << "catch " << changeDistance << " leftLow" << std::endl;
		}
	}

	static void calibCallBack(int event, int x, int y, int flags, void* params) {

		// 宣言
		mouseParam *ptr = static_cast<mouseParam*>(params);

		// 代入
		ptr->x = x;
		ptr->y = y;
		ptr->event = event;
		ptr->flags = flags;

	}
};

void main() {
	try {
		int key;
		KinectApp app;
		app.init();

		while (1) {

			std::cout << "\nq: プログラムを終了" << std::endl;
			std::cout << "c: キャリブレーション" << std::endl;

			key = getchar();

			if (key == 'q') {
				break;
			}
			if (key == 'c') {
				app.calibration();
			}
		}
	}
	catch (std::exception &ex) {
		std::cout << ex.what() << std::endl;
	}
}
