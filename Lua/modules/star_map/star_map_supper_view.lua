local Input = UnityEngine.Input
StarMapSuperView = StarMapSuperView or DeclareView("StarMapSuperView", "star_map/star_map_super_view", Mod.StarMap.Super)
VIEW_DECLARE_LEVEL(StarMapSuperView, ViewLevel.L0)
function StarMapSuperView:StarMapSuperView()
    self.min_scale = 0.475
    self.max_scale = 1.075
    self.star_scale = self.min_scale
    StarMapSuperView.star_scale = self.star_scale
end

function StarMapSuperView:OnCloseClick()
    ViewMgr:CloseView(StarMapSuperView)
    if self.need_back then
        ViewMgr:OpenView(StarMapView)
    end
    self:CloseChildView()
end

function StarMapSuperView:CloseChildView()
    ViewMgr:CloseView(StarMapSuperActiveView)
    ViewMgr:CloseView(StarMapSuperSkillView)
end

function StarMapSuperView:OnBgClick()
    self:CloseChildView()
    if StarMapData.Instance.super_selector then
        StarMapData.Instance.super_selector.isOn = false
    end
end

function StarMapSuperView:LoadCallback(param_t)
    self.need_back = param_t.need_back
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    self.run_ht = Runner.Instance:RunUntilTrue(function ()
        local value = self:GetInputAxis()
        if value ~= 0 then
            local add_value = value
            self:SetScale(add_value * 0.5)
        end
    end)
end

function StarMapSuperView:CloseCallback()
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
        self.run_ht = nil
    end
    self:CloseChildView()
end

function StarMapSuperView:SetScale(add_scale)
    local scale = self.star_scale + add_scale
    if scale > self.max_scale then
        scale = self.max_scale
    end
    if scale < self.min_scale then
        scale = self.min_scale
    end
    self.star_scale = scale
    StarMapSuperView.star_scale = self.star_scale
    UH.LocalScale(self.StarInfoRect, Vector3.New(self.star_scale,self.star_scale,self.star_scale))
end

function StarMapSuperView:OnFangDaClick()
    self:SetScale(0.1)
end

function StarMapSuperView:OnSuoXiaoClick()
    self:SetScale(-0.1)
end

function StarMapSuperView:OnResetClick()
    ViewMgr:OpenView(StarMapResetView,{map_type = StarMapCtrl.MapType.Super})
end

function StarMapSuperView:OnPreClick()
    ViewMgr:OpenView(StarMapAttriView,{map_type = StarMapCtrl.MapType.Super})
end

-- 1 ~ -1
function StarMapSuperView:GetInputAxis()
    local value = 0
    if DEBUG_MODE then
        value = Input.GetAxis("Mouse ScrollWheel")
    else
        value = GmData.Instance:TranslateZ()
        if value > 1 then
            value = 1
        elseif value < -1 then
            value = -1
        end
        value = value * 0.1
    end
    return value
end


StarMapSuperInfo = StarMapSuperInfo or DeclareMono("StarMapSuperInfo", UIWFlushPanel)
function StarMapSuperInfo:StarMapSuperInfo()
    self.line_list_cfg = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},{8,9},{9,10},{10,11},
        {2,12},{12,13},{13,14},{14,15},{15,16},{16,17},{17,18},{18,19},{19,20},{20,21},{21,22},{22,23},{23,24},{24,25},{25,26},{26,27},{27,28},{28,29},{29,30},{30,31},{28,32},{30,31},
        {5,109},{109,110},{110,111},{111,112},{112,113},{113,114},{114,115},{115,116},{116,117},{117,118},{118,119},{119,120},{116,121},{121,122},{122,123},{123,124},{124,125},{125,126},
        {0,97},{97,96},{96,98},{98,99},{99,100},{100,101},{101,102},{102,103},{103,104},{104,105},{105,106},{106,107},{107,108},
        {0,91},{91,92},{92,93},{93,94},{94,95},{95,127},{127,128},
        {25,33},{33,34},{34,35},{35,36},{36,37},{37,38},{38,39},
        {36,40},{40,58},{58,59},{59,60},{59,61},{61,62},{62,63},{63,64},{64,65},{65,66},{66,67},{67,68},{68,69},{69,0},
        {40,41},{41,42},{42,43},{42,48},{43,44},{44,45},{45,46},{46,47},
        {43,49},{49,50},{50,51},{51,52},{52,53},{53,54},{54,55},{55,56},{56,57},
        {53,70},{70,71},{71,72},{72,73},{73,74},{74,75},{75,76},{76,77},{77,78},
        {74,79},{79,80},{80,81},{79,82},{82,83},{83,84},{84,85},{85,86},{86,87},{87,88},{88,89},{89,90},
        {74,129},{129,130},{130,131},{131,132},{132,133},{133,134},{134,135},{135,137},{134,136},{136,137},{137,138},{137,139},{138,140},{139,140},
        {134,141},{141,142},{142,143},{143,144},{144,145},{145,146},{146,147},{147,148},{148,149},{149,150},{150,151},
        {149,152},{152,153},{153,154},{154,155},{155,156},{156,157},{157,158},
        {155,161},{161,162},{162,163},{161,160},{160,159},{159,164},{164,165},
    }

    self.data = StarMapData.Instance
    self.data_cares = {
        {data = self.data.super_star_data, func = self.FlushPanel, init_call = false}
    }
end

function StarMapSuperInfo:Awake()
    UIWFlushPanel.Awake(self)
    self.line_list_data = {}
    for i,v in pairs(self.line_list_cfg) do
        local data = {}
        data.s_node = v[1]
        data.e_node = v[2]
        local s_mono = self:GetMono(v[1])
        local e_mono = self:GetMono(v[2])
        local s_pos = s_mono.gameObject:GetLocalPosition()
        local e_pos = e_mono.gameObject:GetLocalPosition()
        data.s_pos = s_pos
        data.len = Vector2.Distance(s_pos, e_pos)
        local dir_pos = e_pos - s_pos  
        data.rot = 90 - self:GetAngle(dir_pos)
        table.insert(self.line_list_data, data)
    end
    self:FlushPanel()
end

function StarMapSuperInfo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    StarMapData.Instance:SetSelSuperNode({})
    self.data:SetJumpSuper(nil)
    TimeHelper:CancelTimer(self.time_ht)
end

function StarMapSuperInfo:GetAngle(dir_pos)
    local vector = dir_pos.normalized
    local x = vector.x
    local y = vector.y
    local num = math.atan2(x,y)         -- 弧度
    local angle = num * 180 / math.pi   -- 角度 = 弧度 * 360 / 2PI 
    return angle
end

-- 0下标开始
function StarMapSuperInfo:GetMono(index)
    if index <= 49 then
        return self.NodeList1[index + 1]
    elseif index <= 99 then
        return self.NodeList2[index - 50 + 1]
    elseif index <= 149 then
        return self.NodeList3[index - 100 + 1]
    else
        return self.NodeList4[index - 150 + 1]
    end
end

function StarMapSuperInfo:FlushPanel()
    local list_data = Config.starmap_auto.superstar_condition
    local jump_seq = self.data:GetJumpSuper()
    for i,v in pairs(list_data) do
        local mono = self:GetMono(v.star_type)
        mono:SetData(v)
        if jump_seq and v.star_type == jump_seq then
            local pos = mono.gameObject:GetLocalPosition()
            local cur_pos = self.gameObject:GetLocalPosition()
            local jump_pos = cur_pos - pos
            self.gameObject:SetLocalPosition(jump_pos)
            self.time_ht = Invoke(function ()
                mono:ShowSpSel()
                mono:SetSelected(true)
            end,2.5)
            jump_seq = nil
            self.data:SetJumpSuper(nil)
        end
    end

    self.LineList:SetData(self.line_list_data)
end

function StarMapSuperInfo:OnDetailClick()
    self.data.is_super_show_detail = not self.data.is_super_show_detail
    self:FlushPanel()
end


StarMapSuperItem = StarMapSuperItem or DeclareMono("StarMapSuperItem", UIWidgetBaseItem)
function StarMapSuperItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local first_cfg = StarMapData.Instance:GetSuperFirstCfg(data.star_type)
    self.first_cfg = first_cfg
    local bg_img = first_cfg.star_type == 1 and "JiNengDi108" or "TeShuShuXingDi"
    UH.SpriteName(self.BG, bg_img)
    --self.TypeIcon:SetObjActive(first_cfg.star_type ~= 1)
    self.AttriIcon:SetObjActive(first_cfg.star_type ~= 1)
    self.SkillIcon:SetObjActive(first_cfg.star_type == 1)
    if first_cfg.star_type ~= 1 then
        UH.SpriteName(self.AttriIcon, "Attri" .. first_cfg.param1)
        --UH.SpriteName(self.TypeIcon, "SuperDi" .. first_cfg.target)
    else
        local skill_cfg = StarMapData.Instance:GetSkillCfg(first_cfg.param1, first_cfg.param2)
        UH.SpriteName(self.SkillIcon, skill_cfg.icon_id)
    end

    local level = StarMapData.Instance:GetSuperStarLevel(data.star_type) 
    local is_active = level > 0
    UH.SetInter(self.Inter, is_active)
    self.Level:SetObjActive(StarMapData.Instance.is_super_show_detail == true)
    if StarMapData.Instance.is_super_show_detail == true then
        local max_level = StarMapData.Instance:GetSuperMaxLevel(data.star_type)
        if first_cfg.star_type == 1 then
            UH.SetText(self.Level, string.format("Lv.%s", level))
        else
            UH.SetText(self.Level, string.format("%s/%s", level, max_level))
        end
    end
    self:FlushEffect()
end

function StarMapSuperItem:FlushEffect()
    if not self.is_awake then
        return
    end
    local data = self.data
    local level = StarMapData.Instance:GetSuperStarLevel(data.star_type) 
    local is_can_active = StarMapData.Instance:IsCanActiveSuper(data.star_type)
    if is_can_active and level == 0 then
        if not self.effect_ht then
            self.effect_ht = 1
            self.effect_ht = self.EffectTool:Play("7165472")
        end
    else
        if self.effect_ht then
            self.EffectTool:Stop(self.effect_ht)
            self.effect_ht = nil
        end
    end
end

function StarMapSuperItem:OnEnable()
    self.is_awake = true
    self:FlushEffect()
end

function StarMapSuperItem:ShowSpSel()
    self.is_show_sp_sel = true
end

function StarMapSuperItem:OnSelClick(value)
    if value then
        StarMapData.Instance:SetSelSuperNode(self.data)
        local scale = StarMapSuperView.star_scale or 1
        local pos = UH.GetRootPosition(self.gameObject)
        pos.x = pos.x * scale
        if self.first_cfg.star_type ~= 1 then
            ViewMgr:OpenView(StarMapSuperActiveView, {click_pos = pos})
            if ViewMgr:IsOpen(StarMapSuperActiveView) then
                local view_info = ViewMgr:getViewInfo(StarMapSuperActiveView)
                view_info.view_mono:HandlePos(pos)
            end
        else
            ViewMgr:OpenView(StarMapSuperSkillView, {click_pos = pos})
            if ViewMgr:IsOpen(StarMapSuperSkillView) then
                local view_info = ViewMgr:getViewInfo(StarMapSuperSkillView)
                view_info.view_mono:HandlePos(pos)      
            end
        end
        StarMapData.Instance.super_selector = self.Selector
    else
        StarMapData.Instance.super_selector = nil
        ViewMgr:CloseView(StarMapSuperSkillView)
        ViewMgr:CloseView(StarMapSuperActiveView)
    end
    
    if self.is_awake then
        if value then
            if not self.sel_effect_ht then
                if self.is_show_sp_sel then
                    self.is_show_sp_sel = nil
                    self.sel_effect_ht = self.EffectTool:Play("7165478")
                else
                    self.sel_effect_ht = self.EffectTool:Play("7165451")
                end
            end
        else
            if self.sel_effect_ht then
                self.EffectTool:Stop(self.sel_effect_ht)
                self.sel_effect_ht = nil
            end
        end
    end 
end

StarMapLineItem = StarMapLineItem or DeclareMono("StarMapLineItem", UIWidgetBaseItem)
-- data ==== {s_node, e_node, s_pos, rot, len}
function StarMapLineItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject:SetLocalPosition(data.s_pos)
    self.Rect:SetSizeWithCurrentAnchors(0,data.len)
    UH.LocalRotate(self.Rect,Quaternion.Euler(0,0,data.rot))
    local s_level = StarMapData.Instance:GetSuperStarLevel(data.s_node)
    local e_level = StarMapData.Instance:GetSuperStarLevel(data.e_node)
    local e_can_active = e_level > 0
    local s_can_active = s_level > 0
    if not e_can_active then
        e_can_active = StarMapData.Instance:IsCanActiveSuper(data.e_node)
    end
    if not s_can_active and e_can_active then
        s_can_active = StarMapData.Instance:IsCanActiveSuper(data.s_node)
    end
    local img = s_can_active and e_can_active  and "LianJieXian1_JiHuo" or "LianJie1"
    UH.SpriteName(self.Line, img)
end


-- ===================== 超星系属性节点激活 ===================
StarMapSuperActiveView = StarMapSuperActiveView or DeclareView("StarMapSuperActiveView", "star_map/star_map_super_active_view")
VIEW_DECLARE_MASK(StarMapSuperActiveView,ViewMask.None)

function StarMapSuperActiveView:LoadCallback(param_t)
    if param_t.click_pos then
        self:HandlePos(param_t.click_pos)
    end
end

function StarMapSuperActiveView:HandlePos(pos)
    local x = pos.x
    if x > 0 then
        x = x - 300
    else
        x = x + 300
    end
    local new_pos = Vector3.New(x,0,0)
    UH.LocalPos(self.Rect,new_pos)
end


StarMapSuperActivePanel = StarMapSuperActivePanel or DeclareMono("StarMapSuperActivePanel", UIWFlushPanel)
function StarMapSuperActivePanel:StarMapSuperActivePanel()
    self.data = StarMapData.Instance
    self.data_cares = {
        {data = self.data.super_star_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_super_node, func = self.FlushPanel, init_call = false},
    }
end
function StarMapSuperActivePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()

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

function StarMapSuperActivePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for i,v in pairs(self.move_effect_list or {}) do
        v:Destroy()
    end
end

-- node == Config.starmap_auto.superstar_condition
function StarMapSuperActivePanel:FlushPanel()
    local node = self.data:GetSelSuperNode()
    if TableIsEmpty(node) then
        return
    end
    local star_id = node.star_type
    local first_cfg = self.data:GetSuperFirstCfg(star_id)
    if first_cfg.star_type == 1 then
        return
    end
    local is_can_active = self.data:IsCanActiveSuper(star_id)
    UH.SetText(self.TargetText, string.format(Language.StarMap.Text13, Language.StarMap.StarMapType[first_cfg.target]))
    UH.SetText(self.Name, first_cfg.star_name)
    UH.SpriteName(self.AttriIcon, "Attri" .. first_cfg.param1)
    UH.SpriteName(self.TypeImg, "SuperDi" .. first_cfg.target)

    local level = self.data:GetSuperStarLevel(star_id)
    local max_level = self.data:GetSuperMaxLevel(star_id)
    local is_active = level > 0
    local is_max = level >= max_level
    UH.SetText(self.Level, string.format(Language.StarMap.Level1, level, max_level))
    -- local cur_level_str = is_active and "Lv." .. level or "未激活"
    -- UH.SetText(self.CurLevel, cur_level_str)
    -- UH.SetText(self.NextLevel, is_max and "" or ("Lv." .. (level + 1)))
    -- self.UpLevelFlag:SetActive(not is_max)
    self.ActiveFlag:SetActive(not is_active)
    self.Tips:SetActive(not is_can_active)
    self.ActiveBtn:SetActive(not is_active and not is_max)
    self.UpBtn:SetActive(is_active and not is_max)
    self.AutoBtn:SetActive(is_active and not is_max)
    self.MaxFlag:SetActive(is_max)
    self.Stuff:SetActive(not is_max)

    UH.SetInter(self.CellInter, is_active)

    local cur_cfg = is_active and self.data:GetSuperLevelCfg(star_id, level) or first_cfg
    local next_cfg = is_max and cur_cfg or self.data:GetSuperLevelCfg(star_id, level + 1)
    local attri_list = self.data:GetSuperAttriListByCfg(cur_cfg)
    self.NextAttri:SetObjActive(not is_max)

    if not is_max then
        local stuff_data = self.data:GetStuffDataByCfg(next_cfg)
        self.StuffList:SetData(stuff_data)
        if is_active then
            local next_attri_list = self.data:GetSuperAttriListByCfg(next_cfg)
            self.NextAttri:SetData(next_attri_list[1])
        end
    end
    if not is_active then
        self.CurAttri:SetData({})
        self.NextAttri:SetData(attri_list[1])
    else
        self.CurAttri:SetData(attri_list[1])
    end

    UH.SetInter(self.Inter, is_can_active)
    if not is_can_active then
        local tips_text = Language.StarMap.ActiveTip --string.format(Language.StarMap.Text6, node.condition)
        self.tips = tips_text
        UH.SetText(self.TipsText,tips_text)
    end
end

function StarMapSuperActivePanel:OnUpClick(type)
    if self.effect_ing then
        return
    end
    type = type or 1
    local node = self.data:GetSelSuperNode()
    if TableIsEmpty(node) then
        return
    end
    local flag,item_id = self.data:GetSuperNodeUpFlag(node.star_type)
    if flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.StarMap.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
    elseif flag == -2 then
        PublicPopupCtrl.Instance:Error(self.tips)
    elseif flag == -3 then
        PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2],function()
            ShoppingCtrl.Instance:OpenStoreView(0)
        end)
    else
        self:OnUpEvent(type)
    end
end

function StarMapSuperActivePanel:OnUpEvent(type)
    if TableIsEmpty(self.move_effect_list) then
        return
    end
    local node = self.data:GetSelSuperNode()
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
                if type == 2 then
                    StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.AutoUpSupper, node.star_type)
                else
                    StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.LevelUpSuper, node.star_type)
                end
                self.EffectTool:Play("2161055")
            end)
        else
            pos_list[2] = pos_list[1] + Vector3.New(-100,200,0)
            move_effect:Play(pos_list)
        end
        
    end
end


-- ===================== 超星系技能节点节点激活 ===================
StarMapSuperSkillView = StarMapSuperSkillView or DeclareView("StarMapSuperSkillView", "star_map/star_map_super_skill_view")
VIEW_DECLARE_MASK(StarMapSuperSkillView,ViewMask.None)

function StarMapSuperSkillView:LoadCallback(param_t)
    if param_t.click_pos then
        self:HandlePos(param_t.click_pos)
    end
end

function StarMapSuperSkillView:HandlePos(pos)
    local x = pos.x
    if x > 0 then
        x = x - 300
    else
        x = x + 300
    end
    local new_pos = Vector3.New(x,0,0)
    UH.LocalPos(self.Rect,new_pos)
end


StarMapSuperSkillPanel = StarMapSuperSkillPanel or DeclareMono("StarMapSuperSkillPanel", UIWFlushPanel)
function StarMapSuperSkillPanel:StarMapSuperSkillPanel()
    self.data = StarMapData.Instance
    self.data_cares = {
        {data = self.data.super_star_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_super_node, func = self.FlushPanel, init_call = false},
    }
end

function StarMapSuperSkillPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function StarMapSuperSkillPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function StarMapSuperSkillPanel:FlushPanel()
    local node = self.data:GetSelSuperNode()
    if TableIsEmpty(node) then
        return
    end
    local star_id = node.star_type
    local first_cfg = self.data:GetSuperFirstCfg(star_id)
    if first_cfg.star_type == 0 then
        return
    end
    local level = self.data:GetSuperStarLevel(star_id)

    if self.last_level and self.last_star_id then
        if self.last_star_id == star_id and level > self.last_level then
            self.EffectTool:Play(7165475)
        end
    end
    self.last_level = level
    self.last_star_id = star_id

    local max_level = self.data:GetSuperMaxLevel(star_id)
    local is_active = level > 0
    local is_max = level >= max_level

    local cur_cfg = is_active and self.data:GetSuperLevelCfg(star_id, level) or first_cfg
    local next_cfg = is_max and cur_cfg or self.data:GetSuperLevelCfg(star_id, level + 1)

    self.ActiveFlag:SetActive(not is_active)
    self.NextSkill:SetActive(not is_max)
    self.Stuff:SetActive(not is_max)
    self.MaxFlag:SetActive(is_max)
    self.Btn:SetActive(not is_max)

    UH.SetInter(self.CellInter, is_active)

    UH.SetText(self.Level, string.format(Language.StarMap.Level1, level, max_level))
    UH.SetText(self.BtnText, is_active and Language.StarMap.Btn4 or Language.StarMap.Btn5)
    UH.SetText(self.TargetText, string.format(Language.StarMap.Text13, Language.StarMap.StarMapType[first_cfg.target]))
    local is_can_active = self.data:IsCanActiveSuper(star_id)
    UH.SetInter(self.BtnInter, is_can_active)

    local skill_id = cur_cfg.param1
    local skill_level = cur_cfg.param2
    local cur_skill_cfg = self.data:GetSkillCfg(skill_id, skill_level)
    self.Tips:SetActive(not is_can_active)
    self.SkillItem:SetData(cur_skill_cfg)

    UH.SetText(self.Name, cur_skill_cfg.skill_name)

    if not is_can_active then
        local tips_text = Language.StarMap.ActiveTip --string.format(Language.StarMap.Text1, node.condition)
        self.tips = tips_text
        UH.SetText(self.TipsText,tips_text)
    end

    if not is_max then
        local stuff_data = self.data:GetStuffDataByCfg(next_cfg)
        self.StuffList:SetData(stuff_data)
        if is_active then
            local next_skill_level = next_cfg.param2
            local skill_cfg = self.data:GetSkillCfg(skill_id, next_skill_level)
            UH.SetText(self.NextDesc, skill_cfg.description)
        end
    end

    if not is_active then
        UH.SetText(self.CurDesc, Language.StarMap.Text14)
        UH.SetText(self.NextDesc, cur_skill_cfg.description)
    else
        UH.SetText(self.CurDesc, cur_skill_cfg.description)
    end
end

function StarMapSuperSkillPanel:OnUpClick()
    local node = self.data:GetSelSuperNode()
    if TableIsEmpty(node) then
        return
    end
    local flag,item_id = self.data:GetSuperNodeUpFlag(node.star_type)
    if flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.StarMap.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
    elseif flag == -2 then
        PublicPopupCtrl.Instance:Error(self.tips)
    elseif flag == -3 then
        PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2],function()
            ShoppingCtrl.Instance:OpenStoreView(0)
        end)
    else
        StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.LevelUpSuper, node.star_type)
    end
end


StarMapSuperNodeAttri = StarMapSuperNodeAttri or DeclareMono("StarMapSuperNodeAttri", UIWidgetBaseItem)
function StarMapSuperNodeAttri:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if TableIsEmpty(data) then
        UH.SetText(self.Value, Language.StarMap.Text15)
        return
    end
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    local value_str = string.format("%s  +%s", attri_info.attr_name, attri_info.attr_percent_value)
    if data.target then
        value_str = Language.StarMap.StarMapType[data.target] .. value_str
    end
    UH.SetText(self.Value, value_str)
end