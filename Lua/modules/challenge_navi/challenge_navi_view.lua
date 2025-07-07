ChallengeNaviView = ChallengeNaviView or DeclareView("ChallengeNaviView", "challenge_navi/challenge_navi")
VIEW_DECLARE_LEVEL(ChallengeNaviView, ViewLevel.L0)

function ChallengeNaviView:LoadCallback()
    self.List:SetData(ChallengeNaviData.Instance:GetChallengeNaviList())
    self.List:SetObjActive(true)
end

function ChallengeNaviView:OnClickReturn()
    ViewMgr:CloseView(ChallengeNaviView)
end

--------------------------------------ChallengeNaviItem----------------------------------------
ChallengeNaviItem = DeclareMono("ChallengeNaviItem",UIWidgetBaseItem)

function ChallengeNaviItem:SetData(data)
    UH.RenameGame(self,Format("item (%d)",data.index))
    UH.SpriteName(self.Icon,data.id)
    UH.SpriteName(self.Name,[[_LocWenZi]] .. data.id)
    local is_fun_open = FunOpen.Instance:IsFunOpen(data.mod)

    -- if Mod.HarmonyHeaven.Main == data.mod then
    --     is_fun_open = is_fun_open and (RoleData.Instance:GetWorldLevel() >= HarmonyHeavenData.Instance:ConfigOtherStartLevel())
    -- end

    local config = ChallengeNaviData.Instance:GetModData(data.mod)
    self.LockObj:SetActive(not is_fun_open)
    self.NameEff:SetActive(is_fun_open)
    self.TeamTag:SetActive(data.is_team == 1)
    UH.SetInter(self.LockInter,is_fun_open)
    UH.SetText(self.DescTitle,"")
    UH.SetText(self.DescVal,"")
    UH.SetText(self.LeftTimeTitle,"")
    self.LeftTimeVal:SetTime("")
    self.ComTxtObj:SetActive(false)

    -- 鸿蒙天界差异太大，所以特殊处理
    self.HarmonyHeavenItem:SetObjActive(Mod.HarmonyHeaven.Main == data.mod and is_fun_open)
    if Mod.HarmonyHeaven.Main == data.mod then
        if is_fun_open then
            self:FlushItemView(config or {})
            self.HarmonyHeavenItem:FlushShow()
        else
            self.DescTitle.text = Format(Language.ChallengeNavi.FunLockHH, FunOpen.Instance:GetFunOpenLevel(data.mod)) 
        end
    else
        if is_fun_open then
            self:FlushItemView(config or {})
        else
            self.DescTitle.text = Format(Language.ChallengeNavi.FunLock,FunOpen.Instance:GetFunOpenLevel(data.mod)) 
        end
    end
    if data.index >= 7 then
        self.CanvasGroup.alpha = 1
    end
    --显示红点显示
    if not IsEmpty(data.mod) then
        self:FlushRemindView(data.mod)
    end
    --刷新特效显示
    if is_fun_open and data.effect ~= 0 then
        self:SetEffectView(data.effect)
    end
    self.Effect:SetActive(is_fun_open)
    --刷新日常/周常
    if data.label_type ~= 0 then
        UH.SpriteName(self.ImgLabel,[[_LocLabel]] .. data.label_type)
    end
    self.Label:SetActive(data.label_type ~= 0)

    UIWidgetBaseItem.SetData(self,data)
    if not IsEmpty(data.is_guide) then
        self:SetGetGuideUi(data.mod)
    end
end

function ChallengeNaviItem:SetGetGuideUi(mod)
    GuideManager.Instance:RegisterGetGuideUi(ChallengeNaviFunc.GetModName(mod),function()
        self.ItemList:AutoSlide(self.data,true)
        return self.ClickEve
    end)
end

function ChallengeNaviItem:OnDisable()
    if self.data.is_guide == nil then return end
    GuideManager.Instance:UnRegisterGetGuideUi(ChallengeNaviFunc.GetModName(self.data.mod))
end

function ChallengeNaviItem:OnClickMe()
    if FightCtrl.IsActFB() then
        return
    end
    if self.data.open_req_func == nil then
        ViewMgr:OpenViewByKey(self.data.mod)
    else
        self.data.open_req_func()
    end
    ViewMgr:CloseView(ChallengeNaviView)
end

function ChallengeNaviItem:FlushRemindView(mod)
    if self.handle == nil then
        self.handle = RegisterRemind(mod,function(num)
            self.RedPoint:SetNum(num)
        end, true)
    end
end

function ChallengeNaviItem:SetEffectView(effect)
    UH.StopEffect(self.EffectTool,self.effect_handle)
    self.effect_handle = UH.PlayEffect(self.EffectTool,effect,self.Effect)
    UH.SpriteName(self.Icon,nil)
end

function ChallengeNaviItem:OnDestroy()
    if self.data == nil then return end
    UnRegisterRemind(self.data.mod,self.handle)
end

function ChallengeNaviItem:FlushItemView(config)
    if config.variant_func ~= nil then
        local left,total,time_st,time_str = config.variant_func()
        if (config.show_complete_flag == ChallengeNaviConfig.CompleteShowType.Zero and left <= 0) or 
            (config.show_complete_flag == ChallengeNaviConfig.CompleteShowType.Full and left >= total)  then   --显示今日已完成
            self.ComTxtObj:SetActive(true)
            UH.SetText(self.ComTxt, Language.ChallengeNavi.CompleteDesc[config.show_complete_desc == nil and 1 or config.show_complete_desc])
        else
            local desc_str = ""
            if config.variant_map_func ~= nil then
                desc_str = config.variant_map_func(left,total)
            else
                desc_str = ChallengeNaviFunc.DescMapCommom(left,total)
            end
            UH.SetText(self.DescTitle,config.desc_val1)
            UH.SetText(self.DescVal,desc_str)
        end
        --模块时间展示
        if time_st ~= nil and (config.desc_val2 ~= nil or time_str ~= nil) then
            UH.SetText(self.LeftTimeTitle, time_str or config.desc_val2)
            self.LeftTimeVal:SetShowCallBack(function (t)
                if tonumber(t.day) > 0 then
                    return Format(Language.ChallengeNavi.TimeFormat1,tonumber(t.day),tonumber(t.hour))
                elseif tonumber(t.hour) > 0 then
                    return Format(Language.ChallengeNavi.TimeFormat2,tonumber(t.hour))
                elseif tonumber(t.min) > 0 or tonumber(t.s) > 0 then
                    local min = tonumber(t.min) > 0 and tonumber(t.min) or 1
                    return Format(Language.ChallengeNavi.TimeFormat3,min)
                else
                    --计时已到，直接取消显示
                    self.LeftTimeTitle.text = ""
                    return ""
                end
            end)
            self.LeftTimeVal:StampTime(time_st,nil,nil,"")
        end
    else
        UH.SetText(self.DescTitle,config.desc_val1)
    end
end

ChallengeNaviItemHH = DeclareMono("ChallengeNaviItemHH", UIWidgetBaseItem)

function ChallengeNaviItemHH:ChallengeNaviItemHH()
    self.hh_data = HarmonyHeavenCtrl.Instance:Data()
    self.language = Language.HarmonyHeaven.Navi
end

function ChallengeNaviItemHH:FlushShow()
    local info = self.hh_data.fb_info.role_info
	if info.init then return end
    local boss_reward_limit = self.hh_data:ConfigOtherBossRewardLimit()
	local help_reward_limit = self.hh_data:ConfigOtherHelpRewardLimit()
	local reward_times_show = info.rewards_times_max
	local help_times_show = info.help_times

    self.ValShow1.text = string.format(self.language.ValShow1, info.rewards_times_max, boss_reward_limit)
    self.ValShow2.text = string.format(self.language.ValShow2, info.help_times, help_reward_limit)
end