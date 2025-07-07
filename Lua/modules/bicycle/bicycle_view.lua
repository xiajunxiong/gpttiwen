-- 灵骑界面
BicycleView = BicycleView or DeclareView("BicycleView", "bicycle/bicycle_view", Mod.Bicycle.Main)
VIEW_DECLARE_LEVEL(BicycleView,ViewLevel.L0)
function BicycleView:BicycleView()
    self.data = BicycleData.Instance
    self.TabbarList:SetData(
        {
            {name = Language.Bicycle.Tab1, tab_objs = {self.BasicsPanel,self.ScoreInfoObj,self.SelectToggleObj}, open = Mod.Bicycle.Basice, mod = TabMod(Mod.Bicycle.Basice)},
            {name = Language.Bicycle.Tab2, tab_objs = {self.FosterPanel,self.ScoreInfoObj,self.SelectToggleObj}, open = Mod.Bicycle.Foster, mod = TabMod(Mod.Bicycle.Foster)},
            {name = Language.Bicycle.Tab3, tab_objs = {self.SkillPanel,self.SkillInfoObj}, open = Mod.Bicycle.Skill, mod = TabMod(Mod.Bicycle.Skill)},
            {name = Language.Bicycle.Tab5, tab_objs = {self.DianHuaPanel,self.ScoreInfoObj,self.SelectToggleObj}, open = Mod.Bicycle.DianHua, mod = TabMod(Mod.Bicycle.DianHua)},
            {name = Language.Bicycle.Tab4, tab_objs = {self.PetPanel,self.SkillInfoObj}, open = Mod.Bicycle.PetPanel},
        }
    )
    self.Board = {Tabbar = self.TabbarList}
    GuideManager.Instance:RegisterGetGuideUi("BicycleTab1",function ()
        return self.TabbarList.item_list[1],function ()
            self.TabbarList.item_list[1]:Click()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BicycleTab2",function ()
        return self.TabbarList.item_list[2],function ()
            self.TabbarList.item_list[2]:Click()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BicycleTab3",function ()
        return self.TabbarList.item_list[3],function ()
            self.TabbarList.item_list[3]:Click()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BicycleTab4",function ()
        return self.TabbarList.item_list[5],function ()
            self.TabbarList.item_list[5]:Click()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BicycleCloseBtn",function ()
        return self.CloseBtn,function ()
            ViewMgr:CloseView(BicycleView)
        end
    end)
end

function BicycleView:LoadCallback(param_t)
    param_t = param_t or {}
    local index = self:GetTabIndex(param_t.mod_key or 0)
    self.TabbarList:ClickItem(index)
    self:FlushTabbar()
    if self.data:IsOpenGuide() then
        LogDG("开始了灵骑139指引")
        GuideManager.Instance:Start(139)
        self.data:OpenFlag(1)
    end
end

function BicycleView:GetTabIndex(mod_key)
    for i,v in pairs(self.TabbarList:GetData()) do
        if v.open == mod_key then
            return i
        end
    end
    return 1
end

function BicycleView:FlushTabbar()
    --检查并锁住
    local tabbar = self.TabbarList
    tabbar.item_list[4]:SetObjActive(self.data:IsOpenDianHua())
end

function BicycleView:CloseCallback()
    if ViewMgr:IsOpen(BicycleEquipView) then
        ViewMgr:CloseView(BicycleEquipView)
    end
    if ViewMgr:IsOpen(BicycleRefineSkillView) then
        ViewMgr:CloseView(BicycleRefineSkillView)
    end
    if ViewMgr:IsOpen(BicycleHalidomView) then
        ViewMgr:CloseView(BicycleHalidomView)
    end
    self:HandRemind()
end

function BicycleView:OnCloseClick()
    ViewMgr:CloseView(BicycleView)
end

function BicycleView:OnTabClick(data)
    if self.first_flag == nil then
        self.data:CurTabMod(data.open)
        self.first_flag = 1
        return
    end
    self:HandRemind()
    self.data:CurTabMod(data.open)
end

function BicycleView:HandRemind()
    local last_mod = self.data:CurTabMod()
    if last_mod == Mod.Bicycle.Foster then
        local cur_bicycle_id = self.data:GetSelectedData().bicycle_id
        local remind_flag = self.data:BicycleOneUpLevelRemindNum(nil,cur_bicycle_id)
        if remind_flag > 0 then
            self.data.bicycle_up_level_remind_flag[cur_bicycle_id] = 1
            self.data:CheckRemind()
        end
    end
    if last_mod == Mod.Bicycle.Skill then
        local cur_bicycle_id = self.data:GetSelectedData().bicycle_id
        local remind_flag = self.data:BicycleOneUpSkillRemindNum(nil,cur_bicycle_id)
        if remind_flag > 0 then
            self.data.bicycle_up_skill_remind_flag[cur_bicycle_id] = 1
            self.data:CheckRemind()
        end
    end
end

-- ================ BicycleCommonPanel ================

BicycleCommonPanel = BicycleCommonPanel or DeclareMono("BicycleCommonPanel", UIWFlushPanel)
function BicycleCommonPanel:BicycleCommonPanel()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.selected_bicycle_data, func = self.FlushPanel, init_call = false},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false, keys = {"bicycle"}},
        {data = self.data.protocol_info_2104, func = self.FlushList, init_call = false},
        {data = self.data.protocol_info_2108, func = self.FlushPanel, init_call = false},
        {data = self.data.cur_tab, func = self.FlushList, init_call = false},
        {data = self.data.ride_info, func = self.FlushModel, init_call = false},
        {data = self.data.ride_event, func = self.OnRideEvent, init_call = false},
    }
    self.selected_index = 1
    GuideManager.Instance:RegisterGetGuideUi("BicycleFirst",function ()
        return self.BicycleList.item_list[1]
    end)
end

function BicycleCommonPanel:Awake()
    UIWFlushPanel.Awake(self)
    --self:FlushList()
    self:FlushPanel()
end

function BicycleCommonPanel:FlushPanel()
    self.selected_data = self.data:GetSelectedData()
    self:FlushList()
    if self.selected_data == nil or next(self.selected_data) == nil then
        return nil
    end
    self:FlushModel()
end

function BicycleCommonPanel:FlushList()
    local pet_tab_pos = self.data:IsOpenDianHua() and Vector3.New(36.5,-457,0) or Vector3.New(36.5,-357,0)
    self.PetTabRemind:SetLocalPosition(pet_tab_pos)
    self.PetTabRemind:SetActive(self.data:BicyclePetRemindNum() > 0)
    local list_data = self.data:GetBicycleListData()
    self.BicycleList:SetData(list_data)
    local selected_data = self.data:GetSelectedData()
    if self.data.JumpBicycleIndex then
        for i,v in pairs(list_data) do
            if v.seq == self.data.JumpBicycleIndex then
                self.selected_index = v.cell_index
                break
            end
        end
        self.data.JumpBicycleIndex = nil
    elseif selected_data and selected_data.bicycle_id then
        for i,v in pairs(list_data) do
            if v.bicycle_id == selected_data.bicycle_id then
                self.selected_index = v.cell_index
                break
            end
        end
    else
        self.selected_index = 1
    end
    self.BicycleList:SetectItem(self.selected_index or 1, true)
    UH.SetText(self.CountText,string.format(Language.Bicycle.BicycleUnlockCount,self.data:GetBicycleCount(),#list_data))
end

-- model_type 0:普通形象，1:灵饰进阶形象
function BicycleCommonPanel:FlushModel(old_data,is_ride,model_type)
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
				view = self
			}
		)
	end
    if self.ui_obj_role == nil then
		self.ui_obj_role = UIChDrawer.New()
        self.ModelRoleShow:SetShowData(
			{
				ui_obj = self.ui_obj_role,
				view = self
			}
		)
	end
    -- 形象切换 形象预览 默认显示
    local bicycle = self.data:GetSelectedBicycle()
    if is_ride ~= nil then
        self.is_role_ride = is_ride
    else
        self.is_role_ride = self.data:IsRide(bicycle:Index())
    end
    self.cur_model_type = model_type or bicycle:CurModelType()
    self.data.cur_selected_model_type.model_type = self.cur_model_type
    self.SelectToggle.isOn = self.cur_model_type == 1
    local model_id = self.cur_model_type == 0 and bicycle:Model1() or bicycle:Model2()
    
    -- 判断下刷新的模型状态是不是没有变化，没变化不刷新
    if self.cur_flush_model_info then
        self.last_flush_model_info = self.cur_flush_model_info
    end
    self.cur_flush_model_info = {model_id = model_id , is_ride = self.is_role_ride}
    if (self.last_flush_model_info and 
        self.cur_flush_model_info.model_id == self.last_flush_model_info.model_id and
        self.cur_flush_model_info.is_ride == self.last_flush_model_info.is_ride) and
        (self.data:BicycleShowType() ~= BicycleCfg.SHOW_TYPE.BREAK_SUCC or self.is_role_ride == true) then  -- 但是突破成功情况下，在未乘寄灵骑的情况下又嘚刷新下模型动画
        return
    end
    
    if self.is_role_ride then
        self.ModelRoleObj:SetActive(true)
        self.ModelObj:SetActive(false)
        local appearance = TableCopy(RoleData.Instance:GetApearance())
        appearance.ride_id = model_id
        self.ui_obj_role.is_ride = true
        self.ui_obj_role:SetData(appearance, {avatar_type=appearance.avatar_type})
    else
        self.ModelRoleObj:SetActive(false)
        self.ModelObj:SetActive(true)
        self.ui_obj:SetData(DrawerHelper.GetRidePath(model_id))
        self.ui_obj:PlayAnim(ChAnimType.Show)
    end
    local bicycle = self.data:GetSelectedBicycle()
    UH.SetText(self.ModelName,bicycle:Name(self.cur_model_type))
end

function BicycleCommonPanel:GuideActiveClick()
    local first_data = self.data:GetBicycleListData()[1]
    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.ACTIVE,first_data.seq)
end

function BicycleCommonPanel:OnSelectClick()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    local bicycle = self.data:GetSelectedBicycle()
    self.cur_model_type = self.cur_model_type == 0 and 1 or 0
    if not is_unlock or not bicycle:Ornament():IsShow() then
        self:FlushModel(nil,self.is_role_ride,self.cur_model_type)
    else
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.SET_MODEL,bicycle:Index(),self.cur_model_type)
    end
end

function BicycleCommonPanel:OnRideEvent()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    if not is_unlock then
        if self.is_role_ride == true then
            self.is_role_ride = false
        else
            self.is_role_ride = true
        end
        self:FlushModel(nil,self.is_role_ride,self.cur_model_type)
        --PublicPopupCtrl.Instance:Error(Language.Bicycle.ActiveTip)
        return
    end
    local is_ride = self.data:IsRide(self.selected_data.seq)
    if is_ride then
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.CANCEL_RIDE,self.selected_data.seq)
    else
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.RIDE,self.selected_data.seq)
    end
end

function BicycleCommonPanel:OnDestroy()
   -- BicycleData.Instance:SetSelectedData({})
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
    if self.ui_obj_role ~= nil then
		self.ui_obj_role:DeleteMe()
	end
	self.ui_obj_role = nil
    UIWFlushPanel.OnDestroy(self)
end

-- ================ BicycleIconItem ================

BicycleIconItem = BicycleIconItem or DeclareMono("BicycleIconItem", UIWidgetBaseItem)
function BicycleIconItem:SetData(data)
    UIWidgetBaseItem.SetData(self , data)
    self.gameObject.name = string.format("Item%s",data.cell_index)
    --self:HandleColor(data.cell_index)
    local cur_tab_mod = BicycleData.Instance:CurTabMod()
    local red_num = 0
    local bicycle = BicycleData.Instance:GetActiveBicycle(data.bicycle_id) -- 已激活的灵骑
    if cur_tab_mod == Mod.Bicycle.Foster then
        red_num = BicycleData.Instance:BicycleOneUpLevelRemindNum(data)
    elseif cur_tab_mod == Mod.Bicycle.Skill then
        red_num = BicycleData.Instance:BicycleOneUpSkillRemindNum(data)
    elseif cur_tab_mod == Mod.Bicycle.Basice then
        red_num = BicycleData.Instance:BicycleOneUnlockRemindNum(data)
        if red_num < 1 and bicycle then
            local flag = BicycleData.Instance:BicycleEnergyOneRemindNum(bicycle)
            red_num = red_num + (flag > 0 and 1 or 0) -- 精力值红点检查
        end
        if red_num < 1 and bicycle then
            local flag_1 = BicycleData.Instance:BicycleOneEquipRemindNum(bicycle:Equip()[0])
            local flag_2 = BicycleData.Instance:BicycleOneEquipRemindNum(bicycle:Equip()[1])
            local flag = (flag_1 + flag_2) > 0 and 1 or 0
            red_num = red_num + flag -- 装备红点检查
        end
    elseif cur_tab_mod == Mod.Bicycle.DianHua and bicycle then
        red_num = BicycleData.Instance:BicycleDianHuaOneRemindNum(bicycle)
    end
    self.RedPoint:SetNum(red_num)
    self.RemindImg:SetActive(BicycleData.Instance:IsCanPet(data.bicycle_id) == true and red_num == 0)
    UH.SpriteName(self.Icon,data.bicycle_head)
    UH.SpriteName(self.Qua,BicycleCfg.QUA[data.bicycle_color])
end

function BicycleIconItem:OnToggleEvent()
    BicycleData.Instance:SetSelectedData(self.data)
    BicycleData.Instance:SetSelectedSkill({})
end

function BicycleIconItem:HandleColor(index)
    if index == 1 or index == 2 then
        self.Gray.enabled = false
        UH.Color(self.Img, COLORS.White)
    elseif index == 3 then
        self.Gray.enabled = false
        UH.Color(self.Img, COLORS.Purple4)
    else
        self.Gray.enabled = true
        UH.Color(self.Img, COLORS.White)
    end
end

-- ================ BicycleSkillIconItem ============
BicycleSkillIconItem = BicycleSkillIconItem or DeclareMono("BicycleSkillIconItem",UIWidgetBaseItem)
-- data : BicycleSkill
function BicycleSkillIconItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local is_skill = data.skill_id ~= nil and data.skill_id > 0
    local unlock_level
    if not is_unlock then
        unlock_level = self.data:UnlockLevel()
    end
    --local is_empty = unlock_level == nil or unlock_level == 0 --(data.skill_id == nil or data.skill_id <= 0)
    local is_unlock = data:IsUnlock()
    self:ShowEmpty(is_skill,is_unlock, unlock_level)

    if self.AddEffect then
        self.AddEffect:SetActive(not is_skill and is_unlock)
    end

    if self.Remind then
        self.Remind:SetNum(data:Remind())
    end

    if not is_skill then
        if unlock_level then
            UH.SetText(self.LockLevel,string.format(Language.Bicycle.BicycleUnlockTip, unlock_level))
        end
        if self.IsSpecial then
            UH.SetText(self.Name,Language.ItemInfo.Lock)
            self.Lock2Obj:SetActive(false)
        end
        return
    end
    local skill = data
    local skill_cfg = skill:EffectCfg()
    UH.SetIcon(self.Icon,skill:Icon())
    UH.SetText(self.Name,skill:Name())
    UH.SetText(self.Level,string.format("Lv.%s",skill:Level()))
    UH.SpriteName(self.Qua,skill:ColorQua())
    local is_unlock = skill:IsUnlock()
    self.Lock2Obj:SetActive(not is_unlock)
    self.LevelObj:SetActive(is_unlock)
    --self.Interactor.Interactable = is_unlock
    if not is_unlock then
        UH.SetText(self.LockLevel,string.format(Language.Bicycle.BicycleUnlockTip,skill:UnlockLevel()))
    end
end

function BicycleSkillIconItem:ShowEmpty(is_skill, is_unlock, unlock_level)
    unlock_level = unlock_level or 0
    self.Icon:SetObjActive(is_skill)
    self.LevelObj:SetActive(is_skill)
    self.Lock2Obj:SetActive(unlock_level > 0 and not is_unlock)
    self.Lock1Obj:SetActive(unlock_level == 0 and not is_unlock)
end

function BicycleSkillIconItem:OnShowInfoClick()
    if self.IsShowInfo == true then
        if self.data.skill_id > 0 then
            local pos = UH.GetRootPosition(self.gameObject)
            pos.y = pos.y + 320
            if self.data:SkillType() == BicycleCfg.SKILL_TYPE.BASE and self.data.index == 0 then
                pos.x = pos.x
            end
            ViewMgr:OpenView(BicycleSkillInfoView,{skill = self.data, pos = pos,is_special = self.IsSpecial})
        else
            if self.IsSpecial then
                return
            end
            local unlock_level = self.data:UnlockLevel()
            if unlock_level and unlock_level > 0 then
                PublicPopupCtrl.Instance:Error(string.format(Language.Bicycle.SkillUnlockTip,unlock_level))
            end
        end
    end
end

-- 1 点击显示信息，不显示选中框，2 -- 显示选中，不显示信息
function BicycleSkillIconItem:ShowType(show_type)
    show_type = show_type or 2
    if show_type == 1 then
        self.IsShowInfo = true
        self.Selector.interactable = false
    else
        self.IsShowInfo = false
        self.Selector.interactable = true
    end
end

-- ================ BicycleEquipItem ================
BicycleEquipItem = BicycleEquipItem or DeclareMono("BicycleEquipItem",UIWidgetBaseItem)
-- @ data:{BicycleEquip}
function BicycleEquipItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local equip = data.equip
    local is_show = equip:IsShow()
    self.EquipCell:SetObjActive(is_show)
    self.LevelObj:SetActive(is_show)
    if is_show == true then
        self.EquipCell:SetData(Item.Create({item_id = equip:IconId()}))
        UH.SetText(self.LevelText,string.format(Language.Bicycle.Level,equip:Level()))
    end
    UH.SpriteName(self.ShadowImg,equip:Cfg().outline_icon)
    if self.RedPoint ~= nil then
        self.RedPoint:SetNum(BicycleData.Instance:BicycleOneEquipRemindNum(equip))
    end
end

-- ================ BicycleBasePart ================ 

BicycleBasePart = BicycleBasePart or DeclareMono("BicycleBasePart", UIWFlushPanel)
function BicycleBasePart:BicycleBasePart()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.selected_bicycle_data, func = self.FlushPanel, init_call = false},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false},
        {data = self.data.ride_info, func = self.FlushPanel, init_call = false},
        {data = self.data.cur_selected_model_type, func = self.FlushModelTip, init_call = false},
        {data = RoleData.Instance.base_data, func = self.FlushHalidom, keys = {"level"}, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("BicycleRideOKBtn",function ()
        return self.RideBtnImg,function ()
            self:OnRideClick()
        end
    end)
end

function BicycleBasePart:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function BicycleBasePart:FlushPanel(old_data,is_ride)
    self.selected_data = self.data:GetSelectedData()
    if self.selected_data == nil or next(self.selected_data) == nil then
        return nil
    end
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    self.LockStateObj:SetActive(not is_unlock)
    self.CapabilityText:SetObjActive(is_unlock)
    if is_unlock then
        local bicycle = self.data:GetSelectedBicycle()
        UH.SetText(self.CapabilityText,bicycle:Capability())
    end
    if is_ride ~= nil then
        self.is_ride = is_ride
    else
        self.is_ride = self.data:IsRide(self.selected_data.seq)
    end
    local ride_btn_img_name = self.is_ride and "QiCheng2" or "QiCheng1"
    UH.SpriteName(self.RideBtnImg,ride_btn_img_name)

    local bicycle = self.data:GetSelectedBicycle()

    self:FlushModelTip()

    local equip_list = bicycle:Equip()
    self.Equip0:SetData({equip = equip_list[0]})
    self.Equip1:SetData({equip = equip_list[1]})

    self:FlushHalidom()
end

function BicycleBasePart:FlushModelTip()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    local bicycle = self.data:GetSelectedBicycle()
    self.ModelTipObj:SetActive(not bicycle:Ornament():IsShow() and is_unlock and self.data.cur_selected_model_type.model_type == 1)
end

function BicycleBasePart:FlushHalidom()
    local is_open = self.data:IsOpenHalidom()
    self.HalidomObj:SetActive(is_open)
    if not is_open then
        return
    end
    local bicycle = self.data:GetSelectedBicycle()
    if not bicycle then
        return
    end
    local halidom = bicycle:Halidom()
    local level_str = halidom:IsShow() and string.format(Language.Bicycle.Level2, halidom:Level()) or Language.Bicycle.Halidom.Tip1
    UH.SetText(self.HalidomLevel, level_str)
    
    local model_id = halidom:ModelId()
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
			}
		)
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(model_id))
    -- local color = halidom:ModelColor()
    -- self.ModelImg.color = color
    local flag = self.data:BicycleHalidomOneRemindNum(bicycle)
    self.HalidomRedPoint:SetNum(flag > 0 and 1 or 0)
end

function BicycleBasePart:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end

function BicycleBasePart:OnHalidomClick()
    ViewMgr:OpenView(BicycleHalidomView)
end

function BicycleBasePart:OnFashionClick()
    ViewMgr:OpenView(IllusionView)
end

function BicycleBasePart:OnRideClick()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    if not is_unlock then
        if self.is_ride == true then
            self.is_ride = false
        else
            self.is_ride = true
        end
        self:FlushPanel(nil,self.is_ride)
    end
    self.data.ride_event.value = not self.data.ride_event.value
end

function BicycleBasePart:OnOpenEquipClick(index)
    if not index then
        return
    end
    ViewMgr:OpenView(BicycleEquipView,{index = index})
end

-- ================ BicycleSkillPart ================ 

BicycleSkillPart = BicycleSkillPart or DeclareMono("BicycleSkillPart", UIWFlushPanel)
function BicycleSkillPart:BicycleSkillPart()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.selected_bicycle_data, func = self.FlushPanel, init_call = false},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false},
        {data = self.data.protocol_info_2106, func = self.FlushPanel, init_call = false},
        {data = self.data.cur_tab, func = self.FlushPanel, init_call = false},
    }
    self.selected_index = 1
end

function BicycleSkillPart:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function BicycleSkillPart:FlushPanel()
    self.selected_data = self.data:GetSelectedData()
    if self.selected_data == nil or next(self.selected_data) == nil then
        return nil
    end
    self.bicycle = self.data:GetBicycle(self.selected_data.bicycle_id)
    self:FlushSkill()
end

function BicycleSkillPart:FlushSkill()
    local mod_key = self.data.cur_tab.mod
    local base_skill_list = self.bicycle:BaseSkillList()
    local selected_skill = self.data:GetSelectedSkill()
    for i = 1,self.BaseSkillList:Length() do
        self.BaseSkillList[i]:SetData(base_skill_list[i - 1])
        if mod_key == Mod.Bicycle.Skill then
            self.BaseSkillList[i]:ShowType(2)
        else
            self.BaseSkillList[i]:ShowType(1)
        end
    end
    if mod_key == Mod.Bicycle.Skill then
        if TableIsEmpty(selected_skill) and base_skill_list[0]:IsUnlock() then
            --self.BaseSkillList[self.selected_index]:SetSelected(false)
            self.selected_index = 1
            self.BaseSkillList[self.selected_index]:SetSelected(true)
            self.data:SetSelectedSkill(self.BaseSkillList[self.selected_index]:GetData())
        elseif not base_skill_list[0]:IsUnlock() then
            self.NullToggle.isOn = true
        else
            self.BaseSkillList[self.selected_index]:SetSelected(true)
        end
    else
        self.NullToggle.isOn = true
    end
end

function BicycleSkillPart:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function BicycleSkillPart:OnToggleClick(data)
    if data:IsUnlock() or data.skill_id > 0 then
        self.data:SetSelectedSkill(data)
        self.selected_index = data:Index() + 1
    else
        -- 这个是点中未解锁技能时，如果之前选中的技能是已解锁，则继续选中之前那个技能
        local selected_skill = self.data:GetSelectedSkill()
        if selected_skill then
            local index = selected_skill:Index()
            self.BaseSkillList[index + 1]:SetSelected(true)
        else
            self.NullToggle.isOn = true
        end
        local unlock_level = data:UnlockLevel()
        if unlock_level and unlock_level > 0 then
            PublicPopupCtrl.Instance:Error(string.format(Language.Bicycle.SkillUnlockTip,unlock_level))
        end
    end
end

--===============================BicycleTipSub===============================
BicycleTipSub = BicycleTipSub or DeclareMono("BicycleTipSub", UIWFlushPanel)

function BicycleTipSub:BicycleTipSub()
	self.data = BicycleData.Instance
    self.data_cares = {
		{data = self.data.bicycle_energy_tip, func = self.FlushShowTip, init_call = true},
    }
end
function BicycleTipSub:OnPressDown()
    ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.RbButton)
end
function BicycleTipSub:FlushShowTip()
	self.Sub:SetActive(self.data.bicycle_energy_tip.info.bicycle_id ~= nil)
    local bicycle_id = self.data.bicycle_energy_tip.info.bicycle_id
    if not bicycle_id then
        return 
    end
    local cfg = self.data:GetBicycleCfg(bicycle_id)
    UH.SetText(self.TipText,string.format(Language.Bicycle.EnergyTip,cfg.energy_use))
end

function BicycleTipSub:Awake()
	UIWFlushPanel.Awake(self)
    UH.SetText(self.CickText,RichTextHelper.LinkStr(RichTextHelper.ColorStr(Language.CourageChallenge.GoToTip,COLORSTR.Green3),"_ul",COLORSTR.Green3))
end

function BicycleTipSub:OnClickEnter()
	ViewMgr:OpenView(BicycleView)
end

function BicycleTipSub:OnClickClose()
	self.Sub:SetActive(false)
end


BicycleAttriItem = BicycleAttriItem or DeclareMono("BicycleAttriItem",UIWidgetBaseItem)

function BicycleAttriItem:SetData(data)
    UIWAttrItem.SetData(self, data)
    if data.is_add then
        UH.SetText(self.AddValue,string.format("+%s",data.add_value or 0))
    else
        UH.SetText(self.AddValue,data.add_value or 0)
    end
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Value,data.value or 0)
end

BicycleEffect = BicycleEffect or DeclareMono("BicycleEffect",UIWFlushPanel)
function BicycleEffect:BicycleEffect()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.cur_effect,func = self.HandleEffect, init_call = false}
    }
    self.default_step = 20
end

function BicycleEffect:HandleEffect()
    local effect_info = self.data:CurEffect()
    if TableIsEmpty(effect_info) then
        self.TouchMaskObj:SetActive(false)
        return
    end
    self.TouchMaskObj:SetActive(true)
    if effect_info.effect == BicycleCfg.EFFECT.UP_SKILL_START then
        TimeHelper:CancelTimer(self.time_hd)
        self.time_hd = Invoke(function ()
            local skill_index = effect_info.skill_index
            self:MoveEffect(self.SkillEffectPos,self.SkillPosList[skill_index],function()
                BicycleData.Instance:CurEffect({effect = BicycleCfg.EFFECT.UP_SKILL_END,skill_index = effect_info.skill_index})
            end)
        end,0.5)
    elseif effect_info.effect == BicycleCfg.EFFECT.UP_SKILL_END then
        self.EffectTool:Play(effect_info.effect,self.SkillPosList[effect_info.skill_index])
        return
    -- elseif effect_info.effect == BicycleCfg.EFFECT.UP_DIAN_HUA then
    --     self.EffectTool:Play(effect_info.effect)
    end
    self.EffectTool:Play(effect_info.effect)
end

function BicycleEffect:EffectEndEvent()
    self.data:CurEffect({})
    self.TouchMaskObj:SetActive(false)
    BicycleCtrl.Instance:HandleShowType()
end

function BicycleEffect:PlayMoveEffect(effect_obj,start_obj,end_obj,end_func)

end

function BicycleEffect:MoveEffect(start_obj,end_obj,end_func)
    self.cur_step = 1
    TimeHelper:CancelTimer(self.time_handle)
    self.SkillMoveEffectObj:SetActive(true)
    local skill_start_pos = UH.GetRootPosition(start_obj)
    local skill_end_pos = UH.GetRootPosition(end_obj)
    UH.LocalPosG(self.SkillMoveEffectObj,skill_start_pos)
    self.skill_curve = Game.CurveParabola(skill_start_pos, Vector3.New(-2, 1, 0), skill_end_pos, Vector3.New(-1, -2, 0))
    self.time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.moveEffect,self,end_func),1,self.default_step)
end

function BicycleEffect:moveEffect(end_func)
    local pos = self.skill_curve:GetPos(self.cur_step / self.default_step)
    UH.LocalPosG(self.SkillMoveEffectObj,pos)
    self.cur_step = self.cur_step + 1
    if self.cur_step > self.default_step then
        self.SkillMoveEffectObj:SetActive(false)
        end_func()
	end
end

function BicycleEffect:OnDestroy()
    self.skill_curve = nil
    TimeHelper:CancelTimer(self.time_handle)
    TimeHelper:CancelTimer(self.time_hd)
    UIWFlushPanel.OnDestroy(self)
end