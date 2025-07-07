

FightFBViewPetGod = FightFBViewPetGod or DeclareMono("FightFBViewPetGod", UIWFlushPanel)

function FightFBViewPetGod:FightFBViewPetGod()
	self.data = FightCtrl.Instance:Data()
    self.language = Language.Fight.PetGod
    
    self.data:ResetPetGoldInfoType()

	self.data_cares = {
		{data = self.data.pet_god_info, func = self.FlushType, keys = {"type"}},
		{data = self.data.pet_god_info, func = self.FlushInfo, keys = {"info"}},
		-- {data = self.data.pet_god_info, func = self.FlushTimes, keys = {"times"}},
		-- {data = RoleData.Instance:GetBaseData(), func = self.FlushRoleLevelChange, keys = {"level"}},
    }

end

function FightFBViewPetGod:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushItemsShow()
    self:FlushRewards()
    self:FlushRewardDescs()
end

function FightFBViewPetGod:IsLoaded()
    return true
end

-- function FightFBViewPetGod:ChekcSimularIphone()
--     if IS_IPHONEX or EditorSettings:SimularIphoneX() then
--         self.PanelRect:SetSizeWithCurrentAnchors(0, self.PanelRect.rect.width - 100)
--     end
-- end

function FightFBViewPetGod:FlushItemsShow()
    for i = 1, self.ShowItems:Length() do
        self.ShowItems[i]:SetData(self.data.pet_god_list[i])
    end
end

function FightFBViewPetGod:FlushRewards()
    for i = 1, self.RewardLists:Length() do
        self.RewardLists[i]:SetData(self.data.pet_god_list[i].rewards)
    end
end

function FightFBViewPetGod:FlushRoleLevelChange()
	-- self:FlushExpShow()
    -- self:FlushRewardDescs()
end

function FightFBViewPetGod:FlushType()
    local type = self.data.pet_god_info.type
    -- self.TipsObj:SetActive(0 == type)
    self.TipsObj:SetActive(false)
	self:FlushInfo()
end

function FightFBViewPetGod:FlushInfo()
	self:FlushExpShow()
    -- self:FlushEnergy()
end

function FightFBViewPetGod:FlushExpShow()
    local type = self.data.pet_god_info.type
    -- self.ExpDesc:SetActive(0 == type)
    -- self.ExpShow:SetObjActive(type > 0)
    -- if 0 == type then return end
    -- local info = self.data.pet_god_info.info
    -- local exp_num = self.data:GetFightPetGodExpNum(self.data.pet_god_info.info.seq)
	-- self.ExpShow.text = string.format(self.language.ExpShow, exp_num > 0 and exp_num or "")
end

function FightFBViewPetGod:FlushRewardDescs()
	for i = 1, self.RewardDescs:Length() do
        local info = self.data.pet_god_type_level_list[i]
        self.RewardDescs[i].text = string.format(self.language.RewardDesc, info and info.chapter_name or "")
    end
end

-- function FightFBViewPetGod:FlushEnergy()
-- 	self.EnergyNum.text = self.data.pet_god_info.info.power_expend
-- end

-- function FightFBViewPetGod:FlushTimes()
--     local num = self.data.pet_god_info.times.day_remain_time
-- 	self.RecordNum.text = string.format(self.language.RecordNum, RichTextHelper.ColorStr(math.max(num, 0), num > 0 and COLORSTR.Green7 or COLORSTR.Red10))
-- end

function FightFBViewPetGod:OnClickBuyTimes()
    local info = self.data.pet_god_info.info
    local extra_buy_times = self.data:GetPetGodExtraBuyTimes()
    local num = extra_buy_times - (self.data.pet_god_info.times.day_buy_times or 0)
    if num > 0 then
        PublicPopupCtrl.Instance:AlertTip(string.format(self.language.BuyTimesTip, self.data:GetPetGodExtraBuyConsume(), math.max(num, 0)),function()
            FightCtrl.Instance:SendPetGodFBReq(0)
        end, CurrencyType.Gold)
    else
        FightCtrl.Instance:SendPetGodFBReq(0)
    end
end

function FightFBViewPetGod:OnClickFight()
    local info = self.data.pet_god_info.info
    local times = self.data.pet_god_info.times
	SceneCtrl.Instance:RequestEnterFb(FB_TYPE.PET_GOD, info.seq)
    if times.day_remain_time > 0 then
        ViewMgr:CloseView(FightFBView)
    end
end

FightFBViewPetGodItem = DeclareMono("FightFBViewPetGodItem", UIWidgetBaseItem)

function FightFBViewPetGodItem:FightFBViewPetGodItem()
    self.fight_data = FightCtrl.Instance:Data()
    -- self.language = Language.Fight.PetGod
end

function FightFBViewPetGodItem:OnDestroy()
    self:uncareData()
end

function FightFBViewPetGodItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
    self.care_handles = {}
    self:careData(self.fight_data.pet_god_info, BindTool.Bind(self.FlushSel, self), "type")

    self:FlushShow()
end

function FightFBViewPetGodItem:FlushShow()
    local co = NpcHelper.NpcCo(self.data.npc_id)
    if co then
        UH.SetIcon(self.IconSp, co.npc_icon, ICON_TYPE.ITEM)
        self.Name.text = co.name
    end
end

function FightFBViewPetGodItem:FlushSel()
    local is_sel = self.data.seq == self.fight_data.pet_god_info.type
    self.SelObj:SetActive(is_sel)
end

function FightFBViewPetGodItem:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.fight_data.pet_god_info.type = self.data.seq
    ViewMgr:OpenView(PetGodInfoView)
end

FightFBPetGodMenuView = FightFBPetGodMenuView or DeclareMono("FightFBPetGodMenuView", UIWFlushPanel)

function FightFBPetGodMenuView:FightFBPetGodMenuView()
    self.data = FightCtrl.Instance:Data()
    self.language = Language.Fight.PetGod

    self.data_cares = {
		{data = self.data.pet_god_info, func = self.FlushScene, keys = {"scene"}},
    }
end

function FightFBPetGodMenuView:FlushScene()
    local scene = self.data.pet_god_info.scene
    local info = self.data.pet_god_seq_list[scene.fb_seq]
    if nil == info then return end
    self.Name.text = info.level_name
    self.Desc.text = string.format(self.language.BossTip, info.chapter_name)
    -- local co = self.data:GetFightPetGodNpcInfo(info.seq)
    -- if co then
    --     self.Desc.text = string.format(self.language.BossTip, co.name)
    -- end

    self.Grid:SetData(info.rewards)
end

function FightFBPetGodMenuView:OnClickEnter()
    local scene = self.data.pet_god_info.scene
    local info = self.data.pet_god_seq_list[scene.fb_seq]
    if nil == info then return end
    SceneLogic.Instance:AutoToPos(nil, {x = info.x, y = info.y},function ()
		FightData.Instance:AutoToNpc()
	end,true,false)
end

PetGodInfoView = PetGodInfoView or DeclareView("PetGodInfoView", FightConfig.ResPath .. "pet_god_info")

function PetGodInfoView:PetGodInfoView()
	self.data = FightCtrl.Instance:Data()
end

function PetGodInfoView:LoadCallback()
    local type = self.data.pet_god_info.type
    local info = self.data.pet_god_type_level_list[type]
	self.Board:SetData(self:GetType(), info and info.chapter_name or "", Vector2.New(740, 560))
end

function PetGodInfoView:CloseCallback()
	self.data.pet_god_info.type = 0
    self.data:ResetPetGoldInfoInfo()
end

PetGodInfoViewF = PetGodInfoViewF or DeclareMono("PetGodInfoViewF", UIWFlushPanel)

function PetGodInfoViewF:PetGodInfoViewF()
	self.data = FightCtrl.Instance:Data()
    self.language = Language.Fight.PetGod

	self.data_cares = {
		{data = self.data.pet_god_info, func = self.FlushInfo, keys = {"info"}, init_call = false},
    }
end

function PetGodInfoViewF:Awake()
	UIWFlushPanel.Awake(self)

    self:FlushTypeList()
end

function PetGodInfoViewF:FlushTypeList()
	local type = self.data.pet_god_info.type
    local list = self.data.pet_god_type_level_list[type]
    local sel_info = self.data:FightFBPetGodInfoAutoSel()
    self.TypeList:SetData(list.levels)
    self.TypeList:ClickItemData(sel_info)
    self.TypeList:AutoSlide(sel_info, false)
end

function PetGodInfoViewF:FlushInfo()
	local info = self.data.pet_god_info.info
    self.Rewards:SetData(info.rewards)
    self.EnergyNum.text = info.power_expend
    for i = 1, self.Monsters:Length() do
        self.Monsters[i]:SetData(info.monsters[i])
    end
    local max_level = self.data.pet_god_info.max_levels[info.fb_type] or 0
    local next_level = max_level + 1
    self.BtnFightInter.Interactable = next_level >= info.level
    self.BtnFight.text = next_level >= info.level and self.language.BtnFight or self.language.BtnFightGrey
    self.CapReco.text = string.format(self.language.CapReco, RichTextHelper.ColorStr(RoleData.Instance:GetSumScore(), RoleData.Instance:GetSumScore() < info.tuijian_att and COLORSTR.Red8 or COLORSTR.Green7), info.tuijian_att)
    self.CapSweep.text = RoleData.Instance:GetSumScore() < info.saodang_zhanli and string.format(self.language.CapSweep, info.saodang_zhanli) or ""
    self.BtnSweepInter.Interactable = RoleData.Instance:GetSumScore() >= info.saodang_zhanli and max_level >= info.level
    self.TipsImage.enabled = RoleData.Instance:GetSumScore() < info.saodang_zhanli
    self.BtnRateObj:SetActive(info.drop_rate_show > 0)

    local exp_num = self.data:GetFightPetGodExpNum(info.seq)
    self.ExpShow.text = string.format(self.language.ExpShow, exp_num > 0 and exp_num or "")
end


function PetGodInfoViewF:OnClickFight()
    local info = self.data.pet_god_info.info
    SceneCtrl.Instance:RequestEnterFb(FB_TYPE.PET_GOD, info.seq)
    local max_level = self.data.pet_god_info.max_levels[info.fb_type] or 0
    local next_level = max_level + 1
    if next_level >= info.level then
        if self.data:GetFightStrengthNum() < info.power_expend then
            if self.data:GetSweepBuyTimes() - self.data.fight_crystal_sweep.buy_times > 0 then
                ViewMgr:OpenView(FightCrystalSweepItemView)
            end
        else
            ViewMgr:CloseView(PetGodInfoView)
            ViewMgr:CloseView(FightFBView)
        end
    end
end

function PetGodInfoViewF:OnClickSweep()
    local info = self.data.pet_god_info.info
    FightCtrl.Instance:SendPetGodFBReq(1, info.seq)
    local max_level = self.data.pet_god_info.max_levels[info.fb_type] or 0
    local next_level = max_level + 1
    if max_level >= info.level and RoleData.Instance:GetSumScore() >= info.saodang_zhanli then
        if self.data:GetFightStrengthNum() < info.power_expend then
            if self.data:GetSweepBuyTimes() - self.data.fight_crystal_sweep.buy_times > 0 then
                ViewMgr:OpenView(FightCrystalSweepItemView)
            end
        end
    end
end

function PetGodInfoViewF:OnClickRate()
    local info = self.data.pet_god_info.info
    ViewMgr:OpenView(TreasureHuntRateView, {data_list = self.data.pet_god_rate_list[info.drop_rate_show]})
end

PetGodInfoViewTypeItem = DeclareMono("PetGodInfoViewTypeItem", UIWidgetBaseItem)

function PetGodInfoViewTypeItem:PetGodInfoViewTypeItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function PetGodInfoViewTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    local max_level = self.fight_data.pet_god_info.max_levels[data.fb_type] or 0
    local next_level = max_level + 1
	self.TypeName.text = data.level_name
    self.ItemInter.Interactable = next_level >= data.level
end

function PetGodInfoViewTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function PetGodInfoViewTypeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
	if selected then
        self.fight_data.pet_god_info.info:Set(self.data)
    end
end

PetGodInfoViewMonsterItem = DeclareMono("PetGodInfoViewMonsterItem", UIWidgetBaseItem)

function PetGodInfoViewMonsterItem:PetGodInfoViewMonsterItem()
    self.fight_data = FightCtrl.Instance:Data()
    -- self.language = Language.Fight.PetGod
end

function PetGodInfoViewMonsterItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    local is_empty = nil == data
    self.ItemObj:SetActive(not is_empty)
    if is_empty then return end
    UH.SetIcon(self.IconSp, data.icon_id, ICON_TYPE.ITEM)
end
