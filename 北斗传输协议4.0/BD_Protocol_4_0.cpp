#include "BD_Protocol_4_0.h"
RE_BUFFER rebuff;
BDXX bdxx;
UCHR status = 0x80;
UCHR SEND_BLOCKTIME = 0;
UCHR error_flag = 0;
UCHR BSGL = 0;//波束功率0-5位
int(*myprint)(_In_z_ _Printf_format_string_ char const* const, ...);
void init()
{
	//结构体初始化0 TODO
	rebuff.rp = 0;
	rebuff.wp = 0;
	myprint = &printf;
}

void check_status()
{
	static UINT check_status_count = 0;
	if (check_status_count >= 6)
	{
		status &= 0;
		status |= STATUS_BIT_ANSWER;
		check_status_count = 0;
	}

	if (status & STATUS_BIT_ANSWER)//有回复
	{
		switch (status & ~STATUS_BIT_ANSWER)//检查确认位
		{
		case (STEP_NONE | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);//初始化
			break;
		case (STEP_ICJC | STATUS_BIT_CONFIRM):
			status = (STEP_XJZJ | STATUS_BIT_ANSWER);
			break;
		case (STEP_ICJC | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		case (STEP_GNPS | STATUS_BIT_CONFIRM):
			status = (STEP_GNVS | STATUS_BIT_ANSWER);
			break;
		case (STEP_GNPS | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		case (STEP_GNVS | STATUS_BIT_CONFIRM):
			status = (STEP_GNTS | STATUS_BIT_ANSWER);
			break;
		case (STEP_GNVS | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		case (STEP_GNTS | STATUS_BIT_CONFIRM):
			status = (STEP_READY | STATUS_BIT_ANSWER);
			break;
		case (STEP_GNTS | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		case (STEP_XJZJ | STATUS_BIT_CONFIRM):
			status = (STEP_GNPS | STATUS_BIT_ANSWER);
			break;
		case (STEP_XJZJ | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		case (STEP_SJSC | STATUS_BIT_CONFIRM):
			status = (STEP_READY | STATUS_BIT_ANSWER);//就绪
			break;
		case (STEP_SJSC | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		default:
			char aaa[255];
			for (int i = 0; i < 10; ++i)
			{
				aaa[i] = i + 0x30;
			}
			bd_send(aaa, (UINT)(10), bdxx.icxx.yhdz);
			break;
		}
	}
	if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_ICJC | STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		icjc_send();
	}
	else if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_XJZJ | STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		xtzj_send();
	}
	else if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_SJSC | STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		sjsc_send();
	}
	else if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_GNPS | STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		gnps_send();
	}
	else if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_GNVS | STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		gnvs_send();
	}
	else if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_GNTS | STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		gnts_send();
	}
	else if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_READY | STATUS_BIT_ANSWER))
	{

	}
	else
	{
		++check_status_count;
	}
}


void icjc_send()
{
	UCHR sendbuffer[13] = "$ICJC0?0000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = 12;
	sendbuffer[7] = 0;
	sendbuffer[8] = 0;
	sendbuffer[9] = 0;
	sendbuffer[10] = 0;
	sendbuffer[11] = xor_checksum2(sendbuffer, 11);
	mySerialPort.WriteData(sendbuffer, 12);
}

void gnps_send()
{
	UCHR sendbuffer[14] = "$GNPS0?0000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = 0X0D;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = (UINT)BD_PRINT_GNPX_SEQ >> 8;
	sendbuffer[11] = (UINT)BD_PRINT_GNPX_SEQ & 0xff;
	sendbuffer[12] = xor_checksum2(sendbuffer, 12);
	mySerialPort.WriteData(sendbuffer, 13);
}

void gnvs_send()
{
	UCHR sendbuffer[14] = "$GNVS0?0000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = 0X0D;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = (UINT)BD_PRINT_GNVX_SEQ >> 8;
	sendbuffer[11] = (UINT)BD_PRINT_GNVX_SEQ & 0xff;
	sendbuffer[12] = xor_checksum2(sendbuffer, 12);
	mySerialPort.WriteData(sendbuffer, 13);
}

void gnts_send()
{
	UCHR sendbuffer[15] = "$GNTS0?0000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = 0X0E;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = BD_PRINT_GNTX_ZONE;
	sendbuffer[11] = (UINT)BD_PRINT_GNTX_SEQ >> 8;
	sendbuffer[12] = (UINT)BD_PRINT_GNTX_SEQ & 0xff;
	sendbuffer[13] = xor_checksum2(sendbuffer, 13);
	mySerialPort.WriteData(sendbuffer, 14);
}

void xtzj_send()
{
	UCHR sendbuffer[14] = "$XTZJ0?00000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = bdxx.xtzj.lenth;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = (UINT)BD_PRINT_TIME_SEQ >> 8;
	sendbuffer[11] = (UINT)BD_PRINT_TIME_SEQ & 0xff;
	sendbuffer[12] = xor_checksum2(sendbuffer, 12);
	mySerialPort.WriteData(sendbuffer, 13);
}

void sjsc_send()
{
	UCHR sendbuffer[14] = "$SJSC0?00000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = 13;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = (UINT)BD_PRINT_TIME_SEQ >> 8;
	sendbuffer[11] = (UINT)BD_PRINT_TIME_SEQ & 0xff;
	sendbuffer[12] = xor_checksum2(sendbuffer, 12);
	mySerialPort.WriteData(sendbuffer, 13);
}


char* data_encapsulation(char *send_buffer, const char *data, const UINT length_data)
{
	UINT i, length = 2 + length_data, crc = 0;
	for (i = 0; i < length_data; ++i)
		crc += data[i];
	crc += (length >> 8);
	crc += (length & 0xff);
	send_buffer[0] = 0x11;
	send_buffer[1] = (UCHR)(length >> 8);
	send_buffer[2] = (UCHR)(length & 0xff);
	send_buffer[length_data + DATA_FIRM_LENTH - 3] = (crc >> 8) & 0x0ff;
	send_buffer[length_data + DATA_FIRM_LENTH - 2] = crc & 0x0ff;
	send_buffer[length_data + DATA_FIRM_LENTH - 1] = 0x16;
	for (i = 0; i < length_data; ++i)
		send_buffer[3 + i] = data[i];
	return send_buffer;
}

void bd_send(const char *buffer, UINT len, UCHR *dis)
{
	if (BSGL && !error_flag && !SEND_BLOCKTIME&&len > 0)
	{
		char *send_buffer;
		send_buffer = (char *)malloc(sizeof(char)*(len + DATA_FIRM_LENTH));
		txsq_send(data_encapsulation(send_buffer, buffer, len), len + DATA_FIRM_LENTH, dis);
		//txsq_send(buffer, len, dis);
		if (send_buffer != NULL)
			free(send_buffer);
	}
	else
	{
		if (!BSGL)
			myprint("无波束  ");
		if (error_flag)
			myprint("硬件错误  ");
		if (SEND_BLOCKTIME)
			myprint("频度未到:%d秒  ", SEND_BLOCKTIME);
		if (!len)
			myprint("发送长度为0  ");
		myprint("\n");
	}
}

void txsq_send(const char *buffer, UINT len, UCHR *dis)
{
	UINT lenth = bdxx.txsq.lenth + len, i = 0;;
	UCHR sendbuffer[1024] = "$TXSQ0?00000?";//最多只能发送210字节
	sendbuffer[5] = lenth >> 8;
	sendbuffer[6] = lenth & 0xff;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = 0B01000110;
	sendbuffer[11] = dis[0];
	sendbuffer[12] = dis[1];
	sendbuffer[13] = dis[2];
	sendbuffer[14] = (len * 8) >> 8;
	sendbuffer[15] = (len * 8) & 0xff;
	sendbuffer[16] = 0;
	for (; i < len; ++i)
		sendbuffer[17 + i] = *(buffer + i);
	sendbuffer[lenth - 1] = xor_checksum2(sendbuffer, lenth - 1);
	mySerialPort.WriteData(sendbuffer, lenth);
}

void dwsq_send()
{
	//TODO
	//UINT lenth = bdxx.txsq.lenth + len, i = 0;;
	//UCHR sendbuffer[255] = "$TXSQ0?00000?";//最多只能发送210字节
	//sendbuffer[5] = lenth >> 8;
	//sendbuffer[6] = lenth & 0xff;
	//sendbuffer[7] = bdxx.icxx.yhdz[0];
	//sendbuffer[8] = bdxx.icxx.yhdz[1];
	//sendbuffer[9] = bdxx.icxx.yhdz[2];
	//sendbuffer[10] = 0B01000110;
	//sendbuffer[11] = dis[0];
	//sendbuffer[12] = dis[1];
	//sendbuffer[13] = dis[2];
	//sendbuffer[14] = 0;
	//sendbuffer[15] = (len * 8) >> 8;
	//sendbuffer[16] = (len * 8) & 0xff;
	//for (; i < len; ++i)
	//	sendbuffer[17 + i] = *(buffer + i);
	//sendbuffer[lenth - 1] = xor_checksum2(sendbuffer, lenth - 1);
	//mySerialPort.WriteData(sendbuffer, lenth);
}
/*
接收用
异或校验和算法
*/
UCHR xor_checksum(UCHR *buf, UINT location, UINT len)
{
	UINT i, ii;
	UCHR checksum = 0;

	for (i = 0; i < len; ++i)
	{
		ii = (location + i) & RE_BUFFER_SIZE;
		checksum ^= *(buf + ii);
	}

	return checksum;
}

/*
发送用
异或校验和算法
*/
UCHR xor_checksum2(UCHR *buf, UINT len)
{
	UINT i;
	UCHR checksum = 0;

	for (i = 0; i < len; ++i)
	{
		checksum ^= *(buf + i);
	}

	return checksum;
}


void Handle_ZJXX()
{
	if (bdxx.zjxx.iczt || bdxx.zjxx.yjzt || bdxx.zjxx.rzzt != 0x02)
		error_flag = 1;
	for (int i = 0; i < 6; ++i)
	{
		if (bdxx.zjxx.bsgl[i])
		{
			BSGL |= (1 << i);
		}
		else
		{
			BSGL &= ~(1 << i);
		}
	}

}

void Handle_FXXX()
{
	if (bdxx.fkxx.flbz == 4)//发射频度未到
	{
		SEND_BLOCKTIME = bdxx.fkxx.fjxx[3];
	}
}


UINT UCHRtoUINT(UCHR a, UCHR b)
{
	return (UINT)a * 256 + (UINT)b;
}

/*
对于有符号参数，第 1 位符号位统一规定为“0”表示“+”，
“1”表示“-”，其后位数为参数值，用原码表示。
*/
int UCHRtoINT(UCHR a, UCHR b)
{
	int result=0,flag=0;
	if (a & 0x80)
	{
		flag = -1;
	}
	else
	{
		flag = 1;
	}
	for (int i = 0; i < 7; ++i)
	{
		if (a&(1 << i))
			result |= (1 << (i + 8));
	}
	for (int i = 0; i < 8; ++i)
	{
		if (b&(1 << i))
			result |= (1 << i);
	}
	return result* flag;
}

bool check_overflow(RE_BUFFER *buff, UINT value)
{
	if (buff->rp <= buff->wp)//说明写指针未回溯
	{
		if (buff->rp + value <= buff->wp)
			return true;
		else
			return false;
	}
	else
	{
		if (buff->rp + value <= (buff->wp + RE_BUFFER_SIZE))
			return true;
		else
			return false;
	}
}

void Analysis_data(const UCHR *fxfdz, const UCHR h, const UCHR m, const UCHR *buffer, const UINT package_length)
{
	UINT crc = 0, crc1 = 0;
	UINT length, length1;

	if (buffer[package_length - 1] != 0x16)
	{
		return;
	}
	else if (buffer[0] == 0x11)
	{
		length1 = length = ((UINT)buffer[1] << 8) + (UINT)buffer[2];
		crc = ((((UINT)buffer[length + 1]) << 8) & 0xff00) + (buffer[length + 2] & 0x00ff);
		if (length1 < package_length)
		{
			for (; length1 > 0; length1--)
			{
				crc1 += buffer[length1];
			}
			if (crc == crc1)
			{

				UCHR *exdata = (UCHR *)malloc(sizeof(UCHR)*(length - 2));
				memcpy(exdata, buffer + 3, length - 2);
				DATA_Handler(fxfdz, h, m, exdata, length - 2);
				free(exdata);
			}

		}
	}
}

void DATA_Handler(const UCHR *fxfdz, const UCHR h, const UCHR m, const UCHR *data, const UINT lenth)
{
	//TODO
	for (UINT i = 0; i < lenth; ++i)
		printf("%c", *data + i);
	printf("\n");
}

