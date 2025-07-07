Config.scenes = Config.scenes or {}
Config.scenes[112] = 
{
	id = 112,
	name = "新场景3",
	mapid = 110,
	scene_type = 4,
	min_level = 0,
	max_level = 0,
	entry_point = 
	{
		x = 46.26667,
		y = 81.46667,
		z = 30.66244
	},
	npcs = 
	{
		{
			id = 50071,
			seq = 14,
			show_condition = 2,
			name = "测试秘境怪物2",
			x = 46.66667,
			y = 81.46667,
			z = 30.66244,
			landmark = 0,
			dir_y = 0
		},
	
		{
			id = 50070,
			seq = 22,
			show_condition = 2,
			name = "测试秘境怪物1",
			x = 46.66667,
			y = 81.46667,
			z = 30.66244,
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
			x = 42.86666,
			y = 81.46667,
			z = 30.66244
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