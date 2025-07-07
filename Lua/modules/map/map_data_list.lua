
function MapData:MapDataList()
	self:MapDataListIsland()
	self:MapDataListTown()
	self:MapDataListRoom()
end

function MapData:MapDataListIsland()
	self.list_island = {}
	for _, v in pairs(self.config_island_list) do
		self.list_island[v.island_seq] = v
	end
	self.cur_sel_island = SmartData.New()
end

function MapData:GetIslandList()
	return self.list_island
end

function MapData:MapDataListTown()
	self.list_town = {}
	for _, v in pairs(self.config_towns_list) do
		self.list_town[v.island_type] = self.list_town[v.island_type] or {}
		self.list_town[v.island_type][v.seq_2] = v
	end
end

function MapData:GetTownListByIslandType(island_type)
	return self.list_town[island_type]
end

function MapData:MapDataListRoom()
	self.list_room = {}
	for _, v in pairs(self.config_room_list) do
		self.list_room[v.town_type] = self.list_room[v.town_type] or {}
		self.list_room[v.town_type][v.seq_2] = v
	end
end

function MapData:GetRoomListByTownType(town_type)
	return self.list_room[town_type]
end