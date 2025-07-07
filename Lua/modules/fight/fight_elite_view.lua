
local FightEiltePageToIndex = {
    [2] = 1,
    [5] = 2,
}

FightEliteView = FightEliteView or DeclareView("FightEliteView", FightConfig.ResPath .. "fight_elite", Mod.Challenge.FightElite)

VIEW_DECLARE_LEVEL(FightEliteView,ViewLevel.L1)

function FightEliteView:FightEliteView()
	self.data = FightCtrl.Instance:Data()
end

function FightEliteView:LoadCallback(param_t)
end

function FightEliteView:CloseCallback()
end

function FightEliteView:OnClickClose()
	ViewMgr:CloseView(FightEliteView)
end

FightEliteViewF = FightEliteViewF or DeclareMono("FightEliteViewF", UIWFlushPanel)

function FightEliteViewF:FightEliteViewF()
	self.data = FightCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Fight.EliteDungeon

	self.data_cares = {
		{data = self.data.fight_elite_dungeon, func = self.FlushInfo, keys = {"info"}},
		{data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushCosume},
	}
end

function FightEliteViewF:Awake()
	UIWFlushPanel.Awake(self)
    -- Mod.Challenge.FightElite
    -- Mod.Challenge.FightElite2
    local is_110 = RoleData.Instance:GetRoleLevel() >= Config.shengqi_auto.other[1].open_level
    local is_fly = FlyUpData.Instance:GetStage() >= Config.shengqi_auto.other[1].feisheng_type
    self.TypeObj:SetActive(is_110)
    self.ShowListPos.sizeDelta = Vector2.New(302, is_110 and 545 or 617)
    self.Block2:SetActive(not is_fly)
    local page = self.data:GetFightElitePage()
    if is_110 then
        self.TypeTog2.Interactable = is_fly
        self.TypeList[FightEiltePageToIndex[page] or 1].isOn = true
    end
    self:OnToggleType(page)
end

function FightEliteViewF:FlushShow(page)
    local show_list, click_item = self.data:GetFightEliteList(page)
	self.ShowList:SetData(show_list)
	self.ShowList:ClickItemData(click_item)
    self.ShowList:ForceJumpVertical(111 * (self.ShowList:GetByData((click_item)) - 1))
    self.data.fight_elite_dungeon.info:Set(click_item)
end

function FightEliteViewF:FlushInfo()
	local info = self.data.fight_elite_dungeon.info
    if not next(info) then return end
	-- self.RewardListE:SetData(info.rewards_e)
    self.RewardListM:SetData(info.rewards_m)
    self.RewardListI:SetData(info.rewards_i)
	self.Monsters:SetData(info.monsters)

	local have_num = BagData.Instance:GetNumByItemId(info.expend_item)
	self.CusumeINum.text = string.format(self.language.ItemCosume, have_num > 0 and COLORSTR.Green3 or COLORSTR.Red8, have_num)
	UH.SetIcon(self.CusumeIIcon, info.consume:IconId())

	self.EquipDesc.text = info.equip_desc
	if info.crystal > 0 then
        self.EleAttr:SetData(EleCrystal.GetElement(info.crystal))
    end
	self.EleAttr:SetObjActive(info.crystal > 0)
end


function FightEliteViewF:FlushCosume()
	local num = self.data:FightFBYMZHaveNumAllElite()
    self.HaveNumAll.text = string.format(self.language.HaveNumAll, math.min(num, 99), num > 99 and "+" or "")
end

function FightEliteViewF:OnToggleType(value)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self:FlushShow(value)
    self:FlushModelShow(value)
end

function FightEliteViewF:OnClickBlock2(value)
    PublicPopupCtrl.Instance:Center(self.language.Block2Tips)
end

function FightEliteViewF:FlushModelShow(value)
    local res_path = DrawerHelper.GetNpcPath(2 == value and 1152 or 1151)
    local uo = self.ModelShow:UiObj()
    if uo == nil then
        uo = UIObjDrawer.CreateByPath(res_path)
        self.ModelShow:SetShowData(
            {
                ui_obj = uo,
                view = FightRareDungeonView,
                -- load_callback = BindTool.Bind(self.ModelCallback, self),
            }
        )
    else
        uo:SetData(res_path) 
    end
end

function FightEliteViewF:OnClickTeam()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	local info = self.data.fight_elite_dungeon.info
	local target = self.team_data:GetTeamGoalWithId(info.team_target)
	if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
		ViewMgr:CloseView(FightEliteView)
		PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
		return
	end
	if RoleData.Instance:GetRoleLevel() < target.low_level then
		PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
		return
	end
    local do_func = function ()
        if self.team_data:InTeam() then
            TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
            TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
            ViewMgr:OpenView(TeamView)
        else
            ViewMgr:CloseView(FightEliteView)
            self.team_data.cur_platform_target_id = info.team_target
            TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
            PublicPopupCtrl.Instance:Center(Language.Team.MatchTips)
        end
    end
    Call(do_func)
end

function FightEliteViewF:OnClickFight()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local info = self.data.fight_elite_dungeon.info
	if RoleData.Instance:GetRoleLevel() < info.level_min then
		PublicPopupCtrl.Instance:Center(ErrorInfo[100008])
		return
	end
    if info.ymz_type > 1 then
        local do_func = function ()
            if BagData.Instance:GetNumByItemId(info.expend_item) >= 1 then
                ViewMgr:CloseView(FightEliteView)
                if nil ~= info.click_handle then
                    if 5 == info.ymz_type then
                        self.data:SetFightEliteWaitFunc(info.click_handle)
                    else
                        info.click_handle()
                    end
                end
            else
                -- local item_cfg = Item.GetConfig(info.expend_item)
                -- PublicPopupCtrl.Instance:Center(string.format(Language.RichText.ShowStr.item_not_enough_fb4, ColorStr(item_cfg.name,QualityColorStr[item_cfg.color])))
                ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = info.expend_item})})
            end
        end
        Call(do_func)
    else
        if nil ~= info.click_handle then
            info.click_handle()
        end
    end
end

function FightEliteViewF:OnClickCastIcon()
    local info = self.data.fight_elite_dungeon.info
    if info and info.expend_item then
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = info.expend_item})})
    end
end

FightEliteViewItem = DeclareMono("FightEliteViewItem", UIWidgetBaseItem)

function FightEliteViewItem:FightEliteViewItem()
	self.fight_data = FightCtrl.Instance:Data()
	self.language = Language.Fight.EliteDungeon
end

function FightEliteViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	-- self.IconSp.SpriteName = data.icon_index
	self.IconSp.SpriteName = data.scene_seq
	self.Name.text = data.scene_name
	-- self.Level.text = string.format(self.language.Level, self.data.level_min)
	self.Level.text = self.data.difficulty_level
    self.Fly:SetActive(self.data.is_feisheng == FlyUpStageType.High)
end

function FightEliteViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function FightEliteViewItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        self.fight_data.fight_elite_dungeon.info:Set(self.data)
    end
end