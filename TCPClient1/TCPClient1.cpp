// TCPClient1.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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


// 유일한 응용 프로그램 개체입니다.

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

        if (!o.od->index) { // SIBimage는 SharedImageBuffer 타입의 구조체이고, 
                            // 해당 멤버 함수로는 보이드 포인터 자료형인 _pMem이 있다.
            memcpy(o.od->SIBimage[0]._pMem, o.cam.image.data, o.od->dsz);
        }
        else {
            memcpy(o.od->SIBimage[1]._pMem, o.cam.image.data, o.od->dsz);
        }

        o.od->ready_flag = true;
        boost::detail::Sleep(40); // Sleep를 조정함에 따라 영상의 재생 속도가 바뀐다.(주의)
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
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("오류 발생"), MB_ICONERROR);
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
    ti t; // threadInterface 타입의 구조체 변수 t 선언.

    do {
        t.cam.vc.open("E:\\KDH\\TERM_PROJECT_MFC\\TERM_PROJECT_MFC (final)\\TCPClient1\\ObjectCamera.mp4");
        _tprintf(_T("Try VideoCapture Open\n\n"));
    } while (!t.cam.vc.isOpened());

    t.cam.vc >> t.cam.image; // cv::Mat.image라는 클래스에다가 
                             // videoCapture 타입의 객체인 vc를 넣은 것.

    //parameter check
    int nchs = t.cam.image.channels(); // 행렬의 채널 개수 반환
    std::cout << "\tchannel:" << nchs << std::endl;
    if (nchs != 3 && nchs != 4) {
        t.cam.vc.release(); // 채널이 흑백일 경우 영상을 끔
        getchar();
        return 101;
    }

    int dsz = t.cam.image.rows/*행의 개수*/ * t.cam.image.step;

    //Create a new segment with given name and size
    managed_shared_memory segment(create_only, "ODSharedMemory", dsz * 20); // dsz * 20이 기본

                                                                            //allocation 
    t.od = (odi*)segment.allocate(sizeof(odi));
    assert(t.od != NULL);

    //set image size
    t.od->dsz = dsz;
    t.od->iHeight = t.cam.image.size().height; // 불러온 영상의 높이
    t.od->iWidth = t.cam.image.size().width;  // 불러온 영상의 폭

    std::cout << " od dsz   : " << t.od->dsz << std::endl;

    // check allocation , SIB
    assert(&t.od->SIBimage != NULL);

    //SIB allocation, get handle
    for (int i = 0; i < 2; i++) { 
        t.od->SIBimage[i]._pMem = segment.allocate(dsz * 3); // dsz * 3 은 RGB때문에
        assert(&t.od->SIBimage[i]._pMem != NULL); // 안전장치, 영상 안맞으면 나감
        t.od->SIBimage[i]._hMem = segment.get_handle_from_address(t.od->SIBimage[i]._pMem);
        assert(&t.od->SIBimage[i]._hMem != NULL); // 안전장치(체크용)
    }
    //initial data copy 
    memcpy(t.od->SIBimage[0]._pMem, t.cam.image.data, dsz);
    memcpy(t.od->SIBimage[1]._pMem, t.cam.image.data, dsz);
    
    // get handle
    managed_shared_memory::handle_t handle = segment.get_handle_from_address(t.od);
    // 이 핸들을 다른 프로세스에서 참조할 수 있습니다.
    
    //execute
    std::stringstream s;
    char odc_fn[] = "MFCodclient.exe"; // 단순 문자열이면서 응용 프로그램명을 가리킴.
    
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
        // MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 오류 코드를 필요에 따라 수정합니다.
            wprintf(L"심각한 오류: MFC를 초기화하지 못했습니다.\n");
            nRetCode = 1;
        }
        else
        {
            _tsetlocale(LC_ALL, _T(""));
            // MFC 소켓 함수를 사용하려면 먼저 초기화 함수를 호출해야 한다.
            AfxSocketInit();

            // CSocket 객체를 생성하고, 객체 sock의 포트 번호를 기본값으로 준다.
            // 기본값을 줄 경우 운영체제가 자동으로 포트 번호를 할당한다.
            // 생성이 실패할 경우 에러를 띄우며 종료.
            CSocket sock;
            if (!sock.Create())
                ErrQuit(sock.GetLastError());

            // CAsyncSocket::Connect() 함수를 호출하여 서버에 접속한다.
            // 첫 번째 인자는 서버의 주소이고, 두 번째 인자는 "서버의" 포트 번호이다. 클라이언트 포트번호 아님.
            // 서버 주소로는 IP 주소 혹은 도메인 이름을 사용할 수 있는데,
            // 도메인 이름을 사용하면 MFC 내부에서 IP 주소로 자동으로 변환해준다.
            if (!sock.Connect(_T("127.0.0.1"), 8000))
                ErrQuit(sock.GetLastError());

            // 연결되면 Object_Detection 시작
            std::cout << "\t\tinitialize od\n"; // initialize od라는 구문이 cmd에 뜸.
            std::thread p1(program_thread, std::ref(s)); // stringsteam 타입 참조.

            while (!t.od->init_flag) boost::detail::Sleep(100);

            std::thread p2(_camera, std::ref(t)); // ThreadInterface 타입을 참조.

            cv::Mat image, result; // image와 result는 Mat 타입의 객체이다.

            int nDetections;
            std::vector<cv::Rect> boxes;
            std::vector<float> scores; // 검출된 것이 classes가 가리키는 대상과 얼마나 일치하는 가를 보여줌
            std::vector<int> classes; // 무엇을 검출했는지 알려주는 역할. 
                                      // 검출된 대상의 classes 가 1이라고 한다면 검출된 객체는 사람이 됨


            // 송신 측은 메모리대로, 수신 측은 배열 메모리 크기에 +1
            TCHAR buf[256];
            int nbytes;

            // 수신용 버퍼 및 바이트 수
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


// 분석 시작

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
						// 버튼 상태가 0 일 경우 사람 디텍트 
                        if ((t.od->classes[i] == 1)&&(status_button == 0)) {
                            cv::rectangle(image, t.od->boxes[i], cv::Scalar(B, G, R), 2);
                            countpeople++;
                        }
						// 버튼 상태가 1 일 경우 사람 이외의 물건 디텍트
                        else if (((t.od->classes[i] < 70 && t.od->classes[i] > 3) || (t.od->classes[i] > 1 && t.od->classes[i] < 3)) && (status_button == 1)) {
                            cv::rectangle(image, t.od->boxes[i], cv::Scalar(B, G, R), 2);
                            countobject++;
                        }
						// 버튼 상태가 2 일 경우 자동차 디텍트
						else if ((t.od->classes[i] == 3)&& (status_button == 2)) {
							cv::rectangle(image, t.od->boxes[i], cv::Scalar(B, G, R), 2);
							countcar++;
						}
						//else  //_tprintf(_T("디폴트 수신, 현재 버튼 상태 : %d\n"), status_button);
						//_tprintf(_T("수신한 바이트 크기 상태 : %d\n"), receive_nbytes);
                    }

                    t.od->run_flag = false;
                }
                else {
                    boost::detail::Sleep(50);
                }
                
                // 분석 결과를 Mat 타입의 구조체 result에 반영한다.
                result = image.clone();
                imshow("Traffic", result); // 네모 박스가 뜸
                waitKey(20);
                

                // CAsyncSocket::Send() 함수를 이용하여 데이터를 단위 시간 간격으로 무한 번 보낸다.
                if (countpeople > 0) {
                    wsprintf(buf, _T("사람은 %d번 인식 되었습니다.\r\n"), countpeople);
                    nbytes = sock.Send(buf, 256);
                    // 소켓 에러날 경우 프로그램 강제종료
                    if (nbytes == SOCKET_ERROR)
                        ErrQuit(sock.GetLastError());
                    // 소켓 에러가 없을 경우 메세지 전송했다는 프로시저를 콘솔에 출력
                    else {
                        //_tprintf(_T("<%d> 메세지 전송\n"), index); // index는 1씩 증가
                        boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
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
                        status_button = 0;	// 사람 버튼 눌림
                        //_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
                        boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
                    }
                    else if ((receive_nbytes >= 24) && (receive_nbytes <= 26)) {
                        status_button = 1;	// 나머지 버튼 눌림
                        //_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
                        boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
                    }
					else if ((receive_nbytes >= 27) && (receive_nbytes <= 29)) {
						status_button = 2;	// 자동차 버튼 눌림
						//_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}
                    else {
                        //_tprintf(_T("<%d> 디폴트 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
                        boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
                    }
                }
                else if (countobject > 0) {
                    wsprintf(buf, _T("사물은 %d번 인식 되었습니다.\r\n"), countobject);
                    nbytes = sock.Send(buf, 256);
                    // 소켓 에러날 경우 프로그램 강제종료
                    if (nbytes == SOCKET_ERROR)
                        ErrQuit(sock.GetLastError());
                    // 소켓 에러가 없을 경우 메세지 전송했다는 프로시저를 콘솔에 출력
                    else {
                        //_tprintf(_T("<%d> 메세지 전송\n"), index); // index는 1씩 증가
                        boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
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
						status_button = 0;	// 사람 버튼 눌림
						//_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}
					else if ((receive_nbytes >= 24) && (receive_nbytes <= 26)) {
						status_button = 1;	// 나머지 버튼 눌림
						//_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}
					else if ((receive_nbytes >= 27) && (receive_nbytes <= 29)) {
						status_button = 2;	// 자동차 버튼 눌림
						//_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}
					else {
						//_tprintf(_T("<%d> 디폴트 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}
                
                }
				else if (countcar > 0) {
					wsprintf(buf, _T("자동차는 %d번 인식 되었습니다.\r\n"), countcar);
					nbytes = sock.Send(buf, 256);
					// 소켓 에러날 경우 프로그램 강제종료
					if (nbytes == SOCKET_ERROR)
						ErrQuit(sock.GetLastError());
					// 소켓 에러가 없을 경우 메세지 전송했다는 프로시저를 콘솔에 출력
					else {
						//_tprintf(_T("<%d> 메세지 전송\n"), index); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
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
						status_button = 0;	// 사람 버튼 눌림
						//_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}
					else if ((receive_nbytes >= 24) && (receive_nbytes <= 26)) {
						status_button = 1;	// 나머지 버튼 눌림
						//_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}
					else if ((receive_nbytes >= 27) && (receive_nbytes <= 29)) {
						status_button = 2;	// 자동차 버튼 눌림
						//_tprintf(_T("<%d> 메세지 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}
					else {
						//_tprintf(_T("<%d> 디폴트 수신, 현재 버튼 상태 : %d\n"), index++, status_button); // index는 1씩 증가
						boost::detail::Sleep(4); // Sleep(1000); 을 대체한다(1초)
					}

				}
            }



            // 데이터 송신이 끝났으므로 CAsyncSocket::Close() 함수를 호출하여 소켓을 닫는다.
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
        // TODO: 오류 코드를 필요에 따라 수정합니다.
        wprintf(L"심각한 오류: GetModuleHandle 실패\n");
        nRetCode = 1;
    }



    return nRetCode;
}
