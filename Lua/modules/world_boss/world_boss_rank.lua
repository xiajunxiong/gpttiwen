WorldBossRank = WorldBossRank or
                    DeclareView("WorldBossRank", "world_boss/world_boss_rank")

function WorldBossRank:LoadCallback()
    self.Board:SetData(self:GetType(), Language.ArenaMatch.Rank.Title,
                       Vector2.New(932, 654))
end
function WorldBossRank:CloseCallback() WorldBossCtrl.Instance:SubscribeCancel() end
WorldBossRankPanel = WorldBossRankPanel or
                         DeclareMono("WorldBossRankPanel", UIWFlushPanel)
function WorldBossRankPanel:WorldBossRankPanel()
    self.data = WorldBossData.Instance
    self.type = 0
    self.data_cares = {
        {data = self.data:BossRankInfo(), func = self.FlushRankList},
        {data = self.data:MyRankInfo(), func = self.FlushDesc},
    }
end

function WorldBossRankPanel:FlushRankList()
    local info = self.data:BossRankInfo()
    -- 改成当前战斗类型
    if self.type == 0 then
        self.List:SetData(info.list0)
    else
        self.List:SetData(info.list1)
    end
end
function WorldBossRankPanel:onFlush()
    local my_info = self.data:MyRankInfo()
    UH.SetText(self.MyName, RoleData.Instance:GetRoleName())
    UH.SetText(self.ValueType, Language.WorldBoss.ValueType[self.type])
    UH.SetText(self.MyValue,
               self.type == 0 and my_info.shanghai or my_info.zhiliao)
    --[[ local function test(rank, type)
        return {
            rank = rank,
            type = type,
            role_id = rank,
            name = "测试" .. rank,
            level = 60 + rank,
            prof = 1,
            value = 10000 + rank
        }
    end
    local t = {}
    for i = 1, 20, 1 do t[i] = test(i, 0) end ]]
    --[[ self.List:SetData(t) ]]
end
function WorldBossRankPanel:OnClickTip() ViewMgr:OpenView(WorldBossRewardView) end
function WorldBossRankPanel:OnClickDamage()
    self.type = 0
    self:FlushDesc()
    self:FlushRankList()
end
function WorldBossRankPanel:OnClickTreat()
    self.type = 1
    self:FlushDesc()
    self:FlushRankList()
end
function WorldBossRankPanel:FlushDesc()
    local my_info = self.data:MyRankInfo()
    UH.SetText(self.ValueType, Language.WorldBoss.ValueType[self.type])
    UH.SetText(self.MyValue,
               self.type == 0 and my_info.shanghai or my_info.zhiliao)
    UH.SetText(self.RankTip, Language.WorldBoss.RankTip[self.type])
    if self.type == 0 then
        self:FlushMyRankReward(my_info.rank0)
    else
        self:FlushMyRankReward(my_info.rank1)
    end
end
function WorldBossRankPanel:FlushMyRankReward(rank)
    if rank == 0 then
        UH.SetText(self.MyRank, Language.WorldBoss.NoRank)
        self.MyList:SetData({})
        self.None:SetActive(true)
    else
        UH.SetText(self.MyRank, rank)
        local reward = WorldBossData.Instance:GetBossReward(nil, self.type + 1,
                                                            nil)
        self.MyList:SetData(reward.item_list)
        self.None:SetActive(false)
    end
end
WorldBossRankItem = WorldBossRankItem or
                        DeclareMono("WorldBossRankItem", UIWidgetBaseItem)
function WorldBossRankItem:WorldBossRankItem()
    self.sprite_name = {"DiYiMing", "DiErMing", "DiSanMing"}
    self.boss_data = WorldBossData.Instance
end
function WorldBossRankItem:SetData(data)
    UH.SetText(self.Name, data.name)
    if data.rank <= 3 then
        self.Level:SetObjActive(false)
        self.Rank:SetObjActive(true)
        UH.SpriteName(self.Rank, self.sprite_name[data.rank])
    else
        self.Rank:SetObjActive(false)
        self.Level:SetObjActive(true)
        UH.SetText(self.Level, data.rank)
    end
    UH.SetText(self.Value, data.value)
    local reward = self.boss_data:GetBossReward(nil, data.type + 1, data.level)
    self.List:SetData(reward.item_list)

end
