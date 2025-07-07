
PartnerViewUp = PartnerViewUp or DeclareMono("PartnerViewUp", UIWFlushPanel)

function PartnerViewUp:PartnerViewUp()
    self.flush_events = {EventProtocol.ToEventKey(SCPartnerBaseGradeInfo)}

    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Up

    self.data_cares = {
        {data = self.data.item_flush, func = self.onFlush, init_call = false},
        {data = BagData.Instance.item_grid,func = self.onFlush, init_call = false},
        {data = self.data.partner_up_skillf_effect,func = self.FlushSkillFEffect, init_call = false},
	}
end

function PartnerViewUp:OnEnable()
    self.data.cur_sel_tabbar.val = PartnerConfig.PartnerViewTabs.Up
    self:onFlush()
end

function PartnerViewUp:onFlush(ignore_skill)
    -- TimeHelper:CancelTimer(self.timer_dt1)
    TimeHelper:CancelTimer(self.timer_dt3)
    if not ignore_skill then TimeHelper:CancelTimer(self.timer_dt2) end
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local vo = data.vo
    local up_loop = data.up_loop

    local attr_stage = vo.attr_stage
    local cur_up_info = data.cur_up_info
    -- local up_skills_info = info.up_skills_info
    -- local lock_skills = up_skills_info[vo.up_level]

    local role_level = RoleData.Instance:GetRoleLevel()
    local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)

    local is_level_max = nil == next(info.up_info, cur_up_info.level)

    self.BtnUpObj:SetActive(PartnerConfig.UP_STAGE_MAX ~= attr_stage)
    self.BtnBreakObj:SetActive(PartnerConfig.UP_STAGE_MAX == attr_stage and not is_level_max)
    self.LevelMaxObj:SetActive(PartnerConfig.UP_STAGE_MAX == attr_stage and is_level_max)
    
    UH.SetIcon(self.PartnerIconSp, PartnerData.IconId(data), ICON_TYPE.ITEM)
    UH.SetAvatar(self.RoleIconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
    PartnerInfo.SetQuality(self.Quality, info_quality.quality)

    if self.data.up_success_switch then
        self.data.up_success_switch = false
        local stage = 0 == attr_stage and PartnerConfig.UP_STAGE_MAX or attr_stage
        self.LoopItems[stage > 1 and stage - 1 or PartnerConfig.UP_STAGE_MAX]:Stop(true)
        self.LoopItems[stage]:Play(function ()
            if 0 ~= attr_stage then return end
            -- self.timer_dt1 = TimeHelper:AddDelayTimer(function ()
                -- self.UpLoopEffect:Play(10010250)
            -- end, 0.2)
            self.UpLoopEffect:Play(10010250)
            self.timer_dt3 = TimeHelper:AddDelayTimer(function ()
                self:onFlush(true)
            end, 0.4)
            self.timer_dt2 = TimeHelper:AddDelayTimer(function ()
                -- self.LockSkillEffect:Play(10010250)
                self:FlushSkills(data.skill_f)
            end, 0.66)
        end, stage > 1, 0 == attr_stage)
        if 0 == attr_stage then
            self.LoopItems[stage - 1]:Stop(true, true)
            return
        end
    end

    self.Level.text = string.format((not is_level_max and cur_up_info.need_rolelevel > role_level) and self.language.level_en or self.language.level_e, DataHelper.GetDaXie(vo.up_level), cur_up_info.need_rolelevel)

    local function SetIsUpLevelMax(value)
        self.ItemsGridObj:SetActive(value)
    end
    if attr_stage < PartnerConfig.UP_STAGE_MAX then
        self.ItemsGrid:SetData(up_loop[attr_stage + 1].items)
    end
    SetIsUpLevelMax(attr_stage < PartnerConfig.UP_STAGE_MAX)

    for i = 1, self.LoopItems:Length() do
        self.LoopItems[i]:SetData(up_loop[i], self.LoopItems)
    end

    self.red_point_up = self.red_point_up or UiRedPoint.New(self.BtnUpObj, Vector3.New(77.7,24,0))
    self.red_point_up:SetNum(data:UpRedPoint() and 1 or 0)
    self.red_point_break = self.red_point_break or UiRedPoint.New(self.BtnBreakObj, Vector3.New(77.7,24,0))
	self.red_point_break:SetNum(data:UpRedPoint() and 1 or 0)

    if true == ignore_skill then return end
    self:FlushSkills(data.skill_f)
end

function PartnerViewUp:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    -- TimeHelper:CancelTimer(self.timer_dt1)
    TimeHelper:CancelTimer(self.timer_dt2)
    TimeHelper:CancelTimer(self.timer_dt3)
end

function PartnerViewUp:FlushSkills(skill_f)
    self.SkillFItem1:SetData(skill_f)
    self.SkillFItem2:SetData(skill_f)
end

function PartnerViewUp:FlushSkillFEffect()
    if not self.data.partner_up_skillf_effect.val then return end
    self.data.partner_up_skillf_effect.val = false
    self.SkillFEffect:Play(100108203)
    ViewMgr:OpenView(PartnerSkillfUpView)
end

function PartnerViewUp:OnClickUp()
    if PartnerCtrl.Instance:UnLockTips() then return end
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local vo = data.vo
    local up_loop = data.up_loop
    local attr_stage = vo.attr_stage
    if attr_stage < PartnerConfig.UP_STAGE_MAX then
        for _, item in pairs(up_loop[attr_stage + 1].items) do
            if item:Num() < item.cost_num then
                ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item.item_id})})
                return
            end
        end   
    else
        if RoleData.Instance:GetRoleLevel() < data.cur_up_info.need_rolelevel then
            PublicPopupCtrl.Instance:Center(string.format(self.language.level_tips, data.cur_up_info.need_rolelevel))
            return
        end
    end
    -- if not self.BtnBreakObj.activeSelf then
    --     local stuff_item = self.ItemsGrid.item_list[1] and self.ItemsGrid.item_list[1]:GetItem()
    --     local partner_item = self.ItemsGrid.item_list[2] and self.ItemsGrid.item_list[2]:GetItem()
    --     if stuff_item and not partner_item then
    --         -- MainOtherCtrl.Instance:QuickBuyView(stuff_item.item_id,stuff_item.cost_num - stuff_item:Num())
    --         local num = stuff_item.cost_num - stuff_item:Num()
    --         PublicPopupCtrl.Instance:AlertTip(Format(Language.Mall.ShopBuyTip,num,stuff_item:QuaName()),function()
    --             MallData.JumpItemNum = num
    --             MallCtrl.Instance:Jump(stuff_item.item_id,nil,Mod.Mall.Shop)
    --         end)
    --         return
    --     end
    --     if partner_item then
    --         MainOtherCtrl.Instance:GetWayView({item = partner_item})
    --         return
    --     end
    -- end

    if not data:PartnerUpCanSoar() and self.language.soar_tips[vo.soar_count] then
        PublicPopupCtrl.Instance:Center(self.language.soar_tips[vo.soar_count])
        return
    end

    PartnerCtrl.Instance:SendPartnerUp(self.data.cur_sel_partner)
end

function PartnerViewUp:OnClickAttr()
    ViewMgr:OpenView(PartnerUpAttrView)
end

PartnerViewUpLoopItem = DeclareMono("PartnerViewUpLoopItem", UIWidgetBaseItem)

function PartnerViewUpLoopItem:PartnerViewUpLoopItem()
    self.loop_setting = PartnerConfig.LoopSetting
    self.loop_name = Language.Partner.Up.loop_name
    self.point_playing = false
end

function PartnerViewUpLoopItem:SetData(data, loop_items)
    UIWidgetBaseItem.SetData(self, data)
    self.loop_items = loop_items
    if self.point_playing or nil == data or nil == data.seq or nil == data.attr_stage then return end
    local bg_s = self.loop_setting.BgS
    local name_c = self.loop_setting.NameC
    local attr_name = Language.Common.AttrList[data.att_type]
    local loop_val = "+".. (DataHelper.IsPercent(data.att_type) and Percent(data.att_value or 0) or (data.att_value or 0))
    local actived = data.seq <= data.attr_stage
    self.BgSp.SpriteName = actived and bg_s.actived or bg_s.inactive
    self.Name.text = string.format(self.loop_name, actived and name_c.actived or name_c.inactive, attr_name)
    self.Value.text = loop_val
    self.Points:Stop(actived)
    local last_item = loop_items[1 == data.seq and PartnerConfig.UP_STAGE_MAX or data.seq - 1]
    if not last_item:PointPlaying() then
        self:Effecting(data.seq == data.attr_stage + 1)
    end
end

function PartnerViewUpLoopItem:Effecting(value)
    self.ItemEffecting:SetActive(value)
end

function PartnerViewUpLoopItem:Play(func, value, value2)
    if not self.data or self.data.seq == nil then return end
    if not value then
        self.ItemEffect:Play(10010249)
        self:SetData(self.data, self.loop_items)
        self.loop_items[PartnerConfig.UP_STAGE_MAX == self.data.seq and 1 or self.data.seq + 1]:Effecting(true)
        func()
        return
    end
    if not value2 then
        self.point_playing = true
        self.Points:Play(function ()
            self.point_playing = false
            self:SetData(self.data, self.loop_items)
            self.ItemEffect:Play(10010249)
            if PartnerConfig.UP_STAGE_MAX ~= self.data.seq then
                self.loop_items[self.data.seq + 1]:Effecting(true)
            end
            func()
        end)
    else
        self:SetData(self.data, self.loop_items)
        func()
    end
end

function PartnerViewUpLoopItem:Stop(is_up, is_flush)
    self.Points:Stop(is_up)
    self.point_playing = false
    if is_flush then self:SetData(self.data, self.loop_items) end
end

function PartnerViewUpLoopItem:PointPlaying()
    return self.point_playing
end

PartnerUpLoop = PartnerUpLoop or DeclareMono("PartnerUpLoop")

function PartnerUpLoop:PartnerUpLoop()
    self.is_play = false
end

function PartnerUpLoop:Play(func)
    local index = 0
    self:Stop(false)
    self.timer_rt = TimeHelper:AddRunTimer(function ()
        index = index + 1
        if index > 2 then
            func()
        else
            self.PointSps[index].SpriteName = "LvSeXiaoDian01"
        end
    end, 0.01, 4)
end

function PartnerUpLoop:Stop(is_up)
    TimeHelper:CancelTimer(self.timer_rt)
    for i = 1, self.PointSps:Length() - 2 do
        self.PointSps[i].SpriteName = is_up and "LvSeXiaoDian01" or "JinJieDian_Hui1"
    end
end

function PartnerUpLoop:OnDestroy()
    TimeHelper:CancelTimer(self.timer_rt)
end

PartnerViewUpSkilItem1 = DeclareMono("PartnerViewUpSkilItem1", UIWidgetBaseItem)

function PartnerViewUpSkilItem1:PartnerViewUpSkilItem1()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.SkillInfo
end

function PartnerViewUpSkilItem1:SetData(data)
    local info = data.skill_cur
    if nil == data.skill_cur then
        self.ItemObj:SetActive(false)
        return
    end
    self.ItemObj:SetActive(true)
    self.SkillCell:SetData(info)
    self.Name.text = info.name
    self.Tips.text = self.partner_data.partner_friendly_max_level == info.level and self.language.skillf_level_max or ""
    self.SkillCell.SkillLevelF.text = info.level and string.format(self.language.skill_level_f, info.level) or ""
    for i = 1, self.Attrs:Length() do
        local attr = data.attrs_cur[i]
        if attr then
            self.Attrs[i].text = string.format(self.language.skillf_attr, attr.attr_name, attr.attr_value)
        end
    end
end

PartnerViewUpSkilItem2 = DeclareMono("PartnerViewUpSkilItem2", UIWidgetBaseItem)

function PartnerViewUpSkilItem2:PartnerViewUpSkilItem2()
    self.language = Language.Partner.SkillInfo
end

function PartnerViewUpSkilItem2:SetData(data)
    local info = data.skill_next
    if nil == info then
        self.ItemObj:SetActive(false)
        return
    end
    self.ItemObj:SetActive(true)
    self.SkillCell:SetData(info)
    self.Name.text = info.name
    self.Tips.text = string.format(1 == info.level and self.language.skillf_active or self.language.skillf_up, data.partner_name, DataHelper.GetDaXie(data.need_level))
    self.SkillCell.SkillLevelF.text = info.level and string.format(self.language.skill_level_f, info.level) or ""
    for i = 1, self.Attrs:Length() do
        local attr = data.attrs_next[i]
        if attr then
            self.Attrs[i].text = string.format(self.language.skillf_attr, attr.attr_name, attr.attr_value)
        end
    end
end