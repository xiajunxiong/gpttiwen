CatchButterflies =
    CatchButterflies or DeclareView('CatchButterflies', 'catch_butterflies/catch_butterflies', Mod.MayDay.Butterfly)

function CatchButterflies:CatchButterflies()
    PeriodActivityData.Instance:SetRemindData(Mod.MayDay.Butterfly)
end

function CatchButterflies:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(CatchButterflies)
end

CatchButterfliesPanel = CatchButterfliesPanel or DeclareMono('CatchButterfliesPanel', UIWFlushPanel)
function CatchButterfliesPanel:CatchButterfliesPanel()
    self.data = CatchButterfliesData.Instance
    self.data_cares = {
        {data = self.data:Info(), func = self.FlushPanel, init_call = false}
    }
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
end
function CatchButterfliesPanel:FlushPanel()
    self:FlushCount()
    self.List:SetData(self.data:GetActReward())
    UH.SetText(self.Time, string.format(Language.Butterfly.ActTime, self.data:GetActTime()))
end
function CatchButterfliesPanel:onFlush()
    UH.SetText(self.Desc, self.data:Desc())
    self:FlushPanel()
end
function CatchButterfliesPanel:FlushCount()
    UH.SetText(
        self.Count,
        string.format(Language.Butterfly.Count, self.data:MaxCount() - self.data:Count(), self.data:MaxCount())
    )
end
function CatchButterfliesPanel:OnClickToGo()
    -- if LoginData.Instance:IsOnCrossSever() then
    --     ViewMgr:CloseView(CatchButterflies)
    --     PublicPopupCtrl.Instance:Center(Language.Butterfly.CrossTip)
    --     return
    -- end
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    if CatchButterfliesData.Instance:IsCantCatch() then
        PublicPopupCtrl.Instance:Center(Language.Butterfly.CountTip)
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    PublicPopupCtrl.Instance:Center(Language.Butterfly.FindTip)
    ViewMgr:CloseView(CatchButterflies)
    ViewMgr:CloseView(MayDayView)
    SceneLogic.Instance:AutoToPos(self.data:SceneID(), nil, nil, false, true)
end
