-- 造化装备·主要界面
MarbasView = MarbasView or DeclareView("MarbasView", "marbas/marbas_view", Mod.Marbas.Base)
VIEW_DECLARE_LEVEL(MarbasView, ViewLevel.L1)
function MarbasView:MarbasView()
	self:InitGuide()
	self.data = MarbasData.Instance
	self.data:ResetLeftEquipInfo()
end

-- 引导预留接口
function MarbasView:InitGuide()
end
function MarbasView:LoadCallback(param_t)
	local check_func = BindTool.Bind(self.FlushTabbar,self)
	self.tabbar_1_handle = self.data.equip_list:Care(check_func)
	self.tabbar_2_handle = self.data.common_info:Care(check_func)
	self:FlushTabbar()

    self.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Marbas, self.Tabbar, param_t))
end

function MarbasView:FlushTabbar()
	self.Tabbar:SetData(
        {
            {name = Language.Marbas.TabTitle[1],tab_objs = {self.TabList[1]},open = Mod.Marbas.Base, mod = TabMod(Mod.Marbas.Base)},
            {name = Language.Marbas.TabTitle[3],tab_objs = {self.TabList[3], self.LeftEquip},open = Mod.Marbas.Refine, mod = TabMod(Mod.Marbas.Refine)},
            {name = Language.Marbas.TabTitle[4],tab_objs = {self.TabList[4], self.LeftEquip},open = Mod.Marbas.Bathe, mod = TabMod(Mod.Marbas.Bathe)},
            {name = Language.Marbas.TabTitle[5],tab_objs = {self.TabList[5]},open = Mod.Marbas.Blood, mod = TabMod(Mod.Marbas.Blood)},
			{name = Language.Marbas.TabTitle[2],tab_objs = {self.TabList[2]},open = Mod.Marbas.Build, mod = TabMod(Mod.Marbas.Build)},
            -- {name = Language.Marbas.TabTitle[6],tab_objs = {self.TabList[6]},open = Mod.Marbas.Vigorous, mod = TabMod(Mod.Marbas.Vigorous)},
        }
    )
end

function MarbasView:CloseCallback()
    if self.tabbar_1_handle ~= nil then
        self.data.equip_list:Uncare(self.tabbar_1_handle)
        self.tabbar_1_handle = nil
    end

	if self.tabbar_2_handle ~= nil then
        self.data.common_info:Uncare(self.tabbar_2_handle)
        self.tabbar_2_handle = nil
    end
end

-- 覆写baseview逻辑
function MarbasView:SelectTabbar(modKey)
	BaseView.SelectTabbar(self,modKey)

	-- LogError("?fisn !",FunOpen.Instance:IWantSelect(Mod.Marbas, self.Tabbar, {mod_key = modKey}))
	self.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Marbas, self.Tabbar, {mod_key = modKey}))
end

function MarbasView:OnClickClose()
    ViewMgr:CloseView(MarbasView)
end

function MarbasView:OnClickTips()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[315].desc})
end

-- 组件部分
MarbasOperList = MarbasOperList or DeclareMono("MarbasOperList", UIWFlushPanel)
function MarbasOperList:MarbasOperList()
    self.mb_data = MarbasData.Instance

end 

function MarbasOperList:OnDestroy()
    UIWFlushPanel.OnDestroy(self)     
end

function MarbasOperList:SetData(data)
    self.data = data 
    for i = 1,self.partList:Length() do
        local flag = data[i] ~= nil 
        self.partList[i].gameObject:SetActive(flag)
        if flag then 
            self.partList[i]:SetData(data[i])
        end 
    end
end

function MarbasOperList:GetData()
    return self.data
end

function MarbasOperList:ClickItem(index)
	local flag,level = self.mb_data:GetLevelShowEnough(self.data[index].open)
	if not flag then 
		return 
	end 

    self.partList[index]:Click(self.data[index])
end

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
MarbasPartCell =  MarbasPartCell or DeclareMono("MarbasPartCell", UIWidgetBaseItem)
function MarbasPartCell:MarbasPartCell() end
function MarbasPartCell:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = "btn_" .. (data.open or 0) % 10
    self.TabActivator.Actives:Clear()
    if data.tab_objs ~= nil then
		for _, item in pairs(data.tab_objs) do
			self.TabActivator.Actives:Add(item)
		end
	end

    -- 示例代码
	self.TabActivator.Actives:Add(self.select)
	-- self.TabActivator.Deactives:Add(self.NameUnact.gameObject)

    if self.handle ~= nil then
		RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
		self.handle = nil
	end

    if data.mod then
		if self.red_point == nil then
			self.red_point = UiRedPoint.New(self.red_pos,Vector3.zero)
			self.red_point:SetNum(0)
		end
		self.remind_handle_mod = data.mod			--记录注册Remind时的ModId
		self.handle = RemindCtrl.Instance:Remind():RegisterGroup(data.mod,function ()
			if FunOpen.Instance:GetFunIsOpened(data.open) then
				self.red_point:SetNum(RemindCtrl.Instance:Remind():GetGroupNum(data.mod))
			end
		end, true)
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

function MarbasPartCell:TabbarOpenEvent(opened)
	--检查自己是不是下一个等级开启的是就显示，不是就隐藏咯
	if opened == false then
		-- local result = FunOpen.Instance:NextTabbar(self.data.open)
		-- 暂时屏蔽等级链接展示
		-- if result then
		-- 	self.gameObject:SetActive(result.client_id == self.data.open)
		-- end

		-- 对血祭和打造的未开启状态设置为隐藏
		local sp_flag = self.data.open == Mod.Marbas.Build or self.data.open == Mod.Marbas.Blood
		self.gameObject:SetActive(not sp_flag)
	end
	self.BtnInter.Interactable = opened
	self.TipBlock.enabled = not opened
	-- if self.BtnSp and not self.data.is_self_img then
	-- 	UH.SpriteName(self.BtnSp, opened and "tabbar_btn" or "tabbar_btn_lock")
	-- end
end

function MarbasPartCell:OnClickTip()
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

function MarbasPartCell:OnClickTab()
	local flag,limit = MarbasData.Instance:GetLevelShowEnough(self.data.open)

	if not flag then 
		PublicPopupCtrl.Instance:Center(string.format(Language.Marbas.TotalLevelShowLimit,limit))
	end 
end

function MarbasPartCell:Click(data)
	-- 做一个检测然后判断是否触发
	self.ClickEvent:Trigger()
end

function MarbasPartCell:OnDisable()
	self.Toggle.isOn = false
	if self.handle ~= nil then
		RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
		self.handle = nil
	end	
	FunOpen.Instance:UnRegister(self.data.open)
	if self.red_point then
		self.red_point:SetNum(0)
	end
end

function MarbasPartCell:OnRelease()
	UIWidgetBaseItem.OnRelease(self)
	self.gameObject:SetActive(true)
end

function MarbasPartCell:OnDisable()
	self.Toggle.isOn = false
	if self.handle ~= nil then
		RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
		self.handle = nil
	end	
	if self.data then 
	    FunOpen.Instance:UnRegister(self.data.open)
	end 
	if self.red_point then
		self.red_point:SetNum(0)
	end
end

MarbasMaster = MarbasMaster or DeclareMono("MarbasMaster", UIWFlushPanel)
function MarbasMaster:MarbasMaster()
	self.data = MarbasData.Instance

	self.data_cares = {
        {data = self.data.marbas_view_data,func = self.FlushMaster, init_call = false},
    }
end 

function MarbasMaster:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	self.data:SetMasterBlock(false)
end

function MarbasMaster:FlushMaster()
	self.block:SetActive(self.data.marbas_view_data.is_block)
end