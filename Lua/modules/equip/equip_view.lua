EquipView = EquipView or DeclareView("EquipView", "equip_view/equip_view", Mod.Equip)
VIEW_DECLARE_LEVEL(EquipView, ViewLevel.L1)
function EquipView:EquipView()
    self.language = Language.Equip
    GuideManager.Instance:RegisterGetGuideUi("WorkshopTabInset", function ()
		return self.Board.Tabbar.item_list[2]
    end)
    GuideManager.Instance:RegisterGetGuideUi("WorkshopTabUpgrade", function ()
        return self.Board.Tabbar.item_list[1]
    end)

    GuideManager.Instance:RegisterGetGuideUi("WorkshopTabNeveal", function ()
        self.Neveal_pos = 5
        if not FunOpen.Instance:GetFunIsOpened(Mod.Equip.Neveal) or self:NevealOpenFunc() ~= "" then 
            GuideManager.Instance:ForceStop()
            return 
        end 
        return self.Board.Tabbar.item_list[self.Neveal_pos]
    end)
end

function EquipView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), self.language.Title)
    self.Board:SetTabbarVisible(true)
    self.Board:SetCompetitionRank(ServeractivityConfig.CompetitionRank.Type.Equip)
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		self.Board:SetAuditBgShow()
	end
    local tab_param =  {
        {name = self.language.TabTitle[1], tab_objs = {self.TabList[1]}, open = Mod.Equip.Upgrade, mod = TabMod(Mod.Equip.Upgrade)},
        -- {name = self.language.TabTitle[2], tab_objs = {self.TabList[2]}, open = Mod.Equip.Ulti, mod = TabMod(Mod.Equip.Ulti)}, -- 已屏蔽 20211202c
        {name = self.language.TabTitle[3], tab_objs = {self.TabList[3]}, open = Mod.Equip.Inser, mod = TabMod(Mod.Equip.Inser)},
        {name = self.language.TabTitle[4], tab_objs = {self.TabList[4]}, open = Mod.Equip.Smelt, mod = TabMod(Mod.Equip.Smelt)},
        {name = self.language.TabTitle[5], tab_objs = {self.TabList[5]}, open = Mod.Equip.Spirit, mod = TabMod(Mod.Equip.Spirit)},
    }

    if FunOpen.Instance:GetFunIsOpened(Mod.Equip.Neveal) then 
        table.insert(tab_param,{name = Language.Workshop.Tab7, tab_objs = {self.TabList[6]}, 
           open = Mod.Equip.Neveal,open_tip = BindTool.Bind(self.NevealOpenFunc, self)})      
        self.Neveal_pos = #tab_param
    end 
    
    self.Board.Tabbar:SetData(tab_param)
    local param = param_t.mod_key ~= nil and param_t or nil
    self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Equip, self.Board.Tabbar, param))
    self.Currency:DefaultSet()
end

function EquipView:CloseCallback()
    WorkshopData.Instance.complete_item.item = nil
    TaskData.Instance:TaskChainSubContinue()
end

function EquipView:NevealOpenFunc()
    local flag = FlyUpData.Instance:GetStage() >= FlyUpStageType.Low
    return "" -- flag and "" or Language.Workshop.Neveal.OpenTips
end
  
function EquipView:OnFlush()
    if self.Neveal_pos ~= nil and self.Board.Tabbar.item_list[self.Neveal_pos] ~=nil then 
        self.Board.Tabbar.item_list[self.Neveal_pos]:TabbarOpenEvent(
          self:NevealOpenFunc() == "" or Config.equipment_dianhua_auto.other[1].open_condition == 0)
    end 
end 

--===============================LianYaoSub===============================
EuipTipSub = EuipTipSub or DeclareMono("EuipTipSub", UIWFlushPanel)

function EuipTipSub:EuipTipSub()
	-- self.data = PetRefiningData.Instance
    self.data_cares = {
        {data = WorkshopData.Instance.upgrade_remind_sort, func = self.FlushShowTip},-- init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushShowTip, init_call = false},
        {data = BagData.Instance.equip_sort_upgrade, func = self.FlushShowTip, init_call = false},
        -- {data = WorkshopData.Instance.upgrade_remind_sort, func = self.FlushShowTip, init_call = false},
        {data = MainViewData.Instance.rb_toggle, func = self.SetShow, keys = {"isOn"}},
	    -- {data = RoleData.Instance:GetBaseData(), func = self.FlushShowTip, init_call = false, keys = {"level"}},
	    -- {data = RoleData.Instance:GetBaseData(), func = self.FlushShowTip, init_call = false, keys = {"avatar_quality"}},
	    -- {data = self.data.pet_unlock_info, func = self.FlushShowTip, init_call = false},
        {data = LoginData.Instance.base_data, func = self.FlushShow, keys = {"login_state"}}
    }
    self.stove_id = 1
    self.is_show = false
end

function EuipTipSub:SetShow()
    self.Sub:SetActive(not MainViewData.Instance.rb_toggle.isOn and self.is_show)
end

function EuipTipSub:FlushShow()
    self.Sub:SetActive(LoginData.Instance.base_data.login_state == LoginStateType.Logined and not MainViewData.Instance.rb_toggle.isOn and self.is_show)
end
function EuipTipSub:OnPressDown()
    ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.RbButton)
end
function EuipTipSub:FlushShowTip()
    local role_level = RoleLevel()
    if role_level > 36 then
        self:uncareAllData()
        self.Sub:SetActive(false)
        self.is_show = false
        return
    end
    self.is_show = false
    local num = WorkshopData.Instance:CheckUpgradeRemind()
    self.is_show = num > 0
    self.Sub:SetActive(self.is_show and not MainViewData.Instance.rb_toggle.isOn)
    UH.SetText(self.TipText, Language.Equip.EquipUpTip)
end

function EuipTipSub:Awake()
	UIWFlushPanel.Awake(self)
    UH.SetText(self.CickText,RichTextHelper.LinkStr(RichTextHelper.ColorStr(Language.CourageChallenge.GoToTip,COLORSTR.Green3),"_ul",COLORSTR.Green3))
end

function EuipTipSub:OnClickEnter()
	ViewMgr:OpenView(EquipView)
    self.Sub:SetActive(false)
end

function EuipTipSub:OnClickClose()
	self.Sub:SetActive(false)
end