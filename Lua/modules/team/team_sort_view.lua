TeamSortView = TeamSortView or DeclareView("TeamSortView", TeamConfig.ResPath .. "team_sort")

function TeamSortView:TeamSortView()
    self.data = TeamCtrl.Instance:Data()
end

function TeamSortView:OnClickClose()
    ViewMgr:CloseView(TeamSortView)
end

function TeamSortView:CloseCallback()
    self.data.sort_index_first.val = -1
end

TeamSortViewList = TeamSortViewList or DeclareMono("TeamSortViewList", UIWFlushPanel)

function TeamSortViewList:TeamSortViewList()
	self.data = TeamCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data:GetTeamInfo(), func = self.FlushSortItems},
		{data = self.data.sort_index_first, func = self.FlushSortItems, init_call = false},
	}
end

function TeamSortViewList:FlushSortItems()
    local in_team = self.data:InTeam()
    local team_info = self.data:GetTeamInfo()
    if not in_team then
        self.data:CheckNoTeamPartnerOrder()
    end
    local list = in_team and team_info.member_pos_list or team_info.member_list
    for i = 1, self.SortItems:Length() do
        self.SortItems[i]:SetData(list[i])
    end
end

TeamSortViewItem = DeclareMono("TeamSortViewItem", UIWidgetBaseItem)

function TeamSortViewItem:TeamSortViewItem()
    self.team_data = TeamCtrl.Instance:Data()
end

function TeamSortViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = nil ~= data and data.info or nil
    self.Name.text = nil ~= info and info.name or ""

    local in_team = self.team_data:InTeam()
    local value = nil ~= info and (in_team and info.pos_index or info.partner_id) or 0

    self.ArrowObj:SetActive(nil ~= info and self.team_data.sort_index_first.val == value)
    if info then
        self.ModelShow:SetObjActive(true)
        if not self.ui_obj then
            self.ui_obj = UIChDrawer.New()
            if 0 == info.is_partner then
                self.ui_obj:SetData(info.appearance)
            else
                local partner_info = info.partner_info
                self.ui_obj:SetMainSolo("actor/npc/" .. partner_info.res_id ..  ".prefab")
            end
            self.ModelShow:SetShowData(
                {
                    ui_obj = self.ui_obj,
                    view = self,
                    show_shadow = false,
                    -- default_angle = 120
                }
            )

            local re_data = Game.RendererEffectData()
            re_data.materialNames = {nil, "Ghost201"}
            re_data.animationClipName = "UI_jueseguang201"

            self.ui_obj:SetRenderEffect(re_data)
            self.Effect:SetActive(true)
            -- local node = self.ui_obj:SetRenderEffect(re_data)
            -- self.render_effect_event_node =
            --     EventNode.New(
            --     function(loaded)
            --         if loaded then
            --             self.Effect:SetActive(true)
            --             if self.render_effect_event_node then
            --                 self.render_effect_event_node:DeleteMe()
            --                 self.render_effect_event_node = nil
            --             end
            --         end
            --     end
            -- )
            -- self.render_effect_event_node:AddChild(node)
        else
            if 0 == info.is_partner then
                self.ui_obj:SetData(info.appearance)
            else
                local partner_info = info.partner_info
                self.ui_obj:SetMainSolo("actor/npc/" .. partner_info.res_id ..  ".prefab")
            end 
        end
    else
        self.ModelShow:SetObjActive(false)
    end
end

function TeamSortViewItem:OnClickItem()
    local info = nil ~= self.data and self.data.info or nil
    if nil == info then return end
    if self.data.is_leader then
        PublicPopupCtrl.Instance:Center(Language.Team.TipsSortLeader)
        return
    end
    local in_team = self.team_data:InTeam()
    local index = in_team and info.pos_index or info.partner_id
    if self.team_data.sort_index_first.val < 0 then
        self.team_data.sort_index_first.val = index
    elseif self.team_data.sort_index_first.val == index then
        self.team_data.sort_index_first.val = -1
    else
        TeamCtrl.Instance:SendTeamSort(index)
    end
end

