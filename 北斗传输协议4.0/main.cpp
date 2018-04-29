// main.cpp : Defines the entry point for the console application.  
//  

#include "SerialPort.h"  
#include  <iostream>
#include "BD_Protocol_4_0.h" 

DWORD WINAPI myfun1(LPVOID lpParameter);                   //声明线程函数 
DWORD WINAPI myfun2(LPVOID lpParameter);
DWORD myfun1(LPVOID lpParameter)
{
	while (1)
	{
		Receive_Protocol();
		//myprint("hello my c");
		::Sleep(1);
	}

	return 0;
}
DWORD myfun2(LPVOID lpParameter)
{
	while (1)
	{
		check_status();
		//myprint("hello my thread");
		::Sleep(1000);
	}

	return 0;
}
int main()
{
	HANDLE h1,h2;                                         //定义句柄变量  
	init();

	if (!mySerialPort.InitPort(2))
	{
		std::cout << "initPort fail !" << std::endl;
	}
	else
	{
		std::cout << "initPort success !" << std::endl;
	}

	if (!mySerialPort.OpenListenThread())
	{
		std::cout << "OpenListenThread fail !" << std::endl;
	}
	else
	{
		std::cout << "OpenListenThread success !" << std::endl;
	}
	h1 = ::CreateThread(NULL, 0, myfun1, NULL, 0, NULL);   //创建线程  
	h2 = ::CreateThread(NULL, 0, myfun2, NULL, 0, NULL);
	while (1)
	{
		//myprint("1\n");
	}

	return 0;
}
