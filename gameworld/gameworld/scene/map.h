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
	0 阻挡
	1 可走
	2 安全区
	3 挂机区
	4 摆摊区
	*/

	/*
	* PS: 重要！ 
	*     m_map_info[y][x]
	*	  y在前，x在后
	*/

	/**
	* \brief 用于角色每一帧的移动检查
	* \param obj_type 对象类型 \see Obj::OBJ_TYPE
	* \param x X轴坐标
	* \param y Y轴坐标
	* \note 由于客户端与服务端在地图不可走区域的边界的移动有误差，所以当判断为不可走区域时，查看是否在边界，
	*		如果是，则允许通过
	* \return 是否通过检查
	*/
	bool CheckValidate(int obj_type, Axis x, Axis y)
	{
		return this->Validate(obj_type, x, y) || this->BoundryValidate(obj_type, x, y);
	}

	bool Validate(int obj_type, Axis x, Axis y)
	{
		// 如有多种状态则集中定义元素意义
		return x >= 0 && x < m_x && y >= 0 && y < m_y && ((obj_type != Obj::OBJ_TYPE_ROLE) || m_map_info[y][x] > '0');
	}

	/**
	* \brief 检查目标位置是否在不可走区域的边界的周围
	* \param obj_type 对象类型 \see Obj::OBJ_TYPE
	* \param x X轴坐标
	* \param y Y轴坐标
	* \return 是否通过检查
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
			return 'x'; // 表示出边界了
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
			return 'x'; // 表示出边界了
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

	// 安全区
	bool SaveArea(Axis x, Axis y)
	{
		if (x < 0 || x >= m_x || y < 0 || y >= m_y) return false;

		return m_map_info[y][x] > '1';	// 挂机区、摆摊区也是安全区，所以此处是 > '1'
		//return false;	// m_map_info[y][x] 调转x y
	}

	// 挂机区
	bool HookArea(Axis x, Axis y)
	{
		if (x < 0 || x >= m_x || y < 0 || y >= m_y) return false;

		return m_map_info[y][x] == '3';
		//return true;	// m_map_info[y][x] 调转x y
	}

	// 摆摊区
	bool StallArea(Axis x, Axis y)
	{
		if (x < 0 || x >= m_x || y < 0 || y >= m_y) return false;

		return m_map_info[y][x] == '4';
		//return true;	// m_map_info[y][x] 调转x y
	}

	// 过滤并储存可行走点集
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

		std::random_shuffle(m_map_walkable_point.begin(), m_map_walkable_point.end());	// 洗牌，打乱点序
	}

	// 随机获取一个可行走点
	const Posi GetARandWalkablePoint()
	{
		if (m_map_walkable_point.empty()) this->FiltrateAndSaveWalkablePointSet();	// 如果点集用完了，就重新随机一个点集
		if (m_map_walkable_point.empty()) this->FiltrateAndSaveWalkablePointSet();
		if (m_map_walkable_point.empty()) this->FiltrateAndSaveWalkablePointSet();

		if (!m_map_walkable_point.empty())
		{
			Posi ret = m_map_walkable_point.front();	// 取出一个拷贝
			m_map_walkable_point.pop_front();			// 删掉第一个元素

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

	static const int MAP_WALKABLE_POSI_RANGE = 15;			// 可行走点集筛选 - 地图边界排除距离
	static const int MAP_WALKABLE_POSI_SPACE = 15;			// 可行走点集筛选 - 合法点间最小间距

	/**
	* \brief 创建额外的地图信息
	* \note 暂时仅生成一份对地图边界做扩大处理的信息
	*/
	void CreateExtraMapInfo();

	int m_map_id;
	Axis m_x;
	Axis m_y;

	char **m_map_info;
	std::deque<Posi> m_map_walkable_point;

	char **m_extra_map_info;
	char **m_near_boundries_map_info;	// 记录地图上有哪些点是接近边界的
};

#endif