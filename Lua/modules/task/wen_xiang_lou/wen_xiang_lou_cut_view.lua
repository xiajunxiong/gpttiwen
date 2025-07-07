
WenXiangLouCutView = WenXiangLouCutView or DeclareView("WenXiangLouCutView", WenXiangLouConfig.ResPath .. "task_wxl_cut", Mod.TaskWXLCut.Main)
VIEW_DECLARE_MASK(WenXiangLouCutView, ViewMask.Block)

function WenXiangLouCutView:WenXiangLouCutView()
    self.data = WenXiangLouCtrl.Instance:Data()
    self.show_speed = 0.05
end

function WenXiangLouCutView:LoadCallback()
    self.ContentT.text = Language.Task.WXLInfo.cut_show
    TimeHelper:AddDelayTimer(function ()
        self.BlockObj:SetActive(true)
    end, 3)
end

function WenXiangLouCutView:OpenCallback()
    SceneCtrl.Instance:RequestEnterFb(FB_TYPE.WEN_XIANG_LOU)
end

function WenXiangLouCutView:CloseCallback()
    self.data:FirstEnterTalk()
end

function WenXiangLouCutView:TypeWriterEnd()
end

function WenXiangLouCutView:OnClickClose()
    ViewMgr:CloseView(WenXiangLouCutView)
end