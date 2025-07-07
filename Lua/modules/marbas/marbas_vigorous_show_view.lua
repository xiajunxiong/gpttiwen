MarbasVigorousShowView = MarbasVigorousShowView or DeclareView("MarbasVigorousShowView", "marbas/marbas_vigorous_show")
function MarbasVigorousShowView:MarbasVigorousShowView() end
function MarbasVigorousShowView:LoadCallback(param_t)
    self.ui_obj =  UIObjDrawer.New()
    self.ModelShow:SetShowData(
        {
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = false,
        }
    )
    local cfg = MarbasData.Instance:GetVigorousCfg(param_t.show_id)

    self.ui_obj:SetData(DrawerHelper.GetNpcPath(MarbasConfig.ModLevelShow[cfg.level]))
    UH.SetText(self.ItemName,cfg.vigorous_name)

    self.show_id = param_t.show_id

    MarbasData.Instance.vigorous_view_data.mark_id = 0
end

function MarbasVigorousShowView:CloseCallback()
    Delete(self, "ui_obj")
end

function MarbasVigorousShowView:OnClickGo()
    ViewMgr:CloseView(MarbasVigorousShowView)

    MarbasCtrl.Instance:SendEquipOper(MarbasReqType.Wear,self.show_id)
end