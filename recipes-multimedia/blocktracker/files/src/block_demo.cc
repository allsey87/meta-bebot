/**
 * @file april_tags.cpp
 * @brief Example application for April tags library
 * @author: Michael Kaess
 *
 * Opens the first available camera (typically a built in camera in a
 * laptop) and continuously detects April tags in the incoming
 * images. Detections are both visualized in the live image and shown
 * in the text console. Optionally allows selecting of a specific
 * camera in case multiple ones are present and specifying image
 * resolution as long as supported by the camera. Also includes the
 * option to send tag detections via a serial port, for example when
 * running on a Raspberry Pi that is connected to an Arduino.
 */

using namespace std;

#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <list>
#include <sys/time.h>

#define PREAMBLE1  0xF0
#define PREAMBLE2  0xCA
#define POSTAMBLE1 0x53
#define POSTAMBLE2 0x0F

const string usage = "\n"
	"Usage:\n"
	"  apriltags_demo [OPTION...] [IMG1 [IMG2...]]\n"
	"\n"
	"Options:\n"
	"  -h  -?          Show help options\n"
	"  -d              Disable graphics\n"
	"  -t              Timing of tag extraction\n"
	"  -C <bbxhh>      Tag family (default 36h11)\n"
	"  -D <id>         Video device ID (if multiple cameras present)\n"
	"  -F <fx>         Focal length in pixels\n"
	"  -W <width>      Image width (default 640, availability depends on camera)\n"
	"  -H <height>     Image height (default 480, availability depends on camera)\n"
	"  -S <size>       Tag size (square black frame) in meters\n"
	"\n";

const string intro = "\n"
	"April tags test code\n"
	"(C) 2012-2014 Massachusetts Institute of Technology\n"
	"Michael Kaess\n"
	"\n";


// OpenCV library for easy access to USB camera and drawing of images
// on screen
#include <opencv2/core/core.hpp>
#include <iss_capture.h>

#include "tcp_image_socket.h"
#include "uart_socket.h"

// April tags detector and various families that can be selected by command line option
#include <apriltags/TagDetector.h>
#include <apriltags/Tag36h11.h>

// Needed for getopt / command line options processing
#include <unistd.h>
extern int optind;
extern char *optarg;

// utility function to provide current system time (used below in
// determining frame rate at which images are being processed)
double tic() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return ((double)t.tv_sec + ((double)t.tv_usec)/1000000.);
}


#include <cmath>

#ifndef PI
const double PI = 3.14159265358979323846;
#endif
const double TWOPI = 2.0*PI;

/**
 * Normalize angle to be within the interval [-pi,pi].
 */
inline double standardRad(double t) {
	if (t >= 0.) {
		t = fmod(t+PI, TWOPI) - PI;
	} else {
		t = fmod(t-PI, -TWOPI) + PI;
	}
	return t;
}


CTCPImageSocket& operator<<(CTCPImageSocket& m_cTCPImageSocket, const cv::Mat& m_cImage) {
	if(m_cImage.type() == CV_8UC1) {
		m_cTCPImageSocket.Write(m_cImage.data, m_cImage.size().width, m_cImage.size().height);
	} else {
		cv::Mat m_cGrayscale;
		cv::cvtColor(m_cImage, m_cGrayscale, CV_RGB2GRAY);
		m_cTCPImageSocket.Write(m_cGrayscale.data, m_cGrayscale.size().width, m_cGrayscale.size().height);
	}
	return m_cTCPImageSocket;
}


class Demo {

	AprilTags::TagDetector* m_tagDetector;
	AprilTags::TagCodes m_tagCodes;

	CTCPImageSocket m_cTCPImageSocket;

	CPacketControlInterface m_cBaseInterface;
	CPacketControlInterface m_cManipulatorInterface;

	CUARTSocket m_cBaseUARTSocket;
	CUARTSocket m_cManipulatorUARTSocket;

	bool m_draw; // draw image and April tag detections?
	bool m_timing; // print timing information for each tag extraction call

	int m_width; // image size in pixels
	int m_height;
	double m_tagSize; // April tag side length in meters of square black frame
	double m_fx; // camera focal length in pixels
	double m_fy;
	double m_px; // camera principal point
	double m_py;

	list<string> m_imgNames;

	jafp::OvVideoCapture m_cISSCaptureDevice;

	int m_exposure;
	int m_gain;
	int m_brightness;

	int m_deviceId; // camera id (in case of multiple cameras)
public:

	// default constructor
	Demo() :
		// default settings, most can be modified through command line options (see below)
		m_tagDetector(NULL),
		m_tagCodes(AprilTags::tagCodes36h11),

		m_cBaseInterface(m_cBaseUARTSocket),
		m_cManipulatorInterface(m_cManipulatorUARTSocket),

		m_draw(true),
		m_timing(false),

		m_width(640),
		m_height(360),
		m_tagSize(0.024),
		m_fx(554.502453),
		m_fy(555.298755),
		m_px(317.056149),
		m_py(181.126526),

		m_cISSCaptureDevice(jafp::OvVideoCapture::OV_MODE_1280_720_30),

		m_exposure(-1),
		m_gain(-1),
		m_brightness(-1),

		m_deviceId(0)
	{}

	~Demo() {
		m_cISSCaptureDevice.release();
	}
	
	// changing the tag family
	void setTagCodes(string s) {
		if (s=="36h11") {
			m_tagCodes = AprilTags::tagCodes36h11;
		} else {
			cout << "Invalid tag family specified" << endl;
			exit(1);
		}
	}

	// parse command line options to change default behavior
	void parseOptions(int argc, char* argv[]) {
		int c;
		while ((c = getopt(argc, argv, ":h?adtC:F:H:S:W:D:")) != -1) {
			// Each option character has to be in the string in getopt();
			// the first colon changes the error character from '?' to ':';
			// a colon after an option means that there is an extra
			// parameter to this option; 'W' is a reserved character
			switch (c) {
			case 'h':
			case '?':
				cout << intro;
				cout << usage;
				exit(0);
				break;
			case 'd':
				m_draw = false;
				break;
			case 't':
				m_timing = true;
				break;
			case 'C':
				setTagCodes(optarg);
				break;
			case 'F':
				m_fx = atof(optarg);
				m_fy = m_fx;
				break;
			case 'H':
				m_height = atoi(optarg);
				m_py = m_height/2;
				break;
			case 'S':
				m_tagSize = atof(optarg);
				break;
			case 'W':
				m_width = atoi(optarg);
				m_px = m_width/2;
				break;
			case 'D':
				m_deviceId = atoi(optarg);
				break;
			case ':': // unknown option, from getopt
				cout << intro;
				cout << usage;
				exit(1);
				break;
			}
		}

		if (argc > optind) {
			for (int i=0; i<argc-optind; i++) {
				m_imgNames.push_back(argv[optind+i]);
			}
		}
	}

	void setup() {
		m_tagDetector = new AprilTags::TagDetector(m_tagCodes);

		cout << "Connecting to base microcontroller:";
		m_cBaseUARTSocket.Open("/dev/ttySC1", 57600);
		cout << "done" << endl;
		cout << "Connecting to manipulator microcontroller:";
		m_cManipulatorUARTSocket.Open("/dev/ttySC2", 57600);
		cout << "done" << endl;
		cout << "Connecting to dev machine for image streaming:";
		m_cTCPImageSocket.Open("10.0.0.1", 23268);
		cout << "done" << endl;
	}

	void setupVideo() {
		if (!m_cISSCaptureDevice.open()) {
			cerr << "ERROR: Can't find video device " << m_deviceId << "\n";
			exit(1);
		}

		cout << "Camera successfully opened (ignore error messages above...)" << endl;
		cout << "Actual resolution: "
		     << m_cISSCaptureDevice.get(CV_CAP_PROP_FRAME_WIDTH) << "x"
		     << m_cISSCaptureDevice.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

	}


	void processImage(cv::Mat& image_gray) {
		// alternative way is to grab, then retrieve; allows for
		// multiple grab when processing below frame rate - v4l keeps a
		// number of frames buffered, which can lead to significant lag
		//      m_cISSCaptureDevice.grab();
		//      m_cISSCaptureDevice.retrieve(image);

		// detect April tags (requires a gray scale image)

		//image_annotated.create(image_gray.size(), CV_8UC3);
		//cv::cvtColor(image_gray, image_annotated, CV_GRAY2RGB);

		// GetTags
		vector<AprilTags::TagDetection> detections = m_tagDetector->extractTags(image_gray);

		for (int i=0; i<detections.size(); i++) {
			std::vector<cv::Point3f> objPts;
			std::vector<cv::Point2f> imgPts;
			double s = m_tagSize/2.;
			objPts.push_back(cv::Point3f(-s,-s, 0));
			objPts.push_back(cv::Point3f( s,-s, 0));
			objPts.push_back(cv::Point3f( s, s, 0));
			objPts.push_back(cv::Point3f(-s, s, 0));

			std::pair<float, float> p1 = detections[i].p[0];
			std::pair<float, float> p2 = detections[i].p[1];
			std::pair<float, float> p3 = detections[i].p[2];
			std::pair<float, float> p4 = detections[i].p[3];
			imgPts.push_back(cv::Point2f(p1.first, p1.second));
			imgPts.push_back(cv::Point2f(p2.first, p2.second));
			imgPts.push_back(cv::Point2f(p3.first, p3.second));
			imgPts.push_back(cv::Point2f(p4.first, p4.second));

			cv::Mat ct_rvec, ct_tvec;
			cv::Matx33f cameraMatrix(
						 m_fx, 0, m_px,
						 0, m_fy, m_py,
						 0,  0,  1);
			cv::Vec4f distParam(0,0,0,0); // all 0?
			cv::solvePnP(objPts, imgPts, cameraMatrix, distParam, ct_rvec, ct_tvec);
			cv::Mat 
				cb_rvec, 
				cb_tvec, 
				tb_rvec,
				tb_tvec = cv::Mat::zeros(3,1,CV_32F);
        
			/* initialization of zero rotation vector */
			cv::Rodrigues(cv::Mat::eye(3,3,CV_32F), tb_rvec);

			/* tb_tvec projects from the tag into the center of the block */
			tb_tvec.at<float>(2) = -0.0275;

			/* Compose the tag-to-block and camera-to-tag transformations to get
			   the camera-to-block transformation */
			cv::composeRT(tb_rvec, tb_tvec, ct_rvec, ct_tvec, cb_rvec, cb_tvec);

			/* project points is checking correctness */
			std::vector<cv::Point3f> TgtPts;

			TgtPts.push_back(cv::Point3f( 0.0275,  0.0275, 0.0275));
			TgtPts.push_back(cv::Point3f( 0.0275, -0.0275, 0.0275));
			TgtPts.push_back(cv::Point3f(-0.0275, -0.0275, 0.0275));
			TgtPts.push_back(cv::Point3f(-0.0275,  0.0275, 0.0275));

			TgtPts.push_back(cv::Point3f( 0.0275,  0.0275, -0.0275));
			TgtPts.push_back(cv::Point3f( 0.0275, -0.0275, -0.0275));
			TgtPts.push_back(cv::Point3f(-0.0275, -0.0275, -0.0275));
			TgtPts.push_back(cv::Point3f(-0.0275,  0.0275, -0.0275));

			std::vector<cv::Point2f> outImgPoints;

			cv::projectPoints(TgtPts, cb_rvec, cb_tvec, cameraMatrix, distParam, outImgPoints);

			cv::line(image_gray, outImgPoints[0], outImgPoints[1], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[1], outImgPoints[2], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[2], outImgPoints[3], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[3], outImgPoints[0], cv::Scalar(255,255,255,0), 2);

			cv::line(image_gray, outImgPoints[4], outImgPoints[5], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[5], outImgPoints[6], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[6], outImgPoints[7], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[7], outImgPoints[4], cv::Scalar(255,255,255,0), 2);

			cv::line(image_gray, outImgPoints[0], outImgPoints[4], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[1], outImgPoints[5], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[2], outImgPoints[6], cv::Scalar(255,255,255,0), 2);
			cv::line(image_gray, outImgPoints[3], outImgPoints[7], cv::Scalar(255,255,255,0), 2);

			/* extract the position and rotation of the block, relative to the camera */
			cv::Matx33f cR;
			cv::Rodrigues(cb_rvec, cR);
			cv::Matx44f cT(cR(0,0), cR(0,1), cR(0,2), cb_tvec.at<float>(0),
				       cR(1,0), cR(1,1), cR(1,2), cb_tvec.at<float>(1),
				       cR(2,0), cR(2,1), cR(2,2), cb_tvec.at<float>(2),
				       0,       0,       0,       1);
			cv::Matx44f cM( 0,  0,  1,  0,
				       -1,  0,  0,  0,
					0, -1,  0,  0,
					0,  0,  0,  1);
			cv::Matx44f cMT(cM, cT, cv::Matx_MatMulOp());
			/* Dump the output to the console */
			cout << fixed << setprecision(3) << "trans = [" << cMT(0,3) << ", " << cMT(1,3) << ", " << cMT(2,3) << "]" << endl;

			cv::Matx33f cF( 1,  0,  0, 
					0, -1,  0,
					0,  0,  1);
			cv::Matx33f cFR(cF, cR, cv::Matx_MatMulOp());
			float fYaw, fPitch, fRoll;
			fYaw = standardRad(atan2(cFR(1,0), cFR(0,0)));
			fPitch = standardRad(atan2(-cFR(2,0), cFR(0,0) * cos(fYaw) + cFR(1,0) * sin(fYaw)));
			fRoll  = standardRad(atan2(cFR(0,2) * sin(fYaw) - cFR(1,2) * cos(fYaw), -cFR(0,1) * sin(fYaw) + cFR(1,1) * cos(fYaw)));
			/* Dump the output to the console */
			cout << fixed << setprecision(3) << "rot = [" << fYaw << ", " << fPitch << ", " << fRoll << "]" << endl;

			/* Get the distance */
			float fDist = sqrt(cMT(0,3) * cMT(0,3) + cMT(1,3) * cMT(1,3) + cMT(2,3) * cMT(2,3));
			cout << fixed << setprecision(3) << "dist = " << fDist << endl;

			/* For only the first detection */
			if(i == 0) {
				int8_t pnSpeed[2];
				uint8_t punData[2];
				float fTargetDist = 0.15; // 25cm
				float fDistError = fDist - fTargetDist;
				
				pnSpeed[0] = (fDistError * 128 > INT8_MAX) ? INT8_MAX : ((fDistError * 128 < INT8_MIN) ? INT8_MIN : fDistError * 128);
				pnSpeed[1] = (fDistError * 128 > INT8_MAX) ? INT8_MAX : ((fDistError * 128 < INT8_MIN) ? INT8_MIN : fDistError * 128);

				punData[0] = reinterpret_cast<uint8_t&>(pnSpeed[0]);
				punData[1] = reinterpret_cast<uint8_t&>(pnSpeed[1]);

				m_cBaseInterface.SendPacket(CPacketControlInterface::CPacket::EType::SET_DDS_SPEED, punData, 2);
			}

		}

		if(detections.size() == 0) {
			uint8_t punZeroSpeed[2] = {0,0};
			m_cBaseInterface.SendPacket(CPacketControlInterface::CPacket::EType::SET_DDS_SPEED, punZeroSpeed, 2);
		}

		cout << "---" << endl;


		// show the current image including any detections
		for (int i=0; i<detections.size(); i++) {
			// also highlight in the image
			//detections[i].draw(gray_annotated);
		}
	}

	// The processing loop where images are retrieved, tags detected,
	// and information about detections generated
	void loop() {

		cv::Mat image_gray;
		//cv::Mat image_annotated;
		uint8_t punSwitchOn[1] = {1};
		uint8_t punSwitchOff[1] = {0};

		m_cBaseInterface.SendPacket(CPacketControlInterface::CPacket::EType::SET_DDS_ENABLE, punSwitchOn, 1);

		int frame = 0;
		double last_t = tic();
		while (true) {

			// capture frame
			m_cISSCaptureDevice >> image_gray;

			processImage(image_gray);

			// print out the frame rate at which image frames are being processed
			frame++;
			if (frame % 10 == 0) {
				double t = tic();
				cout << "  " << 10./(t-last_t) << " fps" << endl;
				last_t = t;
			}
			
			m_cTCPImageSocket << image_gray;

			/*
			  std::stringstream str;
			  str << "frame_" << std::setfill('0') << std::setw(5) << frame << ".png";
			  cv::imwrite(str.str(), image_gray);
			*/
		}
	}

}; // Demo


// here is were everything begins
int main(int argc, char* argv[]) {
	Demo demo;

	// process command line options
	demo.parseOptions(argc, argv);

	demo.setup();

	// setup image source, window for drawing, serial port...
	demo.setupVideo();

	// the actual processing loop where tags are detected and visualized
	demo.loop();




	return 0;
}
