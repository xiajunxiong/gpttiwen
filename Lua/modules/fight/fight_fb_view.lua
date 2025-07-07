FightFBView = FightFBView or DeclareView("FightFBView", FightConfig.ResPath .. "fight_fb",Mod.FightFB.Material)

VIEW_DECLARE_LEVEL(FightFBView,ViewLevel.L0)

VIEW_DECLARE_MASK(FightFBView,ViewMask.Block)
function FightFBView:FightFBView()
	self.data = FightCtrl.Instance:Data()
	self:RegisterGetGuideUi()
	FightFBView.SubsActived = {}
	DreamNotesCtrl.Instance:SendCallback()
end

function FightFBView:LoadCallback(param_t)
	self:FlushFightView(param_t.open_param or 5)
	if param_t.is_not_show == true then
		self.Effect:SetActive(false)
	end
end

function FightFBView:CloseCallback()
	FightFBView.SubsActived = {}
end

function FightFBView:OnClickClose()
	ViewMgr:CloseView(FightFBView)
end

function FightFBView:OnToggleTab(value)
	self.data:SetFightSelectData(value)
	if 2 == value then
		self.data:FightFBEnergyShow(1 == self.data.fight_fb_ymz.type)
	else
		self.data:FightFBEnergyShow(true)
	end
end

function FightFBView:OnClickTips()
	PublicPopupCtrl.Instance:HelpTip(self.data:GetClickTips())
end

function FightFBView:OnClickAdd()
	ViewMgr:OpenView(FightCrystalSweepItemView)
end

function FightFBView:RegisterGetGuideUi()
	GuideManager.Instance:RegisterGetGuideUi("MaterialBtn", function()
		return self.List[1],function()
			self.List[1]:Trigger(true)
		end
	end)

	GuideManager.Instance:RegisterGetGuideUi("PetIslandBtn", function ()
		return self.List[3],function()
			self.List[3]:Trigger(true)
		end
	end)

	GuideManager.Instance:RegisterGetGuideUi("FightFBYMZBtn", function ()
		return self.List[2],function()
			self.List[2]:Trigger(true)
		end
	end)

	GuideManager.Instance:RegisterGetGuideUi("DreamNotesBtn", function()
		return self.List[5],function()
			self.List[5]:Trigger(true)
		end
	end)
end

function FightFBView:FlushFightView(index)
	for i=1,self.List:Length() do
		self.List[i]:SetData({mod = TabMod(FightMod[i]),open = FightMod[i]})
		self.List[i]:Trigger(index == i)
	end
end

----------------------------FightFBItem----------------------------
FightFBItem = FightFBItem or DeclareMono("FightFBItem", UIWidgetBaseItem)

function FightFBItem:SetData(data)
	for i = 1, self.Subs:Length() do
		self.Subs[i]:SetObjActive(false)
	end

	if self.handle == nil and self.gameObject.activeSelf then
		self.handle = Remind.Instance:RegisterGroup(data.mod,function()
			self.RedPoint:SetNum(Remind.Instance:GetGroupNum(data.mod))
		end, true)
	else
		self.RedPoint:SetNum(Remind.Instance:GetGroupNum(data.mod))
	end
    UIWidgetBaseItem.SetData(self, data)
	if self.data.open and self.gameObject.activeSelf then
		FunOpen.Instance:Register(self.data.open, BindTool.Bind(self.TabbarOpenEvent, self))
	elseif self.TipBlock then
		self.TipBlock:SetActive(false)
	end
	self.is_actived = false
end

function FightFBItem:OnDestroy()
	Remind.Instance:UnRegisterGroup(self.data.mod,self.handle)
	FunOpen.Instance:UnRegister(self.data.open)
end

function FightFBItem:Trigger(isOn)
	self.Toggle.isOn = isOn
end

function FightFBItem:TabbarOpenEvent(opened)
	self.ItemInter.Interactable = opened
	self.TipBlock:SetActive(not opened)
end

function FightFBItem:OnClickTip()
	local opened, open_tip = FunOpen.Instance:GetFunIsOpened(self.data.open)
	PublicPopupCtrl.Instance:Center(open_tip)
end

function FightFBItem:OnToggleTab()
	for i = 1, self.Subs:Length() do
		self.Subs[i]:SetObjActive(true)
	end
	local complete_func = function ()
		for i = 1, #FightFBView.SubsActived do
			FightFBView.SubsActived[i]:SetObjActive(false)
		end
		FightFBView.SubsActived = {}
		for i = 1, self.Subs:Length() do
			table.insert(FightFBView.SubsActived, self.Subs[i])
		end
		self.is_actived = true
	end
	if not self.is_actived then
		local delay_frame = 1
		Runner.Instance:RunUntilTrue(
        function()
            if self.Subs ~= nil then
                for i = 1, self.Subs:Length() do
                    local wait = self.Subs[i]
                    if (wait:IsActiveAndEnabled() and not wait:IsLoaded()) then
                        return false
                    end
                end
            end
			if delay_frame > 0 then		--延迟一帧保证是同时执行的关闭和打开
				delay_frame = delay_frame - 1
				return false
			end
            Call(complete_func)
            return true
        end
    )
	else
		Call(complete_func)
	end
end