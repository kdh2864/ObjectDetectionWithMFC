// TCPClient1.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//
#pragma once

#include "stdafx.h"
#include "TCPClient1.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <sstream>
#include <cstdlib> //std::system
#include <thread>
#include <opencv2\opencv.hpp>

#include "SharedImageBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ������ ���� ���α׷� ��ü�Դϴ�.

CWinApp theApp;


#define TERM_PROJECT_OBDETECTION
#ifdef TERM_PROJECT_OBDETECTION
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

        if (!o.od->index) { // SIBimage�� SharedImageBuffer Ÿ���� ����ü�̰�, 
                            // �ش� ��� �Լ��δ� ���̵� ������ �ڷ����� _pMem�� �ִ�.
            memcpy(o.od->SIBimage[0]._pMem, o.cam.image.data, o.od->dsz);
        }
        else {
            memcpy(o.od->SIBimage[1]._pMem, o.cam.image.data, o.od->dsz);
        }

        o.od->ready_flag = true;
        boost::detail::Sleep(40); // Sleep�� �����Կ� ���� ������ ��� �ӵ��� �ٲ��.(����)
    }
}
#endif

using namespace std;

void ErrQuit(int err)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("���� �߻�"), MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);

}

int main()
{
	int status_button = 0;
    struct shm_remove
    {
        shm_remove() { shared_memory_object::remove("ODSharedMemory"); }
        ~shm_remove() { shared_memory_object::remove("ODSharedMemory"); }
    } remover;

    //declare var
    ti t; // threadInterface Ÿ���� ����ü ���� t ����.

    do {
        t.cam.vc.open("E:\\KDH\\TERM_PROJECT_MFC\\TERM_PROJECT_MFC (final)\\TCPClient1\\ObjectCamera.mp4");
        _tprintf(_T("Try VideoCapture Open\n\n"));
    } while (!t.cam.vc.isOpened());

    t.cam.vc >> t.cam.image; // cv::Mat.image��� Ŭ�������ٰ� 
                             // videoCapture Ÿ���� ��ü�� vc�� ���� ��.

    //parameter check
    int nchs = t.cam.image.channels(); // ����� ä�� ���� ��ȯ
    std::cout << "\tchannel:" << nchs << std::endl;
    if (nchs != 3 && nchs != 4) {
        t.cam.vc.release(); // ä���� ����� ��� ������ ��
        getchar();
        return 101;
    }

    int dsz = t.cam.image.rows/*���� ����*/ * t.cam.image.step;

    //Create a new segment with given name and size
    managed_shared_memory segment(create_only, "ODSharedMemory", dsz * 20); // dsz * 20�� �⺻

                                                                            //allocation 
    t.od = (odi*)segment.allocate(sizeof(odi));
    assert(t.od != NULL);

    //set image size
    t.od->dsz = dsz;
    t.od->iHeight = t.cam.image.size().height; // �ҷ��� ������ ����
    t.od->iWidth = t.cam.image.size().width;  // �ҷ��� ������ ��

    std::cout << " od dsz   : " << t.od->dsz << std::endl;

    // check allocation , SIB
    assert(&t.od->SIBimage != NULL);

    //SIB allocation, get handle
    for (int i = 0; i < 2; i++) { 
        t.od->SIBimage[i]._pMem = segment.allocate(dsz * 3); // dsz * 3 �� RGB������
        assert(&t.od->SIBimage[i]._pMem != NULL); // ������ġ, ���� �ȸ����� ����
        t.od->SIBimage[i]._hMem = segment.get_handle_from_address(t.od->SIBimage[i]._pMem);
        assert(&t.od->SIBimage[i]._hMem != NULL); // ������ġ(üũ��)
    }
    //initial data copy 
    memcpy(t.od->SIBimage[0]._pMem, t.cam.image.data, dsz);
    memcpy(t.od->SIBimage[1]._pMem, t.cam.image.data, dsz);
    
    // get handle
    managed_shared_memory::handle_t handle = segment.get_handle_from_address(t.od);
    // �� �ڵ��� �ٸ� ���μ������� ������ �� �ֽ��ϴ�.
    
    //execute
    std::stringstream s;
    char odc_fn[] = "MFCodclient.exe"; // �ܼ� ���ڿ��̸鼭 ���� ���α׷����� ����Ŵ.
    
    s << odc_fn << " " << handle << " " << t.od << std::ends;
    std::cout << "Main cmd: " << s.str() << std::endl;

    //for ODector, initial parameter setting
    t.od->nDetections = 0;
    t.od->init_flag = false;
    t.od->index = 0;
    t.od->frame_full = false;
    t.od->ready_flag = false;
    t.od->run_flag = false;
   
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // MFC�� �ʱ�ȭ�մϴ�. �ʱ�ȭ���� ���� ��� ������ �μ��մϴ�.
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: ���� �ڵ带 �ʿ信 ���� �����մϴ�.
            wprintf(L"�ɰ��� ����: MFC�� �ʱ�ȭ���� ���߽��ϴ�.\n");
            nRetCode = 1;
        }
        else
        {
            _tsetlocale(LC_ALL, _T(""));
            // MFC ���� �Լ��� ����Ϸ��� ���� �ʱ�ȭ �Լ��� ȣ���ؾ� �Ѵ�.
            AfxSocketInit();

            // CSocket ��ü�� �����ϰ�, ��ü sock�� ��Ʈ ��ȣ�� �⺻������ �ش�.
            // �⺻���� �� ��� �ü���� �ڵ����� ��Ʈ ��ȣ�� �Ҵ��Ѵ�.
            // ������ ������ ��� ������ ���� ����.
            CSocket sock;
            if (!sock.Create())
                ErrQuit(sock.GetLastError());

            // CAsyncSocket::Connect() �Լ��� ȣ���Ͽ� ������ �����Ѵ�.
            // ù ��° ���ڴ� ������ �ּ��̰�, �� ��° ���ڴ� "������" ��Ʈ ��ȣ�̴�. Ŭ���̾�Ʈ ��Ʈ��ȣ �ƴ�.
            // ���� �ּҷδ� IP �ּ� Ȥ�� ������ �̸��� ����� �� �ִµ�,
            // ������ �̸��� ����ϸ� MFC ���ο��� IP �ּҷ� �ڵ����� ��ȯ���ش�.
            if (!sock.Connect(_T("127.0.0.1"), 8000))
                ErrQuit(sock.GetLastError());

            // ����Ǹ� Object_Detection ����
            std::cout << "\t\tinitialize od\n"; // initialize od��� ������ cmd�� ��.
            std::thread p1(program_thread, std::ref(s)); // stringsteam Ÿ�� ����.

            while (!t.od->init_flag) boost::detail::Sleep(100);

            std::thread p2(_camera, std::ref(t)); // ThreadInterface Ÿ���� ����.

            cv::Mat image, result; // image�� result�� Mat Ÿ���� ��ü�̴�.

            int nDetections;
            std::vector<cv::Rect> boxes;
            std::vector<float> scores; // ����� ���� classes�� ����Ű�� ���� �󸶳� ��ġ�ϴ� ���� ������
            std::vector<int> classes; // ������ �����ߴ��� �˷��ִ� ����. 
                                      // ����� ����� classes �� 1�̶�� �Ѵٸ� ����� ��ü�� ����� ��


            // �۽� ���� �޸𸮴��, ���� ���� �迭 �޸� ũ�⿡ +1
            TCHAR buf[256];
            int nbytes;

            // ���ſ� ���� �� ����Ʈ ��
            TCHAR receive_buf[256 + 1];
            int receive_nbytes;
            
            int index = 0;
			int B = 0;
			int G = 0;
			int R = 0;
			int clr_buf = 0;
            UINT countpeople = 0;
            UINT countobject = 0;
			UINT countcar = 0;


// �м� ����

            while (1) {
                image = cv::Mat(t.cam.image);
                if (t.od->run_flag) {
                    for (int i = 0; i < t.od->nDetections; i++) {
                        /*std::cout
                            << "classes: " << t.od->classes[i] << "\t"
                            << "scores : " << t.od->scores[i] << "\t"
                            << "boxes  : " << t.od->boxes[i].x << "\t" << t.od->boxes[i].y << "\t"
                            << t.od->boxes[i].width << "\t" << t.od->boxes[i].height
                            << std::endl;*/
						// ��ư ���°� 0 �� ��� ��� ����Ʈ 
                        if ((t.od->classes[i] == 1)&&(status_button == 0)) {
                            cv::rectangle(image, t.od->boxes[i], cv::Scalar(B, G, R), 2);
                            countpeople++;
                        }
						// ��ư ���°� 1 �� ��� ��� �̿��� ���� ����Ʈ
                        else if (((t.od->classes[i] < 70 && t.od->classes[i] > 3) || (t.od->classes[i] > 1 && t.od->classes[i] < 3)) && (status_button == 1)) {
                            cv::rectangle(image, t.od->boxes[i], cv::Scalar(B, G, R), 2);
                            countobject++;
                        }
						// ��ư ���°� 2 �� ��� �ڵ��� ����Ʈ
						else if ((t.od->classes[i] == 3)&& (status_button == 2)) {
							cv::rectangle(image, t.od->boxes[i], cv::Scalar(B, G, R), 2);
							countcar++;
						}
						//else  //_tprintf(_T("����Ʈ ����, ���� ��ư ���� : %d\n"), status_button);
						//_tprintf(_T("������ ����Ʈ ũ�� ���� : %d\n"), receive_nbytes);
                    }

                    t.od->run_flag = false;
                }
                else {
                    boost::detail::Sleep(50);
                }
                
                // �м� ����� Mat Ÿ���� ����ü result�� �ݿ��Ѵ�.
                result = image.clone();
                imshow("Traffic", result); // �׸� �ڽ��� ��
                waitKey(20);
                

                // CAsyncSocket::Send() �Լ��� �̿��Ͽ� �����͸� ���� �ð� �������� ���� �� ������.
                if (countpeople > 0) {
                    wsprintf(buf, _T("����� %d�� �ν� �Ǿ����ϴ�.\r\n"), countpeople);
                    nbytes = sock.Send(buf, 256);
                    // ���� ������ ��� ���α׷� ��������
                    if (nbytes == SOCKET_ERROR)
                        ErrQuit(sock.GetLastError());
                    // ���� ������ ���� ��� �޼��� �����ߴٴ� ���ν����� �ֿܼ� ���
                    else {
                        //_tprintf(_T("<%d> �޼��� ����\n"), index); // index�� 1�� ����
                        boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
                        countpeople = 0;
                    }

                    receive_nbytes = sock.Receive(receive_buf, 256);
                    receive_buf[receive_nbytes] = _T('\0');
					R = 0;
					G = 0;
					B = 0;
					clr_buf = receive_nbytes % 3;
					if (clr_buf == 0) R = 255;
					else if (clr_buf == 1) G = 255;
					else if (clr_buf == 2) B = 255;
                    if (receive_nbytes == 0)
                        ErrQuit(sock.GetLastError());
                    else if ((receive_nbytes >= 21) && (receive_nbytes <= 23)) {
                        status_button = 0;	// ��� ��ư ����
                        //_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
                        boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
                    }
                    else if ((receive_nbytes >= 24) && (receive_nbytes <= 26)) {
                        status_button = 1;	// ������ ��ư ����
                        //_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
                        boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
                    }
					else if ((receive_nbytes >= 27) && (receive_nbytes <= 29)) {
						status_button = 2;	// �ڵ��� ��ư ����
						//_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}
                    else {
                        //_tprintf(_T("<%d> ����Ʈ ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
                        boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
                    }
                }
                else if (countobject > 0) {
                    wsprintf(buf, _T("�繰�� %d�� �ν� �Ǿ����ϴ�.\r\n"), countobject);
                    nbytes = sock.Send(buf, 256);
                    // ���� ������ ��� ���α׷� ��������
                    if (nbytes == SOCKET_ERROR)
                        ErrQuit(sock.GetLastError());
                    // ���� ������ ���� ��� �޼��� �����ߴٴ� ���ν����� �ֿܼ� ���
                    else {
                        //_tprintf(_T("<%d> �޼��� ����\n"), index); // index�� 1�� ����
                        boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
                        countobject = 0;
                    }

                    receive_nbytes = sock.Receive(receive_buf, 256);
                    receive_buf[receive_nbytes] = _T('\0');
					R = 0;
					G = 0;
					B = 0;
					clr_buf = receive_nbytes % 3;
					if (clr_buf == 0) R = 255;
					else if (clr_buf == 1) G = 255;
					else if (clr_buf == 2) B = 255;
					if (receive_nbytes == 0)
						ErrQuit(sock.GetLastError());
					else if ((receive_nbytes >= 21) && (receive_nbytes <= 23)) {
						status_button = 0;	// ��� ��ư ����
						//_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}
					else if ((receive_nbytes >= 24) && (receive_nbytes <= 26)) {
						status_button = 1;	// ������ ��ư ����
						//_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}
					else if ((receive_nbytes >= 27) && (receive_nbytes <= 29)) {
						status_button = 2;	// �ڵ��� ��ư ����
						//_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}
					else {
						//_tprintf(_T("<%d> ����Ʈ ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}
                
                }
				else if (countcar > 0) {
					wsprintf(buf, _T("�ڵ����� %d�� �ν� �Ǿ����ϴ�.\r\n"), countcar);
					nbytes = sock.Send(buf, 256);
					// ���� ������ ��� ���α׷� ��������
					if (nbytes == SOCKET_ERROR)
						ErrQuit(sock.GetLastError());
					// ���� ������ ���� ��� �޼��� �����ߴٴ� ���ν����� �ֿܼ� ���
					else {
						//_tprintf(_T("<%d> �޼��� ����\n"), index); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
						countcar = 0;
					}

					receive_nbytes = sock.Receive(receive_buf, 256);
					receive_buf[receive_nbytes] = _T('\0');
					R = 0;
					G = 0;
					B = 0;
					clr_buf = receive_nbytes % 3;
					if (clr_buf == 0) R = 255;
					else if (clr_buf == 1) G = 255;
					else if (clr_buf == 2) B = 255;
					if (receive_nbytes == 0)
						ErrQuit(sock.GetLastError());
					else if ((receive_nbytes >= 21) && (receive_nbytes <= 23)) {
						status_button = 0;	// ��� ��ư ����
						//_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}
					else if ((receive_nbytes >= 24) && (receive_nbytes <= 26)) {
						status_button = 1;	// ������ ��ư ����
						//_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}
					else if ((receive_nbytes >= 27) && (receive_nbytes <= 29)) {
						status_button = 2;	// �ڵ��� ��ư ����
						//_tprintf(_T("<%d> �޼��� ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}
					else {
						//_tprintf(_T("<%d> ����Ʈ ����, ���� ��ư ���� : %d\n"), index++, status_button); // index�� 1�� ����
						boost::detail::Sleep(4); // Sleep(1000); �� ��ü�Ѵ�(1��)
					}

				}
            }



            // ������ �۽��� �������Ƿ� CAsyncSocket::Close() �Լ��� ȣ���Ͽ� ������ �ݴ´�.
            sock.Close();

            p1.join();
            p2.join();

            segment.deallocate(t.od->SIBimage[0]._pMem);
            segment.deallocate(t.od->SIBimage[1]._pMem);
            segment.deallocate(t.od);

            std::cout << " Main End !" << std::endl;
            t.cam.vc.release();



            
        }

    }
    else
    {
        // TODO: ���� �ڵ带 �ʿ信 ���� �����մϴ�.
        wprintf(L"�ɰ��� ����: GetModuleHandle ����\n");
        nRetCode = 1;
    }



    return nRetCode;
}
