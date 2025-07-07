
MapViewWorldIItem = DeclareMono("MapViewWorldIItem", UIWidgetBaseItem)

function MapViewWorldIItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.ItemPosition.localPosition = data.pos
	self.Name.text = data.name
end

MapViewWorldLItem = DeclareMono("MapViewWorldLItem", UIWidgetBaseItem)

function MapViewWorldLItem:MapViewWorldLItem()
	self.map_data = MapCtrl.Instance:Data()
	self.mini = MapConfig.Mini
end

function MapViewWorldLItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.ItemPosition.localPosition = data.pos
	self.Name.text = " "..data.name.." "
end

function MapViewWorldLItem:OnClickItem()
	local to_pos = self.data.to_pos
	self.LocalCoord.anchoredPosition = to_pos
	self.map_data.local_coord:Set(self.LocalCoord.anchoredPosition)
	self.LocalToggle.isOn = true
    self.MiniViewRect.localPosition = Vector2.New(- to_pos.x / self.mini.max_size_w * self.mini.width, - to_pos.y / self.mini.max_size_w * self.mini.width)
end