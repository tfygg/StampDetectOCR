#include <iostream>
#include <numeric>
#include <fstream>
#include <locale>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc/types_c.h>  
#include <Windows.h>
#include "PutText.h"

#include "dataStructures.h"

using namespace std;

void ocr()
{
	string ocrBasePath = "../data/ocr/";
	string ocrDictFile = ocrBasePath + "ocr_dict_5530.txt";
	string ocrModelConfiguration = ocrBasePath + "ocr_5530.cfg";
	string ocrModelWeights = ocrBasePath + "ocr_5530.weights";

	vector<string> ocr_classes;
	ifstream ifs(ocrDictFile);

	string line;
	while (getline(ifs, line)) ocr_classes.push_back(line);

	cv::dnn::Net ocr_net = cv::dnn::readNetFromDarknet(ocrModelConfiguration, ocrModelWeights);
	ocr_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	ocr_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

	string img_path = "../images/1.jpg";

	cv::Mat img = cv::imread(img_path);
	cv::Mat blob;
	int img_h = img.rows;
	int img_w = img.cols;
	double scale = img_h*1.0 / 32.0;

	img_w = int(img_w / scale);
	//printf("img_h=%d\n", img_h);
	//printf("img_w=%d\n", img_w);
	if (img_w < 8)
	{
		cout << "img width is less than 8.\n" << endl;
		return;
	}
	//if (img_w < 256)
	//{
	//	img_w = 256;
	//}
	double scale_factor = 1.0 / 255;
	cv::Size size = cv::Size(img_w, 32);
	cv::Scalar mean = cv::Scalar(0, 0, 0);
	bool swapRB = false;
	bool crop = false;
	cv::Mat img_resize;
	resize(img, img_resize, size);

	cv::Mat img_gray;
	cvtColor(img_resize, img_gray, CV_BGR2GRAY);
	cv::dnn::blobFromImage(img_gray, blob, scale_factor, size, 0.0, swapRB, crop);
	const int blob_h = blob.size[2];
	const int blob_w = blob.size[3];
	for (int y = 0; y < blob_h; y++)
	{
		float* scoresData = blob.ptr<float>(0, 0, y);
		for (int x = 0; x < blob_w; x++)
		{
			float score = scoresData[x];
			scoresData[x] = (score - 0.5) / 0.5;
		}
	}

	vector<cv::String> ocr_names;
	vector<int> ocr_out_layers = ocr_net.getUnconnectedOutLayers();
	vector<cv::String> cor_layers_names = ocr_net.getLayerNames();
	ocr_names.resize(ocr_out_layers.size());
	for (size_t i = 0; i < ocr_out_layers.size(); i++)
	{
		ocr_names[i] = cor_layers_names[ocr_out_layers[i] - 1];
	}
	vector<cv::Mat> ocr_net_output;
	ocr_net.setInput(blob);
	ocr_net.forward(ocr_net_output, ocr_names);

	cv::Mat out = ocr_net_output[0];
	//int cols = ocr_classes.size();
	//int row;
	int cols = out.size[3];  // 20
	int rows = out.size[1];  // 11316

	cv::Mat out_reshape(cols, rows, CV_32FC1);
	float* data = (float*)out.data;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			out_reshape.at<float>(j, i) = data[i*cols + j];
		}
	}
	vector<int> ocr_class_ids;
	data = (float*)out_reshape.data;
	for (int j = 0; j < out_reshape.rows; j++, data += out_reshape.cols)
	{
		cv::Mat scores = out_reshape.row(j).colRange(0, out_reshape.cols);
		cv::Point classId;
		double confidence;
		cv::minMaxLoc(scores, 0, &confidence, 0, &classId);
		ocr_class_ids.push_back(classId.x);
	}

	string ocr_predict;
	for (int i = 0; i < ocr_class_ids.size(); i++)
	{
		if ((ocr_class_ids[i] < ocr_classes.size() - 1) && (!((i > 0) && (ocr_class_ids[i - 1] == ocr_class_ids[i]))))
		{
			if (ocr_class_ids[i] != 0)
				ocr_predict += ocr_classes[ocr_class_ids[i]-1];
		}
	}
	std::cout << ocr_predict << std::endl;
}


void ocr(vector<BoundingBox> &bBoxes, vector<cv::String> &detect_names)
{
	string ocrBasePath = "../data/ocr/";
	string ocrDictFile = ocrBasePath + "ocr_dict_5530.txt";
	string ocrModelConfiguration = ocrBasePath + "ocr_5530.cfg";
	string ocrModelWeights = ocrBasePath + "ocr_5530.weights";

	vector<string> ocr_classes;
	ifstream ifs(ocrDictFile);

	string line;
	while (getline(ifs, line)) ocr_classes.push_back(line);

	cv::dnn::Net ocr_net = cv::dnn::readNetFromDarknet(ocrModelConfiguration, ocrModelWeights);
	ocr_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	ocr_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);


	for (int k = 0; k < bBoxes.size(); k++)
	{
		int index = bBoxes[k].classID;
		if (detect_names[index] == "stamp") //附件章就直接跳过
		{
			cout << "Is stamp." << endl;
			continue;
		}
			

		cv::Mat img = bBoxes[k].img;
		cv::Mat blob;
		int img_h = img.rows;
		int img_w = img.cols;
		double scale = img_h*1.0 / 32.0;
		img_w = int(img_w / scale);
		if (img_w < 8)
		{
			cout << "img width is less than 8.\n" << endl;
			continue;
		}
		double scale_factor = 1.0 / 255;
		cv::Size size = cv::Size(img_w, 32);
		cv::Scalar mean = cv::Scalar(0, 0, 0);
		bool swapRB = false;
		bool crop = false;
		cv::Mat img_resize;
		resize(img, img_resize, size);

		cv::Mat img_gray;
		cvtColor(img_resize, img_gray, CV_BGR2GRAY);
		cv::dnn::blobFromImage(img_gray, blob, scale_factor, size, 0.0, swapRB, crop);
		const int blob_h = blob.size[2];
		const int blob_w = blob.size[3];
		for (int y = 0; y < blob_h; y++)
		{
			float* scoresData = blob.ptr<float>(0, 0, y);
			for (int x = 0; x < blob_w; x++)
			{
				float score = scoresData[x];
				scoresData[x] = (score - 0.5) / 0.5;
			}
		}

		vector<cv::String> ocr_names;
		vector<int> ocr_out_layers = ocr_net.getUnconnectedOutLayers();
		vector<cv::String> cor_layers_names = ocr_net.getLayerNames();
		ocr_names.resize(ocr_out_layers.size());
		for (size_t i = 0; i < ocr_out_layers.size(); i++)
		{
			ocr_names[i] = cor_layers_names[ocr_out_layers[i] - 1];
		}
		vector<cv::Mat> ocr_net_output;
		ocr_net.setInput(blob);
		ocr_net.forward(ocr_net_output, ocr_names);

		cv::Mat out = ocr_net_output[0];
		//int cols = ocr_classes.size();
		//int row;
		int cols = out.size[3];  // 20
		int rows = out.size[1];  // 5530 字典大小

		cv::Mat out_reshape(cols, rows, CV_32FC1);
		float* data = (float*)out.data;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				out_reshape.at<float>(j, i) = data[i*cols + j];
			}
		}
		vector<int> ocr_class_ids;
		data = (float*)out_reshape.data;
		for (int j = 0; j < out_reshape.rows; j++, data += out_reshape.cols)
		{
			cv::Mat scores = out_reshape.row(j).colRange(0, out_reshape.cols);
			cv::Point classId;
			double confidence;
			cv::minMaxLoc(scores, 0, &confidence, 0, &classId);
			ocr_class_ids.push_back(classId.x);
		}

		string ocr_predict;
		for (int i = 0; i < ocr_class_ids.size(); i++)
		{
			if ((ocr_class_ids[i] < ocr_classes.size() - 1) && (!((i > 0) && (ocr_class_ids[i - 1] == ocr_class_ids[i]))))
			{
				if (ocr_class_ids[i] != 0)
					ocr_predict += ocr_classes[ocr_class_ids[i]-1];
			}
		}
	
		bBoxes[k].text = ocr_predict;
	}
}


void detect_ocr()
{
	cv::Mat img = cv::imread("../images/jz_test_000_098.jpg");
	string yoloBasePath = "../data/yolo/";
	string yoloClassesFile = yoloBasePath + "yolo.names";
	string yoloModelConfiguration = yoloBasePath + "yolov.cfg";
	string yoloModelWeights = yoloBasePath + "yolov.weights";

	vector<string> detect_classes;
	ifstream ifs(yoloClassesFile.c_str());
	string line;
	while (getline(ifs, line)) detect_classes.push_back(line);

	cv::dnn::Net detect_net = cv::dnn::readNetFromDarknet(yoloModelConfiguration, yoloModelWeights);
	detect_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	detect_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

	cv::Mat detect_blob;
	double scalefactor = 1 / 255.0;
	cv::Size size = cv::Size(416, 416);
	cv::Scalar mean = cv::Scalar(0, 0, 0);
	bool swapRB = false;
	bool crop = false;

	cv::dnn::blobFromImage(img, detect_blob, scalefactor, size, mean, swapRB, crop);

	vector<cv::String> detect_names;
	vector<int> detect_out_layers = detect_net.getUnconnectedOutLayers();
	vector<cv::String> detect_layers_names = detect_net.getLayerNames();

	detect_names.resize(detect_out_layers.size());
	for (size_t i = 0; i < detect_out_layers.size(); i++)
	{
		detect_names[i] = detect_layers_names[detect_out_layers[i] - 1];
	}

	vector<cv::Mat> detect_net_output;
	detect_net.setInput(detect_blob);
	detect_net.forward(detect_net_output, detect_names);

	float detect_conf_threshold = 0.20;
	vector<int> detect_class_ids;
	vector<float> detect_confidences;
	vector<cv::Rect> detect_boxes;
	for (size_t i = 0; i < detect_net_output.size(); i++)
	{
		float* data = (float*)detect_net_output[i].data;
		for (int j = 0; j < detect_net_output[i].rows; j++, data += detect_net_output[i].cols)
		{
			cv::Mat scores = detect_net_output[i].row(j).colRange(5, detect_net_output[i].cols);
			cv::Point classId;
			double confidence;
			cv::minMaxLoc(scores, 0, &confidence, 0, &classId);
			if (confidence > detect_conf_threshold)
			{
				cv::Rect box; int cx, cy;
				cx = (int)(data[0] * img.cols);
				cy = (int)(data[1] * img.rows);
				box.width = (int)(data[2] * img.cols);
				box.height = (int)(data[3] * img.rows);
				box.x = cx - box.width / 2;  //left
				box.y = cy - box.height / 2;  //top
				detect_boxes.push_back(box);
				detect_class_ids.push_back(classId.x);
				detect_confidences.push_back((float)confidence);
			}
		}
	}


	// perform non-maxima suppression
	float nms_threshold = 0.4;
	vector<int> indices;
	cv::dnn::NMSBoxes(detect_boxes, detect_confidences, detect_conf_threshold, nms_threshold, indices);
	vector<BoundingBox> bBoxes;
	for (auto it = indices.begin(); it != indices.end(); ++it)
	{
		BoundingBox bBox;
		bBox.roi = detect_boxes[*it];
		bBox.classID = detect_class_ids[*it];
		bBox.confidence = detect_confidences[*it];
		bBox.boxID = (int)bBoxes.size();
		bBox.img = cv::Mat(img, bBox.roi);
		bBox.text = "";
		bBoxes.push_back(bBox);
	}


	ocr(bBoxes,detect_classes);

	cv::Mat visImg = img.clone();
	for (auto it = bBoxes.begin(); it != bBoxes.end(); it++)
	{
		int top, left, width, height;
		top = (*it).roi.y;
		left = (*it).roi.x;
		width = (*it).roi.width;
		height = (*it).roi.height;
		cv::rectangle(visImg, cv::Point(left, top), cv::Point(left+width, top+height), cv::Scalar(255,0,0), 2);
		string label = (*it).text;
		if (label.length() > 0)
		{
			int baseLine;
			cv::Size label_size = getTextSize(label, cv::FONT_ITALIC, 0.5, 1, &baseLine);
			top = max(top, label_size.height);
			putTextZH(visImg, label.c_str(), cv::Point(left, top - round(1.5*label_size.height)), Scalar(0, 0, 255), 25, "微软雅黑", false, false);
		}

	}
	string windowName = "Object classification";
	cv::namedWindow(windowName, 0);
	cv::imshow(windowName, visImg);
	cv::waitKey(0); 
}

int main()
{
	//ocr();
	detect_ocr();
	return 0;
}
