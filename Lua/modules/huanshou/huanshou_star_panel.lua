-- 幻兽天赋
HuanShouStarPanel = HuanShouStarPanel or DeclareMono("HuanShouStarPanel", UIWFlushPanel)
function HuanShouStarPanel:HuanShouStarPanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false},
        --{data = self.data.bag_item_list, func = self.FlushStuff, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false}
    }
    self.line_list_cfg_1 = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8}
    }
    self.line_list_cfg_2 = {
        {9,10},{10,11},{11,12},{12,13},{13,14},{14,15},{15,16},{16,17}
    }
end

function HuanShouStarPanel:GetLineListData(cfg)
    local list = {}
    for i,v in pairs(cfg) do
        local data = {}
        data.s_node_id = v[1]
        data.e_node_id = v[2]
        local s_mono = self:GetMono(v[1])
        local e_mono = self:GetMono(v[2])
        local s_pos = s_mono.gameObject:GetLocalPosition()
        local e_pos = e_mono.gameObject:GetLocalPosition()
        data.s_pos = s_pos
        data.len = Vector2.Distance(s_pos, e_pos)
        local dir_pos = e_pos - s_pos  
        data.rot = 90 - self:GetAngle(dir_pos)
        table.insert(list, data)
    end
    return list
end


function HuanShouStarPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.line_list_data_1 = self:GetLineListData(self.line_list_cfg_1)
    self.line_list_data_2 = self:GetLineListData(self.line_list_cfg_2)
    self:FlushPanel()
    self:FlushStuff()
end

function HuanShouStarPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function HuanShouStarPanel:FlushPanel()
    local huanshou = self.data:GetSelHuanShou()
    if TableIsEmpty(huanshou) then
        return
    end
    local list_data = Config.huanshou_auto.potential_point
    for i,v in pairs(list_data) do
        local mono = self:GetMono(v.potential_point)
        local star_node = HuanShouStar.New(v.potential_point, huanshou)
        mono:SetData(star_node)
    end
    self.LineList1:SetData(self.line_list_data_1)
    self.LineList2:SetData(self.line_list_data_2)
end

function HuanShouStarPanel:GetAngle(dir_pos)
    local vector = dir_pos.normalized
    local x = vector.x
    local y = vector.y
    local num = math.atan2(x,y)         -- 弧度
    local angle = num * 180 / math.pi   -- 角度 = 弧度 * 360 / 2PI 
    return angle
end

function HuanShouStarPanel:FlushStuff()
    local item_id = Config.huanshou_auto.other[1].potential_item
    local num = Item.GetNum(item_id)
    UH.SetIcon(self.StuffIcon, Item.GetIconId(item_id),ICON_TYPE.ITEM)
    UH.SetText(self.StuffText, string.format(Language.HuanShou.Text11, num))
end

-- 0下标开始
function HuanShouStarPanel:GetMono(index)
    return self.NodeList[index + 1]
end

function HuanShouStarPanel:OnStuffIconClick()
    local item_id = Config.huanshou_auto.other[1].potential_item
    ItemInfoCtrl.Instance:ShowItemInfo(item_id)
end

function HuanShouStarPanel:OnOpenRemineClick()
    ViewMgr:OpenView(HuanShouStarRefine)
end




HuanShouStarNode = HuanShouStarNode or DeclareMono("HuanShouStarNode", UIWidgetBaseItem)

function HuanShouStarNode:OnEnable()
    self.is_awake = true
    self:FlushEffect()
end

function HuanShouStarNode:FlushEffect()
    if not self.is_awake then
        return
    end
    if not self.data then
        return
    end
    if self.IsNotEffect then
        return
    end

    -- 只是节点显示激活特效
    local _type = self.data:Type()
    if _type ~= 0 then
        return
    end
    local level = self.data:Level()
    local is_can_active = self.data:IsCanActive()
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

-- data == HuanShouStar
function HuanShouStarNode:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local _type = data:Type()
    self.Star:SetActive(_type == 0)
    self.Skill:SetActive(_type == 1)
    if _type == 0 then
        self:FlushStar()
        self:FlushEffect()
    elseif _type == 1 then
        self:FlushSkill()
    end
    if self.ShowRemind then
        local remind = HuanShouData.Instance:GetStarUpFlag(data)
        self.RedPoint:SetNum(remind)
    else
        self.RedPoint:SetNum(0)
    end
end

function HuanShouStarNode:FlushStar()
    local node = self.data
    UH.SpriteName(self.Icon, node:AttriIcon())
    local is_can_active = node:IsCanActive()
    local level = node:Level()
    local bg_sprite = "QianNengSuoDing"
    if level > 0 then
        bg_sprite = "QianNengYiJiHuo"
    elseif is_can_active then
        bg_sprite = "QianNengKeJiHuo"
    end
    --self.Lock:SetActive(not is_can_active)
    UH.SpriteName(self.StarBGSprite, bg_sprite)

    local level_str = level > 0 and "Lv." .. level or ""
    UH.SetText(self.Level, level_str)
end

function HuanShouStarNode:FlushSkill()
    local node = self.data
    local skill = node:Skill()
    local is_can_active = node:IsCanActive()
    UH.SetIcon(self.SkillIcon, skill:Icon(), ICON_TYPE.SKILL)
    self.SkillLock:SetActive(not node:IsActive() and not is_can_active)
    UH.SetInter(self.SkillInter, node:IsActive() or is_can_active)

    local level = node:Level()
    local level_str = node:IsActive() and "Lv." .. level or ""
    UH.SetText(self.Level, level_str)
    if self.SkillUnlockEffect then
        
        self.SkillUnlockEffect:SetActive(level == 0 and is_can_active)
    end
end

function HuanShouStarNode:OnClick()
    if not self.IsCanClick then
        return
    end
    local node = self.data
    local pos = UH.GetRootPosition(self.gameObject)
    if node:Type() == 0 then
        HuanShouStarNodeView.StarNodeId = node.id
        ViewMgr:OpenView(HuanShouStarNodeView, {click_pos = pos, toggle = self.Selector})
    else
        HuanShouStarSkillView.StarNodeId = node.id
        ViewMgr:OpenView(HuanShouStarSkillView, {click_pos = pos, toggle = self.Selector})
    end
end

function HuanShouStarNode:OnNodeSelClick(value)
    if not self.IsCanClick then
        return
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



HuanShouStarNodeLine = HuanShouStarNodeLine or DeclareMono("HuanShouStarNodeLine", UIWidgetBaseItem)
function HuanShouStarNodeLine:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject:SetLocalPosition(data.s_pos)
    self.Rect:SetSizeWithCurrentAnchors(0,data.len)
    UH.LocalRotate(self.Rect,Quaternion.Euler(0,0,data.rot))
    local huanshou = HuanShouData.Instance.sel_huanshou
    if TableIsEmpty(huanshou) then
        return
    end
    local s_node = HuanShouStar.New(data.s_node_id, huanshou)
    local e_node = HuanShouStar.New(data.e_node_id, huanshou)
    local img = s_node:IsCanActive() and e_node:IsCanActive()  and "XianYiJiHuo" or "XianWeiJiHuo"
    UH.SpriteName(self.Img, img)
end




-- ===================== 属性节点激活 ===================
HuanShouStarNodeView = HuanShouStarNodeView or DeclareView("HuanShouStarNodeView", "huanshou/huanshou_star_node_view")
HuanShouStarNodeView.StarNodeId = nil
VIEW_DECLARE_MASK(HuanShouStarNodeView,ViewMask.BlockClose)

function HuanShouStarNodeView:LoadCallback(param_t)
    if param_t.click_pos then
        self:HandlePos(param_t.click_pos)
    end
    self.toggle = param_t.toggle
end

function HuanShouStarNodeView:CloseCallback()
    HuanShouStarNodeView.StarNodeId = nil
    if self.toggle then
        self.toggle.isOn = false
    end
end

function HuanShouStarNodeView:HandlePos(pos)
    local x = pos.x
    if x > 0 then
        x = x - 300
    else
        x = x + 300
    end
    local new_pos = Vector3.New(x,0,0)
    UH.LocalPos(self.Rect,new_pos)
end


HuanShouStarNodePanel = HuanShouStarNodePanel or DeclareMono("HuanShouStarNodePanel", UIWFlushPanel)
function HuanShouStarNodePanel:HuanShouStarNodePanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false},
        --{data = self.data.bag_item_list, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }
end
function HuanShouStarNodePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()

    self.move_effect_list = {}
    for i = 1,1 do--self.EffectList:Length() do
        local effect = self.EffectList[i]
        if effect then
            self.move_effect_list[i] = ShengQiMoveEffect.New()
            self.move_effect_list[i]:SetEffectObj(effect)
            self.move_effect_list[i]:SetStartPos(effect:GetLocalPosition())
        end
    end
end

function HuanShouStarNodePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for i,v in pairs(self.move_effect_list or {}) do
        v:Destroy()
    end
end

-- node == Config.starmap_auto.superstar_condition
function HuanShouStarNodePanel:FlushPanel()
    if not HuanShouStarNodeView.StarNodeId then
        return
    end
    local huanshou = self.data:GetSelHuanShou()
    if TableIsEmpty(huanshou) then
        return
    end
    local node = HuanShouStar.New(HuanShouStarNodeView.StarNodeId, huanshou)
    self.node = node
    if node:Type() == 1 then
        return
    end
    self.StarNode:SetData(node)
    --UH.SetText(self.Name, node:Name())
    local level = node:Level()
    local max_level = node:MaxLevel()
    local is_active = node:IsActive()
    local is_can_active = node:IsCanActive()
    local is_max = node:IsMax()
    UH.SetText(self.Level, string.format(Language.HuanShou.Level1, level, max_level))
    --self.Target:SetActive(true)

    self.ActiveFlag:SetActive(not is_active)
    self.Tips:SetActive(not is_can_active)
    self.ActiveBtn:SetActive(not is_active and not is_max)
    self.UpBtn:SetActive(is_active and not is_max)
    --self.AutoBtn:SetActive(is_active and not is_max)
    self.MaxFlag:SetActive(is_max)
    self.Stuff:SetActive(not is_max)

    local cur_cfg = node:LevelCfg()
    local next_cfg = node:NextLevelCfg()
    local attri = node:Attri()
    self.NextAttri:SetObjActive(not is_max)

    if not is_max then
        local stuff_data = node:StuffList()
        self.StuffList:SetData(stuff_data)
        if is_active then
            local next_attri = node:NextAttri()
            self.NextAttri:SetData(next_attri)
        end
    end
    if not is_active then
        self.CurAttri:SetData({})
        self.NextAttri:SetData(attri)
    else
        self.CurAttri:SetData(attri)
    end

    UH.SetInter(self.Inter, is_can_active)
    if not is_can_active then
        local cfg = node:BaseCfg()
        local tips_text = not node.huanshou:IsActive() and Language.HuanShou.Text12 or string.format(Language.HuanShou.Text13, cfg.condition)
        self.tips = tips_text
        UH.SetText(self.TipsText,tips_text)
    end
end

function HuanShouStarNodePanel:OnUpClick(type)
    if self.effect_ing then
        return
    end
    type = type or 1
    local node = self.node
    if TableIsEmpty(node) then
        return
    end
    local flag,item_id = HuanShouData.Instance:GetStarUpFlag(node)
    if flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
    elseif flag == -2 then
        PublicPopupCtrl.Instance:Error(self.tips)
    else
        self:OnUpEvent(type)
    end
end

function HuanShouStarNodePanel:OnUpEvent(type)
    if TableIsEmpty(self.move_effect_list) then
        return
    end
    local node = self.node
    if TableIsEmpty(node) then
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.FabaoLianZhiGuangQiu)
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
                    HuanShouCtrl.Instance:SendReq(HuanShouReqType.UpStar, self.node.huanshou.id, self.node.id)
                end
                self.EffectTool:Play("2161055")
            end, 10)
        else
            pos_list[2] = pos_list[1] + Vector3.New(-100,200,0)
            move_effect:Play(pos_list)
        end
    end
end



-- ===================== 技能节点激活 ===================
HuanShouStarSkillView = HuanShouStarSkillView or DeclareView("HuanShouStarSkillView", "huanshou/huanshou_star_skill_view")
HuanShouStarSkillView.StarNodeId = nil
VIEW_DECLARE_MASK(HuanShouStarSkillView,ViewMask.BlockClose)

function HuanShouStarSkillView:LoadCallback(param_t)
    if param_t.click_pos then
        self:HandlePos(param_t.click_pos)
    end
    self.toggle = param_t.toggle
end

function HuanShouStarSkillView:CloseCallback()
    HuanShouStarSkillView.StarNodeId = nil
    if self.toggle then
        self.toggle.isOn = false
    end
end

function HuanShouStarSkillView:HandlePos(pos)
    local x = pos.x
    if x > 0 then
        x = x - 300
    else
        x = x + 300
    end
    local new_pos = Vector3.New(x,0,0)
    UH.LocalPos(self.Rect,new_pos)
end


HuanShouStarSkillPanel = HuanShouStarSkillPanel or DeclareMono("HuanShouStarSkillPanel", UIWFlushPanel)
function HuanShouStarSkillPanel:HuanShouStarSkillPanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false},
        --{data = self.data.bag_item_list, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false}
    }
end

function HuanShouStarSkillPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function HuanShouStarSkillPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function HuanShouStarSkillPanel:FlushPanel()
    if not HuanShouStarSkillView.StarNodeId then
        return
    end
    local huanshou = self.data:GetSelHuanShou()
    if TableIsEmpty(huanshou) then
        return
    end
    local node = HuanShouStar.New(HuanShouStarSkillView.StarNodeId, huanshou)
    self.node = node
    if node:Type() == 0 then
        return
    end
    self.StarNode:SetData(node)

    local level = node:Level()

    if self.last_level and self.last_star_id then
        if self.last_star_id == node.id and level > self.last_level then
            self.EffectTool:Play(7165475)
        end
    end
    self.last_level = level
    self.last_star_id = node.id

    local max_level = node:MaxLevel()
    local is_active = node:IsActive()
    local is_max = node:IsMax()
    local cur_cfg = node:LevelCfg()
    local next_cfg = node:NextLevelCfg()
    local skill = node:Skill()
    local next_skill = node:NextSkill()
    local is_can_active = node:IsCanActive()

    self.ActiveFlag:SetActive(not is_active)
    self.NextSkill:SetActive(not is_max)
    self.Stuff:SetActive(not is_max)
    self.MaxFlag:SetActive(is_max)
    self.Btn:SetActive(not is_max)
    self.Tips:SetActive(not is_can_active)

    UH.SetText(self.Level, string.format(Language.HuanShou.Level1, level, max_level))
    UH.SetText(self.BtnText, is_active and Language.HuanShou.Text14 or Language.HuanShou.Text15)
    UH.SetInter(self.BtnInter, is_can_active)
    UH.SetText(self.Name, skill:Name())

    if not is_can_active then
        local cfg = node:BaseCfg()
        local tips_text = not node.huanshou:IsActive() and Language.HuanShou.Text12 or string.format(Language.HuanShou.Text13, cfg.condition)
        self.tips = tips_text
        UH.SetText(self.TipsText,tips_text)
    end

    if not is_max then
        local stuff_data = node:StuffList()
        self.StuffList:SetData(stuff_data)
        if is_active then
            UH.SetText(self.NextDesc, next_skill:Desc())
        end
    end

    if not is_active then
        UH.SetText(self.CurDesc, Language.HuanShou.NotEffect)
        UH.SetText(self.NextDesc, skill:Desc())
    else
        UH.SetText(self.CurDesc, skill:Desc())
    end
end

function HuanShouStarSkillPanel:OnUpClick()
    local node = self.node
    if TableIsEmpty(node) then
        return
    end

    local flag,item_id = HuanShouData.Instance:GetStarUpFlag(node)
    if flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
    elseif flag == -2 then
        PublicPopupCtrl.Instance:Error(self.tips)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.FabaoLianZhiGuangQiu)
        HuanShouCtrl.Instance:SendReq(HuanShouReqType.UpStar, node.huanshou.id, node.id)
    end
end




-- ================= 炼丹炉 ====================
HuanShouStarRefine = HuanShouStarRefine or DeclareView("HuanShouStarRefine", "huanshou/huanshou_star_refine", Mod.HuanShou.RefineStar)
VIEW_DECLARE_MASK(HuanShouStarRefine,ViewMask.BgBlockClose)
function HuanShouStarRefine:OnCloseClick()
    ViewMgr:CloseView(HuanShouStarRefine)
end


HuanShouStarRefinePanel = HuanShouStarRefinePanel or DeclareMono("HuanShouStarRefinePanel", UIWFlushPanel)
function HuanShouStarRefinePanel:HuanShouStarRefinePanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = self.data.other_data, func = self.FlushPanel, init_call = false},
    }
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
    self.item_count = 5
    self.curr_type = CurrencyType.ImmortalCoin
    self.price = Config.huanshou_auto.other[1].exchange_price
end

function HuanShouStarRefinePanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetGold(self.Icon, self.curr_type)
    UH.SetText(self.Fee, self.price)
    UH.SetText(self.Tips, Config.language_cfg_auto.textdesc[363].desc)
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(Config.huanshou_auto.other[1].luzi_res))
    self.effect_func = ShengQiMoveEffect.New()
    self.effect_func:SetStartPos(self.SEffect:GetLocalPosition())
    self:FlushPanel()
end

function HuanShouStarRefinePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    self.effect_func:Destroy()
end

function HuanShouStarRefinePanel:FlushPanel()
    local t_index = self.data.other_data.last_lian_dan_count or 0
    for i = 1, 5 do
        local state = 3
        if i <= t_index then
            if i == 1 then
                state = 1
            else
                state = 2
            end
        end
        self.List[i]:SetState(state, i)
    end
end

function HuanShouStarRefinePanel:OnRefineClick()
    if self.effect_ing then
        return
    end
    local remain_count = self.item_count - (self.data.other_data.last_lian_dan_count or 0)
    if remain_count < 1 then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.Tip7)
        return
    end
    local price = self.price
    if MallCtrl.IsNotCurrency(self.curr_type, price) then
        self.effect_ing = true
        local pos_list = {}
        local s_pos = self.effect_func:GetStartPos()
        pos_list[1] = s_pos
        local t_index = self.data.other_data.last_lian_dan_count or 0
        local mono = self.List[t_index + 1]
        pos_list[3] = mono.gameObject:GetLocalPosition() + Vector3(100,0,0)
        if t_index ~= 0 then
            --pos_list[2] = pos_list[1] + Vector3.New(100,200,0)
            pos_list[2] = pos_list[3] + Vector3.New(200,100,0)
        else
            --pos_list[2] = pos_list[1] + Vector3.New(-100,200,0)
            pos_list[2] = pos_list[3]+ Vector3.New(-200,100,0)
        end
        self.effect_func:Play(pos_list, self.SEffect, function ()
            self.effect_ing = false
            HuanShouCtrl.Instance:SendReq(HuanShouReqType.LianDan)
        end,15)
    end
end



HuanShouStarRefineItem = HuanShouStarRefineItem or DeclareMono("HuanShouStarRefineItem")

function HuanShouStarRefineItem:HuanShouStarRefineItem()
    self.effect_cfg = {
        [1] = 7165490,
        [2] = 7165486,
        [3] = 7165487,
        [4] = 7165488,
        [5] = 7165489,
    }
end

-- 1炼制中， 2 等待中， 3空闲中
function HuanShouStarRefineItem:SetState(state, index)
    self.index = index or 1
    self.None:SetActive(state == 3)
    self.ItemCell:SetObjActive(state ~= 3)
    self.Desc:SetObjActive(state ~= 1)
    self.Time:SetObjActive(state == 1)
    if state == 1 then
        local item_id = Config.huanshou_auto.other[1].potential_item
        local item = Item.Create({item_id = item_id})
        self.ItemCell:SetData(item)
        UH.SetText(self.State, ColorStr(Language.HuanShou.Text16, COLORSTR.Red10))
        local timestamp = HuanShouData.Instance.other_data.start_lian_dan_timestamp or 0
        timestamp = timestamp + Config.huanshou_auto.other[1].wait_time
        self.Time:StampTime(timestamp, TimeType.Type_Time_8, Language.HuanShou.Text17)
    elseif state == 2 then
        local item_id = Config.huanshou_auto.other[1].potential_item
        local item = Item.Create({item_id = item_id})
        self.ItemCell:SetData(item)
        UH.SetText(self.State, Language.HuanShou.Text18)
        UH.SetText(self.Desc, string.format(Language.HuanShou.Text19, item:Name()))
    elseif state == 3 then
        UH.SetText(self.State, "")
        UH.SetText(self.Desc, "")
    end
    if self.last_state and state ~= self.last_state then
        local effect = self.effect_cfg[self.index]
        self.EffectTool:Play(effect)
    end
    self.last_state = state
end