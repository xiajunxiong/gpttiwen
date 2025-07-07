
FightRareDungeonView = FightRareDungeonView or DeclareView("FightRareDungeonView", FightConfig.ResPath .. "fight_rare_dungeon", Mod.Challenge.RareDungeon)

VIEW_DECLARE_LEVEL(FightRareDungeonView,ViewLevel.L1)

function FightRareDungeonView:FightRareDungeonView()
	self.data = FightCtrl.Instance:Data()
end

function FightRareDungeonView:LoadCallback(param_t)
	self.data.fight_crystal_sweep.has_view_opened = true
end

function FightRareDungeonView:CloseCallback()
end

function FightRareDungeonView:OnClickClose()
	ViewMgr:CloseView(FightRareDungeonView)
end

FightRareDungeonViewF = FightRareDungeonViewF or DeclareMono("FightRareDungeonViewF", UIWFlushPanel)

function FightRareDungeonViewF:FightRareDungeonViewF()
	self.data = FightCtrl.Instance:Data()
    self.team_data = TeamCtrl.Instance:Data()
    self.language = Language.Fight.RareDungeon

	self.data_cares = {
		{data = self.data.fight_rare_dungeon, func = self.FlushInfo, keys = {"info"}},
		{data = self.data.fight_crystal_sweep, func = self.FlushRemainNum},
	}
end

function FightRareDungeonViewF:Awake()
	UIWFlushPanel.Awake(self)

	self.data:FightFBRareDungeonAutoSel()
	local show_list = self.data.ymz_page_list[3]
	self.ShowList:SetData(show_list)
    -- self.ShowList:JumpVerticalIndex(self.ShowList:GetByData(self.data.fight_rare_dungeon.info:Val()),97)
    local click_data = self.data.fight_rare_dungeon.info:Val()
    self.ShowList:ClickItemData(click_data)
    self.ShowList:ForceJumpVertical(97 * (self.ShowList:GetByData(click_data) - 3.5))
end

function FightRareDungeonViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
end

function FightRareDungeonViewF:FlushInfo()
	local info = self.data.fight_rare_dungeon.info
    if not next(info) then return end

	self:FlushMonster(info.monsters[1])

	self.RewardListE:SetData(info.rewards_e)
    self.RewardListM:SetData(info.rewards_m)
    self.RewardListI:SetData(info.rewards_i)

    if nil == info.rewards_first_pass then
        local rewards_first_pass = {}
        local co = Cfg.CrystalConfigBySeq(info.scene_seq)
        if co then
            for i = 0, #co.first_kill_reward do
                local item_co = co.first_kill_reward[i]
                local item_id = item_co.item_id
                local item_data = Item.Init(item_id, item_co.num, item_co.is_bind)
                item_data.old_id = Cfg.RewardJobBagHasId(item_id, false) and item_id or nil
                table.insert(rewards_first_pass, item_data)
            end
        end
        info.rewards_first_pass = rewards_first_pass
    end
    self.RewardListF:SetData(info.rewards_first_pass)

    local is_pass = self.data:IsFightCrystalPassFb(info.scene_seq)
    local is_geted = self.data:IsFightCrystalRewardGetFb(info.scene_seq)
    local is_lock = RoleData.Instance:GetRoleLevel() < info.level_min
    self.GetedObj:SetActive(is_geted)
    self.GetInter:SetObjActive(not is_geted)
    self.GetInter.Interactable = is_pass

    self.LevelTips.text = string.format(self.language.LevelTips, info.level_min)
    self.LevelTips:SetObjActive(is_lock)
    self.RemainNum:SetObjActive(not is_lock)
    self.ButtonsObj:SetActive(not is_lock)

	local video = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_CRYSTAL_FB, info.monster_group)
    if video then
        if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGray, self, video))
        self:FlushGray(video)
        BattleCtrl.BattleReportSimpleInfo(video.monster_group_id, 0)
    end
end

function FightRareDungeonViewF:FlushRemainNum()
    local info = self.data.fight_crystal_sweep
    local num = info.new_xiyou_play_times_remain
    local total = info.new_xiyou_play_times
    self.RemainNum.text = string.format(self.language.RemainNum, RichTextHelper.ColorStr(num, num > 0 and COLORSTR.Green2 or COLORSTR.Red5), total)

    local info = self.data.fight_rare_dungeon.info
    if not next(info) then return end
    local is_pass = self.data:IsFightCrystalPassFb(info.scene_seq)
    local is_geted = self.data:IsFightCrystalRewardGetFb(info.scene_seq)
    self.GetedObj:SetActive(is_geted)
    self.GetInter:SetObjActive(not is_geted)
    self.GetInter.Interactable = is_pass
end

function FightRareDungeonViewF:FlushGray(video)
    local can_play = BattleVideo.Instance:GetSimpleInfo(video.monster_group_id, 0)
	self.VideoGray.enabled = can_play == 0
end

function FightRareDungeonViewF:FlushMonster(monster)
	-- if nil == monster then
	-- 	self.ModelImg.enabled = false
	-- 	return
	-- end
    local co = Cfg.MonsterById(monster.monster_id, false)
	self.MonsterName.text = monster.name
    self.EleAttr:SetData(MonsterData.GetElement(co), false, co.is_chaos == 1)
    local res_path = DrawerHelper.GetNpcPath(ResHelper.MonsterResId(monster.monster_id))
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
        -- self.ModelImg.enabled = false
        uo:SetData(res_path) 
    end
end

-- function FightRareDungeonViewF:ModelCallback()
    -- self.ModelImg.enabled = true
-- end

function FightRareDungeonViewF:OnClickPlayVideo()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	local info = self.data.fight_rare_dungeon.info
    local video = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_ELITE_TRAILS, info.monster_group)
    if video then
        BattleVideo.Instance:PlayVideoCfg(video)
        BattleCtrl.BattleReportDetailInfo(video.monster_group_id)
    end
end

function FightRareDungeonViewF:OnClickTips()
	PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[130].desc)
end

function FightRareDungeonViewF:OnClickTeam()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	local info = self.data.fight_rare_dungeon.info
	local target = self.team_data:GetTeamGoalWithId(info.team_target)
	if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
		ViewMgr:CloseView(FightRareDungeonView)
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
            ViewMgr:CloseView(FightRareDungeonView)
            self.team_data.cur_platform_target_id = info.team_target
            TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
            PublicPopupCtrl.Instance:Center(Language.Team.MatchTips)
        end
    end
    if 0 == self.data.fight_crystal_sweep.new_xiyou_play_times_remain then
        PublicPopupCtrl.Instance:DialogTips{
            content = self.language.XiYouNumTips,
            confirm = {
                func = function ()
                    Call(do_func)
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            tnr = DTTodayNotRemind.FightXYTips,
        }
    else
        Call(do_func)
    end
end

function FightRareDungeonViewF:OnClickFight()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local info = self.data.fight_rare_dungeon.info
	if RoleData.Instance:GetRoleLevel() < info.level_min then
		PublicPopupCtrl.Instance:Center(ErrorInfo[100008])
		return
	end
    if self.team_data:InTeam() and not self.team_data:IsLeader() then
        PublicPopupCtrl.Instance:Center(ErrorText[57])
		return
    end
    local do_func = function ()
        -- TeamCtrl.Instance:TeamDetectByTargetId(info.team_target,function ()
        --     ViewMgr:CloseView(FightRareDungeonView)
        --     if nil ~= info.click_handle then
        --         info.click_handle()
        --     end
        -- end, 3)
        ViewMgr:CloseView(FightRareDungeonView)
        if nil ~= info.click_handle then
            info.click_handle()
        end
    end
    if 0 == self.data.fight_crystal_sweep.new_xiyou_play_times_remain then
        PublicPopupCtrl.Instance:DialogTips{
            content = self.language.XiYouNumTips,
            confirm = {
                func = function ()
                    Call(do_func)
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            tnr = DTTodayNotRemind.FightXYTips,
        }
    else
        Call(do_func)
    end
end

function FightRareDungeonViewF:OnClickGet()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local info = self.data.fight_rare_dungeon.info
    local is_pass = self.data:IsFightCrystalPassFb(info.scene_seq)
    if is_pass then
        FightCtrl.Instance:SendCrystalFbReq(4, info.scene_seq)
    else
        local monster = info.monsters[1]
        if monster then
            PublicPopupCtrl.Instance:Center(string.format(self.language.FirstPassTips, monster.name))
        end
    end
end

FightRareDungeonViewItem = DeclareMono("FightRareDungeonViewItem", UIWidgetBaseItem)

function FightRareDungeonViewItem:FightRareDungeonViewItem()
	self.fight_data = FightCtrl.Instance:Data()
    self.language = Language.Fight.RareDungeon
end

function FightRareDungeonViewItem:OnDestroy()
	self:uncareData()
end

function FightRareDungeonViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self:uncareData()
    self.care_handles = {}
    self:careData(self.fight_data.fight_crystal_sweep, BindTool.Bind(self.ItemRedPoint, self))

	-- self.Name.text = data.scene_name
    local monster = data.monsters[1]
    if monster then
        self.Name.text = monster.name
        UH.SetIcon(self.MapIcon, monster.icon_id, ICON_TYPE.ITEM)
    end
	-- self.Level.text = string.format(Language.Fight.Level2, data.level_min, data.level_max)
    self.Level.text = string.format(self.language.LevelShow, DataHelper.GetDaXie(data.show_index))
	-- UH.SetIcon(self.MapIcon, data.map_id, ICON_TYPE.MAP)
	local is_lock = RoleData.Instance:GetRoleLevel() < data.level_min
    -- self.HardObj:SetActive(RoleData.Instance:GetRoleScore() < (data.recommend_score or 0))
	self.MaskObj:SetActive(is_lock)
    self.LockObj:SetActive(is_lock)
end

function FightRareDungeonViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
    end
end

function FightRareDungeonViewItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
	if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
        self.fight_data.fight_rare_dungeon.info:Set(self.data)
    end
end

function FightRareDungeonViewItem:ItemRedPoint()
    local is_pass = self.fight_data:IsFightCrystalPassFb(self.data.scene_seq)
    local is_geted = self.fight_data:IsFightCrystalRewardGetFb(self.data.scene_seq)
    self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemObj, Vector3.New(120,40,0))
	self.red_point_item:SetNum(is_pass and not is_geted and 1 or 0)
end