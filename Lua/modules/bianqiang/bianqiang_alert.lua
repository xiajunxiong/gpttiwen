BianQiangAlert = BianQiangAlert or DeclareView("BianQiangAlert", "bianqiang/bianqiang_alert")
VIEW_DECLARE_MASK(BianQiangAlert, ViewMask.BlockClose)

function BianQiangAlert:LoadCallback(param_t)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.task_cfg = param_t.task_cfg
end

function BianQiangAlert:OnFlush()
    UH.SetText(self.Title, self.task_cfg.name)
    UH.SetText(self.Desc, self.task_cfg.other_desc)
end

function BianQiangAlert:OnClickOk()
    ViewMgr:CloseView(BianQiangAlert)
end

function BianQiangAlert:OnClickClose()
    ViewMgr:CloseView(BianQiangAlert)
end
