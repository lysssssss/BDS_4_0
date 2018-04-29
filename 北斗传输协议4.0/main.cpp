// main.cpp : Defines the entry point for the console application.  
//  

#include "SerialPort.h"  
#include  <iostream>

int main()
{

	CSerialPort mySerialPort;

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

	int temp;
	std::cin >> temp;

	return 0;
}
