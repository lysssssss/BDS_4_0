#pragma once
#ifndef __BD_PROTOCOL_4_0_H
#define __BD_PROTOCOL_4_0_H
#include "malloc.h"
#include <string.h>
#include "SerialPort.h"  

const int RE_BUFFER_SIZE = 2047;
typedef unsigned int  UINT;
typedef unsigned char  UCHR; 
extern int(*myprint)(_In_z_ _Printf_format_string_ char const* const, ...);
#define GNTX_LENTH 0x12
#define GNPX_LENTH 0x20
//#define GNVX_LENTH 
struct RE_BUFFER
{
	UINT wp;
	UINT rp;
	UCHR buffer[RE_BUFFER_SIZE];
};
extern RE_BUFFER rebuff;


struct BD_DWSQ  //定位申请
{
	const static UINT lenth = 22;
	UCHR xxlb;//信息类别
	UINT gcsstxg;//高程数据和天线高
	UINT qyss;//气压数据
	UINT rzpd;//入站频度
};

struct BD_TXSQ //通信申请
{
	const static UINT lenth = 18;//不包含电文内容
	UCHR xxlb;//信息类别
	UCHR yhdz[3];//用户地址
	UINT dwcd;//电文长度
	UCHR sfyd;//是否应答
					   //UCHR dwnr[1680];//电文内容
};

struct BD_CKSC //串口输出
{
	const static UINT lenth = 12;
	UCHR cssl;//传输速率
};

struct BD_ICJC //ic检测
{
	const UINT lenth = 12;
	UCHR zh;//帧号
};

struct BD_XTZJ //系统自检
{
	const UINT lenth = 13;
	UINT zjpd;//自检频度
};

struct BD_SJSC //时间输出
{
	const UINT lenth = 13;
	UINT scpd;//输出频度
};

struct BD_BBDQ
{
	const UINT lenth = 11;
};
#define DWXX_LENTH (11+1+3+4+4+4+2+2)
struct BD_DWXX
{
	UCHR xxlb;//信息类别
	UCHR cxdz[3];//查询地址
	UINT T;//定位时刻
	UINT L;//用户位置的大地经度数据
	unsigned  int B;//用户位置的大地纬度数据
	UINT H;//用户位置的大地高程数据
	UINT S;//用户位置的高程异常值
};


struct BD_TXXX
{
	UCHR xxlb;//信息类别
	UCHR fxfdz[3];//发信方地址
	UCHR fxsj_h;//发信时间（小时）
	UCHR fxsj_m;//发信时间（分钟）
	UINT  dwcd;//电文长度
	UCHR dwnr[210];//电文内容
	UCHR crc;//CRC标志
};

#define ICXX_LENTH (11+1+3+1+2+1+1+2)
struct BD_ICXX
{
	UCHR yhdz[3];//用户地址
	UCHR zh;//帧号
	UINT tbid;//通播ID
	UCHR yhtz;//用户特征
	UINT fwpd;//服务频度
	UCHR txdj;//通信等级
	UCHR jmbz;//加密标志
	UINT xsyhzs;//下属用户总数
};

#define ZJXX_LENTH (11+1+1+1+1+6)
struct BD_ZJXX //自检信息
{
	UCHR iczt;//IC卡状态
	UCHR yjzt;//硬件状态
	UCHR dcdl;//电池电量
	UCHR rzzt;//入站状态
	UCHR bsgl[6];//波束功率1-6
};

#define SJXX_LENTH (11+2+1+1+1+1+1)
struct BD_SJXX //时间信息
{
	UINT year;
	UCHR month;
	UCHR day;
	UCHR hour;
	UCHR minute;
	UCHR second;
};

struct BD_BBXX // 版本信息
{

};

#define FKXX_LENTH (11+1+4)

struct BD_FKXX //反馈信息
{
	UCHR flbz;//反馈标志
	UCHR fjxx[4];//附加信息
};

struct BDXX
{
	struct BD_BBDQ bbdq;
	struct BD_BBXX bbxx;
	struct BD_CKSC cksc;
	struct BD_DWSQ dwsq;
	struct BD_DWXX dwxx;
	struct BD_FKXX fkxx;
	struct BD_ICJC icjc;
	struct BD_ICXX icxx;
	struct BD_SJSC sjsc ;
	struct BD_SJXX sjxx;
	struct BD_TXSQ txsq;
	struct BD_TXXX txxx;
	struct BD_XTZJ xtzj;
	struct BD_ZJXX zjxx;
};

void init();
void check_status();
void icjc_send();
void xyzj_send();
void sjsc_send();
void dwsq_send();
void txsq_send(const char *buffer, UINT len, UCHR *dis);
UCHR xor_checksum(UCHR *buf, UINT location, UINT len);
UCHR xor_checksum2(UCHR *buf, UINT len);
void Extract_DWXX(UCHR *buf, UINT i);
void print_dwxx();
void Extract_TXXX(UCHR *buf, UINT i);
void print_txxx();
void Extract_ICXX(UCHR *buf, UINT i);
void print_icxx();
void Extract_ZJXX(UCHR *buf, UINT i);
void print_zjxx();
void Extract_SJXX(UCHR *buf, UINT i);
void print_sjxx();
void Extract_FKXX(UCHR *buf, UINT i);
void print_fkxx();
void Receive_Protocol();
bool check_overflow(RE_BUFFER *buff, UINT value);
void Analysis_data(const UCHR *fxfdz, const UCHR h, const UCHR m, const UCHR *buffer, const UINT package_length);
void DATA_Handler(const UCHR *fxfdz, const UCHR h, const UCHR m, const UCHR *data, const UINT lenth);

#endif


