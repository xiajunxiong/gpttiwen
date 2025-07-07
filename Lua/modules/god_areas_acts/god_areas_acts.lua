GodAreasActs = GodAreasActs or DeclareView("GodAreasActs", "god_areas_acts/god_areas_acts", Mod.GodAreasActs)
function GodAreasActs:GodAreasActs()
    
end
function GodAreasActs:LoadCallback(param_t)
    local data = {
        {name = Language.GodAreasActs.Tabbar[1], tab_objs = {self.Ruins}, open = Mod.GodAreasActs.Ruins},
        {name = Language.GodAreasActs.Tabbar[2], tab_objs = {self.Palaces}, open = Mod.GodAreasActs.Palaces, open_func = function()
			return self:CheckPalacesOpen()
		end, open_tip = Language.GodAreasActs.PalacesOpen},
        {name = Language.GodAreasActs.Tabbar[3], tab_objs = {self.Animals}, open = Mod.GodAreasActs.Animals, open_func = function()
			return self:CheckAnimalsOpen()
		end, open_tip = Language.GodAreasActs.PalacesOpen, mod = Mod.GodAreasActs.Animals},
    }
	local base_info = LandsBetweenData.Instance:GetBaseInfo()
    self.Tabbar:SetData(data)
	if param_t.open_param < 3 then
		self.Tabbar:ClickItem(base_info.cur_status < GOD_SPACE_STATUS.InnerOpen and 1 or 2)
	else
		self.Tabbar:ClickItem(param_t.open_param)
	end
	GodAreasActsCtrl.Instance:CSZhenYingShenShouInfo(2)
end
function GodAreasActs:CheckPalacesOpen()
	local base_info = LandsBetweenData.Instance:GetBaseInfo()
    return base_info.cur_status >= GOD_SPACE_STATUS.InnerOpen
end
function GodAreasActs:CheckAnimalsOpen()
	return GodAreasActsData.Instance:AnimalsIsOpen() ~= ActStatusType.Close
end
function GodAreasActs:OnClickClose()
    ViewMgr:CloseView(GodAreasActs)
end


GodAreasActsBtn = GodAreasActsBtn or DeclareMono("GodAreasActsBtn",UIWidgetBaseItem)

-------Name===========UIText
-------ClickEvent=====LuaUIEvent
-------TabActivator===UIToggleActivator

--[[
data = {
	name = "",			--text on tabbar button label
	tab_objs = nil,		--UnityGameObjects the table if active with this TabbarBtn
	mod = nil,			--红点检查用
	open = nil,			--功能开启用
	open_tip = ""		--未开启标签点击提示
	open_func = func 	--标签开启检查函数 结果返回true or false
	is_self_img = bool	--为true则按钮图片不进行更改
}]]
function GodAreasActsBtn:SetData(data)
	-- self.gameObject.name = "btn_" .. (data.open or 0) % 10
	UIWidgetBaseItem.SetData(self,data)
	UH.SetText(self.Name, data.name)
	-- UH.SetText(self.NameUnact, data.name)
	self.TabActivator.Actives:Clear()
	if data.tab_objs ~= nil then
		for _, item in pairs(data.tab_objs) do
			self.TabActivator.Actives:Add(item)
		end
	end
	-- self.TabActivator.Actives:Add(self.Name.gameObject)
	-- self.TabActivator.Deactives:Add(self.NameUnact.gameObject)
	if self.handle ~= nil then
		RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
		self.handle = nil
	end
	self.RedPoint:SetNum(0)
	if data.mod then
		if data.mod == Mod.GodAreasActs.Animals then
			self.data_cares_animals = GodAreasActsData.Instance:GetBossInfo():Care(BindTool.Bind(self.FlushAnimalsRed, self))
			self:FlushAnimalsRed()
		end
		-- if self.red_point == nil then
		-- 	self.red_point = UiRedPoint.New(self.gameObject,Vector3.New(36, 36))
		-- 	self.red_point:SetNum(0)
		-- end
		-- self.remind_handle_mod = data.mod			--记录注册Remind时的ModId
		-- self.handle = RemindCtrl.Instance:Remind():RegisterGroup(data.mod,function ()
		-- 	if FunOpen.Instance:GetFunIsOpened(data.open) then
		-- 		self.red_point:SetNum(RemindCtrl.Instance:Remind():GetGroupNum(data.mod))
		-- 	end
		-- end, true)
	end
	if data.open_func then
		FunOpen.Instance:SelfCondition(data.open, data.open_func)
	end
	if data.open then
		FunOpen.Instance:Register(data.open, BindTool.Bind(self.TabbarOpenEvent, self))
	elseif self.TipBlock then
		self.TipBlock.enabled = false
	end
end
function GodAreasActsBtn:FlushAnimalsRed()
	self.RedPoint:SetNum(GodAreasActsData.Instance:AnimalsRemind())
end

function GodAreasActsBtn:TabbarOpenEvent(opened)
	--检查自己是不是下一个等级开启的是就显示，不是就隐藏咯
	if opened == false then
		--[[ local result = FunOpen.Instance:NextTabbar(self.data.open)
		if result then
			self.gameObject:SetActive(result.client_id == self.data.open)
		end ]]
	end
	self.BtnInter.Interactable = opened
	self.TipBlock.enabled = not opened
	--[[ if self.BtnSp and not self.data.is_self_img and not self.SelfImg then
		UH.SpriteName(self.BtnSp, opened and "tabbar_btn" or "tabbar_btn_lock")
	end ]]
end

function GodAreasActsBtn:OnClickTip()
	local opened, open_tip = FunOpen.Instance:GetFunIsOpened(self.data.open)
	if opened then
		-- 特殊
		if self.BtnInter.Interactable == false then
			if self.data.open_tip then
				if type(self.data.open_tip) == "string" then
					PublicPopupCtrl.Instance:Center(self.data.open_tip)
				elseif type(self.data.open_tip) == "function" then
					PublicPopupCtrl.Instance:Center(self.data.open_tip())
				end
			else
				PublicPopupCtrl.Instance:Center(open_tip)
			end
		end
	else
		if self.data.open_tip then
			if type(self.data.open_tip) == "string" then
				PublicPopupCtrl.Instance:Center(self.data.open_tip)
			elseif type(self.data.open_tip) == "function" then
				PublicPopupCtrl.Instance:Center(self.data.open_tip())
			end
		else
			PublicPopupCtrl.Instance:Center(open_tip)
		end
	end
end
function GodAreasActsBtn:Click(data)
	-- 做一个检测然后判断是否触发
	self.ClickEvent:Trigger()
end

function GodAreasActsBtn:OnDisable()
	self.Toggle.isOn = false
	if self.handle ~= nil then
		RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
		self.handle = nil
	end	
	FunOpen.Instance:UnRegister(self.data.open)
	--[[ if self.red_point then
		self.red_point:SetNum(0)
	end ]]
end

function GodAreasActsBtn:OnRelease()
	UIWidgetBaseItem.OnRelease(self)
	self.gameObject:SetActive(true)
	if self.data_cares_animals then
		GodAreasActsData.Instance:GetBossInfo():Uncare(self.data_cares_animals)
		self.data_cares_animals = nil
	end
end