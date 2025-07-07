ShengQiView = ShengQiView or DeclareView("ShengQiView", "sheng_qi/sheng_qi_view", Mod.ShengQi.Main)
VIEW_DECLARE_LEVEL(ShengQiView, ViewLevel.L0)
function ShengQiView:ShengQiView()
    self.data = ShengQiData.Instance
    self.tab_list_data = {
        {name = Language.ShengQi.Tab1, tab_objs = {self.PublicPanelObj,self.AttriPanelObj}, open = Mod.ShengQi.Attri, mod = TabMod(Mod.ShengQi.Attri)},
        {name = Language.ShengQi.Tab2, tab_objs = {self.PublicPanelObj,self.UpPanelObj}, open = Mod.ShengQi.Up, mod = TabMod(Mod.ShengQi.Up), open_tip = Language.ShengQi.Tip1},
        --{name = Language.ShengQi.Tab3, tab_objs = {self.PublicPanelObj,self.SkillPanelObj}, open = Mod.ShengQi.Skill, mod = TabMod(Mod.ShengQi.Skill), open_tip = Language.ShengQi.Tip1},
        {name = Language.ShengQi.Tab4, tab_objs = {self.PublicPanelObj,self.FuShiPanelObj}, open = Mod.ShengQi.FuShi, mod = TabMod(Mod.ShengQi.FuShi), open_tip = Language.ShengQi.Tip1},
        {name = Language.ShengQi.Tab5, tab_objs = {self.BookPanelObj}, open = Mod.ShengQi.Book},
    }
    self.TabbarList:SetData(self.tab_list_data)
    self.Board = {Tabbar = self.TabbarList}

    GuideManager.Instance:RegisterGetGuideUi("ShengQiTabAttri",function ()
        return self.TabbarList.item_list[1],function ()
            self.TabbarList.item_list[1]:Click()
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("ShengQiTabLevel",function ()
        return self.TabbarList.item_list[2],function ()
            self.TabbarList.item_list[2]:Click()
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("ShengQiTabFuShi",function ()
        return self.TabbarList.item_list[3],function ()
            self.TabbarList.item_list[3]:Click()
        end
    end)

end

function ShengQiView:LoadCallback(param_t)
    param_t = param_t or {}
    local index = self:GetTabIndex(param_t.mod_key or 0)
    self.care_ht = self.data.sheng_qi_list:Care(BindTool.Bind(self.FlushTabbar,self))
    self.tab_ht = self.data.sel_tab_data:Care(BindTool.Bind(self.FlushSelTab, self))
    self.TabbarList:ClickItem(index)
    self:FlushTabbar()
end

function ShengQiView:CloseCallback()
    self.data:ClearSelStuff()
    self.data.sheng_qi_list:Uncare(self.care_ht)
    self.data:SetSelTab({})
    self.data.sel_tab_data:Uncare(self.tab_ht)
end

function ShengQiView:OnTabClick(data)
    self.data:SetSelTab(data)
    self.cur_tab_data = data
end

function ShengQiView:FlushSelTab()
    local tab_data = self.data:GetSelTab()
    if self.cur_tab_data and self.cur_tab_data ~= tab_data then
        local index = self:GetTabIndex(tab_data.open)
        self.TabbarList:ClickItem(index)
    end
end

function ShengQiView:FlushTabbar()
    --检查并锁住
    local tabbar = self.TabbarList
    if TableIsEmpty(self.data:GetShowList()) then
        tabbar.item_list[1]:TabbarOpenEvent(true)
        tabbar.item_list[2]:TabbarOpenEvent(false)
        tabbar.item_list[3]:TabbarOpenEvent(false)
        tabbar.item_list[4]:TabbarOpenEvent(true)
        self.TabbarList:ClickItem(1)
    else
        tabbar.item_list[1]:TabbarOpenEvent(true)
        tabbar.item_list[2]:TabbarOpenEvent(true)
        tabbar.item_list[3]:TabbarOpenEvent(true)
        tabbar.item_list[4]:TabbarOpenEvent(true)
    end
end

function ShengQiView:GetTabIndex(mod_key)
    for i,v in pairs(self.tab_list_data) do
        if v.open == mod_key then
            return i
        end
    end
    return 1
end

function ShengQiView:OnCloseClick()
    ViewMgr:CloseView(ShengQiView)
end

function ShengQiView:OnTipClick()
    local tab_data = self.data:GetSelTab()
    local mod = tab_data.open
    if mod == Mod.ShengQi.Attri or mod == Mod.ShengQi.Book then
        PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[319].desc)
    elseif mod == Mod.ShengQi.Up then
        PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[346].desc)
    elseif mod == Mod.ShengQi.FuShi then
        PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[347].desc)
    end
end

-- >>>>>>>>> 左侧的公用信息
ShengQiPublicPanel = ShengQiPublicPanel or DeclareMono("ShengQiPublicPanel", UIWFlushPanel)
function ShengQiPublicPanel:ShengQiPublicPanel()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sheng_qi_list, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_sheng_qi, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_tab_data, func = self.FlushPanel, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("ShengQiBag",function ()
        return self.BagBtnGuide,BindTool.Bind(self.OnOpenBagClick,self)
    end)
end

function ShengQiPublicPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ShengQiPublicPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetSelShengQi({})
end

function ShengQiPublicPanel:FlushPanel()
    local list = self.data:GetShowList()
    for i = 1, self.List:Length() do
        self.List[i]:SetData(list[i] or {})
    end
    if not TableIsEmpty(list) then
        local sel_sheng_qi = self.data:GetSelShengQi()
        if TableIsEmpty(sel_sheng_qi) then
            self.List[1]:SetSelected(true)
            self.data:SetSelShengQi(list[1])
        else
            for i,v in pairs(list) do
                if v.index == sel_sheng_qi.index then
                    self.List[i]:SetSelected(true)
                    self.data:SetSelShengQi(list[i])
                end
            end
        end
    end
end

function ShengQiPublicPanel:OnOpenBagClick()
    ViewMgr:OpenView(ShengQiBagView)
end


-- =================================== 对应的部件 ============================================



-- >>>>> 圣器 sheng_qi_item.prefab
ShengQiItem = ShengQiItem or DeclareMono("ShengQiItem", UIWidgetBaseItem)
function ShengQiItem:SetData(data)  -- data ==== ShengQi
    UIWidgetBaseItem.SetData(self,data)
    local item_id = data.item_id or 0
    self.Nothing:SetActive(item_id == 0)
    self.Show:SetActive(item_id > 0)
    local qua = 0
    if item_id > 0 then
        self:ShowFunc()
        qua = data:Qua()
    end
    UH.SpriteName(self.QuaImg, ShengQiCfg.ShengQiItemQua[qua])
    self.RedPoint:SetNum(0)
    if item_id == 0 then
        self.RedPoint:SetNum(ShengQiData.Instance:ShengQiUseRemind())
        self.FuShiList.text = ""
    else
        local mod_key = ShengQiData.Instance:GetSelTab().open or 0
        local remind_num = 0
        if mod_key == Mod.ShengQi.Up then
            remind_num = ShengQiData.Instance:UplevelRemind(data)
            if remind_num == 0 then
                remind_num = ShengQiData.Instance:HelpPetRemind(data)
            end
        end
        if remind_num == 0 and mod_key == Mod.ShengQi.FuShi then
            remind_num = ShengQiData.Instance:FuShiRemind(data)
        end
        self.RedPoint:SetNum(remind_num)
        local str = data:QuaShowPoint()
        self.FuShiList.text = str
    end
end

function ShengQiItem:ShowFunc()
    local data = self.data
    UH.SetIcon(self.Icon, data:IconId())
    UH.SetText(self.Name, data:Name())
    UH.SetText(self.Level, string.format("Lv.%s",data:Level()))
    local state = data:State()
    UH.SpriteName(self.StateImg, ShengQiCfg.StateImg[state])
end

function ShengQiItem:OnAddClick()
    ViewMgr:OpenView(ShengQiBagView)
end

function ShengQiItem:OnToggleClick()
    if self.data.item_id and self.data.item_id > 0 then
        ShengQiData.Instance:SetSelShengQi(self.data)
        ShengQiData.Instance:ClearSelStuff()
    end
end

function ShengQiItem:OnItemClick()
    local func_list = {}
    local state = self.data:State()
    local sel_tab= ShengQiData.Instance:GetSelTab()
    --if not TableIsEmpty(sel_tab) and sel_tab.open == Mod.ShengQi.Attri then
        if state == ShengQiCfg.State.Rest then
            return
        elseif state == ShengQiCfg.State.Fight then
            table.insert(func_list, ItemInfoFunc.ShengQiReady)
        elseif state == ShengQiCfg.State.Ready then
            table.insert(func_list, ItemInfoFunc.ShengQiFight)
        end
        table.insert(func_list, ItemInfoFunc.ShengQiRest)
    --end

    local info = {
		item_list = {self.data},
		func = function ()
			return func_list
		end
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end


-- >>>>> 圣器技能 sheng_qi_skill_item.prefab
ShengQiSkillItem = ShengQiSkillItem or DeclareMono("ShengQiSkillItem", UIWidgetBaseItem)
function ShengQiSkillItem:SetData(data) -- data ==== ShengQiSkill
    UIWidgetBaseItem.SetData(self, data)
    local skill_id = data.skill_id or 0
    self.Lock:SetActive(skill_id <= 0)
    self.Show:SetActive(skill_id > 0)
    if skill_id > 0 then
        local cfg = data:Cfg()
        local type = data:Type()
        UH.SetText(self.Name, data:Name())
        UH.SetText(self.Name2, data:Name())
        UH.SetIcon(self.Icon, data:Icon(), ICON_TYPE.BUFF)
        local logo = cfg.skill_logo or ""
        UH.SetText(self.TypeText, type == SHENG_QI_SKILL_TYPE.INITIATIVE and logo or Language.ShengQi.Text11)

        local qua = data:Qua()
        local qua_name = ShengQiCfg.SkillQua[qua]
        self.Qua:SetObjActive(qua_name ~= nil)
        if qua_name then
            UH.SpriteName(self.Qua, qua_name)
        end

        self.Qua2:SetObjActive(self.IsShowQua2 == true)
        if self.IsShowQua2 then
            UH.SpriteName(self.Qua2, "SkillQua" .. qua)
            UH.SpriteName(self.Qua2Flag, "DengJiDi" .. qua)
            UH.SetText(self.Qua2Type, Language.ShengQi.SkillQuaType[qua])
        end
        self.NewFlag:SetActive(data.is_new == true)
    end
    
end

function ShengQiSkillItem:OnShowClick()
    if self.IsCanClick == true then
        local pos = UH.GetRootPosition(self.Icon.gameObject)
        if self.X then
            pos.x = self.X
        end
        if self.Y then
            pos.y = self.Y
        end
        if self.data:Type() == SHENG_QI_SKILL_TYPE.TALENT then
            ViewMgr:OpenView(ShengQiSkillTip, {skill = self.data, pos = pos})
        else
            ViewMgr:OpenView(ShengQiSKillInfoView, {skill = self.data, pos = pos, is_book = self.IsBook})
        end   
    end
end

function ShengQiSkillItem:OnLockClick()
    if self.IsCanClick == true then

    end
end




-- >>>>> 圣器模型显示
ShengQiModelInfo = ShengQiModelInfo or DeclareMono("ShengQiModelInfo")
function ShengQiModelInfo:ShengQiModelInfo()
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
			}
		)
	end
end

-- data == ShengQi
function ShengQiModelInfo:SetData(data)
    self.data = data
    self.ui_obj:SetData(DrawerHelper.GetWeaponPath(data:ResId()))
    UH.SetText(self.Level, string.format(Language.ShengQi.Level2, data:Level()))
    UH.SetText(self.Name, data:Name())
    local score = data:Score()
    local score_level = data:ScoreLevel()
    self.ScoreInfo:SetScore(score,score_level)
end

function ShengQiModelInfo:OnDestroy()
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
end





-- >>>>> 圣器筛选类型
ShengQiTypeItem = ShengQiTypeItem or DeclareMono("ShengQiTypeItem", UIWidgetBaseItem)
function ShengQiTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local text = data.text
    if data.qua then
        if QualityColorStr[data.qua] then
            text = ColorStr(text, QualityColorStr[data.qua])
        end
    end
    UH.SetText(self.Text, text)
end


-- >>>>> 圣器属性item1
ShengQiAttriItem = ShengQiAttriItem or DeclareMono("ShengQiAttriItem", UIWidgetBaseItem)
-- data ==== ShengQiData:FormatAttriInfo
function ShengQiAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_nothing = TableIsEmpty(data) or data.is_nothing == true
    if self.Info then
        self.Info:SetActive(not is_nothing)
    end
    if self.NullTip then
        self.NullTip:SetObjActive(is_nothing)
    end
    if is_nothing then
        if self.NullTip then
            UH.SetText(self.NullTip, data.null_tip)
        end
    else
        UH.SetText(self.Name, data.name)
        UH.SetText(self.Value, data.value_str)
        if self.Tip then
            UH.SetText(self.Tip, data.tip)
        end
    end
end

function ShengQiAttriItem:OnClick()
    if self.CanClick then
        ViewMgr:OpenView(ShengQiAttriDesc, {attri_type = self.data.attri_type})
    end
end

-- >>>>> 圣器属性item2
ShengQiAttriItem2 = ShengQiAttriItem2 or DeclareMono("ShengQiAttriItem2", UIWidgetBaseItem)
-- data ==== ShengQiData:FormatAttriInfo or {name,value_str}
function ShengQiAttriItem2:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name or "")
    UH.SetText(self.Value, data.value_str or "")
end


-- 圣器属性描述
ShengQiAttriDesc = ShengQiAttriDesc or DeclareView("ShengQiAttriDesc", "sheng_qi/sheng_qi_attr_desc_view")
VIEW_DECLARE_MASK(ShengQiAttriDesc,ViewMask.BlockClose)
function ShengQiAttriDesc:LoadCallback(param)
    local attri_type = param.attri_type
    if not attri_type then
        return
    end
    local cfg = ShengQiData.Instance:GetAttriDescCfg(attri_type)
    if TableIsEmpty(cfg) then
        return
    end
    UH.SetText(self.Name, cfg.att_name)
    UH.SetText(self.Desc, cfg.att_dec)
end










-- ============= 特效抛物线逻辑 ================
ShengQiMoveEffect = ShengQiMoveEffect or BaseClass()
function ShengQiMoveEffect:__init()
    self.default_step = 20
end

function ShengQiMoveEffect:SetStartPos(s_pos)
    self.s_pos = s_pos
end
function ShengQiMoveEffect:GetStartPos()
    return self.s_pos
end

function ShengQiMoveEffect:SetPosList(pos_list)
    self.move_pos_list = pos_list
end
function ShengQiMoveEffect:SetEffectObj(effect_obj)
    self.effect_obj = effect_obj
end
function ShengQiMoveEffect:SetEndFunc(end_func)
    self.end_func = end_func
end
function ShengQiMoveEffect:SetStepNum(default_step)
    self.default_step = default_step
end
function ShengQiMoveEffect:Play(pos_list, effect_obj, end_func, default_step)
    self.move_pos_list = pos_list or self.move_pos_list
    self.effect_obj = effect_obj or self.effect_obj
    self.end_func = end_func or self.end_func
    self.cur_step = 1
    self.default_step = default_step or self.default_step
    TimeHelper:CancelTimer(self.time_handle)
    self.effect_obj:SetActive(true)
    self.time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.moveEffect,self),1,self.default_step)
end

function ShengQiMoveEffect:moveEffect()
    local pos = FormatCurve(self.move_pos_list,self.cur_step / self.default_step)
    UH.LocalPosG(self.effect_obj,pos)
    self.cur_step = self.cur_step + 1
    if self.cur_step > self.default_step then
        self.effect_obj:SetActive(false)
        UH.LocalPosG(self.effect_obj,self.move_pos_list[1])
        if self.end_func then
            self.end_func()
        end
        self:Cancel()
	end
end

function ShengQiMoveEffect:Cancel()
    TimeHelper:CancelTimer(self.time_handle)
end

function ShengQiMoveEffect:Destroy()
    self:Cancel()
end