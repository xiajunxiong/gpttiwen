GuildMission = GuildMission or DeclareView("GuildMission", "guild/guild_mission")
--, Mod.Guild.GuildInfo)
function GuildMission:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Guild.Mission)
end

GuildMissionPanel = GuildMissionPanel or DeclareMono("GuildMissionPanel", UIWFlushPanel)
function GuildMissionPanel:GuildMissionPanel()
    self.data = GuildData.Instance
end
function GuildMissionPanel:onFlush()
    -- 获取进度 --获取任务描述 当前奖励
    local w = tonumber(os.date("%w", os.time()))
    UH.SetText(
        self.LblGold,
        (w == 0 or w == 6) and Language.Guild.MissionGold[0] or
            string.format(Language.Guild.MissionGold[1], DataHelper.GetDaXie(tonumber(w)))
    )
    local all_info, task_info = self.data:GWTTaskInfo()
    local count, rank_list = self.data:GWTRankInfo()
    self.NoneObj:SetActive(count == 0)
    self.BarObj:SetActive(all_info.task_id ~= 0)
    self.NextObj:SetActive(all_info.task_id == 0)
    UH.SetText(
        self.LblTitle,
        all_info.task_id == 0 and Language.Guild.MisstionDesc[2] or Language.Guild.MisstionDesc[1]
    )
    all_info.finish_times = all_info.finish_times > 5 and 5 or all_info.finish_times
    UH.SpriteName(self.PerSp, all_info.finish_times == 0 and nil or all_info.finish_times)
    local data = self.data:GWTRewardInfo(all_info.finish_times)
    self.RewardList:SetData(
        {
            Item.Create({item_id = CommonItemId.Coin, num = data.coin}),
            Item.Create({item_id = CommonItemId.GongXian, num = data.gongxian}),
            Item.Create(data.reward)
        }
    )
    for i = 1, self.HadRewardObjs:Length(), 1 do
        self.HadRewardObjs[i]:SetActive(all_info.finish_times > 0)
    end
    self.NoneReward:SetActive(all_info.finish_times == 0)
    self.RankList:SetData(rank_list)

    if all_info.task_id == 0 then
        UH.SetText(self.LblDesc, Language.Guild.MissionNone)
        return
    end

    UH.SetText(self.LblDesc, task_info.desc)
    self.ProgBar:SetProgress(all_info.task_prog / task_info.param_2)
    self.ProgBar:SetText(string.format("%s/%s", all_info.task_prog, task_info.param_2))
end
function GuildMissionPanel:OnClickTip()
    ViewMgr:OpenView(GuildMissionReward)
end

GuildGWTRankItem = GuildGWTRankItem or DeclareMono("GuildGWTRankItem", UIWidgetBaseItem)

function GuildGWTRankItem:SetData(data)
    UH.SetText(self.LblName, data.name)
    UH.SetText(self.LblGongXian, data.cont)
    if data.index <= 3 then
        UH.SpriteName(self.RankSp, string.format("paiming%d", data.index))
        UH.SpriteName(self.BgSp, string.format("paiming%ddi", data.index))
        UH.SetText(self.LblLevel, "")
    else
        UH.SpriteName(self.RankSp, string.format("putongpaimingdi", data.index))
        UH.SpriteName(self.BgSp, "paimingputongdi")
        UH.SetText(self.LblLevel, data.index)
    end
    self.BaoObj:SetActive(data.cont >= GuildData.Instance.family_mission_other.boom)
    local reward = GuildData.Instance:GWTRankReward(data.cont)
    local avatar_id = RoleData.Instance:GetRoleProf(data.prof)
    self.Avatar:SetAvatar(data.avatar_type, data.avatar_id,data.avatar_quality)
    self.RewardCell:SetData(Item.Create(reward.gongxian_reward))
    --优化自己的显示
    if data.name == RoleData.Instance:GetRoleName() then
        UH.Color(self.LblName, COLORS.Red7)
    else
        UH.Color(self.LblName, COLORS.Black1)
    end
end

GuildMissionReward = GuildMissionReward or DeclareView("GuildMissionReward", "guild/guild_mission_reward")
function GuildMissionReward:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Guild.TitleReward, Vector2.New(578,458))--(580, 478))
    self.data = GuildData.Instance
end
function GuildMissionReward:OnFlush()
    local all_info = self.data:GWTTaskInfo()
    for i = 1, self.LevelObjs:Length() do
        UH.SetText(self.LevelObjs[i], string.format(Language.Guild.LevelDesc2, DataHelper.GetDaXie(i)))
        UH.SpriteName(self.IconObjs[i], all_info.finish_times >= i and "jianglihuode" or "jiangliweihuode")
        self.GouObjs[i]:SetActive(all_info.finish_times >= i)
        UH.SetText(self.WeekObjs[i], string.format(Language.Guild.MissionWeek, DataHelper.GetDaXie(i)))
    end
end
