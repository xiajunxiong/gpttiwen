LandsBetweenCityView = LandsBetweenCityView or DeclareView("LandsBetweenCityView", "lands_between/lands_between_city",Mod.LandsBetween.City)
function LandsBetweenCityView:LandsBetweenCityView()

end

function LandsBetweenCityView:OpenCallback() 
    if LandsBetweenData.Instance.faction_user.wait_num == nil then
        LandsBetweenCtrl.Instance:OperateQueryFactionUser({reset = 1})
    elseif LandsBetweenData.Instance.faction_user.wait_num > 0 then 
        LandsBetweenCtrl.Instance:OperateQueryFactionUser({reset = 0})
    end 
end 

function LandsBetweenCityView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenCityView)
    LandsBetweenData.Instance.faction_user.flush = 0
end

LandsBetweenCityPanel = LandsBetweenCityPanel or DeclareMono("LandsBetweenCityPanel", UIWFlushPanel)
function LandsBetweenCityPanel:LandsBetweenCityPanel()
    self.data = LandsBetweenData.Instance

    self.data_cares = {
        {data = self.data.my_info, func = self.FlushPanel},
        {data = self.data.faction_user, func = self.FlushPanel},
        {data = self.data.faction_base, func = self.FlushPanel},
    }

    self.list:SetCompleteCallBack(function()
        if self.wait_mark then
            self.list:AutoSlide(self.wait_pos)
            self.wait_mark = nil
        end
    end)

end 

function LandsBetweenCityPanel:Awake()
    UIWFlushPanel.Awake(self)
    

end

function LandsBetweenCityPanel:FlushPanel()
    local city_param = self.data:GetCityParam()
    UH.SetText(self.city_name,city_param.name)
    UH.SetText(self.member_num,city_param.member)
    UH.SetText(self.group_name,city_param.group_name)
    UH.SetText(self.empty_lands,city_param.empty_lands)
    UH.SetText(self.hold_on,city_param.hold_on)
    UH.SpriteName(self.CityIcon, city_param.icon_str)
    local member_list = self.data:GetMemberList()
    self.list:SetData(member_list)
end

function LandsBetweenCityPanel:OnDraging()
    local num = #self.list.item_list
    local checker_y = (num-5) * 114 + (num-6) * 5
    if self.MemberRect.localPosition.y >= checker_y then 
        if self.wait_mark == nil and self.data.faction_user.wait_num > 0 then 
            self.wait_mark = true

            self.wait_pos = num - 5
            LandsBetweenCtrl.Instance:OperateQueryFactionUser({reset = 0})
        end 
    end 
end


LandsBetweenCityMemberCell = LandsBetweenCityMemberCell or DeclareMono("LandsBetweenCityMemberCell", UIWidgetBaseItem)
function LandsBetweenCityMemberCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self.master:SetActive(data.index == 1)
    
    UH.SetText(self.name,data.name)
    UH.SetText(self.guild,data.guild)
    UH.SetText(self.score,Language.LandsBetween.Desc.Scroe..data.score)
    UH.SetText(self.achievement,Language.LandsBetween.Desc.Achievement.. data.achievement)

    
    UH.SetAvatar(self.head, data.avatar_type ,data.headshot_id)

end