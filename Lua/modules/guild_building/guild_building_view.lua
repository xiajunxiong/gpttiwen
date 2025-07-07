GuildBuildingView = GuildBuildingView or DeclareView("GuildBuildingView", "guild/guild_building", Mod.Guild.GuildBuilding)
VIEW_DECLARE_LEVEL(GuildBuildingView, ViewLevel.L1)
VIEW_DECLARE_MASK(GuildBuildingView, ViewMask.BgBlock)

function GuildBuildingView:GuildBuildingView()
    
end

function GuildBuildingView:OnHelpClick()
    local build_desc_id = Config.family_auto.other[1].build_desc_id or 1
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[build_desc_id].desc)
end

function GuildBuildingView:CloseCallback()
    
end

function GuildBuildingView:OnCloseClick()
    ViewMgr:CloseView(GuildBuildingView)
end

----------------GuildBuildingPanel----------------
GuildBuildingPanel = GuildBuildingPanel or DeclareMono("GuildBuildingPanel", UIWFlushPanel)
function GuildBuildingPanel:GuildBuildingPanel()
    self.Data = GuildBuildingData.Instance
    self.language = Language.GuildBuilding
    self.data = self.Data.info_data
    self.select_building_data = self.Data.select_building_data
    self.Data:SelectBuildingData()
    self.is_wait = false
    self.data_cares = {
        {data = self.select_building_data, func = self.FlushLeft, init_call = false},
        {data = GuildData.Instance.guild_info, func = self.FlushLeft, keys = {"exp"}, init_call = false},
        {data = self.data, func = self.FlushRight, keys = {"info", "speed_info"}},
        {data = self.data, func = self.FlushLeft, keys = {"finish_time"}},
        {data = self.Data.remind_data, func = self.FlushEffect, init_call = false},
    }
end

function GuildBuildingPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushEffect()
    self.Data:SetTodayRemind()
end

function GuildBuildingPanel:FlushEffect()
    if self.Data:IsPlayingEffect() then
        TimeHelper:CancelTimer(self.ht_effect_2)
        self.ht_effect_2 = TimeHelper:AddDelayFrameTimer(function()
            if self.effect then
                self.EffectTool:Stop(self.effect)
            end
            self.effect = self.EffectTool:Play(7165043)
        end)
        self.is_wait = false
        TimeHelper:CancelTimer(self.ht_effect)
        self.ht_effect = Invoke(BindTool.Bind(self.PlayLevelUpTxt, self), 0.5)
    end
end

function GuildBuildingPanel:PlayLevelUpTxt()
    if not TableIsEmpty(self.Data.remind_data.info) then
        for k, v in pairs(self.Data.remind_data.info) do
            if v > 0 then
                PublicPopupCtrl.Instance:Center(Format(self.language.FinishTip, self.Data:BuildingName(k)))
            end
        end
    end
    self.Data:IsPlayingEffect(false)
    self.Data:ResetAllNowFinishBuilding()
end

function GuildBuildingPanel:FlushLeft()
    local build_type = self.select_building_data.info.build_type
    --#2394659 LuaException
    if build_type == nil then
        return
    end
    UH.SpriteName(self.ImgSetIcon, "Icon_" .. build_type)
    UH.SetText(self.TxtBuildName, self.Data:BuildingName(build_type))
    UH.SetText(self.TxtBuildLevel, self.select_building_data.info.build_level)

    local now_level_config = self.Data:BuildingConfig(build_type)
    UH.SetText(self.TxtBuildEffect, now_level_config.descripion)
    local _, now_money = GuildData.Instance:GetIsFullMoney()
    local need_money = now_level_config.up_level
    UH.SetText(self.TxtMoney, Format(Language.Common.Cstr, now_money >= need_money and COLORSTR.Green9 or COLORSTR.Red20, now_money, need_money))
    UH.SetText(self.TxtTime, self.select_building_data.info.finish_time == 0 and self.language.Time[1] or self.language.Time[2])
    self.Timer:CloseCountDownTime()
    if self.select_building_data.info.finish_time == 0 then
        self.Timer:SetTime(self:GetTimeDesc(now_level_config.up_expend_time))
    else
        self.Timer:StampTime(self.select_building_data.info.finish_time)
    end
    local next_level_config = self.Data:BuildingConfig(build_type, self.select_building_data.info.build_level + 1)
    if not TableIsEmpty(next_level_config) then
        UH.SetText(self.TxtNextEffect, Format(self.language.NextLevelEffect[1], next_level_config.build_level, next_level_config.descripion))
    else
        UH.SetText(self.TxtNextEffect, self.language.NextLevelEffect[2])
    end
    self.ObjBtnBuild:SetActive(self.select_building_data.info.build_level == 0)
    -- self.ObjBtnDown:SetActive(self.select_building_data.info.build_level ~= 0)
    self.ObjBtnUp:SetActive(self.select_building_data.info.build_level ~= 0)
    self.ObjBtnBuilding:SetActive(self.select_building_data.info.finish_time > 0)
    UH.SetText(self.TxtBuilding, self.language.BuildingBtn[self.Data:IsSpeedUping() and 2 or 1])
    self.ObjCanSpeed:SetActive(self.Data:IsSpeedUping())
end

function GuildBuildingPanel:FlushRight()
    self.is_wait = false
    for i = 1, self.Buildings:Length() do
        self.Buildings[i]:SetData(self.data.info[i])
    end
    self.Buildings[self.select_building_data.index]:SetSelected(true)
    self.Buildings[self.select_building_data.index]:OnSelect(true)
    if ViewMgr:IsOpen(DialogTipsView) then
        ViewMgr:CloseView(DialogTipsView)
        PublicPopupCtrl.Instance:Center(self.language.InfoChangeTip)
    end
    UH.SetText(self.TxtSpeedUpTimes, self.Data:BuildPermission() and Format(self.language.SpeedUpTimes, self.Data:TodayCanBuildTimes()) or "")
end

-- 将消耗市场转化为00:00:00返回
function GuildBuildingPanel:GetTimeDesc(second)
    local hour = math.floor(second / 60 / 60)
    local min = math.floor(second / 60) % 60
    local sec = math.floor(second % 60)
    local add_0 = function(num)
        if num < 10 then
            return "0" .. num
        end
        return num
    end
    return add_0(hour) .. ":" .. add_0(min) .. ":" .. add_0(sec)
end

function GuildBuildingPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht_effect_2)
    TimeHelper:CancelTimer(self.ht_effect)
    if self.Data:IsPlayingEffect() then
        self:PlayLevelUpTxt()
    end
end

-- 点击升级建造
function GuildBuildingPanel:OnBuildUpClick()
    if not self:CommonCheck() then
        return
    end
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    -- 检查今日建设次数
    if self.Data:TodayCanBuildTimes() == 0 then
        PublicPopupCtrl.Instance:Center(self.language.BuildTimesTip)
        return
    end
    local build_type = self.select_building_data.info.build_type
    local build_level = self.data.info[build_type].build_level
    -- 检查是否已满级
    local next_level_config = self.Data:BuildingConfig(build_type, build_level + 1)
    if TableIsEmpty(next_level_config) then
        PublicPopupCtrl.Instance:Center(self.language.LevelMaxTip)
        return
    end
    -- 判断是否有建筑建造升级中
    if self.data.finish_time > 0 then
        local now_building_type = self.data.now_building_type
        local now_building_level = self.data.info[now_building_type].build_level
        PublicPopupCtrl.Instance:Center(Format(self.language.NowBuildingTip[now_building_level > 0 and 1 or 0], self.Data:BuildingName(self.data.now_building_type)))
        return
    end
    -- 检查资金是否足够
    local _, now_money = GuildData.Instance:GetIsFullMoney()
    local now_level_config = self.Data:BuildingConfig(build_type)
    if now_level_config.up_expend_capital > now_money then
        PublicPopupCtrl.Instance:Center(self.language.MoneyTip)
        return
    end
    -- 检查是否因大厅而不能升级
    local now_hall_config = self.Data:BuildingConfig(GuildBuildingType.Hall)
    if build_type ~= GuildBuildingType.Hall and build_level >= now_hall_config.paramet_1 then
        PublicPopupCtrl.Instance:Center(self.language.NeedLevelHallTip)
        return
    end
    local can_speed_tip = self.Data:TodayCanSpeedTimes() > 0 and self.language.NowCanSpeedTip or ""
    PublicPopupCtrl.Instance:AlertTip(Format(self.language.BuildUpTip, self.Data:BuildingName(build_type), can_speed_tip), function()
        -- 无法在跨服操作
        -- if LoginData.Instance:IsOnCrossSever() then
        --     PublicPopupCtrl.Instance:Center(self.language.CrossingCannotOpen)
        --     return
        -- end
        self.is_wait = true
        GuildBuildingCtrl.Instance:SendGuildBuildingReq(build_type, GuildBuildingReqType.Up)
    end)
end

-- 点击降级
function GuildBuildingPanel:OnBuildDownClick()
    if not self:CommonCheck() then
        return
    end
    local build_type = self.select_building_data.info.build_type
    local build_level = self.data.info[build_type].build_level
    -- 检查是否已达最低等级
    if build_level == 0 then
        PublicPopupCtrl.Instance:Center(self.language.LevelMinTip)
        return
    end
    -- 厢房情况
    if build_type == GuildBuildingType.Wing then
        if not self:WingCheck(build_level - 1) then
            return
        end
    end
    local last_config = self.Data:BuildingConfig(build_type, build_level - 1)
    -- 大厅情况
    if build_type == GuildBuildingType.Hall then
        -- 其他建筑升级时大厅不能降级
        for k, v in pairs(self.data.info) do
            if v.finish_time > 0 then
                PublicPopupCtrl.Instance:Center(self.language.HallCannotDownTip)
                return
            end
        end
        -- 大厅降级需要判断厢房情况
        if not self:WingCheck(last_config.paramet_1) then
            return
        end
    end

    -- 弹窗拆除
    local now_hall_config = self.Data:BuildingConfig(GuildBuildingType.Hall)
    local build_name = self.Data:BuildingName(build_type)
    local str = ""
    if build_type == GuildBuildingType.Hall then
        if build_level == 1 then
            str = Format(self.language.BuildDownTip[4], build_name)
        else
            str = Format(self.language.BuildDownTip[3], build_name, last_config.paramet_1)
        end
    else
        if build_level == 1 then
            str = Format(self.language.BuildDownTip[2], build_name)
        else
            str = Format(self.language.BuildDownTip[1], build_name, build_name, build_level - 1)
        end
    end
    PublicPopupCtrl.Instance:AlertTip(str, function()
        self.is_wait = true
        GuildBuildingCtrl.Instance:SendGuildBuildingReq(build_type, GuildBuildingReqType.Down)
    end)
end

-- 升降级通用检查
function GuildBuildingPanel:CommonCheck()
    if self.is_wait then
        return false
    end
    if TableIsEmpty(self.select_building_data.info) then
        return false
    end
    local build_type = self.select_building_data.info.build_type
    local build_level = self.data.info[build_type].build_level
    -- 检查权限
    if not self.Data:BuildPermission() then
        PublicPopupCtrl.Instance:Center(self.language.PermissionTip)
        return false
    end
    -- 检查是否正在升级
    if self.data.info[build_type].finish_time > 0 then
        PublicPopupCtrl.Instance:Center(self.language.BuildingTip)
        return false
    end
    return true
end

-- 厢房情况检查
function GuildBuildingPanel:WingCheck(build_level)
    local now_person = GuildData.Instance:GetNowPerson()
    local person_max = GuildData.Instance:GetMaxPerson(false)
    -- 检查家族人数是否超过上一级厢房限制人数
    local last_config = self.Data:BuildingConfig(GuildBuildingType.Wing, build_level)
    if not TableIsEmpty(last_config) then
        if now_person > (person_max + last_config.paramet_1) then
            PublicPopupCtrl.Instance:Center(self.language.PersonWillFullTip)
            return false
        end
    end
    return true
end

-- 点击建造中（加速）
function GuildBuildingPanel:OnBuildingClick()
    if self.Data:IsSpeedUping() then
        ChatData.Instance:SelChannelId(ChatConfig.ChannelId.guild, true)
        ViewMgr:CloseView(GuildView)
        ViewMgr:OpenView(ChatView)
        ViewMgr:CloseView(GuildBuildingView)
    else
        PublicPopupCtrl.Instance:Center(self.language.BuildingTip)
    end
end

--------------GuildBuildingItem--------------
GuildBuildingItem = GuildBuildingItem or DeclareMono("GuildBuildingItem", UIWidgetBaseItem)
function GuildBuildingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local build_name = GuildBuildingData.Instance:BuildingName(data.build_type, true)
    for i = 1, self.Names:Length() do
        UH.SetText(self.Names[i], build_name)
    end
    self.ObjGrey:SetActive(data.build_level == 0)
    self.ObjTime:SetActive(data.finish_time > 0)
    self.Timer:CloseCountDownTime()
    if data.finish_time == 0 then
        self.Timer:SetTime("")
    else
        self.Timer:StampTime(data.finish_time)
    end
end

function GuildBuildingItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        GuildBuildingData.Instance:SelectBuildingData(self.BuildType, self.data)
    end
end

function GuildBuildingItem:OnDestroy()

end