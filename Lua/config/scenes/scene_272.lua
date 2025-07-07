Config.scenes = Config.scenes or {}
Config.scenes[272] = 
{
	id = 272,
	name = "乌桓氏族村长",
	mapid = 199,
	scene_type = 0,
	min_level = 0,
	max_level = 0,
	entry_point = 
	{
		x = 76,
		y = 86.26666,
		z = 53.99464
	},
	npcs = 
	{
		{
			id = 46665,
			seq = 702,
			show_condition = 0,
			name = "村长",
			x = 65.8,
			y = 60.86667,
			z = 53.78742,
			landmark = 0,
			dir_y = 11
		},
	
		{
			id = 46674,
			seq = 703,
			show_condition = 0,
			name = "拜访者",
			x = 63.6,
			y = 60.86667,
			z = 53.78742,
			landmark = 0,
			dir_y = 9
		}
	},
	doors = 
	{
		{
			id = 224,
			name = "乌桓氏族",
			level = 1,
			to_scene_id = 111,
			type = 2,
			x = 76,
			y = 93.66667,
			z = 54.04242
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