StarMapView = StarMapView or DeclareView("StarMapView", "star_map/star_map_view", Mod.StarMap.Main)
VIEW_DECLARE_LEVEL(StarMapView, ViewLevel.L0)

function StarMapView:StarMapView()
    self.data = StarMapData.Instance
    self.tab_list_data = {
        {name = Language.StarMap.Text17, name2 = Language.StarMap.Text18, tab_objs = {self.RoleMap}, open = Mod.StarMap.Role, mod = TabMod(Mod.StarMap.Role), is_self_img = true},
        {name = Language.StarMap.Text19, name2 = Language.StarMap.Text20, tab_objs = {self.PetMap}, open = Mod.StarMap.Pet, mod = TabMod(Mod.StarMap.Pet), is_self_img = true},
        {name = Language.StarMap.Text21, name2 = Language.StarMap.Text22, tab_objs = {self.PartnerMap}, open = Mod.StarMap.Partner, mod = TabMod(Mod.StarMap.Partner), is_self_img = true},
    }
    self.TabbarList:SetData(self.tab_list_data)
    self.Board = {Tabbar = self.TabbarList}
end

function StarMapView:LoadCallback(param_t)
    param_t = param_t or {}
    local index = self:GetTabIndex(param_t.mod_key or 0)
    self.TabbarList:ClickItem(index)
    --self:OnTabClick(self.tab_list_data[index])
    --self.ht = self.data.sel_node:Care(BindTool.Bind(self.FlushDisplay, self))
    self.ht = self.data.sel_node:Care(function ()
        self:FlushDisplay()
    end)
    self.remind_ht = self.data.remind_data:Care(function ()
        self:FlushSuperRemind()
        self:FlushStage()
    end)
    self:FlushDisplay()
    self:FlushSuperRemind()
    local item_id = Config.starmap_auto.other[1].superstar_id
    UH.SetIcon(self.TipIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
    local tips = string.format(Language.StarMap.Text16, Item.GetQuaName(item_id))
    UH.SetText(self.TipText, tips)
end

function StarMapView:CloseCallback()
    self.data:SetSelTab({})
    self.data.sel_node:Uncare(self.ht)
    StarMapData.Instance:SetSelNode({})
    self.data.remind_data:Uncare(self.remind_ht)
end

function StarMapView:FlushSuperRemind()
    local remind = self.data:RemindSuper()
    self.SuperRemind:SetNum(remind)
end

function StarMapView:FlushDisplay()
    local hole_max = self.data:GetMaxMapHole()
    self.UpInfo:SetActive(hole_max >= 0)
    self.LeftTop.alpha = hole_max < 0 and 1 or 0
    self.LeftTop.interactable = hole_max < 0
    self.RevertPanel:SetActive(hole_max >= 0)
    self.StageInfo:SetActive(hole_max < 0)
end

function StarMapView:FlushStage(map_type)
    local map_type = map_type or self.data:GetMapType()
    local list_data = {}
    for i = 0, StarMapData.MaxStage do
        table.insert(list_data, {stage = i, map_type = map_type})
    end
    self.StageList:SetData(list_data)
    local cur_stage = self.data:GetStage(map_type)
    if self.cur_stage == nil then
        self.StageList:SetectItem(cur_stage + 1, true)
        self:FlushMapShow(list_data[cur_stage + 1])
        self.cur_stage = cur_stage
    end
end

-- data == {stage = i, map_type = map_type})
function StarMapView:FlushMapShow(data)
    local map_type = data.map_type
    local stage = data.stage
    local list
    if map_type == StarMapCtrl.MapType.ROLE then
        list = self.RoleMapList
    elseif map_type == StarMapCtrl.MapType.PET then
        list = self.PetMapList
    elseif map_type == StarMapCtrl.MapType.PARTNER then
        list = self.PartnerMapList
    else
        return
    end
    if self.last_map_obj then
        self.last_map_obj:SetActive(false)
    end
    StarMapView.Stage = stage
    list[stage + 1]:SetActive(true)
    self.last_map_obj = list[stage + 1]
end

function StarMapView:GetTabIndex(mod_key)
    for i,v in pairs(self.tab_list_data) do
        if v.open == mod_key then
            return i
        end
    end
    return 1
end

function StarMapView:OnTabClick(data)
    self.data:SetSelTab(data)
    self.cur_stage = nil
    self:FlushStage()
end

function StarMapView:OnCloseClick()
    local hole_max = self.data:GetMaxMapHole()
    if hole_max >= 0 then
        StarMapData.Instance:SetSelNode({})
        return
    end
    ViewMgr:CloseView(StarMapView)
end

function StarMapView:OnOpenSuperClick()
    ViewMgr:OpenView(StarMapSuperView, {need_back = true})
    ViewMgr:CloseView(StarMapView)
end

function StarMapView:OnRevertPanelClick()
    StarMapData.Instance:SetSelNode({})
end

function StarMapView:OnPreviewClick()
    local map_type = self.data:GetMapType()
    ViewMgr:OpenView(StarMapAttriView, {map_type = map_type})
end

function StarMapView:OnDetialClick()
    PublicPopupCtrl.Instance:HelpTip(348)
end

function StarMapView:OnResetClick()
    -- local map_type = self.data:GetMapType()
    -- if not self.data:IsCanReset(map_type) then
    --     PublicPopupCtrl.Instance:Error("请先激活星位")
    --     return
    -- end
    -- ViewMgr:OpenView(StarMapResetView, {map_type = map_type})
end

function StarMapView:OnSelStageClick(data)
    self:FlushMapShow(data)
end


StarMapPanel = StarMapPanel or DeclareMono("StarMapPanel", UIWFlushPanel)
function StarMapPanel:StarMapPanel()
    self.data = StarMapData.Instance
    self.data_cares = {
        {data = self.data.star_data, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_node, func = self.FlushPanel, init_call = false},
        {data = self.data.is_show_detail, func = self.FlushPanel, init_call = false},
    }
    self.pos_cfg = {
        [0] = {
            [0] = {{151,402},{831,177},{149,-310},{-558,78}},
            [1] = {{151,-301},{819,-301},{149,171},{-512,-304}},
            [2] = {{42,287},{-554,107},{115,-381},{830,36}},
            [3] = {{80,102},{744,184},{81,-344},{-587,182}},
            [4] = {{887,180},{-5,180},{-607,-275},{540,-275}},
        },
        [1] = {
            [0] = {{151,369},{733,155},{150,-328},{-443,160}},
            [1] = {{-782,-336},{-443,161},{147,-328},{735,157}},
            [2] = {{-739,161},{-491,-266},{726,-261},{150,160}},
            [3] = {{841,-375},{720,210},{-15,-371},{-494,215}},
            [4] = {{-284,-478},{-573,17},{227,204},{766,-290}},
        },
        [2] = {
            [0] = {{-115,238},{844,245},{214,-325},{-626,-70}},
            [1] = {{150,370},{739,159},{152,-330},{-441,157}},
            [2] = {{797,183},{-130,183},{555,-278},{-748,-278}},
            [3] = {{149,156},{737,156},{149,-323},{-442,155}},
            [4] = {{734,-330},{149,-330},{736,153},{-443,153}},
        },
    }
    self.reset_scale = Vector3.New(78/128,78/128,78/128)
    self.reset_post = Vector3.New(0,0,0)
    self.max_scale = Vector3.New(1.2,1.2,1.2)
end

function StarMapPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function StarMapPanel:OnDisable()
    UIWFlushPanel.OnDisable(self)
end

function StarMapPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
end

function StarMapPanel:FlushPanel()
    local map_type = self.MapType or StarMapCtrl.MapType.ROLE
    local stage = self.Stage or 0
    if not map_type then
        return
    end
    local list_data = self.data:GetMapCfg(map_type, stage) or {}
    for i,v in pairs(list_data) do
        local seq = v.star_type
        local mono = self.NodeList[seq + 1]
        mono:SetData(v)
    end

    local skill_hole_data = self.data:GetSkillHoleListCfg(map_type)
    for i = 1, self.SkillList:Length() do
        if skill_hole_data[i] then
            self.SkillList[i]:SetData(skill_hole_data[i])
        end
    end

    self:FlushDisplay()
end

function StarMapPanel:FlushDisplay()
    local sel_node = self.data:GetSelNode()
    if TableIsEmpty(sel_node) then
        -- UH.LocalScale(self.Rect, Vector3.New(78/120,78/120,78/120))
        -- self.Rect.gameObject:SetLocalPosition(Vector3.New(0,0,0))
        self:PlayDisplay(self.reset_scale, self.reset_post)
        if StarMapData.Instance.sel_selector then
            StarMapData.Instance.sel_selector.isOn = false
        end
    else
        --UH.LocalScale(self.Rect, Vector3.New(1.2,1.2,1.2))
        local map_type = sel_node.map_type
        local cfg = self.pos_cfg[map_type][sel_node.grade]
        local pos_cfg
        if sel_node.star_type == 0 then
            pos_cfg = cfg[1]
        else
            pos_cfg = cfg[2 + sel_node.skill_hole]
        end
        local t_pos = Vector3.New(pos_cfg[1] or 0,pos_cfg[2] or 0,0)
        --self.Rect.gameObject:SetLocalPosition(Vector3.New(pos_cfg[1] or 0,pos_cfg[2] or 0,0))

        self:PlayDisplay(self.max_scale, t_pos)
    end
end

function StarMapPanel:PlayDisplay(t_scale, t_pos)
    self.t_scale = t_scale
    self.t_pos = t_pos
    self.begin_scale = self.Rect.localScale
    self.begin_pos = self.Rect.gameObject:GetLocalPosition()
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    self.t = 0
    self.run_ht = Runner.Instance:RunUntilTrue(function ()
        self.t = self.t + 0.15
        if self.t >= 1 then
            self.t = 1
        end
        local scale_v = Vector3.Lerp(self.begin_scale, self.t_scale, self.t)
        UH.LocalScale(self.Rect, scale_v)
        local pos_v = Vector3.Lerp(self.begin_pos, self.t_pos, self.t)
        self.Rect.gameObject:SetLocalPosition(pos_v)
        if self.t >= 1 then
            self.run_ht = nil
            return true
        end
    end)
end


StarMapUpInfo = StarMapUpInfo or DeclareMono("StarMapUpInfo", UIWFlushPanel)
function StarMapUpInfo:StarMapUpInfo()
    self.data = StarMapData.Instance
    self.data_cares = {
        {data = self.data.sel_node, func = self.FlushPanel, init_call = false},
        {data = self.data.star_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false},
    }
end

function StarMapUpInfo:Awake()
    UIWFlushPanel.Awake(self)
    self.move_effect_list = {}
    for i = 1,self.EffectList:Length() do
        local effect = self.EffectList[i]
        if effect then
            self.move_effect_list[i] = ShengQiMoveEffect.New()
            self.move_effect_list[i]:SetEffectObj(effect)
            self.move_effect_list[i]:SetStartPos(effect:GetLocalPosition())
        end
    end
end

function StarMapUpInfo:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function StarMapUpInfo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
     for i,v in pairs(self.move_effect_list or {}) do
        v:Destroy()
    end
end

function StarMapUpInfo:FlushPanel()
    local sel_node = self.data:GetSelNode()
    if TableIsEmpty(sel_node) then
        return
    end
    local map_type = sel_node.map_type
    local star_id = sel_node.star_type
    local stage = sel_node.grade
    local node = StarMapNode.New(map_type, star_id, stage)
    self.node = node
    local is_max = node:IsMax()
    local is_active = node:IsActive()
    UH.SpriteName(self.StarBG, node:AttriBG())
    UH.SpriteName(self.StarIcon, node:AttriIcon())
    UH.SetText(self.StarName, node:Name())
    UH.SetText(self.Level, node:LevelAndMaxDesc())
    self.ActiveFlag:SetActive(not node:IsActive())
    --UH.SetText(self.CurLevel, node:LevelDesc())
    --UH.SetText(self.NextLevel,node:NextLevelDesc())
    --self.LevelUpObj:SetActive(not is_max)
    self.UpBtn:SetActive(is_active and not is_max)
    self.AutoBtn:SetActive(is_active and not is_max)
    self.ActiveBtn:SetActive(not is_active)
    self.MaxFlag:SetActive(is_max)
    --local show_add_attri_list = node:ShowAddAttriList()
    --self.AttriList:SetData(show_add_attri_list)
    self:FlushStuff()

    local is_can_active = node:IsCanActive()
    UH.SetInter(self.Inter, is_can_active)
    if not is_can_active then
        local tips_text = Language.StarMap.ActiveTip --string.format(Language.StarMap.Text1, node:Condition())
        self.tips = tips_text
        UH.SetText(self.ActiveTip, tips_text)
    else
        UH.SetText(self.ActiveTip, "")
    end
    
    -- 属性信息
    self.NextAttriInfo:SetObjActive(not is_max and is_active)
    self.AttriInfo:SetObjActive(is_active)
    self.NotAttriInfo:SetObjActive(not is_active)
    if not is_max and is_active then
        local attris = node:NextAttriList()
        self.NextAttriInfo:SetData(attris[1])
    end
    local attris = node:AttriList()
    if is_active then
        self.AttriInfo:SetData(attris[1])
    else
        self.NotAttriInfo:SetData(attris[1])
    end
end

function StarMapUpInfo:FlushStuff()
    if not self.node then
        return
    end
    self.StuffList:SetData(self.node:StuffList())
end

function StarMapUpInfo:OnUpClick(type)
    if self.effect_ing then
        return
    end
    local node = self.node
    if TableIsEmpty(node) then
        return
    end
    type = type or 1
    local flag, item_id = self.node:GetCtrlFlag()
    if flag == -3 then
        PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2],function()
            ShoppingCtrl.Instance:OpenStoreView(0)
        end)
    elseif flag == -2 then
        local cfg = node:BaseCfg()
        local tips_text = string.format(Language.StarMap.Text1, cfg.condition)
        PublicPopupCtrl.Instance:Error(tips_text)
    elseif flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.StarMap.MaxLevelTip)
    elseif flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.StarMap.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
    else
        self:OnUpEvent(type)
        -- if type == 1 then
        --     StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.LevelUp, node.map_type,node.id, node.stage)
        -- elseif type == 2 then
        --     StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.AutoUp, node.map_type,node.id, node.stage)
        -- end
    end
end

function StarMapUpInfo:OnUpEvent(type)
    if TableIsEmpty(self.move_effect_list) then
        return
    end
    local node = self.node
    if TableIsEmpty(node) then
        return
    end
    self.effect_ing = true
    self.effect_end_pos = self.EndEffect:GetLocalPosition()
    for i,move_effect in pairs(self.move_effect_list) do
        local pos_list = {}
        local s_pos = move_effect:GetStartPos()
        pos_list[1] = s_pos
        pos_list[3] = self.effect_end_pos
        if i == #self.move_effect_list then
            pos_list[2] = pos_list[1] + Vector3.New(100,200,0)
            move_effect:Play(pos_list, nil, function ()
                self.effect_ing = false
                if type == 1 then
                    StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.LevelUp, node.map_type,node.id, node.stage)
                elseif type == 2 then
                    StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.AutoUp, node.map_type,node.id, node.stage)
                end
                self.EffectTool:Play("2161055")
            end)
        else
            pos_list[2] = pos_list[1] + Vector3.New(-100,200,0)
            move_effect:Play(pos_list)
        end
        
    end
end



StarMapNodeItem = StarMapNodeItem or DeclareMono("StarMapNodeItem", UIWidgetBaseItem)
function StarMapNodeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local map_type = data.map_type
    local id = data.star_type
    local node = StarMapNode.New(map_type, id, data.grade)
    UH.SpriteName(self.AttriBG, node:AttriBG())
    UH.SpriteName(self.AttriIcon, node:AttriIcon())

    local hole_max = StarMapData.Instance:GetMaxMapHole()
    local is_show_detial = StarMapData.Instance:GetShowDetail()
    self.Text:SetObjActive(is_show_detial or hole_max == data.skill_hole)
    if is_show_detial or hole_max == data.skill_hole then
        UH.SetText(self.Text, node:LevelAndMaxDesc(2))
    end

    local level = node:Level()
    if self.last_node and self.last_level then
        if level > self.last_level and self.last_node.map_type == node.map_type and self.last_node.id == node.id and self.last_node.stage == node.stage then
            self.EffectTool:Play(2161055)
        end
    end
    self.last_node = node
    self.last_level = level

    self:FlushEffect()
end

function StarMapNodeItem:FlushEffect()
    if not self.is_awake then
        return
    end
    if not self.last_node then
        return
    end
    local level = self.last_node:Level()
    local is_can_active = self.last_node:IsCanActive()
    if is_can_active and level == 0 then
        if not self.effect_ht then
            self.effect_ht = self.EffectTool:Play("7165452")
        end
    else
        if self.effect_ht then
            self.EffectTool:Stop(self.effect_ht)
            self.effect_ht = nil
        end
    end
end

function StarMapNodeItem:OnEnable()
    self.is_awake = true
    self:FlushEffect()
end

function StarMapNodeItem:OnNodeSelClick(value)
    if value then
        StarMapData.Instance:SetSelNode(self.data)
        StarMapData.Instance.sel_selector = self.Selector
    else
        StarMapData.Instance:SetSelNode({})
    end

    if self.is_awake then
        if value then
            if not self.sel_effect_ht then
                self.sel_effect_ht = self.EffectTool:Play("7165451")
            end
        else
            if self.sel_effect_ht then
                self.EffectTool:Stop(self.sel_effect_ht)
                self.sel_effect_ht = nil
            end
        end
    end
end

StarMapLine  = StarMapLine or DeclareMono("StarMapLine")
function StarMapLine:StarMapLine()
    self.data = StarMapData.Instance
    self.ht = self.data.star_data:Care(BindTool.Bind(self.FlushLine,self))
end

function StarMapLine:OnEnable()
    self:FlushLine()
end

function StarMapLine:FlushLine()
    local stage = StarMapView.Stage or 0
    local s_level = self.data:GetStarLevel(self.MapType, self.SNode, stage)
    local e_level = self.data:GetStarLevel(self.MapType, self.VNode, stage)
    local e_can_active = e_level > 0
    local s_can_active = s_level > 0
    if not e_can_active then
        e_can_active = StarMapData.Instance:IsCanActive(self.MapType, self.VNode, stage)
    end
    if not s_can_active and e_can_active then
        s_can_active = StarMapData.Instance:IsCanActive(self.MapType, self.SNode, stage)
    end

    self.Line:SetActive(e_can_active and s_can_active and (s_level > 0 or e_level > 0))
end

function StarMapLine:OnDestroy()
    self.data.star_data:Uncare(self.ht)
end

-- 星图技能孔
StarMapSkillHole = StarMapSkillHole or DeclareMono("StarMapSkillHole", UIWidgetBaseItem)
function StarMapSkillHole:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local super_seq =  StarMapData.Instance:GetHoleSkillInfo(data.starmap_tpye, data.skill_hole)
    self.Lock:SetActive(super_seq == -1)
    self.Icon:SetObjActive(super_seq > 0)
    self.super_seq = super_seq
    if super_seq > 0 then
        local skill_cfg = StarMapData.Instance:GetSuperSkillCfg(super_seq)
        if not TableIsEmpty(skill_cfg) then
            UH.SpriteName(self.Icon, skill_cfg.icon_id)
        else
            self.Icon:SetObjActive(false)
        end
    end

    self:FlushEffect()
end

function StarMapSkillHole:FlushEffect()
    if not self.is_awake then
        return
    end
    local data = self.data
    local super_seq =  StarMapData.Instance:GetHoleSkillInfo(data.starmap_tpye, data.skill_hole)
    if super_seq == 0 then
        if not self.effect_ht then
            self.effect_ht = self.EffectTool:Play("7165476")
        end
    else
        if self.effect_ht then
            self.EffectTool:Stop(self.effect_ht)
            self.effect_ht = nil
        end
    end
end

function StarMapSkillHole:OnEnable()
    self.is_awake = true
    self:FlushEffect()
end

function StarMapSkillHole:OnClick()
    if self.super_seq == -1 then
        PublicPopupCtrl.Instance:Center(Language.StarMap.Tip3)
    else
        ViewMgr:OpenView(StarMapSelSkillView, {map_type = self.data.starmap_tpye, hole_index = self.data.skill_hole})
    end
end


StarMapSkillItem = StarMapSkillItem or DeclareMono("StarMapSkillItem", UIWidgetBaseItem)
function StarMapSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon, self.data.icon_id, ICON_TYPE.BUFF)
    UH.SetText(self.Name, data.skill_name)
    UH.SetText(self.Level, "Lv" .. data.skill_level)
end

function StarMapSkillItem:OnClick()
    if self.IsCanClick then
        local param_t = TableCopy(self.data)
        local pos = UH.GetRootPosition(self.Icon.gameObject)
        param_t.pos = pos
        ViewMgr:OpenView(StarMapSkillTip, param_t)
    end
end

StarMapStuffItem = StarMapStuffItem or DeclareMono("StarMapStuffItem", UIWidgetBaseItem)
function StarMapStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local has_num
    local num_str
    local need_num = data.num
    if data.item_id == CommonItemId.Coin then
        has_num = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
        num_str = need_num
        if has_num < need_num then
            num_str = ColorStr(num_str, COLORSTR.Red8)
        end
    else
        has_num = Item.GetNum(data.item_id)
        num_str = DataHelper.ConsumeNum(has_num, need_num,true)
    end
    local item = Item.Create({item_id = data.item_id, num = num_str})
    self.ItemCell:SetData(item)
    UH.SetText(self.Name, item:QuaName())
end

function StarMapStuffItem:GetEffectObj()
    return self.Effect
end


-- <<弃用>>
StarMapUpAttriItem = StarMapUpAttriItem or DeclareMono("StarMapUpAttriItem", UIWidgetBaseItem)
function StarMapUpAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_add = data.attri_value ~= data.next_value and data.next_value ~= nil
    self.UpObj:SetActive(is_add)
    self.NextValue:SetObjActive(is_add)
    local cur_attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    UH.SetText(self.AttriName, cur_attri_info.attr_name)
    UH.SetText(self.AttriValue, cur_attri_info.attr_percent_value)

    if is_add then
        local next_attri_info = CommonStruct.AttrStruct(data.attri_type, data.next_value)
        UH.SetText(self.NextValue, next_attri_info.attr_percent_value)
    end
end

-- 新版属性
StarMapUpAttriAndLevelItem = StarMapUpAttriAndLevelItem or DeclareMono("StarMapUpAttriAndLevelItem", UIWidgetBaseItem)
-- {level, attri_type, attri_value}
function StarMapUpAttriAndLevelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level, "Lv." .. data.level)
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    UH.SetText(self.Name, attri_info.attr_name)
    UH.SetText(self.Value, "+" .. attri_info.attr_percent_value)
end



StarMapStageItem = StarMapStageItem or DeclareMono("StarMapStageItem", UIWidgetBaseItem)
-- data == {map_type, stage}
function StarMapStageItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.StageNum, DataHelper.GetDaXie(data.stage + 1))
    local is_unlock = StarMapData.Instance:IsUnlockStage(data.map_type, data.stage)
    self.Lock:SetActive(not is_unlock)
    self.Remind:SetNum(StarMapData.Instance:GetMapRemind(data.map_type, data.stage))
    UH.SetInter(self.Inter,is_unlock)
end

function StarMapStageItem:OnLockClick()
    PublicPopupCtrl.Instance:Error(Language.StarMap.Tip4)
end



-- 圣器技能详情弹框
StarMapSkillTip = StarMapSkillTip or DeclareView("StarMapSkillTip", "star_map/star_map_skill_tip")
VIEW_DECLARE_MASK(StarMapSkillTip, ViewMask.BlockClose)
function StarMapSkillTip:StarMapSkillTip()
    
end

-- {icon_id, desc, name, level}
function StarMapSkillTip:LoadCallback(param_t)
    param_t = param_t or {}
    if TableIsEmpty(param_t) then
        return
    end
    
    UH.SetText(self.LblName, param_t.name or param_t.skill_name)
    UH.SetText(self.LblDesc, param_t.desc or param_t.description)
    local level = param_t.level or param_t.skill_level or 0
    local icon_type = param_t.icon_type or ICON_TYPE.BUFF
    UH.SetIcon(self.SkillIcon, param_t.icon_id, ICON_TYPE.BUFF)
    UH.SetText(self.Level, "Lv" .. level)

    if param_t.pos ~= nil  then
        self:CheckPos(param_t.pos)
        UH.LocalPos(self.PosRect,param_t.pos)
    end 
end

function StarMapSkillTip:CheckPos(pos)
    if pos.x > 340 then
        pos.x = 340
    end
    if pos.x < -340 then
        pos.x = -340
    end
    if pos.y > 88 then
        pos.y = 88
    end
    if pos.y < -450 then
        pos.y = -450
    end
end