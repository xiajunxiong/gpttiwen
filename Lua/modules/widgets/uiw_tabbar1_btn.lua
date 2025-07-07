UIWTabbar1Btn = UIWTabbar1Btn or DeclareMono("UIWTabbar1Btn",UIWidgetBaseItem)

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
function UIWTabbar1Btn:SetData(data)
	self.gameObject.name = "btn_" .. (data.open or 0) % 10
	UIWidgetBaseItem.SetData(self,data)
	self.Name.text = data.name
	self.NameUnact.text = data.name
	if self.Name2 and self.NameUnact2 then
		UH.SetText(self.Name2, data.name2)
		UH.SetText(self.NameUnact2, data.name2)
	end
	self.TabActivator.Actives:Clear()
	if data.tab_objs ~= nil then
		for _, item in pairs(data.tab_objs) do
			self.TabActivator.Actives:Add(item)
		end
	end
	self.TabActivator.Actives:Add(self.Name.gameObject)
	self.TabActivator.Deactives:Add(self.NameUnact.gameObject)
	if self.handle ~= nil then
		RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
		self.handle = nil
	end
	if data.mod then
		if self.red_point == nil then
			self.red_point = UiRedPoint.New(self.gameObject,Vector3.New(36, 36))
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

function UIWTabbar1Btn:TabbarOpenEvent(opened)
	--检查自己是不是下一个等级开启的是就显示，不是就隐藏咯
	if opened == false then
		local result = FunOpen.Instance:NextTabbar(self.data.open)
		if result then
			self.gameObject:SetActive(result.client_id == self.data.open)
		end
	end
	self.BtnInter.Interactable = opened
	self.TipBlock.enabled = not opened
	if self.BtnSp and not self.data.is_self_img and not self.SelfImg then
		UH.SpriteName(self.BtnSp, opened and "tabbar_btn" or "tabbar_btn_lock")
	end
end

function UIWTabbar1Btn:OnClickTip()
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
function UIWTabbar1Btn:Click(data)
	-- 做一个检测然后判断是否触发
	self.ClickEvent:Trigger()
end

function UIWTabbar1Btn:OnDisable()
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

function UIWTabbar1Btn:OnRelease()
	UIWidgetBaseItem.OnRelease(self)
	self.gameObject:SetActive(true)
end