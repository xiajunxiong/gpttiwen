MainFunctionPanel = MainFunctionPanel or DeclareMono("MainFunctionPanel", UIWFlushPanel)

function MainFunctionPanel:MainFunctionPanel()
    EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.FlushIcon, self))
    self.data_cares = {
        {data = MainViewData.Instance.view_data, func = self.FlushIcon, keys = {"flush_rb"}},
    }
    self.Diwh = Vector2.New(640, 460)
    self.Listxy = Vector2.zero
end
function MainFunctionPanel:Awake()
    UIWFlushPanel.Awake(self)
    if IS_STANDALONE() then
        UH.LocalScale(self.RootRect, STANDALONE_UI_VECTOR3)
    end
end

function MainFunctionPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushIcon()
end
function MainFunctionPanel:FlushIcon()
    local all_info = TableCopy(Config.sundries_auto.main_screen_button)
    local list1 = {}
    local list2 = {}
    for i, v in ipairs(all_info) do
        if type(v.mod) == "string" then
            v.mod = GetMod(v.mod)
        end
        if type(v.open) == "string" then
            v.open = GetModOpen(v.open)
        end
        if v.sprite == "XunBao" then
            v.lock_func = function()
                local list = TreasureHuntData.Instance:GetOpenedTreasureID()
                return next(list) ~= nil 
            end
        end
        table.insert(v.place == 1 and list1 or list2, v)
    end
    self.is_complete_top = false
    self.is_complete_bottom = false
    self.ListTop:SetCompleteCallBack(BindTool.Bind(self.ListCreateComplete, self, true))
    self.ListBottom:SetCompleteCallBack(BindTool.Bind(self.ListCreateComplete, self, false))
    self.ListTop:SetData(list1)
    self.ListBottom:SetData(list2)
    self:RegisterGetGuideUi("EquipBtn", "ZhuangBei")
    self:RegisterGetGuideUi("PartnerBtn", "HuoBan")
    self:RegisterGetGuideUi("IllustratedBtn", "TuJian")
    self:RegisterGetGuideUi("ChallengeBtn", "TiaoZhan")
    self:RegisterGetGuideUi("FabaoBtn", "FaBao")
    self:RegisterGetGuideUi("BicycleMain", "LingQi")
    self:RegisterGetGuideUi("AthleticsBtn", "JingJi")
    self:RegisterGetGuideUi("RolePromoteBtn", "JinSheng")
    self:RegisterGetGuideUi("TreasureHuntViewBtn", "XunBao")
    self:RegisterGetGuideUi("WorkshopBtn", "GongFang")
    self:RegisterGetGuideUi("BagViewOpenBtn", "BeiBao")
    self:RegisterGetGuideUi("ExperienceViewBtn", "LiLian")
    self:RegisterGetGuideUi("FormationViewBtn", "BuZhen")
    self:RegisterGetGuideUi("LianYaoBtn", "LianYao")
    self:RegisterGetGuideUi("ZaoHuaBtn", "ZaoHuaZhuangBei")
    self:RegisterGetGuideUi("ShengQiMain", "ShengQiTuBiao")
    self:RegisterGetGuideUi("LiveMain", "ShengHuo")
    -- self:RegisterGetGuideUi("KnightMain", "XiaYi")
end
function MainFunctionPanel:ListCreateComplete(is_top)
    if is_top == true then
        self.is_complete_top = true
    else
        self.is_complete_bottom = true
    end
    if self.is_complete_top and self.is_complete_bottom then
        local a = self:ShowLineNum(self.ListTop.item_list)
        local b = self:ShowLineNum(self.ListBottom.item_list)
        self.Diwh:Set(521, 200)
        self.ListTopRect.sizeDelta = self.Diwh
        if a == 2 and b == 2 then
            self.Listxy:Set(0, 0)
            self.TopRect.anchoredPosition = self.Listxy
        elseif a == 1 and b == 2 then
            self.Listxy:Set(0, -86)
            self.TopRect.anchoredPosition = self.Listxy
        elseif a == 2 and b == 1 then
            self.Listxy:Set(0, 0)
            self.TopRect.anchoredPosition = self.Listxy
        elseif a == 1 and b == 1 then
            self.Listxy:Set(0, -184)
            self.TopRect.anchoredPosition = self.Listxy
        elseif a == 3 and b == 2 then
            self.Diwh:Set(521, 300)
            self.ListTopRect.sizeDelta = self.Diwh
            self.Listxy:Set(0, 100)
            self.TopRect.anchoredPosition = self.Listxy
        elseif a == 3 and b == 1 then
            self.Diwh:Set(521, 300)
            self.ListTopRect.sizeDelta = self.Diwh
            self.Listxy:Set(0, 0)
            self.TopRect.anchoredPosition = self.Listxy
        end
        self.Listxy:Set(0, b == 2 and 0 or -98)
        self.BottomRect.anchoredPosition = self.Listxy
        self:DiShow(a + b)
    end
end
function MainFunctionPanel:DiShow(line)
    if line == 2 then
        self.Diwh:Set(640, 276)--272)
    elseif line == 3 then
        self.Diwh:Set(640, 362)--356)
    elseif line == 4 then
        self.Diwh:Set(640, 460)
    elseif line == 5 then
        self.Diwh:Set(640, 544)
    end
    self.DiRect.sizeDelta = self.Diwh
    -- self.DiImage.enabled = line == 4
    -- self.Di2Image.enabled = line == 2
    -- self.Di3Image.enabled = line == 3
end
function MainFunctionPanel:ShowLineNum(list)
    local num = 0
    for i, v in ipairs(list) do
        if v:GetActiveSelf() then
            num = num + 1
        end
    end
    return math.ceil(num/6)
end
function MainFunctionPanel:RegisterGetGuideUi(key, name)
    GuideManager.Instance:RegisterGetGuideUi(key, BindTool.Bind(self.GuideCheckFunc, self, name))
end
function MainFunctionPanel:GuideCheckFunc(check_name)
    if ViewMgr:IsOpen(KeyUseView) then
        local item = KeyUseData.Instance:GetItem()
        if item ~= nil then
            MainOtherCtrl.Instance:SetWaitViewFunc(function()
                MainOtherCtrl.Instance:OpenKeyUseView(item)
            end)
        end
        ViewMgr:CloseView(KeyUseView)
    end
    MainViewData.Instance:ShowRbButton()
    for k, v in pairs(self.ListTop:GetData()) do
        if v.sprite == check_name then
            return self.ListTop.item_list[self.ListTop:GetSelectedIndex(v)],function ()
                ViewMgr:OpenView(GetView(v.view))
            end
        end
    end
    for k, v in pairs(self.ListBottom:GetData()) do
        if v.sprite == check_name then
            return self.ListBottom.item_list[self.ListBottom:GetSelectedIndex(v)],function ()
                ViewMgr:OpenView(GetView(v.view))
            end
        end
    end
end

MainViewModBtn = MainViewModBtn or DeclareMono("MainViewModBtn", UIWidgetBaseItem)
function MainViewModBtn:MainViewModBtn()
    self.zero = Vector3.zero
end
function MainViewModBtn:SetData(data)
    self:Init()
    UIWidgetBaseItem.SetData(self, data)
    self:Additional()
    self:RedPoint()
    self:FunctionOpen()
    self:FunctionGuide()
    self:FlushGray()
    if IS_AUDIT_VERSION then
        if not GLOBAL_CONFIG:PkgInfo().is_youyan then
            if AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] then
                UH.SetText(self.Label, "")
                UH.SpriteName(self.Icon)
                UH.SpriteName(self.AuditIcon, data.sprite.."_"..(GLOBAL_CONFIG:Urls().audit_seq))
            else
                UH.SetText(self.Label, data.text)
                UH.SpriteName(self.Icon, data.sprite)
                UH.SpriteName(self.AuditIcon)
            end
        else
            UH.SetText(self.Label, data.text)
            local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset(data.sprite)
            if streaming_has then
                ChannelAgentConsts.SetRawImg(self.AuditRawIcon, path)
                self.AuditRawIconObj:SetActive(true)
                UH.SpriteName(self.Icon)
            else
                UH.SpriteName(self.Icon, data.sprite)
                UH.SpriteName(self.AuditIcon)
                self.AuditRawIconObj:SetActive(false)
            end
        end
    else
        UH.SetText(self.Label, data.text)
        UH.SpriteName(self.Icon, data.sprite)
        UH.SpriteName(self.AuditIcon)
    end
end
--初始化
function MainViewModBtn:Init()
    self:OnRelease()
    self.ModRedPoint:SetNum(0, true)
    self.NewOpen:SetActive(false)
    self.XianShi:SetActive(false)
    self.EquipBetter:SetActive(false)
end
--附加内容
function MainViewModBtn:Additional()
    if self.data.view == "ExperienceView" then
        if self.CourageChallengeSub then
            self.CourageChallengeSub:SetParent(self.gameObject)
            UH.LocalPosG(self.CourageChallengeSub, self.zero)
            -- self.CourageChallengeSub:SetActive(true)
        end
    elseif self.data.view == "BicycleView" then
        if self.BicycleTipSub then
            self.BicycleTipSub:SetParent(self.gameObject)
            UH.LocalPosG(self.BicycleTipSub, self.zero)
            --  self.BicycleTipSub:SetActive(true)
        end
    elseif self.data.view == "PetRefining" then
        if self.LianYaoSub then
            self.LianYaoSub:SetParent(self.gameObject)
            UH.LocalPosG(self.LianYaoSub, self.zero)
            -- self.LianYaoSub:SetActive(true)
        end
    elseif self.data.sprite == "XunBao" then
        if self.XianShi then
            local _, show_xianshi = TreasureHuntData.Instance:GetOpenedTreasureID()
            self.XianShi:SetActive(show_xianshi)
        end
    elseif self.data.sprite == "ZhuangBei" and self.data.pos == 1 then
        if self.EquipTipSub then
            self.EquipTipSub:SetParent(self.gameObject)
            UH.LocalPosG(self.EquipTipSub, self.zero)
        end
    end
end
--红点显示
function MainViewModBtn:RedPoint()
    if self.data.view == "TempBag" then
        self.ModRedPoint:SetNum(1, true)
    end

    if self.data.mod then
        RemindCtrl.Instance:Remind():UnRegisterGroup(self.data.mod, self.handle_redpoint)
        self.handle_redpoint = nil
        self.handle_redpoint = RemindCtrl.Instance:Remind():RegisterGroup(self.data.mod,function()
            local num = RemindCtrl.Instance:Remind():GetGroupNum(self.data.mod)
            if self.data.pos and self.data.pos == 1 then
                MainViewData.Instance:FlushRbRedPoint(self.data.mod, num)
            end
            if self.ModRedPoint then self.ModRedPoint:SetNum(num, true) end 
        end,true)
    else
        if self.data then
            RemindCtrl.Instance:Remind():UnRegisterGroup(self.data.mod, self.handle_redpoint)
            self.handle_redpoint = nil
        end
        self.ModRedPoint:SetNum(0, true)
    end
end
--功能开启
function MainViewModBtn:FunctionOpen()
    if self.data.open then
        FunOpen.Instance:Register(self.data.open,function(opened)
            --当从false变到true的时候显示new图标
            local _opened = opened
            if self.data.lock_func then
                _opened = opened and self.data.lock_func()
            end
            self.gameObject:SetActive(_opened)
        end)
    else
        if self.data.lock_func then
            self.gameObject:SetActive(self.data.lock_func())
        else
            self.gameObject:SetActive(not self.data.close)
        end
    end
end
function MainViewModBtn:FunctionGuide()
    --工坊监听 到某等级后仍有低级装备 提示该换装备了
    if self.data.view == "Workshop" then
        self:InitRoleLevelGuide()
        -- self.handle_role_level = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.RoleLevelGuide, self), "level")
    end
    if self.data.sprite == "BeiBao" then
        self:InitEquipBetter()
        self.handle_equip_better = BagData.Instance.better_equip:Care(BindTool.Bind(self.EquipBetterShow, self), "is_better")
        self:EquipBetterShow()
    end
end
function MainViewModBtn:FlushGray()
    local is_gray = false
    local view_name = self.data.view
    if view_name == "ShengQiView" then
        if not ShengQiData.Instance:IsFly() then
            is_gray = true
        end
    end
    self.Interactor.Interactable = not is_gray
end
function MainViewModBtn:EquipBetterShow()
    self.EquipBetter:SetActive(BagData.Instance.better_equip.is_better)
end
function MainViewModBtn:RoleLevelGuide()
    if self.data == nil then return end
    local level = RoleData.Instance:GetRoleLevel()
    if self.data.view == "Workshop" and EquipGuideLevel[level] == true then
        local has_low = BagData.Instance:IsHasLowEquip()
        if has_low then
            MainViewData.Instance:ShowRbButton()
            local param_t = {
                desc = Language.Guide.EquipDesc,
                pos = Vector2.New(-59,97),
                show = 3,
            }
            self.guide_tip = UiGuideTip.New(self.gameObject,param_t)
        end
    end
end

function MainViewModBtn:OnClick(data)
    if self.data == nil then return end
    if GuildCtrl.IsHasGuild(self.data.view) then --家族的处理
        return
    end
    if self.data.view == "ShengQiView" then
        if not ShengQiData.Instance:IsFly() then
            local need_fly_level = ShengQiData.Instance:NeedFlyLevel() or 0
            if need_fly_level > 0 then
                --PublicPopupCtrl.Instance:Center(string.format(Language.ShengQi.Tip2,DataHelper.GetDaXie(need_fly_level)))
                PublicPopupCtrl.Instance:Center(Language.ShengQi.Tip3)
                return
            end
        end
    end
    if self.data.view == "ChallengPopupView" then
        ViewMgr:OpenView(ChallengeNaviView)
        return
    end
    if type(self.data.view) == "string" then
        ViewMgr:OpenView(GetView(self.data.view) or GetView(self.data.view_name))
    else
        ViewMgr:OpenView(self.data.view or GetView(self.data.view_name))
    end
end
function MainViewModBtn:OnPressDown()
    ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.RbButton)
end

--只有在List被销毁时才会OnRelease,这里Release了个寂寞
--修改在初始化时调用
function MainViewModBtn:OnRelease()
    if self.data == nil then return end
    FunOpen.Instance:UnRegister(self.data.open)
    self:InitRoleLevelGuide()
    self:InitEquipBetter()
    RemindCtrl.Instance:Remind():UnRegisterGroup(self.data.mod, self.handle_redpoint)
    UIWidgetBaseItem.OnRelease(self)--内部有self.data = nil的逻辑
end
function MainViewModBtn:InitRoleLevelGuide(delete)
    if self.handle_role_level then
        RoleData.Instance:GetBaseData():Uncare(self.handle_role_level)
        self.handle_role_level = nil
    end
    --[[ if self.guide_tip then        
        self.guide_tip:DeleteMe()
        self.guide_tip = nil
    end ]]
end
function MainViewModBtn:InitEquipBetter()
    if self.handle_equip_better then
        BagData.Instance.better_equip:Uncare(self.handle_equip_better)
        self.handle_equip_better = nil
    end
end
function MainViewModBtn:GetActiveSelf()
    return self.gameObject.activeSelf
end