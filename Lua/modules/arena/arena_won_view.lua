ArenaWonView = ArenaWonView or DeclareView("ArenaWonView","arena/arena_won")
---CurRankTxt====Text
---rise==========Text

function ArenaWonView:ArenaWonView()
    -- self.rise.text = ArenaCtrl.Instance.up_rank
    -- self.CurRankTxt.text = tostring(ArenaData.Instance.RoleInfo.rank)

    -- self.ui_obj = UIChDrawer.CopyMainRoleData()
    -- self.model:SetShowData(
    --     {
    --         ui_obj = self.ui_obj,
    --         view = self
    --     }
    -- )
    -- local data_list = {}
    -- local main_role = RoleData.Instance:GetBaseData().appearance.avatar_type
    -- data_list[1] = {type = main_role,is_main = true}
    -- self:GetPartnerIcon(data_list)
    -- self.List:SetData(data_list)
    local textList = ArenaData.Instance:GetResultInfo()
    for i=1,self.TextArray:Length() do
        UH.SetText(self.TextArray[i], textList[i])
    end
end

-- function ArenaWonView:GetPartnerIcon(data_list)
--     local config = Config.partner_cfg_auto.partner_info
--     for k,v in pairs(config)do
--         if PartnerCtrl.Instance:Data():GetPartnerFightOrderList(v.partner_id)[1] ~= nil then
--             table.insert(data_list,{type = v.icon_id,is_main = false})
--         end
--     end
-- end

function ArenaWonView:OnClose()
    ViewMgr:CloseView(ArenaWonView)
    BattleCtrl.Instance:DelayFinish(function()
        ViewMgr:OpenView(ArenaView)
    end)
end


ArenaWonItem = ArenaWonItem or DeclareMono("ArenaWonItem",UIWidgetBaseItem)
function ArenaWonItem:SetData(data)
    if data.is_main then
        self.avataroj:SetActive(true)
        self.avatar:SetAvatar(data.type)
    else
        self.iconoj:SetActive(true)
        UH.SetIcon(self.icon,data.type,1)
    end
end