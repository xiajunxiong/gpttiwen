PeakTokenCh = PeakTokenCh or BaseClass(SceneObj)

function PeakTokenCh:__init()
    self.vo = nil
end

function PeakTokenCh:__delete()
    if self.effect_handle then
        SE:Stop(self.effect_handle)
        self.effect_handle = nil
    end
end

function PeakTokenCh:FlushDrawer()
    self:FlushRoleDrawer()
    self.body.main_anim:SetAnimActive(false)
    self.body:SetScale(self.vo.res_size)
end

function PeakTokenCh:InitEnd()
    if self.effect_handle then
        SE:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    if self.effect_handle == nil then
        self.effect_handle = SE:Play(SceneEffectConfig.EffectList.PeakTokenEffect, self:GetPos(), function(obj, key, time)end, self.root_node)
    end
end

function PeakTokenCh:InteractionBehavior()
    PeakTokenCtrl.Instance:OnNpcDialogView(self.vo)
end