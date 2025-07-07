RushDungeonMenu =
    RushDungeonMenu or DeclareView("RushDungeonMenu", "rush_dungeon/rush_dungeon_menu", Mod.RushDungeon.Main)

RushDungeonMenuPanel = RushDungeonMenuPanel or DeclareMono("RushDungeonMenuPanel", UIWFlushPanel)
function RushDungeonMenuPanel:RushDungeonMenuPanel()
    self.data = RushDungeonData.Instance
    self.data_cares = {
        {data = self.data:RankInfo(), func = self.FlushRank, init_call = true},
        {data = self.data.mop_up_info, func = self.FlushReward, init_call = false}
    }
end

function RushDungeonMenuPanel:onFlush()
    RushDungeonCtrl.Instance:SendBraveGroundReq(
        {
            req_type = BRAVE_GROUND_REQ_TYPE.RANK_INFO
        }
    )
    --[[ local info = self.data.mop_up_info
    if info.level > 0 then
        local can_get = info.is_get == 0
        self.Rewards[1]:SetActive(can_get)
        self.Rewards[2]:SetActive(can_get)
        info.show_level = info.level
        info.show_index = info.index
    else
        info.show_level = self.data.mop_up_list[1].level
        info.show_index = 1
    end
    self:FlushReward() ]]
    UH.SetText(self.RewardTex[2], Language.Activity.RewardPreview)
end
function RushDungeonMenuPanel:FlushReward()
    local level = self.data:GetOpenLevel()
    UH.SetText(self.OpenLevel, string.format(Language.RushDungeon.OpenLevel, level))
    local info = self.data.mop_up_info
    if self.handle1 then
        self.EffectTool:Stop(self.handle1)
        self.handle1 = nil
    end
    if self.handle2 then
        self.EffectTool:Stop(self.handle2)
        self.handle2 = nil
    end
    if info.level <= 0 then
        self.RewardTex[1]:SetActive(false)
        UH.SetText(self.RewardDesc, Language.RushDungeon.RewardTips[1])
    else
        self.RewardTex[1]:SetActive(true)
        local can_get = info.is_get == 0
        UH.SetText(self.RewardTex[2], Language.RushDungeon.RewardTips[can_get and 3 or 2])
        UH.SetText(self.RewardDesc, Language.RushDungeon.RewardTips[can_get and 1 or 2])
        self.Grays[1].enabled = not can_get
        self.Grays[2].enabled = not can_get
        if can_get then
            self.handle1 = self.EffectTool:Play(100108178)
            self.handle2 = self.EffectTool:Play(100108179)
        end
    end

    local lock = info.level < info.show_level
    self.LockTip:SetActive(lock)
    self.BtnGet:SetActive(not lock)

    local cfg = self.data:GetLevelCfg(info.show_level)
    UH.SetText(self.RewardTitle, string.format(Language.RushDungeon.RewardTitle, cfg.role_level, cfg.level, cfg.level))
    local list = self.data:GetMopUpReward()
    self.RewardList:SetData(list)
    self:FlushArrow()
end
function RushDungeonMenuPanel:OnClickLeft()
    self.data:FlushShowReward(-1)
    self:FlushArrow()
end
function RushDungeonMenuPanel:OnClickRight()
    self.data:FlushShowReward(1)
    self:FlushArrow()
end
function RushDungeonMenuPanel:FlushArrow()
    local index = self.data:GetModMaxIndex()
    if self.data.mop_up_info.show_index >= index then
        self.RightArrow:SetActive(false)
        self.LeftArrow:SetActive(true)
    elseif self.data.mop_up_info.show_index <= self.data.mop_up_info.index then
        self.RightArrow:SetActive(true)
        self.LeftArrow:SetActive(false)
    else
        self.RightArrow:SetActive(true)
        self.LeftArrow:SetActive(true)
    end
end
function RushDungeonMenuPanel:OnClickGet()
    --改成打开界面
    ViewMgr:OpenView(RushDungeonReward)
    --[[ self.Rewards[1]:SetActive(not self.Rewards[1].activeSelf)
    self.Rewards[2]:SetActive(not self.Rewards[2].activeSelf)
    if self.Rewards[1].activeSelf then
        local info = self.data.mop_up_info
        if info.level > 0 then
            info.show_level = info.level
            info.show_index = info.index
        else
            info.show_level = self.data.mop_up_list[1].level
            info.show_index = 1
        end
    end ]]
end
function RushDungeonMenuPanel:OnClickGetReward()
    local info = self.data.mop_up_info
    if info.level <= 0 then
        PublicPopupCtrl.Instance:Center(Language.RushDungeon.GetTips[1])
    elseif info.is_get == 1 then
        PublicPopupCtrl.Instance:Center(Language.RushDungeon.GetTips[2])
    else
        -- LogError("发送领取扫荡奖励协议")
        RushDungeonCtrl.Instance:SendBraveGroundReq(
            {
                req_type = BRAVE_GROUND_REQ_TYPE.SAO_DANG
            }
        )
    end
end
function RushDungeonMenuPanel:FlushRank()
    local level = self.data:GetOpenLevel()
    UH.SetText(self.OpenLevel, string.format(Language.RushDungeon.OpenLevel, level))
    local data = self.data:RankInfo().rank_list
    local count = table.nums(data)
    self.NoneObj:SetActive(count == 0)
    local his_level = self.data:GetHistoryLevel()
    self.BottomList[1].text = string.format(Language.RushDungeon.BottomDesc[1], 0)
    self.BottomList[2].text = string.format(Language.RushDungeon.BottomDesc[2], his_level)
    self.BottomList[3].text = string.format(Language.RushDungeon.BottomDesc[3], 0)
    self.List:SetData(data)
    if count == 0 then
        return
    end
    local role_id = RoleData.Instance:GetRoleId()
    local cur_level = 0
    local cur_rank = 9999
    for _, v in ipairs(data) do
        if v.role_id == role_id then
            cur_level = math.max(cur_level, v.reached_level)
            cur_rank = math.min(cur_rank, v.rank)
        end
        --[[ for i = 1, v.role_info_count do
            if v.role_info[i].role_id == role_id then
                cur_level = math.max(cur_level, v.reached_level)
                cur_rank = math.min(cur_rank, v.rank)
            end
        end ]]
    end
    self.BottomList[1].text = string.format(Language.RushDungeon.BottomDesc[1], cur_level)
    if cur_rank ~= 9999 then
        self.BottomList[3].text = string.format(Language.RushDungeon.BottomDesc[3], cur_rank)
        local rewardInfo = RushDungeonData.Instance:GetRankReward(cur_rank)
        local temp = {}
        for _, value in pairs(rewardInfo.reward_show) do
            table.insert(temp, Item.Create(value))
        end
        self.CellList:SetData(temp)
    else
        self.BottomList[3].text = string.format(Language.RushDungeon.BottomDesc[3], 0)
    end
   
end

function RushDungeonMenuPanel:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[85].desc})
end

function RushDungeonMenuPanel:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(RushDungeonMenu)
end

function RushDungeonMenuPanel:OnClickGoto()
    AudioMgr:PlayEffect(AudioType.UI, "login_start_click")
    ViewMgr:CloseView(RushDungeonMenu)
    BehaviorCtrl.Exe(90)
end

function RushDungeonMenuPanel:OnClickTeam()
    ViewMgr:CloseView(RushDungeonMenu)
    local target = TeamData.Instance.team_goal_list[7]
    if RoleData.Instance:GetRoleLevel() < target.low_level then
        PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
        return
    end
    if TeamData.Instance:InTeam() then
        ViewMgr:OpenView(TeamView)
        TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
        TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
    else
        TeamData.Instance.cur_platform_target_id = target.target_id
        TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
    end
end

RushDungeonMenuItem = RushDungeonMenuItem or DeclareMono("RushDungeonMenuItem", UIWidgetBaseItem)

function RushDungeonMenuItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.BgSp, data.rank % 2 == 0 and "White" or "Yellow")
    UH.SetText(self.LevelTex, data.reached_level)
    if data.rank <= 3 then
        self.RankArray[data.rank]:SetActive(true)
    else
        UH.SetText(self.RankArray[4], data.rank)
    end
    --[[ local str = ""
    for i = 1, data.role_info_count do
        if data.role_info[i].role_id == RoleData.Instance:GetRoleId() then
            str = str .. ColorStr(data.role_info[i].role_name, COLORSTR.Yellow22) .. " "
        else
            str = str .. data.role_info[i].role_name .. " "
        end
    end ]]
    UH.SetText(self.NamesTex, data.role_name)

    local rewardInfo = RushDungeonData.Instance:GetRankReward(data.rank)
    local temp = {}
    for key, value in pairs(rewardInfo.reward_show) do
        table.insert(temp, Item.Create(value))
    end
    self.CellList:SetData(temp)
end
