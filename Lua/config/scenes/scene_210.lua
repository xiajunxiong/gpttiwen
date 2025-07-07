Config.scenes = Config.scenes or {}
Config.scenes[210] = 
{
	id = 210,
	name = "弈剑轩",
	mapid = 205,
	scene_type = 0,
	min_level = 0,
	max_level = 0,
	entry_point = 
	{
		x = 68.56,
		y = 167.13,
		z = 1.901361
	},
	npcs = 
	{
		{
			id = 46758,
			seq = 791,
			show_condition = 0,
			name = "先生",
			x = 89.58,
			y = 184.61,
			z = 4.879659,
			landmark = 0,
			dir_y = 180
		},
	
		{
			id = 46766,
			seq = 792,
			show_condition = 0,
			name = "门生",
			x = 88.16,
			y = 179.15,
			z = 4.879659,
			landmark = 0,
			dir_y = 0
		},
	
		{
			id = 46773,
			seq = 793,
			show_condition = 0,
			name = "同门",
			x = 90.32,
			y = 179.31,
			z = 4.879659,
			landmark = 0,
			dir_y = 0
		},
	
		{
			id = 48003,
			seq = 6003,
			show_condition = 1,
			name = "剑客大弟子",
			x = 87.39667,
			y = 174.82,
			z = 4.879659,
			landmark = 0,
			dir_y = 271
		},
	
		{
			id = 40528,
			seq = 10528,
			show_condition = 0,
			name = "剑客导师",
			x = 96.59666,
			y = 167.02,
			z = 6.004659,
			landmark = 0,
			dir_y = 270
		},
	
		{
			id = 40529,
			seq = 10529,
			show_condition = 0,
			name = "剑客导师助手",
			x = 87.39667,
			y = 158.62,
			z = 4.879659,
			landmark = 0,
			dir_y = 281
		}
	},
	doors = 
	{
		{
			id = 174,
			name = "玄都",
			level = 1,
			to_scene_id = 108,
			type = 2,
			x = 62.47,
			y = 167.04,
			z = 1.88597
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