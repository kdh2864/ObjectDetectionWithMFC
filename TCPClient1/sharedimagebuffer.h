#pragma once
#ifdef _DEBUG
#pragma comment(lib, "opencv_world401d.lib")
#else
#pragma comment(lib, "opencv_world401.lib")
#endif
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <opencv2\opencv.hpp>

#include <libavcodec\avcodec.h>
#include <libavdevice\avdevice.h>
#include <libavfilter\avfilter.h>
#include <libavformat\avformat.h>
#include <libavutil\avutil.h>

#define nimage 3

struct SharedImageBuffer
{
    SharedImageBuffer()
        : _valid(false), _hMem(0), _pMem(NULL)
    {}

    //Mutex to protect access to the queue
    boost::interprocess::interprocess_mutex      mutex;

    //Condition to wait when the queue is empty
    boost::interprocess::interprocess_condition  cond_empty;
    boost::interprocess::interprocess_condition  cond_init;

    //Condition to wait when the queue is full
    boost::interprocess::interprocess_condition  cond_full;

    //Is there any message
    bool _valid;
    boost::interprocess::managed_shared_memory::handle_t _hMem;
    void * _pMem;

    static SharedImageBuffer* 
    Alloc(boost::interprocess::managed_shared_memory * segment, 
          int sz)
    {
        SharedImageBuffer* SIB = (SharedImageBuffer*)segment->allocate(sizeof(SharedImageBuffer));
        assert(SIB != NULL);
        SIB->_pMem = segment->allocate(sz*3);
        assert(SIB->_pMem != NULL);
        SIB->_hMem = segment->get_handle_from_address(SIB->_pMem);
		assert(SIB->_hMem != NULL);
        return SIB;
    }

    static void 
    DeAlloc(boost::interprocess::managed_shared_memory * segment, 
            SharedImageBuffer* SIB)
    {
        assert(SIB);
        assert(SIB->_pMem);
        segment->deallocate(SIB->_pMem);
        segment->deallocate(SIB);
    }
};

typedef struct ODetectInterface {
	SharedImageBuffer SIBimage[2];
	unsigned int dsz;

	//camera frame
	int iHeight;
	int iWidth;
	unsigned int index;
	bool frame_full;

	//bool flag;
	bool init_flag;
	bool ready_flag;
	bool run_flag;

	//output 
	int nDetections;
	cv::Rect boxes[255];
	float scores[255];
	int classes[255];
}odi;

typedef struct CameraInterface {
	//opencv camera
	cv::VideoCapture vc;
	cv::Mat image;
}ci;

typedef struct ThreadInterface {
	odi *od;
	ci cam;
}ti;