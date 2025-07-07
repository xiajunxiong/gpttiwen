-- @Author: Ghost
-- @Date: 2022-08-26 14:18:15
-- @Last Modified by:   Ghost
-- @Last Modified time: 2022-08-26 14:18:15
ActYxlcView = ActYxlcView or DeclareView('ActYxlcView', 'serveractivity/act_yxlc/act_yxlc_view', Mod.MidAutumnFestival.Yxlc)
VIEW_DECLARE_MASK(ActYxlcView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(ActYxlcView, ViewLevel.Lt)

function ActYxlcView:ActYxlcView()

end

function ActYxlcView:LoadCallback()

end
function ActYxlcView:OnBtnClose()
    ViewMgr:CloseView(ActYxlcView)
end

ActYxlcPanel = ActYxlcPanel or DeclareMono("ActYxlcPanel", UIWFlushPanel)
function ActYxlcPanel:ActYxlcPanel()
    self.data = ActYxlcData.Instance
    self.data_cares = {
        {data = self.data.SD_YXLCInfo, keys = {'val'}, func = self.FlushPanelInfo, init_call = false},
        {data = self.data.SD_YXLCInfo, keys = {'m_total_chongzhi'}, func = self.FlushChongzhi, init_call = false},
    }
end

function ActYxlcPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanelInfo()
    self:FlushChongzhi()
    self.DjcTime:SetShowCallBack(function(t)
        return Format(Language.ActYxlc.Time, t.day, t.hour, t.min)
    end)
    self.DjcTime:CreateActTime(ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG, TimeType.Type_Time_3)
end

function ActYxlcPanel:FlushPanelInfo()
    self.ListItems:SetData(self.data:GetSC_YXLCInfo_Data())
end

function ActYxlcPanel:FlushChongzhi()
    UH.SetText(self.LBYczYB, string.format(Language.ActYxlc.Tip1, self.data.SD_YXLCInfo.m_total_chongzhi))
end

-- 月夕累充奖励item
ActYxlcItem = ActYxlcItem or DeclareMono('ActYxlcItem', UIWidgetBaseItem)
function ActYxlcItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local data_config = data.config
    local str_lc = Language.ActYxlc.Tip
    local isRec = data.m_receive_reward_flag ~= 0
    self.can_reward = data.canRec
    if self.Handle_Effect ~= nil then
        UH.StopEffect(self.UIEffectTool, self.Handle_Effect)
    end
    if isRec then -- 已经领取
        self.ImgReced:SetObjActive(true)
        self.BtnRec:SetObjActive(false)
    else
        self.ImgReced:SetObjActive(false)
        self.BtnRec:SetObjActive(true)
        if self.can_reward then -- 可领取
            UH.SpriteName(self.SpaBtnRec, 'HuangAnNiu')
            UH.SetText(self.LbRec, Language.Common.BtnState[0])
            self.Handle_Effect = UH.PlayEffect(self.UIEffectTool, 6165315)
        else
            UH.SpriteName(self.SpaBtnRec, 'LvAnNiu')
            UH.SetText(self.LbRec, Language.ActYxlc.Qwcz)
            self.Handle_Effect = UH.PlayEffect(self.UIEffectTool, 6165316)
        end
    end
    UH.SetText(self.LbLc, string.format(str_lc, data_config.acc_recharge_down / 10))
    self.ListReward:SetData(DataHelper.FormatItemList(data.reward))
end

function ActYxlcItem:CheckCanReward()
end

function ActYxlcItem:OnBtnRec()
    if IS_AUDIT_VERSION then
        RoleCtrl.Instance:SendAuditBehaviourSubmitReq(0)
    end
    if self.can_reward then
        ActYxlcCtrl.Instance:CSReqReward(self.data.config.seq)
    else
        ViewMgr:OpenViewByKey(Mod.Store.Recharge)
    end
end

ActZqhdBoard = ActZqhdBoard or DeclareMono('ActZqhdBoard', UIWFlushPanel)

function ActZqhdBoard:ActZqhdBoard()

end

function ActZqhdBoard:Awake()
    UIWFlushPanel.Awake(self)
end

