AllSoulsStorybook = AllSoulsStorybook or DeclareView("AllSoulsStorybook","all_souls_storybook/all_souls_storybook",Mod.AllSoulsStorybook.Main)
function AllSoulsStorybook:AllSoulsStorybook()
    GuideManager.Instance:RegisterGetGuideUi("AllSoulsStorybookItem", function()
        if self.List and self.List:GetItem(1) ~= nil then
            return self.List:GetItem(1)
        end
    end)
    
    GuideManager.Instance:RegisterGetGuideUi("AllSoulsStorybookBtn", function()
        if self.BtnGoto ~= nil then
            return self.BtnGoto
        end
    end)
end
function AllSoulsStorybook:LoadCallback()

end
function AllSoulsStorybook:CloseCallback()
    GuideManager.Instance:UnRegisterGetGuideUi("AllSoulsStorybookItem")
    GuideManager.Instance:UnRegisterGetGuideUi("AllSoulsStorybookBtn")
end
-----------------------------
AllSoulsStorybookPanel = AllSoulsStorybookPanel or DeclareMono("AllSoulsStorybookPanel", UIWFlushPanel)
function AllSoulsStorybookPanel:AllSoulsStorybookPanel()
    self.data = AllSoulsStorybookData.Instance
    self.select_data = nil
    self.data_cares = {
        {data = self.data:FBInfo(), func = self.FlushReward, keys = {"fetch"}},
        {data = self.data:FBInfo(), func = self.FlushTimes, keys = {"pass"}},
        {data = self.data:FBInfo(), func = self.FlushTimes, keys = {"boss_fetch"}},
    }
end

function AllSoulsStorybookPanel:onFlush()
    self:FlushList()
end

function AllSoulsStorybookPanel:FlushReward()
    --获取是否勾选
    if self.select_data then
        if RoleData.Instance:GetRoleLevel() < self.select_data.open_level then
            self.RewardToggle.isOn = false
            self.TipsObj:SetActive(false)
        else
            local flag = self.data:IsFetch(self.select_data.fb_index)
            self.TipsObj:SetActive(true)
            self.RewardToggle.isOn = not flag
        end
        local cfg 
        for i = 1, self.BossIcons:Length() do
            cfg = Cfg.NpcById(self.select_data["Boss_"..i])
            if cfg then
                UH.SetIcon(self.BossIcons[i], cfg.npc_icon)
            end
        end
    else
        self.TipsObj:SetActive(true)
        self.RewardToggle.isOn = false
    end
    
end
function AllSoulsStorybookPanel:FlushTimes()

    for i = 1, self.BossTimes:Length() do
        UH.SetText(self.BossTimes[i], self.data:IsFetchBoss(i) and Language.AllSoulsStorybook.Times2 or Language.AllSoulsStorybook.Times1)
    end
    --奖励次数
    -- local total = self.data:RewardTime()
    -- -- local pass = total - self.data:FBInfo().pass
    -- local pass = self.data:FBInfo().pass >= total and total or self.data:FBInfo().pass
    -- UH.SetText(self.Times, string.format(Language.AllSoulsStorybook.RewardTime, pass, total))
    -- self.MaxObj:SetActive(pass >= total)
end

function AllSoulsStorybookPanel:OnClickFetch()
    if self.select_data == nil then
        return
    end
    local flag = self.data:IsFetch(self.select_data.fb_index)
    AllSoulsStorybookCtrl.FetchSet(self.select_data.fb_index, flag and 1 or 0)
end

function AllSoulsStorybookPanel:FlushList()
    self.List:SetCompleteCallBack(function()self.List:JumpVerticalIndex(self:GetIndex(), 157)end)
    self.List:SetDataList(self.data:FBList(),self:GetIndex())
end

function AllSoulsStorybookPanel:GetIndex()
    -- if GuideManager.Instance:IsGuide() then
    --     return 1
    -- end
    return self.data:GetAutoSelectIndex()
end

function AllSoulsStorybookPanel:OnClickItem(data)
    UH.SetText(self.Name, data.fb_name)
    UH.SetText(self.Level, string.format(Language.Common.Levels, data.open_level))
    UH.SetText(self.Desc, data.fb_dec2)
    self.ListReward:SetData(data.reward_list)
    self.select_data = data
    self:FlushReward()
end
function AllSoulsStorybookPanel:OnClickClose()
    ViewMgr:CloseView(AllSoulsStorybook)
end
function AllSoulsStorybookPanel:OnClickMatch()
    if self.select_data.open_level > RoleData.Instance:GetRoleLevel() then
        PublicPopupCtrl.Instance:Center(Language.AllSoulsStorybook.RoleLevelNotEnough)
        return
    end
    -- if TeamData.Instance:IsAutoMatch() and TeamData.Instance.cur_platform_target_id == self.select_data.target_id then
    --     ViewMgr:OpenView(TeamView)
    --     return
    -- end
    if TeamData.Instance:InTeam() then
        if TeamData.Instance:IsLeader() then
            self:GotoMatch()
        else
            ViewMgr:OpenView(TeamView)
        end
    else
        TeamData.Instance.cur_platform_target_id = self.select_data.target_id
        TeamData.Instance.auto_sel_target = true
        ViewMgr:OpenView(TeamPlatformView)
    end
end
function AllSoulsStorybookPanel:GotoMatch()
    local target = TeamData.Instance:GetTeamGoalWithId(self.select_data.target_id)
    TeamCtrl.Instance:SendTeamMatch(TeamData.Instance.cur_platform_target_id, TeamConfig.CrossType.is_cross, TeamConfig.MatchType.cancel)
    TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
    TeamData.Instance.cur_platform_target_id = self.select_data.target_id
    TeamCtrl.Instance:SendTeamMatch(self.select_data.target_id, TeamConfig.CrossType.is_cross, TeamConfig.MatchType.start)
    ViewMgr:OpenView(TeamView)
end
function AllSoulsStorybookPanel:OnClickGoto()
    if self.select_data == nil then
        return
    end
    if self.select_data.open_level > RoleData.Instance:GetRoleLevel() then
        PublicPopupCtrl.Instance:Center(Language.AllSoulsStorybook.RoleLevelNotEnough)
        return
    end
    if not TeamData.Instance:InTeam() then
        PublicPopupCtrl.Instance:Center(string.format(Language.Team.NeedNumAndTeam, self.data.other.team_limit))
        -- self:GotoMatch()
        TeamData.Instance.cur_platform_target_id = self.select_data.target_id
        TeamCtrl.Instance:SendTeamMatch(self.select_data.target_id, TeamConfig.CrossType.is_cross, TeamConfig.MatchType.start)
        MainViewData.Instance:ToggleTTTeam()
        return
    end
    if TeamData.Instance:MemberNumRole() < self.data.other.team_limit then
        PublicPopupCtrl.Instance:Center(Language.Team.NumberNotEnoughShiTuFB)
        self:GotoMatch()
        return
    end
    --组队且满足人数
    SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ALL_SOULS_FB, self.select_data.fb_index)
    ViewMgr:CloseView(AllSoulsStorybook)
end
AllSoulsStorybookFBItem = AllSoulsStorybookFBItem or DeclareMono("AllSoulsStorybookFBItem", UIWidgetBaseItem)
function AllSoulsStorybookFBItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.BgSp, data.pic_info)
    UH.SetText(self.Name, data.fb_name)
    UH.SetText(self.Level, string.format(Language.Common.Levels, data.open_level))
    UH.SetText(self.Desc, data.fb_dec1)
    self.LockObj:SetActive(RoleData.Instance:GetRoleLevel() < data.open_level)
    if RoleData.Instance:GetRoleLevel() < data.open_level then
        UH.SetText(self.LockInfo, string.format(Language.AllSoulsStorybook.OpenInfo, data.open_level))
    end
end
function AllSoulsStorybookFBItem:Click()
    UIWidgetBaseItem.Click(self)
    self.ItemClick:Trigger()
end