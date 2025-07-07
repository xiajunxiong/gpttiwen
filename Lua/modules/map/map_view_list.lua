
MapViewList = MapViewList or DeclareMono("MapViewList", UIWFlushPanel)

function MapViewList:MapViewList()
	self.data = MapCtrl.Instance:Data()
	self.language = Language.Map

	self.data_cares = {
        {data = self.data.cur_sel_island, func = self.FlushLocalList, init_call = false},
		-- {data = self.data.cur_sel_task, func = self.FlushMenu},
	}
end

function MapViewList:Awake()
	UIWFlushPanel.Awake(self)

	self.Board:SetData(self:GetType(), self.language.LocalList, Vector2.New(982, 558))
	self.Board:SetCloseFunc(function ()
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
		self.ToggleList.isOn = false
	end)
	
	self:FlushIslandList()
end

function MapViewList:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushIslandList()
end

function MapViewList:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.data.cur_sel_island:Set({})
end

function MapViewList:FlushIslandList()
	local island_list = self.data:GetIslandList()
	self.IslandList:SetData(island_list)
	self.IslandList:ClickItemData(island_list[1])
end

function MapViewList:FlushLocalList()
	local town_list = self.data:GetTownListByIslandType(self.data.cur_sel_island.island_seq)
	self.LocalList:SetData(town_list)
end

MapViewListIslandItem = DeclareMono("MapViewListIslandItem", UIWidgetBaseItem)

function MapViewListIslandItem:MapViewListIslandItem()
	self.map_data = MapCtrl.Instance:Data()
end

function MapViewListIslandItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.IslandName.text = data.island_type
end

function MapViewListIslandItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function MapViewListIslandItem:OnClickItem()
	self.map_data.cur_sel_island:Set(self.data)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
end

MapViewListTownItem = DeclareMono("MapViewListTownItem", UIWidgetBaseItem)

function MapViewListTownItem:MapViewListTownItem()
	self.map_data = MapCtrl.Instance:Data()
end

function MapViewListTownItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.TownName.text = data.type_2
	local room_list = self.map_data:GetRoomListByTownType(data.seq_2)
	self.RoomList:SetData(room_list)
end

function MapViewListTownItem:SetupCompleted()
	return self.RoomList:SetupCompleted()
end

MapViewListRoomItem = DeclareMono("MapViewListRoomItem", UIWidgetBaseItem)

function MapViewListRoomItem:MapViewListRoomItem()
	self.map_data = MapCtrl.Instance:Data()
end

function MapViewListRoomItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.RoomName.text = data.type_2
end

function MapViewListRoomItem:OnClickItem()
	-- Debuger.LogError("正在前往"..self.data.type_2.."...")
	MapCtrl.Instance:AutoToPos(self.data.scene_id)
end
