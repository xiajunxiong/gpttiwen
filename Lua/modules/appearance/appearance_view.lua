AppearanceView = AppearanceView or DeclareView("AppearanceView", "appearance/AppearanceView",Mod.Appearance.Main)

VIEW_DECLARE_LEVEL(AppearanceView,ViewLevel.L1)

function AppearanceView:LoadCallback(param_t)
    self.Board:SetTabbarVisible(true)
    self.Board:SetData(self:GetType())
    self.Board.Tabbar:SetData({
        {name = Language.Appearance.Tab1, tab_objs = {self.Tab1}, open = Mod.Appearance.Main},
        {name = Language.Appearance.Tab2, tab_objs = {self.Tab5,self.Tab2}, open = Mod.Appearance.Bubbling,mod = TabMod(Mod.Appearance.Bubbling)},
        {name = Language.Appearance.Tab3, tab_objs = {self.TabTitle,self.Tab3}, open = Mod.Appearance.Title,mod = TabMod(Mod.Appearance.Title)},
        {name = Language.Appearance.Tab4, tab_objs = {self.Tab5,self.Tab4}, open = Mod.Appearance.TailTag,mod = TabMod(Mod.Appearance.TailTag)},
    })
    self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Appearance, self.Board.Tabbar, param_t.open_param))
    self.Board:SetRootName("board1")
    self.Currency:DefaultSet()
end

function AppearanceView:CloseCallback()
    AppearanceData.Instance:Initialization()
    AppearanceData.Instance:ClearRedPoint()
end

--=========================AppearanceBoard=========================
AppearanceBoard = AppearanceBoard or DeclareMono("AppearanceBoard")

function AppearanceBoard:AppearanceBoard()
	self.view_type = nil
end

function AppearanceBoard:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function AppearanceBoard:SetTabbarVisible(visible)
	self.Tabbar:SetObjActive(visible)
end

function AppearanceBoard:SetData(view_type)
	self.view_type = view_type
end

function AppearanceBoard:OnRelease()
	self.Tabbar:SetData({})
	self.Tabbar:SetObjActive(false)
end

function AppearanceBoard:SetRootName(name)
    self.Root.name = name
end

--=========================AppearanceTip=========================
AppearanceTip = AppearanceTip or DeclareView("AppearanceTip", "appearance/AppearanceTip")

function AppearanceTip:AppearanceTip()
    self.Board:SetData(self:GetType(),Language.Appearance.Currency,Vector2.New(650, 432))	
end

function AppearanceTip:LoadCallback()
    local list = {}
    for i=1,3 do
        table.insert(list,{content = Config.language_cfg_auto.textdesc[46 + i].desc})
    end
    self.GridList:SetData(list)
end

----------------------------AppearanceTipItem----------------------------
AppearanceTipItem = AppearanceTipItem or DeclareMono("AppearanceTipItem", UIWidgetBaseItem)

function AppearanceTipItem:SetData(data)
    UH.SetText(self.Content,data.content or "")
end

--=========================AppearanceBubble=========================
AppearanceBubble = AppearanceBubble or DeclareMono("AppearanceBubble")

function AppearanceBubble:SetData(res_id,v2,desc)
    self.Rect.sizeDelta = v2
    local res_config = AppearanceData.Instance:GetConfigByResId(AppearanceConfig.Type.Bubble,res_id)
    if res_config then
        local icon_type = AppearanceConfig.IconType[res_config.res_type]
        local icon_pre = AppearanceConfig.IconPre[res_config.res_type]
        UH.SetIcon(self.Icon,icon_pre .. res_id,icon_type)
        local bubble_color = AppearanceConfig.BubbleColor[res_id]
        UH.SetText(self.Desc,ColorStr(desc or res_config.name,bubble_color))
        local pos_config = Config.face_cfg_auto.bubble_pos[res_id]
        for i=1,self.BubbleList:Length() do
            local bubble_pre = AppearanceConfig.BubblePre[i]
            if pos_config and pos_config[bubble_pre] ~= "" then
                UH.SetIcon(self.BubbleList[i],bubble_pre .. res_id,icon_type)
                UH.AnchoredPosition(self.BubbleRect[i],self:GetVector(pos_config[bubble_pre]))
            else
                UH.SetIcon(self.BubbleList[i],nil)
            end
        end
    else
        UH.SetIcon(self.Icon,nil)
    end
    self.Icon:SetObjActive(res_config~=nil)
end

function AppearanceBubble:GetVector(value)
    local split = Split(value,"|")
    return Vector2.New(tonumber(split[1]),tonumber(split[2]))
end

--=========================AppearancePopupView=========================
AppearancePopupView = AppearancePopupView or DeclareView("AppearancePopupView", "appearance/AppearancePopupView")

function AppearancePopupView:AppearancePopupView()
    self.Board:SetData(self:GetType(),Language.Appearance.AddTitle,Vector2.New(866, 625))	
end

function AppearancePopupView:LoadCallback()
    local level = AppearanceData.Instance:GetAppearancInfo().face_level
    if level == 0 then
        self.List:SetData({
            {level = level+1,title_name = Language.Appearance.PopupLevelTitle[2]}
        })
    elseif level == GameEnum.APPEARANCE_MAX_LEVEL_NUM then
        self.List:SetData({{level = level,title_name = Language.Appearance.PopupLevelTitle[1]}})
    else
        self.List:SetData({
            {level = level,title_name = Language.Appearance.PopupLevelTitle[1]},
            {level = level+1,title_name = Language.Appearance.PopupLevelTitle[2]}
        })
    end
end

----------------------------AppearancePopupPageItem----------------------------
AppearancePopupPageItem = AppearancePopupPageItem or DeclareMono("AppearancePopupPageItem", UIWidgetBaseItem)

function AppearancePopupPageItem:SetData(data)
    UH.SetText(self.Title,data.title_name)
    local list = AppearanceData.Instance:GetAddList(data.level)
    table.sort(list,function(a,b)
        return a.job < b.job
    end)
    self.AttrList:SetData(list)
end

----------------------------AppearancePopupItem----------------------------
AppearancePopupItem = AppearancePopupItem or DeclareMono("AppearancePopupItem", UIWidgetBaseItem)

function AppearancePopupItem:SetData(data)
    local prof = RoleData.Instance:GetRoleProf()
    local color = data.job == prof and COLORSTR.Yellow6 or COLORSTR.Blue4
    UH.SetText(self.ProfName,ColorStr(Language.Common.Prof[data.job],color))
    UH.SpriteName(self.Prof,data.job == prof and "XiaoZhiYeDi1" or "XiaoZhiYeDi2")
    local add_name = ""
    for i,v in ipairs(data.add_list) do
        local attr_name = v.attr_name .. "  +" .. v.attr_value
        add_name = add_name .. "        ".. attr_name
    end
    local add_color = data.job == prof and COLORSTR.Yellow7 or COLORSTR.Gray4
    UH.SetText(self.AddName,ColorStr(add_name,add_color))
end