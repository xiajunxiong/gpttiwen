AppearancePublicView = AppearancePublicView or DeclareMono("AppearancePublicView", UIWFlushPanel)
function AppearancePublicView:AppearancePublicView()
	self.data = AppearanceData.Instance
	self.data_cares = {
		{data = self.data.select_data, func = self.FlushListView, init_call = false, keys = {"type"}},
		{data = self.data.select_data, func = self.FlushItemView, init_call = false, keys = {"data"}},
		{data = self.data.appearance_red_piont, func = self.FlushPublicView, init_call = false},
		{data = self.data.stamp_data, func = self.FlushLimitTime, init_call = false},
    }
    self.LimitTime:SetCallBack(function()
        local UseTimeTips = Language.Appearance.UseTimeTips
        self.LimitTime:SetTime(Format(Language.Appearance.UseTimeTip,UseTimeTips[0]))
    end)
end

function AppearancePublicView:Awake()
    UIWFlushPanel.Awake(self)
end

function AppearancePublicView:OnClickButton(type)
    self.select_button_type = type
    self:FlushItemView()
    self:FlushListView()
end

function AppearancePublicView:FlushListView()
    local type = self.select_button_type or 1
    local sub_type = self.data:GetSelectType()
    local data_list = self.data:GetAppearancList(sub_type,type)
    local seq = self.data:GetSelectData().seq
    self.List[type]:SetDataList(data_list,self:GetIndex(data_list,seq))
    for i=1,self.TabName:Length() do
        UH.SetText(self.TabName[i],Language.Appearance.TabName[i] .. Language.Appearance.TypeName[sub_type])
    end
    self.NotItemTip:SetActive(#data_list == 0)
    if #data_list == 0 then
        UH.SetText(self.ItemTip,Language.ItemInfo.None .. Language.Appearance.TypeName[sub_type])
    end
end

function AppearancePublicView:GetIndex(list,seq)
    for k,v in pairs(list) do
        if seq and v.seq == seq then
            return k
        end
    end
end

function AppearancePublicView:FlushPublicView()
    self:FlushListView()
    self:FlushItemView()
end

function AppearancePublicView:FlushItemView()
    local data = self.data:GetSelectData()
    if data.res_type == nil then
        return
    end
    local index = data.res_type + 1
    local pre = AppearanceConfig.IconPre[data.res_type]
    local icon_type = AppearanceConfig.IconType[data.res_type]

    if data.res_type == AppearanceConfig.Type.Bubble then
        self.Icon[index]:SetData(data.res_id,Vector2.New(174,65))
    else
        if data.res_id ~= 0 then
            UH.SetIcon(self.Icon[index],pre .. data.res_id,icon_type)
        else
            UH.SetText(self.TitleName,TitleData.GetQuaName(data.title_id))
            UH.SetIcon(self.Icon[index],nil)
        end
        self.Title:SetActive(data.res_id == 0)
    end
    UH.SetText(self.ContentTip,data.res_desc)
    --家族称号
    if TitleData.IsGuildTitle(data.title_id) then
        UH.SetText(self.TitleName2, data.name)
        index = index + 1
    end
    if TitleData.IsGuildTitle(data.title_id) and GuildData.Instance:GetGuildID() > 0 then
        UH.SetText(self.TitleName2, string.format(Language.Guild.TitleName, GuildData.Instance:GetGuildName() , GuildData.Instance:GetJobName(GuildData.Instance:GetMemberVo().post)))
    end
    for i=1,self.IconList:Length() do
        self.IconList[i]:SetActive(i == index)
    end

    local state = AppearanceData.Instance:GetActiveFlag(data.seq)
    if state ~= 0 then
        local is_equip = self.data:IsEquipData(data.res_type,data.seq)
        UH.SetText(self.EquipState,Language.Appearance.EquipState[is_equip and 2 or 1])
    end
    local is_duel_title = DuelData.Instance:IsDuelTitle(tonumber(data.title_id) or 0)
    self.NotHasTip:SetActive(state == 0)
    self.BtnClick:SetActive(state ~= 0 and not is_duel_title)
    UH.SetText(self.ItemName,data.name or "")

    self:FlushLimitTime()
end

--刷新有效期时间
function AppearancePublicView:FlushLimitTime()
    local data = self.data:GetSelectData()
    if data == nil then return end
    local state = AppearanceData.Instance:GetActiveFlag(data.seq)
    self.LimitTime:CloseCountDownTime()
    local UseTimeTips = Language.Appearance.UseTimeTips
    if data.time == 0 then
        if data.special_time ~= 0 and data.special_time ~= "" then
            self.LimitTime:SetTime(Format(Language.Appearance.UseTimeTip,data.special_time))
        else
            self.LimitTime:SetTime(Format(Language.Appearance.UseTimeTip,UseTimeTips[state]))
        end
    else
        local stamp_time = self.data:GetFaceStampTime(data.seq)
        if stamp_time == 0 then
            self.LimitTime:SetTime(Format(Language.Appearance.UseTimeTip,UseTimeTips[0]))
        else
            self.LimitTime:StampTime(stamp_time,TimeType.Type_Time_5,Language.Appearance.UseTimeTip)
        end
    end
end

function AppearancePublicView:OnClick()
    local data = self.data:GetSelectData()
    if self.data:IsEquipData(data.res_type,data.seq) then
        AppearanceCtrl.Instance:SendAddReq(2,data.res_type)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    else
        AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
        AppearanceCtrl.Instance:SendAddReq(1,data.seq)
    end
end


-- ========== 称号的界面 ============
AppearancePublicViewTitle = AppearancePublicViewTitle or DeclareMono("AppearancePublicViewTitle", AppearancePublicView)
function AppearancePublicViewTitle:FlushListView()
    local type = self.select_button_type or 1
    local sub_type = self.data:GetSelectType()
    local data_list = self.data:GetAppearancTitleList(type)
    local seq = self.data:GetSelectData().seq
    self.List[1]:SetDataList(data_list,self:GetIndex(data_list,seq))
    for i=1,self.TabName:Length() do
        UH.SetText(self.TabName[i],Language.Appearance.TitleType[i])
    end
    self.NotItemTip:SetActive(#data_list == 0)
    if #data_list == 0 then
        UH.SetText(self.ItemTip,Language.ItemInfo.None .. Language.Appearance.TypeName[sub_type])
    end
end


----------------------------AppearancePublicItem----------------------------
AppearancePublicItem = AppearancePublicItem or DeclareMono("AppearancePublicItem", UIWidgetBaseItem)

function AppearancePublicItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.res_id ~= -1 then
        if self.Exp then
            UH.SetText(self.Exp,data.add_exp)
        end
        self.Use:SetActive(AppearanceData.Instance:IsEquipData(data.res_type,data.seq))
    else
        self.Use:SetActive(false)
    end
    if self.Panel then
        self.Panel:SetActive(data.res_id ~= -1)
    else
        self.Lock:SetActive(data.res_id == -1 or AppearanceData.Instance:GetActiveFlag(data.seq) == 0)
    end
    self:FlushIcon(data.res_id,data.res_type,data.title_id)
    self:FlushRedPiont()
end

--刷新图标
function AppearancePublicItem:FlushIcon(res_id,res_type,title_id)
    local is_bubble = res_type == AppearanceConfig.Type.Bubble
    if res_id > 0 then
        if is_bubble then
            self.Bubble:SetData(res_id,Vector2.New(174,65))
        else
            local icon_pre = AppearanceConfig.IconPre[res_type].. res_id
            UH.SetIcon(self.Icon,icon_pre,AppearanceConfig.IconType[res_type])
        end
        UH.SetText(self.Title,"")
    elseif res_id == 0 then
        UH.SetText(self.Title,TitleData.GetQuaName(title_id))
    else
        UH.SetText(self.Title,"")
    end
    if TitleData.IsGuildTitle(self.data.title_id) and GuildData.Instance:GetGuildID() > 0 then
        UH.SetText(self.TitleName2, string.format(Language.Guild.TitleName, GuildData.Instance:GetGuildName() , GuildData.Instance:GetJobName(GuildData.Instance:GetMemberVo().post)))
        UH.SetText(self.Title,"")
    end
    self.Icon4:SetActive(TitleData.IsGuildTitle(self.data.title_id))
    self.Bubble:SetObjActive(res_id > 0 and is_bubble)
    self.Icon:SetObjActive(res_id > 0 and not is_bubble)
end

function AppearancePublicItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.res_type then
        AppearanceData.Instance:SetSelectData(self.data)
    end
end

--刷新红点
function AppearancePublicItem:FlushRedPiont()
    local flag = AppearanceData.Instance:GetRemindTipsFlag(self.data.seq)
    if self.RedPiont then
        self.RedPiont:SetNum(flag and 1 or 0)
    end
end

function AppearancePublicItem:OnClickItem()
    AppearanceData.Instance:SetRemindTipsFlag(self.data.seq,false)
    if self.RedPiont then
        self.RedPiont:SetNum(0)
    end
end

--title_id 或者 称号配置
----------------------------AppearanceTitleItem----------------------------
AppearanceTitleItem = AppearanceTitleItem or DeclareMono("AppearanceTitleItem", UIWidgetBaseItem)

function AppearanceTitleItem:SetData(data)
    local config = data.res_id and data or TitleData.GetTitleConfig(data.title_id)
    if config.res_id ~= 0 then
        if string.find(config.res_id,"title_") then
            UH.SetIcon(self.TitleIcon,config.res_id)
        else
            UH.SetIcon(self.TitleIcon,"title_" .. config.res_id)
        end
    else
        UH.SetText(self.TitleName,TitleData.GetQuaName(data.title_id))
        UH.SetIcon(self.TitleIcon,nil)
    end
    self.Title:SetActive(config.res_id == 0)
end