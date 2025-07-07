-- 神域探索等级奖励
ShenYuProbeLevelReward = ShenYuProbeLevelReward or DeclareView("ShenYuProbeLevelReward", "shen_yu_probe/shen_yu_probe_level_reward")
function ShenYuProbeLevelReward:OnCloseClick()
    ViewMgr:CloseView(ShenYuProbeLevelReward)
end

function ShenYuProbeLevelReward:LoadCallback()
    UH.SetText(self.Tip, Language.ShenYuProbe.Text2)
end


ShenYuProbeLevelRewardPanel = ShenYuProbeLevelRewardPanel or DeclareMono("ShenYuProbeLevelRewardPanel", UIWFlushPanel)

function ShenYuProbeLevelRewardPanel:ShenYuProbeLevelRewardPanel()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.camp_data, func = self.FlushPanel, init_call = false},
        {data = self.data.role_data, func = self.FlushPanel, init_call = false},
    }
end

function ShenYuProbeLevelRewardPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ShenYuProbeLevelRewardPanel:FlushPanel()
    local list_data = self.data.cfg.explore_level
    self.List:SetData(list_data)
end

ShenYuProbeLevelRewardItem = ShenYuProbeLevelRewardItem or DeclareMono("ShenYuProbeLevelRewardItem", UIWidgetBaseItem)
function ShenYuProbeLevelRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level, string.format(Language.ShenYuProbe.Level, data.level))
    local huobi_list = {}
    if data.military_exploits > 0 then
        local huobi = HuoBi[CurrencyType.GodSpaceExploits]
        table.insert(huobi_list, Item.Create({item_id = huobi, num = data.military_exploits}))
    end
    if data.quintessence > 0 then
        local huobi = HuoBi[CurrencyType.GodSpaceEssence]
        table.insert(huobi_list, Item.Create({item_id = huobi, num = data.quintessence}))
    end
    local item_list = DataHelper.FormatItemList(data.level_reward)
    item_list = AndList(huobi_list, item_list)
    self.ItemList:SetData(item_list)
    local flag = ShenYuProbeData.Instance:GetLevelFlag(data.level)
    self.GetBtn:SetActive(flag == 1)
    self.NotFinish:SetActive(flag == 0)
    self.Geted:SetActive(flag == -1)
    self.RedPoint:SetNum(flag == 1 and 1 or 0)
end


function ShenYuProbeLevelRewardItem:OnGetClick()
    ShenYuProbeCtrl.Instance:SendTemplateReq({
        oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_FETCH_EXPLORE_LV_REWARD,
        param_1 = self.data.level,
    })
end