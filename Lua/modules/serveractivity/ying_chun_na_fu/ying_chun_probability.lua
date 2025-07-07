YingChunProBabilityView = YingChunProBabilityView or DeclareView("YingChunProBabilityView",'serveractivity/ying_chun_na_fu/ying_chun_probability')
function YingChunProBabilityView:YingChunProBabilityView()
end

function YingChunProBabilityView:LoadCallback(param_t)
end

function YingChunProBabilityView:OnClickClose()
    ViewMgr:CloseView(YingChunProBabilityView)
end


YingChunProBabilityPanel = YingChunProBabilityPanel or DeclareMono("YingChunProBabilityPanel", UIWFlushPanel)

function YingChunProBabilityPanel:YingChunProBabilityPanel()
    self.data = YingChunData.Instance
end

function YingChunProBabilityPanel:Awake()
    UIWFlushPanel.Awake(self)
    local probality=YingChunData.Instance:GetProbility()
    UH.SetText(self.CenterReward,Language.YingChunNaFu.CenterReward..probality.CenterReward)
    UH.SetText(self.AroundReward,Language.YingChunNaFu.AroundReward..probality.AroundReward)
end
