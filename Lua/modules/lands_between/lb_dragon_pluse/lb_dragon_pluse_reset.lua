LBDragonPluseResetView = LBDragonPluseResetView or DeclareView("LBDragonPluseResetView", "lb_dragon_pluse/lbd_monster_reset", nil, ViewLayer.Top + 2)
VIEW_DECLARE_MASK(LBDragonPluseResetView, ViewMask.BgBlockNoAnim)

function LBDragonPluseResetView:LBDragonPluseResetView()
end

function LBDragonPluseResetView:LoadCallback(param_t)
    self.confim_func = param_t.confim_func
    -- LogD("item_list = ", item_list)

    self.Content.text = param_t.content
    -- self.ResetVal.text = string.format(self.language.back_show, points)
    self.CostVal.text = param_t.cost
end

function LBDragonPluseResetView:OnClickConfirm()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
    if self.confim_func ~= nil then 
        self.confim_func()
    end 
	ViewMgr:CloseView(LBDragonPluseResetView)
end

function LBDragonPluseResetView:OnClickCancel()
    ViewMgr:CloseView(LBDragonPluseResetView)
end