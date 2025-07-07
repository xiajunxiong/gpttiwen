
FightViewGather = FightViewGather or DeclareMono("FightViewGather", UIWFlushPanel)

function FightViewGather:FightViewGather()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Fight.Gather

	self.data_cares = {
		{data = self.data.cur_sel_types, func = self.FlushShowList},
		{data = self.data.cur_sel_gather, func = self.FlushDetail},
		{data = self.data.gather_energy, func = self.FlushEnergy},
	}
end

function FightViewGather:Awake()
	UIWFlushPanel.Awake(self)
	local gather_type_list = self.data:GetGatherTypeList()
	self.TypeList:SetData(gather_type_list)
	self.TypeList:ClickItemData(gather_type_list[self.data.cur_sel_types.gather_type])
end

function FightViewGather:OnEnable()
	UIWFlushPanel.OnEnable(self)
    self.data.cur_sel_tabbar.val = FightConfig.FightTabType.gather
end

function FightViewGather:FlushShowList()
	self.ToggleGroupShow:SetAllTogglesOff()
	local gather_list = self.data:GetGatherListWithType()
	self.data.cur_sel_gather:Set(nil == self.data.cur_sel_type and gather_list[next(gather_list)] or self.data:GetItemWithType(gather_list, self.data.cur_sel_type))
	self.ShowList:SetData(gather_list)
	self.ShowList:ClickItemData(self.data.cur_sel_gather:Val())

	local cur_sel_gather = self.data.cur_sel_gather
	self.BtnGather.text = self.language.btn_gather[cur_sel_gather.scene_type_1]
	self.GatherTitleSp.SpriteName = FightConfig.GatherTitleSp[cur_sel_gather.scene_type_1]
	self.AnimTool:SetObjActive(false)
	UISpriteAnimTool.AnimName(self.AnimTool, cur_sel_gather.scene_type_1 .. "_")
	UISpriteAnimTool.PicNum(self.AnimTool, FightConfig.GatherPicNum[cur_sel_gather.scene_type_1])
	self.AnimTool:SetObjActive(true)
end

function FightViewGather:FlushDetail()
	local cur_sel_gather = self.data.cur_sel_gather
	self.Title.text = cur_sel_gather._scene.name
	self.Content.text = cur_sel_gather.fb_explain_text
	-- self.Monsters:SetData(cur_sel_gather.monsters)
	self.Rewards:SetData(cur_sel_gather.rewards)
	self.BgMapSp:SetByPath(string.format(FightConfig.PngPath, cur_sel_gather.bg_map))
	self:FlushSkillLevel(cur_sel_gather)
end

function FightViewGather:FlushSkillLevel(cur_sel_gather)
	local skill_id = FightConfig.GatherSkillId[cur_sel_gather.scene_type_1]
	for k, v in pairs(SkillData.Instance.life_skill_list)do
		if skill_id == v.skill_id then
			self.Level.text = string.format(self.language.CurLevel, self.language.SkillName[cur_sel_gather.scene_type_1], v.skill_level)
			return
		end
	end
end

function FightViewGather:FlushEnergy()
	local gather_energy = self.data.gather_energy
	-- self.EnergyProgress:SetProgress(gather_energy.val / gather_energy.total)
	-- self.EnergyProgress:SetText(gather_energy.val .. "/" .. gather_energy.total)
	self.RemainNum.text = string.format(self.language.remine_desc, gather_energy.val / gather_energy.per)
	self.FullObj:SetActive(self.data.gather_energy.val == self.data.gather_energy.total)

	-- self.red_point_gather = self.red_point_gather or UiRedPoint.New(self.BtnGatherObj, Vector3.New(77.7,24,0))
	-- self.red_point_gather:SetNum(self.data.gather_energy.val >= FightConfig.GATHER_ENERGY_REMIND_VAL and 1 or 0)
end

function FightViewGather:OnClickGather()
	if RoleData.Instance:GetRoleLevel() < self.data.cur_sel_gather.level_min then
		PublicPopupCtrl.Instance:Center(ErrorInfo[100008])
		return
	end
	if FightData.Instance:GetGatherEnergy() == 0 then
		ViewMgr:OpenView(ItemConsumeTipView,{item_id = 24391})
		--PublicPopupCtrl.Instance:Center(Language.MainOther.GatherNotTimes)
		return
	end
	if nil ~= self.data.cur_sel_gather.click_handle then
		MainOtherCtrl.Instance:CloseGather()
		FightData.Instance:SetCollectGatherList{}
		self.data.cur_sel_gather.click_handle()
	end
	AutoInvokerCtrl.Instance:ClearCommands()
	GetWayParamFunc.GatherFunc()
end

function FightViewGather:OnClickTips()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.collect_scene_auto.other[1].tip})
end

FightViewGatherTypeItem = DeclareMono("FightViewGatherTypeItem", UIWidgetBaseItem)

function FightViewGatherTypeItem:FightViewGatherTypeItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightViewGatherTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.TypeName.text = data.name
end

function FightViewGatherTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function FightViewGatherTypeItem:OnClickItem()
	self.fight_data.cur_sel_types.gather_type = self.data.type
end

FightViewGatherShowItem = DeclareMono("FightViewGatherShowItem", UIWidgetBaseItem)

function FightViewGatherShowItem:FightViewGatherShowItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightViewGatherShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = data._scene.name
	self.Level.text = string.format(Language.Fight.Level, data.level_min, data.level_max)
	self.CurObj:SetActive(data.value_cur)
	self.RecoObj:SetActive(data.value_reco)
	UH.SetIcon(self.MapIcon, data.map_id, ICON_TYPE.MAP)
	self.MaskObj:SetActive(RoleData.Instance:GetRoleLevel() < data.level_min)
end

function FightViewGatherShowItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function FightViewGatherShowItem:OnClickItem()	
	self.fight_data.cur_sel_gather:Set(self.data)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
end