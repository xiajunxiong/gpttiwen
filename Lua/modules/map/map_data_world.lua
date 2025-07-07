
function MapData:MapDataWorld()
	self:MapDataWorldNameList()
end

function MapData:MapDataWorldNameList()
	self.world_name_list = {
		[0] = {},
		[1] = {},
	}
	local local_w = MapConfig.LocalW
	for _, v in pairs(self.config_word_name_show) do
		self.world_name_list[v.type][v.id] = v
		self.world_name_list[v.type][v.id].pos = Vector2.New(v.x, v.y)
		self.world_name_list[v.type][v.id].to_pos = Vector2.New(v.jump_x, v.jump_y)
	end
end

function MapData:GetWorldNameIList()
	return self.world_name_list[0]
end

function MapData:GetWorldNameLList()
	return self.world_name_list[1]
end
