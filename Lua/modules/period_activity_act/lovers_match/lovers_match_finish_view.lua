LoversMatchFinishView = LoversMatchFinishView or DeclareView("LoversMatchFinishView", "period_activity_act/lovers_match/lovers_match_finish")
VIEW_DECLARE_MASK(LoversMatchFinishView, ViewMask.BgBlock)

function LoversMatchFinishView:OnClickClose()
    ViewMgr:CloseView(LoversMatchFinishView)
end

LoversMatchFinishPanel = LoversMatchFinishPanel or DeclareMono("LoversMatchFinishPanel", UIWFlushPanel)
function LoversMatchFinishPanel:LoversMatchFinishPanel()
end

function LoversMatchFinishPanel:Awake()
    UIWFlushPanel.Awake(self)
    local finish_info = LoversMatchData.Instance:GetResult()
    self.is_win = finish_info.is_finish == 1
    self:FlushEffectActive(self.is_win and true or false)

    local has_reward = finish_info.has_reward
    self.Title:SetActive(has_reward)
    self.GridList:SetObjActive(has_reward)
    self.Over:SetActive(not has_reward)
    local is_show_dowm = self.is_win
	self.Down:SetActive(is_show_dowm)
    self.FailObj:SetActive(not is_show_dowm)
	UH.LocalPos(self.Up, Vector3.New(0, is_show_dowm and 175 or 100, 0))

    local item_list = LoversMatchData.Instance:GetReward(finish_info.reward_level)
    self.GridList:SetData(DataHelper.FormatItemList(item_list))
end

function LoversMatchFinishPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.is_win then
        LoversMatchCtrl.Instance:CheckFriend()
    end
end

----------------------------------------------------------
--设置特效
function LoversMatchFinishPanel:FlushEffectActive(is_win)
    local index = is_win and 2 or 4
    self.Effect[index]:SetActive(true)
end

function LoversMatchFinishPanel:OnClickOK()
    ViewMgr:CloseView(LoversMatchFinishView)
end
