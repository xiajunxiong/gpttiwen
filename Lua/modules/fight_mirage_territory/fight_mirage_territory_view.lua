FightMirageTerritoryView = FightMirageTerritoryView or DeclareView("FightMirageTerritoryView","fight_mirage_territory/fight_mirage_territory",Mod.Experience.MirageTerritory)
VIEW_DECLARE_LEVEL(FightMirageTerritoryView,ViewLevel.L1)

function FightMirageTerritoryView:FightMirageTerritoryView()
	self.Data = FightMirageTerritoryData.Instance
end

function FightMirageTerritoryView:LoadCallback()
	self.List:SetData(self.Data:GetAreaData())
	self:FlushPanel()
	self.handle = self.Data.dispatch_data:Care(BindTool.Bind(self.FlushPanel, self))
end

function FightMirageTerritoryView:FlushPanel()
	UH.SetText(self.Times,string.format(Language.MirageTerritory.Times,self.Data:GetDispatchTimes(),self.Data.dispatch_data.all_num))
end

function FightMirageTerritoryView:CloseCallback()
    self.Data.dispatch_data:Uncare(self.handle)
	if self.anim_cache then
	end
end

function FightMirageTerritoryView:OnClickClose()
	ViewMgr:CloseView(FightMirageTerritoryView)
end

function FightMirageTerritoryView:OnClickTips()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[195].desc})
end

FightMirageTerritoryItem = FightMirageTerritoryItem or DeclareMono("FightMirageTerritoryItem",UIWidgetBaseItem)
function FightMirageTerritoryItem:FightMirageTerritoryItem()
	self.eff_handle = nil
end
function FightMirageTerritoryItem:SetData(data)
	UH.SetText(self.Name,data.area_name)
	UH.SpriteName(self.AreaSprite,MirageTerritoryConfig.AreaSprite[data.seq])
	UH.SetText(self.ProfTxt1,Language.MirageTerritory.Profession[data.job1])
	UH.SetText(self.ProfTxt2,Language.MirageTerritory.Profession[data.job2])
	UH.SpriteName(self.ProfIcon1,RoleData.GetProfSp(data.job1))
	UH.SpriteName(self.ProfIcon2,RoleData.GetProfSp(data.job2))
	UH.SetText(self.AreaNum,data.area_num.."/".."<color=#"..COLORSTR.Red1.. ">"..data.all_num.."</color>")
	self.List:SetData(DataHelper.FormatItemList(data.reward_list))
	if self.eff_handle == nil then
		self.eff_handle = self.EffectTool:PlayByEffectId(FightMirageTerritoryData.GetEffectId(data.seq), self.unityMono.gameObject)
	end
	UIWidgetBaseItem.SetData(self, data)
end

function FightMirageTerritoryItem:OnClickItem()
	-- if FlyUpData.Instance:GetStage() >= FlyUpStageType.Low then
	local is_open, tips = FunOpen.Instance:GetFunIsOpened(Mod.Experience.MirageTerritory)
	if is_open then
		FightMirageTerritoryData.Instance:SetCurAreaData(self.data)
		ViewMgr:OpenView(FightMirageTerritoryLevelView)
	else
		PublicPopupCtrl.Instance:Center(tips)--(Language.MirageTerritory.FlyUpCanEnter)
	end
end