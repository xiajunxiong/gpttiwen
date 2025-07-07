ExperienceView = ExperienceView or DeclareView("ExperienceView", "experience/experience_view",Mod.Experience.Main)

VIEW_DECLARE_LEVEL(ExperienceView,ViewLevel.L0)

function ExperienceView:LoadCallback(param_t)
    UH.SetText(self.Tips,Language.Experience.HelpTips)
    local list = ExperienceData.Instance:GetExperienceList()
    self.List:SetCompleteCallBack(function()self.ListAnim:Play()end)
    self.List:SetData(list)
    self:CheckGetGuideUi(list)
end

function ExperienceView:CheckGetGuideUi(list)
    for k,v in pairs(list) do
        if not IsEmpty(v.is_guide) then
            GuideManager.Instance:RegisterGetGuideUi(ChallengeNaviFunc.GetModName(v.mod),function()
                self.List:JumpVerticalIndex(v.index,180)
                if self.List.item_list and self.List.item_list[v.index] then
                    return self.List.item_list[v.index]
                end
            end)
        end
    end
end

function ExperienceView:OnClickClose()
    ViewMgr:CloseView(ExperienceView)
end

function ExperienceView:OnClickHelp()
    local help_id = Config.experience_auto.other[1].explain
    PublicPopupCtrl.Instance:HelpTip(help_id)
end

--===========================ExperienceViewItem===========================
ExperienceViewItem = ExperienceViewItem or DeclareMono("ExperienceViewItem", UIWidgetBaseItem)

function ExperienceViewItem:SetData(data)
    UH.RenameGame(self,[[Item]] .. data.index)
    local index = data.index % 2 ~= 0 and 1 or 2
    if self.PanelList[index] then
        self.PanelList[index]:SetData(data)
    end
    for i=1,self.PanelList:Length() do
        self.PanelList[i]:SetObjActive(i == index)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function ExperienceViewItem:OnClickItem()
    if FightCtrl.IsActFB() then
        return
    end
    local opened, open_tip = FunOpen.Instance:GetFunIsOpened(self.data.mod)
    if not opened then
        PublicPopupCtrl.Instance:Center(open_tip)
        return
    end
    local config = ChallengeNaviData.Instance:GetModData(self.data.mod)
    if config and config.open_req_func then
        config.open_req_func()
    else
        ViewMgr:OpenViewByKey(self.data.mod)
    end
    ViewMgr:CloseView(ExperienceView)
end

--===========================ExperiencePanelItem===========================
ExperiencePanelItem = ExperiencePanelItem or DeclareMono("ExperiencePanelItem", UIWidgetBaseItem)

function ExperiencePanelItem:SetData(data)
    UH.SpriteName(self.Icon,data.id)
    UH.SpriteName(self.Title,[[_LocTitle]] .. data.id)
    if data.label_type ~= 0 then
        UH.SpriteName(self.ImgLabel,[[_LocLabel]] .. data.label_type)
    end
    self.Label:SetActive(data.label_type ~= 0)
    local split_list = Split(data.reward_sign,"|")
    for i=1,self.SignList:Length() do
        if split_list[i] then
            self.SignList[i]:SetData(split_list[i])
        end
        self.SignList[i]:SetObjActive(split_list[i] ~= nil)
    end
    local is_fun_open = FunOpen.Instance:IsFunOpen(data.mod)
    self:FlushDescView(data,is_fun_open)
    self.Lock:SetActive(not is_fun_open)
    self.Effect:SetActive(is_fun_open)
    if is_fun_open and data.effect ~= 0 then
        self:SetEffectView(data.effect)
    end
    self.EffectTool:SetObjActive(is_fun_open)
    UH.SetInter(self.Inter,is_fun_open)
    --显示红点显示
    if is_fun_open and not IsEmpty(data.mod) then
        self:FlushRemindView(data.mod)
    else
        self.RedPoint:SetNum(0)
    end    
    UIWidgetBaseItem.SetData(self, data)
end

function ExperiencePanelItem:FlushRemindView(mod)
    if self.handle == nil then
        self.handle = RegisterRemind(mod,function(num)
            self.RedPoint:SetNum(num)
        end, true)
    end
end

function ExperiencePanelItem:SetEffectView(effect)
    TimeHelper:CancelTimer(self.ShowTimer)
    self.ShowTimer = Invoke(function()
        UH.StopEffect(self.EffectTool,self.effect_handle)
        self.effect_handle = UH.PlayEffect(self.EffectTool,effect,self.Icon.gameObject)
        UH.SpriteName(self.Icon,nil)
    end,0.5)
end

function ExperiencePanelItem:OnDestroy()
    TimeHelper:CancelTimer(self.ShowTimer)
    UH.StopEffect(self.EffectTool,self.effect_handle)
    if self.data == nil then return end
    UnRegisterRemind(self.data.mod,self.handle)
end

function ExperiencePanelItem:FlushDescView(data,is_fun_open)
    local config = ChallengeNaviData.Instance:GetModData(data.mod)
    if is_fun_open then
        if config.variant_func ~= nil then
            local left,total,time_st,time_str = config.variant_func()
            if (config.show_complete_flag == ChallengeNaviConfig.CompleteShowType.Zero and left <= 0) or 
                (config.show_complete_flag == ChallengeNaviConfig.CompleteShowType.Full and left >= total)  then   --显示今日已完成
                UH.SetText(self.Desc, Language.ChallengeNavi.CompleteDesc[config.show_complete_desc == nil and 1 or config.show_complete_desc])
            else
                local desc_str = ""
                if config.variant_map_func ~= nil then
                    desc_str = config.variant_map_func(left,total)
                else
                    desc_str = ChallengeNaviFunc.DescMapCommom(left,total)
                end
                UH.SetText(self.Desc,config.desc_val1 .. "：" .. desc_str)
            end
            --模块时间展示
            if time_st ~= nil and (config.desc_val2 ~= nil or time_str ~= nil) then
                self.Time:SetShowCallBack(function (t)
                    if tonumber(t.day) > 0 then
                        return Format(Language.ChallengeNavi.TimeFormat1,tonumber(t.day),tonumber(t.hour))
                    elseif tonumber(t.hour) > 0 then
                        return Format(Language.ChallengeNavi.TimeFormat2,tonumber(t.hour))
                    elseif tonumber(t.min) > 0 or tonumber(t.s) > 0 then
                        local min = tonumber(t.min) > 0 and tonumber(t.min) or 1
                        return Format(Language.ChallengeNavi.TimeFormat3,min)
                    else
                        return ""
                    end
                end)
                local time_tip = (time_str or config.desc_val2 or "") .. "：%s"
                self.Time:StampTime(time_st,nil,time_tip,"")
            else
                self.Time:SetTime("")
            end
        else
            UH.SetText(self.Desc,config.desc_val1)
        end
    else
        self.Time:SetTime("")
        self.Desc.text = Format(Language.ChallengeNavi.FunLock,FunOpen.Instance:GetFunOpenLevel(data.mod)) 
    end
end
--===========================ExperienceSignItem===========================
ExperienceSignItem = ExperienceSignItem or DeclareMono("ExperienceSignItem", UIWidgetBaseItem)

function ExperienceSignItem:SetData(data)
    UH.SetText(self.Name,data)
end