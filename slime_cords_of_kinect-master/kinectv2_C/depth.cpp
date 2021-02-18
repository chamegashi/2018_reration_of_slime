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

	// �\��
	int depthWidth;
	int depthHeight;

	std::vector<unsigned short> depthBuffer;

	// window �p

	const char* DepthWindowName = "depth image";

	// check depth �p

	int rightHighPoint;
	int rightLowPoint;
	int leftHighPoint;
	int leftLowPoint;

	// check depth index �p

	int rightHighIndex;
	int rightLowIndex;
	int leftHighIndex;
	int leftLowIndex;


	// ��̔��f�� depth

	int changeDistance = 200;

	// �}�E�X�p
	struct mouseParam {
		int x;
		int y;
		int event;
		int flags;
	};

public:

	// kincet �̏�����

	void init() {

		//kinect �̎擾
		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
		ERROR_CHECK(kinect->Open());

		//Depth ���[�_�擾
		CComPtr<IDepthFrameSource> depthFrameSource;
		ERROR_CHECK(kinect->get_DepthFrameSource(&depthFrameSource));
		ERROR_CHECK(depthFrameSource->OpenReader(&depthFrameReader));

		// Depth size �擾
		CComPtr<IFrameDescription> depthFrameDescripton;
		ERROR_CHECK(depthFrameSource->get_FrameDescription(&depthFrameDescripton));
		ERROR_CHECK(depthFrameDescripton->get_Width(&depthWidth));
		ERROR_CHECK(depthFrameDescripton->get_Height(&depthHeight));

		// Depth �̍ő�A�ŏ��擾
		unsigned short minDeptheliableDistance;
		unsigned short maxDeptheliableDistance;

		ERROR_CHECK(depthFrameSource->get_DepthMinReliableDistance(&minDeptheliableDistance));
		ERROR_CHECK(depthFrameSource->get_DepthMaxReliableDistance(&maxDeptheliableDistance));

		std::cout << "depth min  : " << minDeptheliableDistance << std::endl;
		std::cout << "depth max  : " << maxDeptheliableDistance << std::endl;
		std::cout << "depth width : " << depthWidth << std::endl;
		std::cout << "depth height : " << depthHeight << std::endl;

		// �o�b�t�@�[�쐬

		depthBuffer.resize(depthWidth * depthHeight);
		std::cout << "depthBuffer size : " <<  depthBuffer.size() << std::endl;

		// index �쐬

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
				std::cout << "\n�R���\�[���ő���ł����� now" << std::endl;
				break;
			}
		}
	}

	// ��_�̏�����

	void pointCalibration() {

		std::cout << "\n��_�̏������̎��s" << std::endl;

		// �f�[�^�̎擾
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

		std::cout << "\n��_�̏���������" << std::endl;
	}

	// �L�����u���[�V����

	void calibration() {

		std::cout << "�L�����u���[�V�����̎��s" << std::endl;
		std::cout << "q : �L�����u���[�V�����̏I��" << std::endl;
		std::cout << "���N���b�N : �ʒu�̎擾" << std::endl;

		// �ϐ��̐錾
		mouseParam mouseEvent;

		// window �̐���
		while (1) {
			
			updateDepthFrame();

			// depth �f�[�^��\������
			cv::Mat depthImage(depthHeight, depthWidth, CV_8UC1);

			// Depth data -> gray data
			for (int i = 0; i < depthImage.total(); ++i) {
				depthImage.data[i] = depthBuffer[i] % 255;
			}

			cv::imshow(DepthWindowName, depthImage);

			// �R�[���o�b�N�ݒ�
			cv::setMouseCallback(DepthWindowName, calibCallBack, &mouseEvent);

			// �L�[�R���t�B�O
			auto key = cv::waitKey(10);
			
			// �I��
			if (key == 'q') {
				cv::destroyWindow(DepthWindowName);
				std::cout << "\n�R���\�[���ő���ł����� now" << std::endl;
				break;
			}
			
			// ���W�̎擾
			else if (mouseEvent.event == cv::EVENT_LBUTTONDOWN) {
				
				std::cout << "�擾�����l" << std::endl;
				std::cout << "x ���W : " << mouseEvent.x << std::endl;
				std::cout << "y ���W : " << mouseEvent.y << std::endl;
				std::cout << "�z��̈ʒu : " << depthWidth * mouseEvent.y + mouseEvent.x	<< std::endl;
			}
		}
	}

private:
	
	// update depth frame
	void updateDepthFrame() {

		//Depth�t���[���擾
		CComPtr<IDepthFrame> depthFrame;
		auto ret = depthFrameReader->AcquireLatestFrame(&depthFrame);
		if (ret != S_OK) {
			return;
		}

		// �f�[�^�擾
		ERROR_CHECK(depthFrame->CopyFrameDataToArray(depthBuffer.size(), &depthBuffer[0]));
	}

	// depth �̕\��

	void drawDepthFrame() {

		// depth �f�[�^��\������
		cv::Mat depthImage(depthHeight, depthWidth, CV_8UC1);

		// Depth data -> gray data
		for (int i = 0; i < depthImage.total(); ++i) {
			depthImage.data[i] = depthBuffer[i] % 255;
		}

		cv::imshow(DepthWindowName, depthImage);
	}

	// 4�_�̕\��

	void drawFourPoint() {
		
		// depth ��ʕ\��
		cv::Mat depthImage(depthHeight, depthWidth, CV_8UC1);

		// Depth data -> gray data
		for (int i = 0; i < depthImage.total(); ++i) {
			depthImage.data[i] = depthBuffer[i] % 255;
		}

		// �f�[�^�̃C���f�b�N�X���擾�A�\��
		cv::circle(depthImage, cv::Point(depthWidth / 4 * 3, depthHeight / 4 * 3), 10, cv::Scalar(0, 0, 255), 2);
		cv::circle(depthImage, cv::Point(depthWidth / 4 * 3, depthHeight / 4), 10, cv::Scalar(0, 0, 255), 2);
		cv::circle(depthImage, cv::Point(depthWidth / 4, depthHeight / 4 * 3), 10, cv::Scalar(0, 0, 255), 2);
		cv::circle(depthImage, cv::Point(depthWidth / 4, depthHeight / 4), 10, cv::Scalar(0, 0, 255), 2);

		cv::imshow(DepthWindowName, depthImage);
	}

	// depth �̕ω��̎擾

	void checkDepth() {

		// �f�[�^�̎擾

		if (rightHighPoint == NULL || rightLowPoint == NULL || leftHighPoint == NULL || leftLowPoint == NULL) {
			std::cout << "����������Ă��܂���B���������s�����I�I" << std::endl;
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

		// �錾
		mouseParam *ptr = static_cast<mouseParam*>(params);

		// ���
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

			std::cout << "\nq: �v���O�������I��" << std::endl;
			std::cout << "c: �L�����u���[�V����" << std::endl;

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
