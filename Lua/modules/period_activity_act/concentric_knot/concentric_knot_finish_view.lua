ConcentricKnotFinishView = ConcentricKnotFinishView or DeclareView("ConcentricKnotFinishView", "period_activity_act/concentric_knot/concentric_knot_finish")
VIEW_DECLARE_MASK(ConcentricKnotFinishView, ViewMask.BgBlockClose)

function ConcentricKnotFinishView:CloseCallback()
    ViewMgr:CloseView(ConcentricKnotView)
end

function ConcentricKnotFinishView:OnClickClose()
    ViewMgr:CloseView(ConcentricKnotFinishView)
end

ConcentricKnotFinishPanel = ConcentricKnotFinishPanel or DeclareMono("ConcentricKnotFinishPanel", UIWFlushPanel)
function ConcentricKnotFinishPanel:ConcentricKnotFinishPanel()
end

function ConcentricKnotFinishPanel:Awake()
    UIWFlushPanel.Awake(self)
    local finish_info = ConcentricKnotData.Instance:GetResultInfo()
    self.is_win = finish_info.is_finish == 1
    self:FlushEffectActive(self.is_win and true or false)

    local has_reward = finish_info.has_reward
    self.Title:SetActive(has_reward)
    self.GridList:SetObjActive(has_reward)
    self.Over:SetActive(not has_reward)
    local is_show_dowm = self.is_win
	self.Down:SetActive(is_show_dowm)
    self.FailObj:SetActive(false) --not is_show_dowm)
	UH.LocalPos(self.Up, Vector3.New(0, is_show_dowm and 175 or 100, 0))

    if self.is_win then
        local item_list = ConcentricKnotData.Instance:GetRewardShow()
        self.GridList:SetData(DataHelper.FormatItemList(item_list))
    end

    local moqi_cfg = ConcentricKnotData.Instance:GetMoQiCfg(finish_info.use_time)
    if moqi_cfg then
        UH.SetText(self.OkText1, moqi_cfg.dec_first)
        UH.SetText(self.OkText2, moqi_cfg.dec_second)
    end
end

----------------------------------------------------------
--设置特效
function ConcentricKnotFinishPanel:FlushEffectActive(is_win)
    local index = is_win and 2 or 4
    self.Effect[index]:SetActive(true)
end

function ConcentricKnotFinishPanel:OnClickOK()
    ViewMgr:CloseView(ConcentricKnotFinishView)
end
