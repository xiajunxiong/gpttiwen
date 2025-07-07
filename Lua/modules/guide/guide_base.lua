GuideBase = GuideBase or BaseClass()
function GuideBase:__init()
    self.mgr = GuideManager.Instance
end
function GuideBase:__delete()
    self.mgr = nil
end
function GuideBase:Start(step_cfg)
    -- 处理步骤
end
function GuideBase:Continue()
    -- 继续步骤
end
function GuideBase:Finish()
    -- 直接结束
end
function GuideBase:View()
    self.view = self.mgr:GuideView()
end