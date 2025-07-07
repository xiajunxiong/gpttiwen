RoleView = RoleView or DeclareView("RoleView","role/role",Mod.Role.RoleAttr)

VIEW_DECLARE_LEVEL(RoleView,ViewLevel.L1)
function RoleView:RoleView()
    HeartSkillCtrl.Instance:SendHeartSkillReq()
end

function RoleView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(),Language.Role.Role)
	self.Board:SetTabbarVisible(true)
	self.Board:SetCompetitionRank(ServeractivityConfig.CompetitionRank.Type.Role)
	local tabbar = self.Board.Tabbar
	local prof = RoleData.Instance:GetBaseData().profession
	local tabs = {
		{name = Language.Role.Tab1,tab_objs = {self.TabAttri},mod = TabMod(Mod.Role.RoleAttr), open = Mod.Role.RoleAttr},
		{name = Language.Role.Tab5,tab_objs = {self.TabSkill},mod = TabMod(Mod.Role.RoleSkill), open = Mod.Role.RoleSkill},
		{name = Language.Role.Tab7,tab_objs = {self.TabUniqueSkill},mod = TabMod(Mod.Role.TabUniqueSkill),open = Mod.Role.TabUniqueSkill,open_func = function()
			return FlyUpData.Instance:GetStage() >= FlyUpStageType.Low
		end,open_tip = Language.FlyUp.RoleOpenTip,},
		{name = Language.Role.Tab3,tab_objs = {self.TabHeartSkill},mod = TabMod(Mod.Role.RoleHeartSkill), open = Mod.Role.RoleHeartSkill},
		{name = Language.Role.Tab2,tab_objs = {self.TabPrestige},mod = TabMod(Mod.Role.RolePrestige),open = Mod.Role.RolePrestige},
		{name = Language.Role.Tab6,tab_objs = {self.TabSetting},mod = TabMod(Mod.Role.RoleSetting),open = Mod.Role.RoleSetting},
	}
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		tabs = {
			{name = Language.Role.Tab6,tab_objs = {self.TabSetting},mod = TabMod(Mod.Role.RoleSetting),open = Mod.Role.RoleSetting},
		}
	end
	if RoleData.Instance:GetRoleProf() == 0 then
		table.remove(tabs,2)
	end
	if type(tabbar) ~= "function" then
		tabbar:SetData(tabs)
		if param_t then
			tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Role, tabbar, param_t.open_param))
		else
			tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Role, tabbar, 1))
		end
	else
		LogError("TRIGGER TABBAR ERROR!!!====",self.Board:IsLoaded(),self:IsActiveAndEnabled(),self.gameObject.activeSelf,
		self.gameObject:GetParent().name,self.gameObject:GetParent().activeSelf)	
	end

	self.Currency:SetCurrency(CurrencyType.CoinBind, CurrencyType.ImmortalCoin, CurrencyType.Gold, CurrencyType.SkyGlass)
	
	GuideManager.Instance:RegisterGetGuideUi("LSTabbar", function ()
		return self.Board.Tabbar.item_list[2],function ()
			self.Board.Tabbar:ClickItem(2)
		end
	end)
end

function RoleView:CloseCallback()
	GuideManager.Instance:UnRegisterGetGuideUi("LSTabbar")
end

RagePointList = RagePointList or DeclareMono("RagePointList", UIWFlushPanel)
--底
--特效是出现和消失
function RagePointList:SetValue(value)
	for i = 1, 8 do
		self.ItemList[i]:SetActive(i <= value)
	end
end
function RagePointList:OnDisable()
	self.old_value = 0
end
--火
function RagePointList:SetShow(value, effect)
	for i = 1, 8 do
		self.PointList[i]:SetActive(i <= value)
	end
	if effect then
		self:PlayEffect(value)
	end
	self.old_value = value
end
function RagePointList:PlayEffect(value)
	if self.old_value == nil or not self:IsActiveAndEnabled() then
		return
	end
	if self.old_value ~= value then
		if self.old_value < value then
			for i = 1, 8 do
				if i > self.old_value and i <= value then
					self.EffectTool:Play("10010892",self.EffectList[i])
				end
			end
		else
			for i = 1, 8 do
				if i <= self.old_value and i > value then
					self.EffectTool:Play("10010893",self.EffectList[i])
				end
			end
		end
	end
end