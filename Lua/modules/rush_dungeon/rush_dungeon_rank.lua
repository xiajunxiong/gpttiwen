RushDungeonRank = RushDungeonRank or DeclareView("RushDungeonRank", "rush_dungeon/rush_dungeon_rank")
function RushDungeonRank:RushDungeonRank()
    self.lang = Language.RushDungeon
end

function RushDungeonRank:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Rank, Vector2.New(1006, 560))
end

RushDungeonRankPanel = RushDungeonRankPanel or DeclareMono("RushDungeonRankPanel", UIWFlushPanel)
function RushDungeonRankPanel:RushDungeonRankPanel()
    self.data = RushDungeonData.Instance
    self.data_cares = {
        {data = self.data:RankInfo(), func = self.FlushMyRank, init_call = true},
    }
end

function RushDungeonRankPanel:FlushMyRank()
    local data = self.data:RankInfo().rank_list
    local count = table.nums(data)
    self.None:SetActive(count == 0)
    local his_level = self.data:GetHistoryLevel()
    local week_pass = self.data:WeekPass()
    self.BottomList[1].text = string.format(Language.RushDungeon.BottomDesc2[1], week_pass)
    self.BottomList[2].text = string.format(Language.RushDungeon.BottomDesc2[2], his_level)
    self.BottomList[3].text = string.format(Language.RushDungeon.BottomDesc2[3], 0)
    self.RankList:SetData(data)
    if count == 0 then return end
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
    self.BottomList[1].text = string.format(Language.RushDungeon.BottomDesc2[1], week_pass)
    
    if cur_rank ~= 9999 then
        self.BottomList[3].text = string.format(Language.RushDungeon.BottomDesc2[3], cur_rank)
        local rewardInfo = RushDungeonData.Instance:GetRankReward(cur_rank)
        local temp = {}
        for _, value in pairs(rewardInfo.reward_show) do
            table.insert(temp, Item.Create(value))
        end
        self.CellList:SetData(temp)
    else
        self.BottomList[3].text = string.format(Language.RushDungeon.BottomDesc2[3], 0)
    end
end

RushRankItem = RushRankItem or DeclareMono("RushRankItem", UIWidgetBaseItem)

function RushRankItem:SetData(data)
    -- local str = ""
    --[[ for i = 1, data.role_info_count do
        str = str .. data.role_info[i].role_name .. " "
    end ]]
    UH.SetText(self.Name, data.role_name)
    if data.rank <= 3 then
        UH.SpriteName(self.RankImg, data.rank)
        UH.SetText(self.Rank, "")
    else
        UH.SpriteName(self.RankImg, "")
        UH.SetText(self.Rank, data.rank)
    end
    UH.SetText(self.JiFen, data.reached_level)
    UH.Alpha(self.RankBg, data.rank % 2 == 0 and 0.7 or 0.3)
    UIWidgetBaseItem.SetData(self, data)
    local rewardInfo = RushDungeonData.Instance:GetRankReward(data.rank)
    if rewardInfo == nil then return end
    local temp = {}
    for key, value in pairs(rewardInfo.reward_show) do
        table.insert(temp, Item.Create(value))
    end
    self.RewardList:SetData(temp)
end