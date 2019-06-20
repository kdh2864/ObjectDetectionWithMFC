#pragma once


#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <sstream>
#include <cstdlib> //std::system
#include <thread>
#include <opencv2\opencv.hpp>

#include "SharedImageBuffer.h"

using std::string;

using namespace cv;
using namespace boost::interprocess;
using namespace boost::detail;

void program_thread(std::stringstream & cmd)
{
	std::system(cmd.str().c_str());
}

void _camera(ti &o) {
	while (1) {
		o.cam.vc >> o.cam.image;

		if (!o.od->index) {
			memcpy(o.od->SIBimage[0]._pMem, o.cam.image.data, o.od->dsz);
		}
		else {
			memcpy(o.od->SIBimage[1]._pMem, o.cam.image.data, o.od->dsz);
		}

		o.od->ready_flag = true;
		boost::detail::Sleep(40);
	}
}

int main(int argc, char* argv[])
{
	struct shm_remove
	{
		shm_remove() { shared_memory_object::remove("ODSharedMemory"); }
		~shm_remove() { shared_memory_object::remove("ODSharedMemory"); }
	} remover;

	//declare var
	ti t;

	do {
		t.cam.vc.open("E:\\BTS_girl.mp4");
		std::cout << "Try VideoCapture Open" << std::endl << std::endl;
	} while (!t.cam.vc.isOpened());
		
	t.cam.vc >> t.cam.image;

	//parameter check
	int nchs = t.cam.image.channels();
	std::cout << "\tchannel:" << nchs << std::endl;
	if (nchs != 3 && nchs != 4) {
		t.cam.vc.release();
		getchar();
		return 101;
	}

	int dsz = t.cam.image.rows * t.cam.image.step;

	//Create a new segment with given name and size
	managed_shared_memory segment(create_only, "ODSharedMemory", dsz * 20);

	//allocation 
	t.od = (odi*)segment.allocate(sizeof(odi));
	assert(t.od != NULL);

	//set image size
	t.od->dsz = dsz;
	t.od->iHeight = t.cam.image.size().height;
	t.od->iWidth = t.cam.image.size().width;

	std::cout << " od dsz   : " << t.od->dsz << std::endl;

	// check allocation , SIB
	assert(&t.od->SIBimage != NULL);

	//SIB allocation, get handle
	for (int i = 0; i < 2; i++) {
		t.od->SIBimage[i]._pMem = segment.allocate(dsz * 3);
		assert(&t.od->SIBimage[i]._pMem != NULL);
		t.od->SIBimage[i]._hMem = segment.get_handle_from_address(t.od->SIBimage[i]._pMem);
		assert(&t.od->SIBimage[i]._hMem != NULL);
	}
	//initial data copy
	memcpy(t.od->SIBimage[0]._pMem, t.cam.image.data, dsz);
	memcpy(t.od->SIBimage[1]._pMem, t.cam.image.data, dsz);

	// get handle
	managed_shared_memory::handle_t handle = segment.get_handle_from_address(t.od);

	//execute
	std::stringstream s;
	char odc_fn[] = "MFCodclient.exe";

	s << odc_fn << " " << handle << " " << t.od << std::ends;
	std::cout << "Main cmd: " << s.str() << std::endl;

	//for ODector, initial parameter setting
	t.od->nDetections = 0;
	t.od->init_flag = false;
	t.od->index = 0;
	t.od->frame_full = false;
	t.od->ready_flag = false;
	t.od->run_flag = false;

	//RUN
	std::cout << "\t\tinitialize od\n";
	std::thread p1(program_thread, std::ref(s));

	while (!t.od->init_flag) boost::detail::Sleep(100);

	std::thread p2(_camera, std::ref(t));
	
	cv::Mat image, result;

	int nDetections;
	std::vector<cv::Rect> boxes;
	std::vector<float> scores;
	std::vector<int> classes;

#define person
//#define object

	while (1) {
		image = cv::Mat(t.cam.image);
		if (t.od->run_flag) {
			for (int i = 0; i < t.od->nDetections; i++) {
				//std::cout
				//	<< "classes: " << t.od->classes[i] << "\t"
				//	<< "scores : " << t.od->scores[i] << "\t"
				//	<< "boxes  : " << t.od->boxes[i].x << "\t" << t.od->boxes[i].y << "\t"
				//	<< t.od->boxes[i].width << "\t" << t.od->boxes[i].height
				//	<< std::endl;
#ifdef person
				if (t.od->classes[i] == 1) {
					cv::rectangle(image, t.od->boxes[i], cv::Scalar(0, 0, 255), 2);
				}
#endif
#ifdef object 
				if (t.od->classes[i] < 70 && t.od->classes[i] != 1) {
					cv::rectangle(image, t.od->boxes[i], cv::Scalar(0, 0, 255), 2);
				}
#endif
			}

			t.od->run_flag = false;
		}
		else {
			boost::detail::Sleep(50);
		}

		result = image.clone();
		imshow("result", result);
		waitKey(20);
	}

	p1.join();
	p2.join();
	
	segment.deallocate(t.od->SIBimage[0]._pMem);
	segment.deallocate(t.od->SIBimage[1]._pMem);
	segment.deallocate(t.od);

	std::cout << " Main End !" << std::endl;
	t.cam.vc.release();
	return 0;
}