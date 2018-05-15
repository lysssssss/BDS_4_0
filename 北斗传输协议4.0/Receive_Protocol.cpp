/***********************Project Version1.0*************************
@项目名:北斗传输4.0(C++)

@File_name:BD_Protocol_4_0.cpp

@File_UpdateTime:2018年5月16日02:39:46

@File_Version:1.0a

@说明:处理接收到的数据

本程序基于C++的北斗短报文传输程序
在VS2017里运行通过

Viual Studio 2017 Version:V141
Windows SDK Version:10.0.16299.0
******************************************************************/
#include "BD_Protocol_4_0.h"

void Receive_Protocol()
{
	UINT i = rebuff.rp;
	static UINT error_count = 0;
	if (rebuff.rp != rebuff.wp)
	{
		if (error_count >= 1024)
		{
			//TODO
			myprint("===================错误===================错误===================错误===================错误\n");
			rebuff.rp = rebuff.wp = 0;
			error_count = 0;
			return;
		}
		if (check_overflow(&rebuff, 7))//检查是否符合读取条件，7为读取到长度的字节数。&XXXXMM
		{
			if (rebuff.buffer[i] == '$')
			{

				if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'G'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'N'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'T'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//GNTX
				{
					UINT _lenth = 0;
#ifdef GNTX_LENTH
					_lenth = GNTX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{

						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_GNTX(rebuff.buffer, i);
						}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'G'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'N'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'P'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//GNPX
				{
					UINT _lenth = 0;
#ifdef GNPX_LENTH
					_lenth = GNPX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{

						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_GNPX(rebuff.buffer, i);
						}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'G'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'N'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'V'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//GNVX
				{
					UINT _lenth = 0;
#ifdef GNVX_LENTH
					_lenth = GNVX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{

						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_GNVX(rebuff.buffer, i);
						}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'D'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'W'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//定位信息
				{
					UINT _lenth = 0;
#ifdef DWXX_LENTH
					_lenth = DWXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_DWXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}

				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'T'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//通信信息
				{
					UINT _lenth = 0;
#ifdef TXXX_LENTH
					_lenth = TXXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_TXXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'I'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'C'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//IC信息
				{
					UINT _lenth = 0;
#ifdef ICXX_LENTH
					_lenth = ICXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_ICXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'Z'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'J'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//自检信息
				{
					UINT _lenth = 0;
#ifdef ZJXX_LENTH
					_lenth = ZJXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_ZJXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'S'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'J'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//时间信息
				{
					UINT _lenth = 0;
#ifdef SJXX_LENTH
					_lenth = SJXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_SJXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'B'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'B'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//版本信息
				{

				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'F'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'K'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//反馈信息
				{
					UINT _lenth = 0;
#ifdef FKXX_LENTH
					_lenth = FKXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_FKXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
						error_count = 0;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else
				{
					//不可解析
					++error_count;
					rebuff.rp = (rebuff.rp + 1) & RE_BUFFER_SIZE;
				}
			}
			else
			{
				++error_count;
				rebuff.rp = (rebuff.rp + 1) & RE_BUFFER_SIZE;
			}
		}
		else
		{
			++error_count;
		}
	}
}