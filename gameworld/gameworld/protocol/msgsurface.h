#ifndef __MSG_SURFACE_H__
#define __MSG_SURFACE_H__

#include "servercommon/userprotocal/msgheader.h"
#include "other/surface/surfaceconfig.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	
	struct SurfaceData
	{
		SurfaceData() { this->Reset(); }
		void Reset(){}

		int id;
		int level;
		int end_time;
	};

	class SCSurfaceAllInfo					//!< 1980	�û�ȫ����Ϣ
	{
	public:
		SCSurfaceAllInfo();
		MessageHeader		header;

		short wear_surface[SURFACE_TYPE_MAX];			//!< ��ǰ����λʹ�õĻû�id �����±�Ϊ���ñ��type
		int surface_count;
		SurfaceData surface_list[SURFACE_MAX_NUM];
	};

	class SCSurfaceWearInfo					//!< 1983	�û�������Ϣ
	{
	public:
		SCSurfaceWearInfo();
		MessageHeader		header;

		short wear_surface[SURFACE_TYPE_MAX];			//!< ��ǰ����λʹ�õĻû�id �����±�Ϊ���ñ��type
	};

	class SCSurfaceSingleInfo					//!< 1982	�û�������Ϣ
	{
	public:
		SCSurfaceSingleInfo();
		MessageHeader		header;

		enum SURFACE_REASON_TYPE
		{
			SURFACE_REASON_TYPE_DEFAULT = 0,				//Ĭ�� ����ʾ
			SURFACE_REASON_TYPE_ACTIVATE_NOTICE = 1,		//����ʱװ���� ��ʾ	
			SURFACE_REASON_TYPE_END_TIME_NOTICE = 2,		//���� ��ʾ	
		};

		int reason;					
		SurfaceData surface;
	};

	class CSSurfaceOp						//!< 1981	�û�����
	{
	public:
		CSSurfaceOp();
		MessageHeader		header;
		enum OP_TYPE
		{
			WEAR = 1,			//!< param1 = �û�id  Ϊ0ʱ����װ��   param2 ΪҪ�ѵĲ�λ
			EXCHANGE ,			//!< param1 = Ҫ����seq
		};

		int op_type;
		int param1;
		int param2;
	};
	class CSSurefaceDecompose			//!< 1984 ʱװ�ֽ�(ͨ����Ʒ�ֽ�)(֮ǰ���߼�����ȥ������,�����������ϵͳʹ��, fashioncollection.hpp)
	{
	public:
		CSSurefaceDecompose();
		MessageHeader		header;

		short column;		//��Ʒ���ڵı�����
		short index;		//��Ʒ���ڵĸ���
		int num;			//�ֽ������
	};

}

#pragma pack(pop)

#endif

