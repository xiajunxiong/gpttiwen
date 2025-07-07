Capturing = Capturing or DeclareView('Capturing', 'catch_butterflies/capturing', Mod.Butterfly.Capturing)
VIEW_DECLARE_MASK(Capturing, ViewMask.Block)
function Capturing:Capturing()
    self.data = CatchButterfliesData.Instance
end
function Capturing:LoadCallback()
    UH.SetText(self.Tip, Language.Butterfly.CatchTip)
    self.audio_handle = AudioMgr:PlayEffectHandle(AudioType.UI, CommonAudio.ZhuaHuDie)
end
function Capturing:OnClickCatch()
    if self.Anima.isPlaying then
        if self.audio_handle then
            self.audio_handle:StopEffect()
        end
        self.Anima:Stop()
        local is_win = self.Line.fillAmount >= 0.378 and self.Line.fillAmount <= 0.619
        local obj_vo = self.data:ObjVo()
        if obj_vo then
            CatchButterfliesCtrl.Capturing(obj_vo.obj_id, is_win and 1 or 0)
        else
            LogError("没获取到蝴蝶对象")
        end
        self.handle_delay =
            TimeHelper:AddDelayTimer(
            function()
                self.data:SetObjVo(nil)
                ViewMgr:CloseView(Capturing)
            end,
            self.data:ShowTime()
        )
    -- 0.378 0.619
    -- ViewMgr:CloseView(Capturing)
    end
end
function Capturing:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
    
end
