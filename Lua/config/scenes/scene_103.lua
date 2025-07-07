Config.scenes = Config.scenes or {}
Config.scenes[103] = 
{
	id = 103,
	name = "测试野外",
	mapid = 104,
	scene_type = 0,
	min_level = 0,
	max_level = 0,
	entry_point = 
	{
		x = 150.6,
		y = 151.93,
		z = 104.1509
	},
	npcs = 
	{
		{
			id = 40000,
			seq = 1,
			show_condition = 0,
			name = "测试NPC1",
			x = 152.4,
			y = 152.93,
			z = 104.0338,
			landmark = 0,
			dir_y = 0
		}
	},
	doors = 
	{
		{
			id = 7,
			name = "测试主城",
			level = 1,
			to_scene_id = 104,
			type = 0,
			x = 194.2638,
			y = 168.0029,
			z = 108.1857
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