#ifndef __MAP_H__
#define __MAP_H__

#include "config/mapconfig.h"
#include "obj/obj.h"

#include <algorithm>
#include <deque>

class Map
{
public:
	static const Axis OFFSET_BOUNDRY = 10;
	static const Axis NEAR_BOUNDRY_DISTANCE = 15;

public:
	Map(const ConfigMap &configmap);
	~Map();

	/*
	0 �赲
	1 ����
	2 ��ȫ��
	3 �һ���
	4 ��̯��
	*/

	/*
	* PS: ��Ҫ�� 
	*     m_map_info[y][x]
	*	  y��ǰ��x�ں�
	*/

	/**
	* \brief ���ڽ�ɫÿһ֡���ƶ����
	* \param obj_type �������� \see Obj::OBJ_TYPE
	* \param x X������
	* \param y Y������
	* \note ���ڿͻ����������ڵ�ͼ����������ı߽���ƶ��������Ե��ж�Ϊ����������ʱ���鿴�Ƿ��ڱ߽磬
	*		����ǣ�������ͨ��
	* \return �Ƿ�ͨ�����
	*/
	bool CheckValidate(int obj_type, Axis x, Axis y)
	{
		return this->Validate(obj_type, x, y) || this->BoundryValidate(obj_type, x, y);
	}

	bool Validate(int obj_type, Axis x, Axis y)
	{
		// ���ж���״̬���ж���Ԫ������
		return x >= 0 && x < m_x && y >= 0 && y < m_y && ((obj_type != Obj::OBJ_TYPE_ROLE) || m_map_info[y][x] > '0');
	}

	/**
	* \brief ���Ŀ��λ���Ƿ��ڲ���������ı߽����Χ
	* \param obj_type �������� \see Obj::OBJ_TYPE
	* \param x X������
	* \param y Y������
	* \return �Ƿ�ͨ�����
	*/
	bool BoundryValidate(int obj_type, Axis x, Axis y)
	{
		return x >= 0 && x < m_x && y >= 0 && y < m_y && ((obj_type != Obj::OBJ_TYPE_ROLE) || m_extra_map_info[y][x] > 0);
	}

	bool IsInsideMap(Axis x, Axis y)
	{
		return x >= 0 && x < m_x && y >= 0 && y < m_y;
	}

	bool IsInsideMap(float x, float y)
	{
		return x >= 0 && x < m_x && y >= 0 && y < m_y;
	}

	char GetMapGrid(Axis x, Axis y)
	{
		if (this->IsInsideMap(x, y))
		{
			return m_map_info[y][x];
		}
		else
		{
			return 'x'; // ��ʾ���߽���
		}
	}

	char GetMapBoundryInfo(Axis x, Axis y)
	{
		if (this->IsInsideMap(x, y))
		{
			return m_extra_map_info[y][x];
		}
		else
		{
			return 'x'; // ��ʾ���߽���
		}
	}

	void GetArea(Axis *ru_x, Axis *ru_y)
	{
		*ru_x = m_x;
		*ru_y = m_y;
	}

	void GetArea(Posi *ru)
	{
		ru->x = m_x;
		ru->y = m_y;
	}

	// ��ȫ��
	bool SaveArea(Axis x, Axis y)
	{
		if (x < 0 || x >= m_x || y < 0 || y >= m_y) return false;

		return m_map_info[y][x] > '1';	// �һ�������̯��Ҳ�ǰ�ȫ�������Դ˴��� > '1'
		//return false;	// m_map_info[y][x] ��תx y
	}

	// �һ���
	bool HookArea(Axis x, Axis y)
	{
		if (x < 0 || x >= m_x || y < 0 || y >= m_y) return false;

		return m_map_info[y][x] == '3';
		//return true;	// m_map_info[y][x] ��תx y
	}

	// ��̯��
	bool StallArea(Axis x, Axis y)
	{
		if (x < 0 || x >= m_x || y < 0 || y >= m_y) return false;

		return m_map_info[y][x] == '4';
		//return true;	// m_map_info[y][x] ��תx y
	}

	// ���˲���������ߵ㼯
	void FiltrateAndSaveWalkablePointSet()
	{
		int x_start = rand() % (MAP_WALKABLE_POSI_SPACE + 1) + MAP_WALKABLE_POSI_RANGE;
		int y_start = rand() % (MAP_WALKABLE_POSI_SPACE + 1) + MAP_WALKABLE_POSI_RANGE;
		for (int p_x = x_start; p_x < (m_x - MAP_WALKABLE_POSI_RANGE);)
		{
			for (int p_y = y_start; p_y < (m_y - MAP_WALKABLE_POSI_RANGE);)
			{
				if ('1' == m_map_info[p_y][p_x] && !this->IsPosNearBoundry(p_x, p_y))
				{
					m_map_walkable_point.push_back(Posi(p_x, p_y));
				}

				p_y += (rand() % (MAP_WALKABLE_POSI_SPACE + 1) + MAP_WALKABLE_POSI_SPACE);
			}

			p_x += (rand() % (MAP_WALKABLE_POSI_SPACE + 1) + MAP_WALKABLE_POSI_SPACE);
		}

		std::random_shuffle(m_map_walkable_point.begin(), m_map_walkable_point.end());	// ϴ�ƣ����ҵ���
	}

	// �����ȡһ�������ߵ�
	const Posi GetARandWalkablePoint()
	{
		if (m_map_walkable_point.empty()) this->FiltrateAndSaveWalkablePointSet();	// ����㼯�����ˣ����������һ���㼯
		if (m_map_walkable_point.empty()) this->FiltrateAndSaveWalkablePointSet();
		if (m_map_walkable_point.empty()) this->FiltrateAndSaveWalkablePointSet();

		if (!m_map_walkable_point.empty())
		{
			Posi ret = m_map_walkable_point.front();	// ȡ��һ������
			m_map_walkable_point.pop_front();			// ɾ����һ��Ԫ��

			return ret;
		}
		else
		{
#ifdef _DEBUG
			assert(0);	
#endif
			return Posi(0, 0);
		}	
	}

	bool IsPosNearBoundry(Axis x, Axis y);

	void PrintExtendBoundry(Axis x, Axis y, bool is_print_boundry);
	void PrintIsNeerBoundry(Axis x, Axis y);

	void GmPrintfMapWalkableList()
	{
		printf(" | MapID: %d | MaxSize: (%d, %d)\n    ListSize: %d", m_map_id, m_x, m_y, (int)m_map_walkable_point.size());
		if (m_map_walkable_point.empty())
		{
			printf("\n\tAttention! Null Walkable List. Return.\n");
			return;
		}
		printf("\n");
		int size = -1;
		for (std::deque<Posi>::iterator loop = m_map_walkable_point.begin(); loop != m_map_walkable_point.end(); ++loop)
		{
			if (size >= 3)
			{
				printf("\n");
				size = 0;
			}
			else
			{
				++size;
			}
			printf("\t(%d, %d)[%d]", loop->x, loop->y, m_map_info[loop->y][loop->x] - '0');
		}
		printf("\n");
	}

protected:
	Map(const Map &_m);
	Map &operator=(const Map &_m);
private:

		//			    		(0,m_y)			(m_x,m_y)
		//						    -------------
		//							|			|
		//							|			|
		//                          |Y			|
		//        					|			|
		//       					|	  X		|
		//                          -------------
		//						(0,0)			(m_x,0)

	static const int MAP_WALKABLE_POSI_RANGE = 15;			// �����ߵ㼯ɸѡ - ��ͼ�߽��ų�����
	static const int MAP_WALKABLE_POSI_SPACE = 15;			// �����ߵ㼯ɸѡ - �Ϸ������С���

	/**
	* \brief ��������ĵ�ͼ��Ϣ
	* \note ��ʱ������һ�ݶԵ�ͼ�߽������������Ϣ
	*/
	void CreateExtraMapInfo();

	int m_map_id;
	Axis m_x;
	Axis m_y;

	char **m_map_info;
	std::deque<Posi> m_map_walkable_point;

	char **m_extra_map_info;
	char **m_near_boundries_map_info;	// ��¼��ͼ������Щ���ǽӽ��߽��
};

#endif