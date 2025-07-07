GuildHonourDesc = GuildHonourDesc or DeclareView("GuildHonourDesc", "guild/guild_honour_desc")
function GuildHonourDesc:GuildHonourDesc()
    self.lang = Language.GuildHonourDesc
    self.cur_index = 1
end
function GuildHonourDesc:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Title,Vector2.New(780, 600))
end
function GuildHonourDesc:OnFlush()
    self.TypeList:SetData(self.lang.TypeList)
    self.TypeList:ClickItem(1)
end
function GuildHonourDesc:OnClickItem(data)
    if data.index ~= self.cur_index then
        self.PageList[self.cur_index]:SetActive(false)
    end
    self.PageList[data.index]:SetActive(true)
    self.cur_index = data.index
end
--11111111111111111111
HonourPage1 = HonourPage1 or DeclareMono("HonourPage1", UIWFlushPanel)
function HonourPage1:onFlush()
    self.data = GuildData.Instance
    self.lang = Language.GuildHonourDesc.Mission
    for i = 1,5 do
        UH.SetText(self.TitleList[i], self.lang["Title"..i])
    end
    local list = self.data.family_mission_mission
    for index, value in ipairs(list) do
        UH.SetText(self.TypeList[index], self.lang.MissType[index])
        UH.SetText(self.ValueList[index], value.gongxian)
        UH.SetText(self.Count1List[index], self.lang.Count1)
        UH.SetText(self.LimitList[index], self.lang.MissType[index]..string.format(self.lang.Count, value.param_2))
    end
    local list2 = self.data.family_mission.gongxian_reward_day
    for index, value in ipairs(list2) do
        if value.min_rank == value.max_rank then
            UH.SetText(self.DayList[index], string.format(self.lang.Level[1], DataHelper.GetDaXie(value.min_rank), value.gongxian))
        else
            UH.SetText(self.DayList[index], string.format(self.lang.Level[2], DataHelper.GetDaXie(value.min_rank), DataHelper.GetDaXie(value.max_rank), value.gongxian))
        end
    end
    local list3 = self.data.family_mission_reward
    for index, value in ipairs(list3) do
        UH.SetText(self.CountList[index], string.format(self.lang.Reward, DataHelper.GetDaXie(value.times), value.gongxian))
    end
end
--2222222222222222222222
HonourPage2 = HonourPage2 or DeclareMono("HonourPage2", UIWFlushPanel)
function HonourPage2:onFlush()
    self.lang = Language.GuildHonourDesc.Answer
    for i=1,3 do
        UH.SetText(self.TitleList[i], self.lang["Title"..i])
    end
end
---3333333333333333333
HonourPage3 = HonourPage3 or DeclareMono("HonourPage3", UIWFlushPanel)
function HonourPage3:onFlush()
    self.lang = Language.GuildHonourDesc.ShiLian
    self.data = GuildData.Instance
    for i=1,3 do
        if i == 2 then
            local honour_score = self.data:GetHonorShiLianScore()
            UH.SetText(self.TitleList[i], string.format(self.lang["Title"..i], honour_score))
        else
            UH.SetText(self.TitleList[i], self.lang["Title"..i])
        end
    end
    local reward_group = self.data:GetTrialRankRewardGroup()
    for i = 1, self.ChapterList1:Length() do
        if reward_group[i] then
            if reward_group[i].min_rank == reward_group[i].max_rank then
                UH.SetText(self.ChapterList1[i], string.format(self.lang.SingleRankFormat, reward_group[i].min_rank, self.data.family_auto.boss_reward[reward_group[i].reward_group_id].gongxian))
            elseif i == #reward_group then
                UH.SetText(self.ChapterList1[i], string.format(self.lang.LastRankFormat, reward_group[i].min_rank, self.data.family_auto.boss_reward[reward_group[i].reward_group_id].gongxian))
            else
                UH.SetText(self.ChapterList1[i], string.format(self.lang.MultRankFormat, reward_group[i].min_rank, reward_group[i].max_rank, self.data.family_auto.boss_reward[reward_group[i].reward_group_id].gongxian))
            end
        end
    end
end
----44444444444444444444
HonourPage4 = HonourPage4 or DeclareMono("HonourPage4", UIWFlushPanel)
function HonourPage4:onFlush()
    self.lang = Language.GuildHonourDesc.Honour
    for i=1,2 do
        UH.SetText(self.TitleList[i], self.lang["Title"..i])
    end
    local list = Config.family_honor_auto.act_reward
    for index, value in ipairs(list) do
        UH.SetText(self.ResultList[value.result_type], string.format(self.lang.ValueList[value.result_type],value.gongxian))
    end
end
---5555555555555555555555555
HonourPage5 = HonourPage5 or DeclareMono("HonourPage5", UIWFlushPanel)
function HonourPage5:onFlush()
    self.lang = Language.GuildHonourDesc.Melee
    self.data = GuildData.Instance
    local gongxian = self.data.family_war_other.gongxian
    for i=1,3 do
        if i == 2 then
            UH.SetText(self.TitleList[i], string.format( self.lang["Title"..i], gongxian))
        else
            UH.SetText(self.TitleList[i], self.lang["Title"..i])
        end
    end
    local list = self.data.family_war_auto.rank_reward
    for index, value in ipairs(list) do
        if value.rank_min == value.rank_max then
            UH.SetText(self.LevelList[index], string.format(self.lang.Level[1], DataHelper.GetDaXie(value.rank_min), value.gongxian))
        else
            UH.SetText(self.LevelList[index], string.format(self.lang.Level[2], DataHelper.GetDaXie(value.rank_min), DataHelper.GetDaXie(value.rank_max), value.gongxian))
        end
    end
end
---666666666666666666666666666
HonourPage6 = HonourPage6 or DeclareMono("HonourPage6", UIWFlushPanel)
function HonourPage6:onFlush()
    self.lang = Language.GuildHonourDesc.Famous
    self.data = GuildData.Instance
    local hof_sort = {}
    local count_index = 1
    for k, v in pairs(self.data.family_auto.hof_sort) do
        if hof_sort[v.type + 1] == nil then
            count_index = 1
            hof_sort[v.type + 1] = {}
            local t = {}
            t.gongxian = v.gongxian
            t.rank_min = v.rank_min
            t.rank_max = v.rank_max
            hof_sort[v.type + 1][count_index] = t
        elseif hof_sort[v.type + 1][count_index].gongxian == v.gongxian then
            hof_sort[v.type + 1][count_index].rank_max = v.rank_max
        else
            count_index = count_index + 1
            local t = {}
            t.gongxian = v.gongxian
            t.rank_min = v.rank_min
            t.rank_max = v.rank_max
            hof_sort[v.type + 1][count_index] = t
        end
    end
    --LogError("hof_sort",hof_sort)
    
    for i = 1, self.TitleList:Length() do
        UH.SetText(self.TitleList[i], self.lang["Title"..i])
    end
    for i = 1, 3 do
        for j = 1, self["LevelList"..i]:Length() do
            if hof_sort[i][j] == nil then
                self["LevelList"..i][j]:SetObjActive(false)
            else
                self["LevelList"..i][j]:SetObjActive(true)
                if hof_sort[i][j].rank_min == hof_sort[i][j].rank_max then
                    UH.SetText(self["LevelList"..i][j], string.format(self.lang.Level[1], hof_sort[i][j].rank_min, hof_sort[i][j].gongxian))
                else
                    UH.SetText(self["LevelList"..i][j], string.format(self.lang.Level[2], hof_sort[i][j].rank_min, hof_sort[i][j].rank_max, hof_sort[i][j].gongxian))
                end
            end
        end
    end
end