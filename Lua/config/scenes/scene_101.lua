Config.scenes = Config.scenes or {}
Config.scenes[101] = 
{
	id = 101,
	name = "落星山",
	mapid = 105,
	scene_type = 0,
	min_level = 0,
	max_level = 0,
	entry_point = 
	{
		x = 136.8167,
		y = 162.7,
		z = 62.5425
	},
	npcs = 
	{
		{
			id = 40000,
			seq = 1,
			show_condition = 0,
			name = "测试NPC1",
			x = 142.6167,
			y = 160.9,
			z = 63.66204,
			landmark = 0,
			dir_y = 91
		},
	
		{
			id = 40001,
			seq = 2,
			show_condition = 0,
			name = "沫璃",
			x = 166.6167,
			y = 155.3,
			z = 64.60743,
			landmark = 0,
			dir_y = 317
		},
	
		{
			id = 40001,
			seq = 3,
			show_condition = 0,
			name = "沫璃",
			x = 157.1537,
			y = 177.1161,
			z = 63.61949,
			landmark = 0,
			dir_y = 202
		},
	
		{
			id = 40002,
			seq = 21,
			show_condition = 0,
			name = "测试NPC3",
			x = 157.0167,
			y = 176.7,
			z = 63.6139,
			landmark = 0,
			dir_y = 0
		},
	
		{
			id = 40001,
			seq = 24,
			show_condition = 0,
			name = "沫璃",
			x = 155.2167,
			y = 140.1,
			z = 64.65474,
			landmark = 0,
			dir_y = 0
		}
	},
	doors = 
	{
		{
			id = 4,
			name = "落星山",
			level = 1,
			to_scene_id = 101,
			type = 0,
			x = 176.6167,
			y = 159.7,
			z = 64.30951
		},
	
		{
			id = 378,
			name = "玄都",
			level = 1,
			to_scene_id = 108,
			type = 0,
			x = 178.0167,
			y = 160.9167,
			z = 64.28889
		}
	},
	monsters = 
	{
	},
	gathers = 
	{
	},
	decorations = 
	{
	}
}