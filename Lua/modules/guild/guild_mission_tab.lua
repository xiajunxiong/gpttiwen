GuildMissionTab = GuildMissionTab or DeclareMono("GuildMissionTab", UIWFlushPanel)

function GuildMissionTab:GuildMissionTab()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.gwt_all_info,func = self.FlushMission}
    }
end 

function GuildMissionTab:onFlush()
end

function GuildMissionTab:FlushMission()
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

function GuildMissionTab:OnClickTip()
    ViewMgr:OpenView(GuildMissionReward)
end

