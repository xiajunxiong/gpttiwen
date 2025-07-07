
PartnerViewModel = PartnerViewModel or DeclareMono("PartnerViewModel", UIWFlushPanel)

function PartnerViewModel:PartnerViewModel()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerViewModel:onFlush()
    -- if -1 == self.data.cur_sel_partner then return end
    -- local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    -- local info = data.info
    -- if not self.ui_obj then
    --     self.ui_obj = UIObjDrawer.New()
	--     self.ui_obj:SetData("actor/npc/" .. info.res_id ..  ".prefab")
    --     self.ModelShow:SetShowData(
    --         {
    --             ui_obj = self.ui_obj,
    --             view = self
    --         }
    --     )
    -- else
    --     self.ui_obj:SetData("actor/npc/" .. info.res_id ..  ".prefab")
    -- end
end