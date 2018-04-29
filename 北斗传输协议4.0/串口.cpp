// chuankou1.cpp : 定义控制台应用程序的入口点。
//


#include<iostream>  
#include<windows.h>  

using namespace std;

int main123()
{
	//这里是打开串口通信
	HANDLE hCom=NULL;  //全局变量，串口句柄
	/*hCom = CreateFile("\\\\.\\COM11",  //COM11口
		GENERIC_READ | GENERIC_WRITE, //允许读和写
		0,  //独占方式
		NULL,
		OPEN_EXISTING,  //打开而不是创建
		0, //异步  1为同步
		NULL);*/
	if (hCom == (HANDLE)-1)
	{
		cout << "打开串口失败" << endl;
		return -1;
	}
	else {
		cout << "打开串口成功" << endl;
	}
	SetupComm(hCom, 1024, 1024);
	//超时
	// 设置通信超时时间 SET THE COMM TIMEOUTS.
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;
	if (!SetCommTimeouts(hCom, &timeouts)) {
		cout << "超时设置失败" << endl;
	}
	else {
		cout << "超时设置成功" << endl;
	}
	//通信设置
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 115200;
	dcb.ByteSize = 8;
	dcb.Parity = 0;
	dcb.StopBits = 1;
	if (SetCommState(hCom, &dcb)) {
		cout << "配置串口成功" << endl;
	}

	//发送读取数据
	char data[1];
	memcpy(data, "A", 1);
	DWORD dwWrittenLen = 0;
	char buffer[1];
	DWORD dwReadLen = 0;
	for (int i = 0; i < 10; ++i) {
		if (!WriteFile(hCom, data, 1, &dwWrittenLen, NULL)) {
			cout << "发送串口数据失败" << endl;
		}
		else {
			cout << "发送数据成功：" << data[0] << endl;
		}
		
		if (!ReadFile(hCom, buffer, sizeof(buffer), &dwReadLen, NULL)) {
		cout << "接收串口数据失败" << endl;
		}
		else {
		cout << "接收数据："<<hex<<buffer[0] << endl;
		cout << dec;
		}
	}
	//关闭串口
	if (!CloseHandle(hCom)) {
		cout << "无法关闭串口" << endl;
	}
	else {
		cout << "关闭串口成功" << endl;
	}
	while (1);
	return 0;
}