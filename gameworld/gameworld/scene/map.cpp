#include "map.h"
#include "config/mapconfig.h"

#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <map>

Map::Map(const ConfigMap &configmap) : m_map_id(configmap.id), m_x(configmap.width), m_y(configmap.height)
{
	assert((int)configmap.mask.length() >= configmap.width * configmap.height);

	m_map_info = (char **)::malloc(sizeof(char*) * m_y);
	::memset(m_map_info, 0, sizeof(char*) * m_y);

	m_extra_map_info = (char **)::malloc(sizeof(char*) * m_y);
	::memset(m_extra_map_info, 0, sizeof(char*) * m_y);

	m_near_boundries_map_info = (char **)::malloc(sizeof(char*) * m_y);
	::memset(m_near_boundries_map_info, 0, sizeof(char*) * m_y);

	for (Axis i = 0; i < m_y; ++i)
	{
		m_map_info[i] = (char *)malloc(sizeof(char) * m_x);
		::memset(m_map_info[i], 0, sizeof(char) * m_x);

		memcpy(m_map_info[i], configmap.mask.c_str() + m_x * i, m_x);

		m_extra_map_info[i] = (char *)malloc(sizeof(char) * m_x);
		::memset(m_extra_map_info[i], 0, sizeof(char) * m_x);

		m_near_boundries_map_info[i] = (char *)malloc(sizeof(char) * m_x);
		::memset(m_near_boundries_map_info[i], 0, sizeof(char) * m_x);
	}

	this->CreateExtraMapInfo();
	this->FiltrateAndSaveWalkablePointSet();
}

void Map::CreateExtraMapInfo()
{
	static const int direction[8] = { 1, 0, -1, 0, 0, 1, 0, -1 };

	std::map<long long, Posi> boundry_zeros;
	for (Axis i = 0; i < m_y; ++i)
	{
		for (Axis j = 0; j < m_x; ++j)
		{
			if ('0' != m_map_info[i][j]) continue;

			// 判断是否边界的'0'
			for (int k = 0; k < 8; k += 2)
			{
				Axis y = i + direction[k];
				Axis x = j + direction[k + 1];
				if (y < 0 || y >= m_y || x < 0 || x >= m_x) continue;

				if ('0' != m_map_info[y][x])
				{
					long long index = TranslateInt64(j, i);
					Posi pos(j, i);
					boundry_zeros.insert(std::map<long long, Posi>::value_type(index, pos));
					break;
				}
			}
		}
	}
	//将边界向四周扩大
	std::map<long long, Posi>::iterator it = boundry_zeros.begin();
	for (; it != boundry_zeros.end(); ++it)
	{
		Posi pos = it->second;

		Axis ul_y = GetMax(pos.y - OFFSET_BOUNDRY, 0);
		Axis ul_x = GetMax(pos.x - OFFSET_BOUNDRY, 0);
		Axis lr_y = GetMin(pos.y + OFFSET_BOUNDRY, m_y - 1);
		Axis lr_x = GetMin(pos.x + OFFSET_BOUNDRY, m_x - 1);

		for (int y = ul_y; y <= lr_y; ++y)
		{
			for (int x = ul_x; x <= lr_x; ++x)
			{
				m_extra_map_info[y][x] = 1;
			}
		}

		ul_y = GetMax(pos.y - NEAR_BOUNDRY_DISTANCE, 0);
		ul_x = GetMax(pos.x - NEAR_BOUNDRY_DISTANCE, 0);
		lr_y = GetMin(pos.y + NEAR_BOUNDRY_DISTANCE, m_y - 1);
		lr_x = GetMin(pos.x + NEAR_BOUNDRY_DISTANCE, m_x - 1);

		for (int y = ul_y; y <= lr_y; ++y)
		{
			for (int x = ul_x; x <= lr_x; ++x)
			{
				m_near_boundries_map_info[y][x] = 1;
			}
		}
	}
}

bool Map::IsPosNearBoundry(Axis x, Axis y)
{
	return m_near_boundries_map_info[y][x] == 1;
}

void Map::PrintExtendBoundry(Axis x, Axis y, bool is_print_boundry)
{
	Axis ul_y = GetMax(y - OFFSET_BOUNDRY, 0);
	Axis ul_x = GetMax(x - OFFSET_BOUNDRY, 0);
	Axis lr_y = GetMin(y + OFFSET_BOUNDRY, m_y - 1);
	Axis lr_x = GetMin(x + OFFSET_BOUNDRY, m_x - 1);
	
	if (is_print_boundry)
	{
		for (int i = ul_y; i <= lr_y; ++i)
		{
			for (int j = ul_x; j <= lr_x; ++j)
			{
				if (i == y && j == x)
				{
					if (m_extra_map_info[i][j] == 1)
					{
						printf("Y");
					}
					else
					{
						printf("X");
					}
				}
				else
				{
					printf("%d", m_extra_map_info[i][j]);
				}
			}
			printf("\n");
		}
	}
	else
	{
		for (int i = ul_y; i <= lr_y; ++i)
		{
			for (int j = ul_x; j <= lr_x; ++j)
			{
				if (i == y && j == x)
				{
					if (m_map_info[i][j] == '1')
					{
						printf("Y");
					}
					else
					{
						printf("X");
					}
				}
				else
				{
					printf("%c", m_map_info[i][j]);
				}
			}
			printf("\n");
		}
	}
	printf("\n");
	
}

void Map::PrintIsNeerBoundry(Axis x, Axis y)
{
	Axis ul_y = GetMax(y - OFFSET_BOUNDRY, 0);
	Axis ul_x = GetMax(x - OFFSET_BOUNDRY, 0);
	Axis lr_y = GetMin(y + OFFSET_BOUNDRY, m_y - 1);
	Axis lr_x = GetMin(x + OFFSET_BOUNDRY, m_x - 1);

	for (int i = ul_y; i <= lr_y; ++i)
	{
		for (int j = ul_x; j <= lr_x; ++j)
		{
			if (i == y && j == x)
			{
				if (this->IsPosNearBoundry(j, i))
				{
					printf("Y");
				}
				else
				{
					printf("X");
				}
			}
			else
			{
				printf("%d", m_near_boundries_map_info[i][j]);
			}
		}
		printf("\n");
	}
}

Map::~Map()
{
	for (Axis i = 0; i < m_y; ++i)

	{
		::free(m_map_info[i]);
		::free(m_extra_map_info[i]);
		::free(m_near_boundries_map_info[i]);
	}

	::free(m_map_info);
	::free(m_extra_map_info);
	::free(m_near_boundries_map_info);
	m_map_info = 0;
}

