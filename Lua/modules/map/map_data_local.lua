
function MapData:MapDataLocal()
	self:MapDataLocalW()
	self:MapDataLocalL()
	self:MapDataLocalT()
end

function MapData:MapDataLocalW()
	self.local_w_list = {}
	local local_w = MapConfig.LocalW
	for i = 1, local_w.num_max do
		self.local_w_list[i] = {
			png_path = string.format(local_w.png_path, i),
			half_width = local_w.half_width,
			half_height = local_w.half_height,
		}
	end
end

function MapData:GetLocalWList()
	return self.local_w_list
end

function MapData:MapDataLocalL()
	self.local_l_list = {}
	local local_l = MapConfig.LocalL
	for _, v in pairs(self.config_irregular_region_xy) do
		self.local_l_list[v.id] = {
			png_path = string.format(local_l.png_path, v.id),
			half_width = v.w / 2,
			half_height = v.h / 2,
			id = v.id
		}
	end
end

function MapData:GetLocalLList()
	return self.local_l_list
end

function MapData:MapDataLocalT()
	self.local_t_info = {
		step_count = 0,
		list = {}
	}
	self.local_coord_list = {}
	for _, v in pairs(self.config_word_type_show) do
		if 1 == v.is_show_word then
			self.local_t_info.list[v.id] = v
			self.local_t_info.list[v.id].pos = Vector2.New(v.x, v.y)
			self.local_coord_list[v.scene_id] = Vector2.New(-v.x, -v.y)
		end
	end
	self.local_coord = SmartData.New(Vector2.zero)
end

function MapData:GetLocalTInfo()
	local list = {}
	local list_t = {}
	local step_count = 0
	local local_t = MapConfig.LocalT
	local offset_x = 0.5 * self.view_rect.width + local_t.half_width
	local offset_y = 0.5 * self.view_rect.height + local_t.half_height
	for _, v in pairs(self.local_t_info.list) do
		v.half_width = local_t.half_width
		v.half_height = local_t.half_height
		if v.pos.x > -self.local_coord.x - offset_x
			and v.pos.x < -self.local_coord.x + offset_x
			and v.pos.y > -self.local_coord.y - offset_y
			and v.pos.y < -self.local_coord.y + offset_y
		then
			table.insert(list, v)
			step_count = step_count + 1
		else
			table.insert(list_t, v)
		end
	end
	for _, v in pairs(list_t) do
		table.insert(list, v)
	end
	return {
		list = list,
		step_count = step_count
	}
end

function MapData:GetLocalTownInfoByID(_id)
	local local_t = self:GetLocalTInfo()
	for k,v in pairs(local_t.list) do 
		if v.id == _id then 
			return v
		end 
	end 

	return nil
end 

function MapData:GetLocalTInfoShowing()
	if nil == self.local_t_info_showing then
		self.local_t_info_showing = self:GetLocalTInfo()
	end
	return self.local_t_info_showing.list
end

function MapData:GetLocalCoord(scene_id)
	return self.local_coord_list[scene_id] or Vector2.zero
end
