ShenYuProbeBoosRank = ShenYuProbeBoosRank or DeclareView("ShenYuProbeBoosRank", "shen_yu_probe/shen_yu_probe_boos_rank")
function ShenYuProbeBoosRank:ShenYuProbeBoosRank()
    self.data = ShenYuProbeData.Instance
end

function ShenYuProbeBoosRank:LoadCallback(param_t)
    ShenYuProbeCtrl.Instance:SendTemplateReq({
        oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_EXPLORE_BOSS_RANK_LIST,
    })
end

function ShenYuProbeBoosRank:OnCloseClick()
    ViewMgr:CloseView(ShenYuProbeBoosRank)
end


ShenYuProbeBoosRankPanel = ShenYuProbeBoosRankPanel or DeclareMono("ShenYuProbeBoosRankPanel", UIWFlushPanel)
function ShenYuProbeBoosRankPanel:ShenYuProbeBoosRankPanel()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.rank_data, func = self.FlushPanel, init_call = false},
    }
end

function ShenYuProbeBoosRankPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ShenYuProbeBoosRankPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.rank_data:Set({})
end

function ShenYuProbeBoosRankPanel:FlushPanel()
    local list = self.data.rank_data:Val()
    if TableIsEmpty(list) then
        list = {
            [1] = {id = 0, rank = 1, num = 0},
            [2] = {id = 1, rank = 2, num = 0},
            [3] = {id = 2, rank = 3, num = 0},
            [4] = {id = 3, rank = 4, num = 0},
        }
    end
    self.List:SetData(list)
end



ShenYuProbeBoosRankItem = ShenYuProbeBoosRankItem or DeclareMono("ShenYuProbeBoosRankItem", UIWidgetBaseItem)
function ShenYuProbeBoosRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local rank = data.rank or 1
    self.RankImg:SetObjActive(rank < 4)
    self.RankNum:SetObjActive(rank >= 4)
    if rank < 4 then
        UH.SpriteName(self.RankImg, "PaiMing" .. rank)
    else
        UH.SetText(self.RankNum, rank)
    end
    UH.SpriteName(self.BoxImg, "BaoXiang" .. rank)

    local name = string.format(Language.LandsBetween.Desc.CityName[1],Language.LandsBetween.FactionName[data.id])
    UH.SetText(self.Name, name)
    local head_img = LandsBetweenData.Instance:GetCityIconStr(data.id)
    UH.SpriteName(self.HeadImg, head_img)
    UH.SetText(self.Value, string.format(Language.ShenYuProbe.Text1, data.num))
end

function ShenYuProbeBoosRankItem:OnBoxClick()
    local cfg = ShenYuProbeData.Instance.cfg.boss_ranking[self.data.rank]
    local pos = UH.GetRootPosition(self.BoxImg.gameObject)
    pos = pos + Vector3(80, 0, 0)
    local tab = {
        pos = pos,
        item_list = DataHelper.FormatItemList(cfg.reward_item),
        pivot = 0
    }
    ViewMgr:OpenView(ShowItemView, tab)
end