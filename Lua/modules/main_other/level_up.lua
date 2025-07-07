LevelUpView = LevelUpView or DeclareView("LevelUpView","main_other/level_up3", 0, ViewLayer.Top)

VIEW_DECLARE_LEVEL(LevelUpView,ViewLevel.Lt)
VIEW_DECLARE_MASK(LevelUpView,ViewMask.None)

function LevelUpView:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, "level_up")
end

function LevelUpView:OnFlush()
    --文字显示时间
    self.LevelTex.text = RoleData.Instance:GetRoleLevel()
    --[[ self.delay_handle1 = TimeHelper:AddDelayTimer(function ()
        self.LevelTex.text = RoleData.Instance:GetRoleLevel()
    end, 0.9) ]]
    --界面关闭时间
    self.delay_handle3 = TimeHelper:AddDelayTimer(function ()
        self:EffectEnd()
    end, 2)--2.8)
    --场景里的特效
    self.delay_handle2 = TimeHelper:AddDelayTimer(function ()
        if MainPlayer then
            SE:Play(SceneEffectConfig.EffectList.LevelEffect, nil, nil, MainPlayer.root_node)
        end
    end, 0.6)
end

function LevelUpView:EffectEnd()
    ViewMgr:CloseView(LevelUpView)
end
function LevelUpView:CloseCallback()
    TimeHelper:CancelTimer(self.delay_handle1)
    TimeHelper:CancelTimer(self.delay_handle2)
    TimeHelper:CancelTimer(self.delay_handle3)
end