
MapViewLocal = MapViewLocal or DeclareMono("MapViewLocal", UIWFlushPanel)
--ExpCrystalBtn=====GameObject

function MapViewLocal:MapViewLocal()
	self.data = MapCtrl.Instance:Data()
	self.activity_data = ActivityCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.view_rect, func = self.OnViewRect, init_call = false},
		{data = self.data.local_coord, func = self.OnLocalCoord, init_call = false},
		{data = self.data.irregular_state, func = self.FlushTownList, init_call = false},
		{data = self.data.irregular_state, func = self.FlushLocalInter},
		{data = self.activity_data.activity_status_event, func = self.FlushWorldBossSub},
		{data = RoleData.Instance:GetBaseData(), func = self.FlushExpCrystal, keys = {"level"}},
	}
	
	GuideManager.Instance:RegisterGetGuideUi("MapYingduEffect", 
		function ()
			if not self.complete_flush then
				return nil,nil
			end 

			return self.EffectTool , function ()
				-- 对准皇城
				local target_cfg = self.data:GetLocalTownInfoByID(MapItemEnum.YingDu)
				if target_cfg == nil then return end 
				self.LocalCoord.anchoredPosition = Vector2(-target_cfg.x,-target_cfg.y)
				self.data.local_coord:Set(self.LocalCoord.anchoredPosition)

				--略微延迟
				TimeHelper:AddDelayTimer(function() 
					self.ItemLs[5]:TryActItem()
					self.ItemLs[5].UnLockPlay:Play()
				end,0.4)
			end 
		end 
	)

	GuideManager.Instance:RegisterGetGuideUi("MapYingduXijiaoEffect", 
		function ()
			if not self.complete_flush then
				return nil,nil
			end 

			return self.EffectTool , function ()
			    -- 对准西郊
				local target_cfg = self.data:GetLocalTownInfoByID(MapItemEnum.YingDuXiJiao)
				if target_cfg == nil then return end 
			    self.LocalCoord.anchoredPosition = Vector2(-target_cfg.x,-target_cfg.y)
				self.data.local_coord:Set(self.LocalCoord.anchoredPosition)

				--略微延迟
				TimeHelper:AddDelayTimer(function() 
					self.ItemLs[4]:TryActItem()
					self.ItemLs[4].UnLockPlay:Play()
				end,0.4)
		    end 
	    end 
	)


	local guide_town_item = function (town_id)
		local item_list = self.TownList.item_list
		local mono
		for i,v in pairs(item_list) do
			local data = v:GetData()
			if data.id == town_id then
				mono = v
				break
			end
		end
		if not mono then
			return nil, nil
		end
		return mono, function ()
			--mono:OnClickItem()
			local vo = mono:GetData()
			SceneLogic.Instance:FlyToPos(vo.scene_id)
			ViewMgr:CloseView(MapView)
		end
	end
	-- 去玄都南郊
	GuideManager.Instance:RegisterGetGuideUi("MapXuanDuNanJiao",function ()
		local mono = guide_town_item(56)
		if mono ~= nil then
			return mono, function ()
				--mono:OnClickItem()
				local vo = mono:GetData()
				SceneLogic.Instance:FlyToPos(vo.scene_id)
				ViewMgr:CloseView(MapView)
			end
		end
		-- local item_list = self.TownList.item_list
		-- local mono
		-- for i,v in pairs(item_list) do
		-- 	local data = v:GetData()
		-- 	if data.id == 56 then
		-- 		mono = v
		-- 		break
		-- 	end
		-- end
		-- if not mono then
		-- 	return nil, nil
		-- end
		-- return mono, function ()
		-- 	--mono:OnClickItem()
		-- 	local vo = mono:GetData()
		-- 	SceneLogic.Instance:FlyToPos(vo.scene_id)
		-- 	ViewMgr:CloseView(MapView)
		-- end
    end)
	GuideManager.Instance:RegisterGetGuideUi("MapWanCun",function ()
		local mono = guide_town_item(48)
		return mono
	end)

	
	self.complete_flush = false
end

function MapViewLocal:Awake()
	self:InitLocalList()
	UIWFlushPanel.Awake(self)
	self:FlushLocalCoord()
	self:InitWorldList()
end

function MapViewLocal:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	GuideManager.Instance:UnRegisterGetGuideUi("MapYingduEffect")
	GuideManager.Instance:UnRegisterGetGuideUi("MapYingduXijiaoEffect")
end 

function MapViewLocal:FlushLocalCoord()
	self.LocalCoord.anchoredPosition = self.data:GetLocalCoord(SceneData.Instance:SceneId())
	self.data.local_coord:Set(self.LocalCoord.anchoredPosition)
end

function MapViewLocal:FlushWorldBossSub()
	local status = ActivityData.Instance:GetActivityStatus(ActType.WorldBoss)
	local status2 = ActivityData.Instance:GetActivityStatus(ActType.WorldBoss2)
	local status3 = ActivityData.Instance:GetActivityStatus(ActType.WorldBoss3)
	self.WorldBossSub:SetActive(ActStatusType.Open == status or ActStatusType.Open == status2 
    or ActStatusType.Open == status3)
end

function MapViewLocal:OnViewRect()
	self.data.local_coord:Set(self.LocalCoord.anchoredPosition)
	self:FlushTownList()
	self:OnLocalCoord()
end

function MapViewLocal:OnLocalCoord()
	if not self.data.view_rect_ready then return end
	self:FlushWorldList()
	self:FlushLocalShowing()
	self:FlushTownShowing()
end

function MapViewLocal:InitWorldList()
	local local_w_list = self.data:GetLocalWList()
	for i = 1, self.ItemWs:Length() do
		self.ItemWs[i]:SetData(local_w_list[i])
	end
end

function MapViewLocal:FlushWorldList()
	for i = 1, self.ItemWs:Length() do
		self.ItemWs[i]:IsShowing()
	end
end

function MapViewLocal:InitLocalList()
	local local_l_list = self.data:GetLocalLList()
	for i = 1, self.ItemLs:Length() do
		self.ItemLs[i]:SetData(local_l_list[i])
	end
end

function MapViewLocal:FlushLocalShowing()
	for i = 1, self.ItemLs:Length() do
		self.ItemLs[i]:IsShowing()
	end
end

function MapViewLocal:FlushLocalInter()
	for i = 1, self.ItemLs:Length() do
		self.ItemLs[i]:FlushInter()
	end
end

function MapViewLocal:FlushTownList()
	local local_t_info = self.data:GetLocalTInfo()
	self.TownList.StepCount = local_t_info.step_count
	self.TownList:SetData(local_t_info.list)

	if not self.complete_flush then self.complete_flush = true end 
end

function MapViewLocal:FlushTownShowing()
	self.TownList.StepCount = 0
	local list = self.data:GetLocalTInfoShowing()
	for i = 1, #list do
		self.TownList:SetItemDataByKey(i, list[i])
	end
end

function MapViewLocal:OnLocalMapDrag()
	self.data.local_coord:Set(self.LocalCoord.anchoredPosition)
end

function MapViewLocal:FlushExpCrystal()
    --self.ExpCrystalBtn:SetActive(FunOpen.Instance:GetFunIsOpened(Mod.Fight.Field) == true)
end


function MapViewLocal:OnClickExpCry()
    ViewMgr:OpenViewByKey(Mod.Fight.Field)
end

MapViewLocalItem = DeclareMono("MapViewLocalItem", UIWidgetBaseItem)

function MapViewLocalItem:MapViewLocalItem()
	self.map_data = MapCtrl.Instance:Data()
end

function MapViewLocalItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
end

function MapViewLocalItem:IsShowing()
	if self.data.lock_state then
		if not self.ImageSetter:TextureIsNil() then
			self.ImageSetter:SetTextureNil()
		end
		return
	end
	local is_show = false
	local local_coord = self.map_data.local_coord
	local offset_x = 0.5 * self.map_data.view_rect.width + self.data.half_width
	if IS_IPHONEX or EditorSettings:SimularIphoneX() then
		offset_x = offset_x + 100
	end
	local offset_y = 0.5 * self.map_data.view_rect.height + self.data.half_height
	if self.ItemPosition.localPosition.x < -local_coord.x + offset_x
		and self.ItemPosition.localPosition.x > -local_coord.x - offset_x
		and self.ItemPosition.localPosition.y < -local_coord.y + offset_y
		and self.ItemPosition.localPosition.y > -local_coord.y - offset_y
	then
		is_show = true
	end
	if is_show then
		if self.ImageSetter:TextureIsNil() then
			self.ImageSetter:SetByPathSync(self.data.png_path)
		end
	-- else
	-- 	if not self.ImageSetter:TextureIsNil() then
	-- 		self.ImageSetter:SetTextureNil()
	-- 	end
	end
end

function MapViewLocalItem:FlushInter()
	self.ItemObj:SetActive(not self.data.lock_state)
end

function MapViewLocalItem:TryActItem()
	self.ItemObj:SetActive(true)
	if self.ImageSetter:TextureIsNil() then
		self.ImageSetter:SetByPathSync(self.data.png_path)
	end
end 

MapViewLocalWItem = DeclareMono("MapViewLocalWItem", MapViewLocalItem)

MapViewLocalLItem = DeclareMono("MapViewLocalLItem", MapViewLocalItem)

MapViewLocalTItem = DeclareMono("MapViewLocalTItem", UIWidgetBaseItem)

function MapViewLocalTItem:MapViewLocalTItem()
	self.map_data = MapCtrl.Instance:Data()
	self.mini = MapConfig.Mini
	self.is_init = false
end

function MapViewLocalTItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	if self.is_init then
		-- self:IsShowing()
		return
	end
	self.ItemPosition.localPosition = data.pos
	self.ItemInter.Interactable = data.lock_state
	self.ClickInter.interactable = 1 == data.inter
	self.Name.text = RichTextHelper.SizeStr(data.name, data.font)
	self.TypeSp.SpriteName = "lt_" .. data.png_type
	local value = SceneData.Instance:SceneId() == data.scene_id
	self.CoordObj:SetActive(value)
	if value then
		self.map_data.mini_center_pos:Set(Vector2.New(data.pos.x / self.mini.max_size_w * self.mini.width, data.pos.y / self.mini.max_size_h * self.mini.height))
	end
	self.is_init = true
	-- self:IsShowing()
end

function MapViewLocalTItem:IsShowing()
	-- local is_show = false
	-- local local_coord = self.map_data.local_coord
	-- local offset_x = 0.5 * self.map_data.view_rect.width + self.data.half_width
	-- local offset_y = 0.5 * self.map_data.view_rect.height + self.data.half_height
	-- if self.ItemPosition.localPosition.x < -local_coord.x + offset_x
	-- 	and self.ItemPosition.localPosition.x > -local_coord.x - offset_x
	-- 	and self.ItemPosition.localPosition.y < -local_coord.y + offset_y
	-- 	and self.ItemPosition.localPosition.y > -local_coord.y - offset_y
	-- then
	-- 	is_show = true
	-- end
	-- self.ItemTObj:SetActive(is_show)
end

function MapViewLocalTItem:OnClickItem()
	if 0 == self.data.inter then return end
	if not self.data.lock_state then
		PublicPopupCtrl.Instance:Center(string.format(Language.Map.TipsLock, self.map_data:GetMapMinLevel(self.data.restrict_in)))
		return
	end
	if MapData.Instance:IsCanToMini(self.data.scene_id) then
		MapData.Instance:SetSelectSceneId(self.data.scene_id)
		ViewMgr:OpenView(MapMiniView,self.data.scene_id)
	end
end

