-- 月夕灯谜 主界面
YxdmView = YxdmView or DeclareView('YxdmView', 'serveractivity/yxdm/yxdm', Mod.MidAutumnFestival.Yxdm)
VIEW_DECLARE_LEVEL(YxdmView, ViewLevel.Lt)
VIEW_DECLARE_MASK(YxdmView, ViewMask.BgBlock)

function YxdmView:YxdmView()
    self.Time:SetShowCallBack(function(t)
        return Format(Language.YxDm.Time, t.day, t.hour, t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_YUE_XI_DENG_MI, TimeType.Type_Time_3)
end

function YxdmView:LoadCallback(param_t)
    local config = YxdmData.Instance:GetShowReward()
    self.List:SetData(config)
end

function YxdmView:OnCloseClick()
    ViewMgr:CloseView(YxdmView)
end

function YxdmView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[354].desc)
end

----------------------------YxdmPanel----------------------------
YxdmPanel = YxdmPanel or DeclareMono('YxdmPanel', UIWFlushPanel)
function YxdmPanel:YxdmPanel()
    self.Data = YxdmData.Instance
    self.data = self.Data.yxdm_data
    self.language = Language.YxDm
    self.data_cares = {{data = self.data, func = self.FlushAll, init_call = false}}
    --UH.SetText(self.Title, self.language.yxdm)
end

function YxdmPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAll()
end

function YxdmPanel:OnFlush()
end

function YxdmPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function YxdmPanel:onDengClick()
    if self.data.info.today_answer_count < self.Data.today_max_answer then
        YxdmCtrl.Instance:SendReq(YxdmReqType.Start)
    else
        PublicPopupCtrl.Instance:Center(self.language.Over)
    end
end

function YxdmPanel:FlushAll()
    if self.data.info.cur_question_id ~= 0 and self.data.info.today_answer_count ~= self.Data.today_max_answer then
        ViewMgr:OpenView(YxdmCmView)
    end
end
