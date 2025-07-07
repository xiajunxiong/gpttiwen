NeoColorChangeSuccView = NeoColorChangeSuccView or DeclareView("NeoColorChangeSuccView", "neo_chara_change/neo_color_change_succ_view")
-----ModelShow1========ModelUIShow
VIEW_DECLARE_LEVEL(NeoColorChangeSuccView, ViewLevel.L0)
VIEW_DECLARE_MASK(NeoColorChangeSuccView, ViewMask.BlockClose)

function NeoColorChangeSuccView:NeoColorChangeSuccView()
    self.ui_obj = nil
end

function NeoColorChangeSuccView:LoadCallback()
    local role_apear = NeoColorChangePanel.baseApearance()
    local cur_sel_co = NeoCharaChangeData.Instance:ColorChangeData().color_index
    role_apear.color = cur_sel_co
    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.New()
    end
    self.ui_obj:SetData(role_apear)
    self.ModelShow1:SetShowData({
        ui_obj = self.ui_obj,
        view = self,
    })
end

function NeoColorChangeSuccView:CloseCallback()
    Delete(self,[[ui_obj]])
end
