IllusionLevelUpView = IllusionLevelUpView or DeclareView("IllusionLevelUpView", IllusionConfig.ResPath .. "illusion_level_up")
VIEW_DECLARE_MASK(IllusionLevelUpView, ViewMask.BlockClose)
function IllusionLevelUpView:IllusionLevelUpView()
    self.Data = IllusionData.Instance
end

function IllusionLevelUpView:LoadCallback(param_t)
end

function IllusionLevelUpView:CloseCallback()
end

----------------------------IllusionLevelUpPanel----------------------------
IllusionLevelUpPanel = IllusionLevelUpPanel or DeclareMono("IllusionLevelUpPanel", UIWFlushPanel)
function IllusionLevelUpPanel:IllusionLevelUpPanel()
    self.Data = IllusionData.Instance
    self.language = Language.IllusionUp
    self.data = self.Data.illusion_lv_info
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:RemindItemList(), BindTool.Bind(self.FlushAll, self))
    end

    self.ParabolaBoard:SetData({x_offset = -2, speed_time = 1, complete_func = function()
        -- self.ObjTargetEffect:SetActive(false)
        -- self.ObjTargetEffect:SetActive(true)
        if self.effect_ht then
            self.EffectTool:Stop(self.effect_ht)
        end
        self.effect_ht = self.EffectTool:Play(self.star_effect_id)
        self:SendUp()
    end})
end

function IllusionLevelUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.id = self.Data.lv_need_info.id
    self.pos = self.Data.lv_need_info.pos
    self.star_effect_id = 1
    -- 每个星星的间隔
    self.add_y = -22
    self:FlushAll()
end

function IllusionLevelUpPanel:FlushAll()
    self.ObjBlock:SetActive(false)
    self.lv = self.Data:IllusionLv(self.id)
    if self.lv == 0 then
        ViewMgr:CloseView(IllusionLevelUpView)
        return
    end
    self.cfg = Cfg.SurfaceInfoById(self.id)
    self.lv_cfg = self.Data:GetLvUpCfg(self.id)
    if TableIsEmpty(self.cfg) or TableIsEmpty(self.lv_cfg) then
        ViewMgr:CloseView(IllusionLevelUpView)
        return
    end
    local item = Item.Init(self.cfg.active_stuff_id, 1, 1)
    self.TopCell:SetData(item)
    self.StarList:SetData(self.Data:IllusionLvStarData(self.lv))

    
    local str_max = self.Data:IllusionMaxLv()
    local str_next = (self.lv + 1) <= str_max and (self.lv + 1) or str_max
    UH.SetText(self.TxtName, item:QuaName())

    UH.SetText(self.TxtStrLvNow, Format(self.language.Lv, self.lv))
    UH.SetText(self.TxtStrLvNext, Format(self.language.Lv, str_next))
    self.ObjStrLvNext:SetActive(self.lv ~= str_max)
    

    local can_up, str, p1, p2, p3 = self.Data:IsCanLvUp(self.id)
    self.ObjTip:SetActive(false)

    local spend_cfg = self.Data:GetLvUpCfg(self.id)
    if not TableIsEmpty(spend_cfg) then
        local spend_item = spend_cfg.strengthen_consume_item[0]
        local has_num = Item.GetNum(spend_item.item_id)
        local need_num = spend_item.num
        local color = has_num >= need_num and COLORSTR.Green4 or COLORSTR.Red8
        local num_str = Format(Language.Common.Cstr, color, has_num, need_num)
        local spend_item_data = Item.Init(spend_item.item_id, num_str, true)
        self.Cell:SetData(spend_item_data)
    end
    
    self.ObjMaxShow:SetActive(self.lv == str_max)
    self.Remind:SetNum(can_up and 1 or 0)

    self.List:SetData(self:GetShowData())
end

function IllusionLevelUpPanel:ListPlayEffect()
    for k, v in pairs(self.List.item_list) do
        v:PlayEffect()
    end
end

function IllusionLevelUpPanel:GetShowData()
    local list = {}
    local base_attr = self.Data:GetLvUpAttr(self.id, nil, self.Data:IllusionMaxLv(), true)
    local now_str_attr = self.Data:GetLvUpAttr(self.id)
    local next_str_attr = self.Data:GetLvUpAttr(self.id, nil, self.lv + 1)
    local is_max = TableIsEmpty(next_str_attr)
    if TableIsEmpty(now_str_attr) then
        -- 0级
        now_str_attr = base_attr
    end
    if TableIsEmpty(next_str_attr) then
        -- 满级
        next_str_attr = base_attr
    end
    for i, v in ipairs(base_attr) do
        local t = {}
        t.now = CommonStruct.AttrAdd(v.attr_type, v.attr_value, now_str_attr[i].attr_value)
        t.next = CommonStruct.AttrAdd(v.attr_type, v.attr_value, next_str_attr[i].attr_value)
        t.is_max = is_max
        table.insert(list, t)
    end
    return list
end

function IllusionLevelUpPanel:OnUpClick()
    local can_up, str, p1, p2, p3 = self.Data:IsCanLvUp(self.id)
    if can_up then
        self.StartPos.transform.position = self.CellPos.transform.position
        self.TargetPos.transform.position = self.pos
        local star_effect, t2 = math.floor((self.lv + 1) / 5) + 1, (self.lv + 1) % 5
        star_effect = t2 == 0 and star_effect -1 or star_effect
        self.star_effect_id = IllusionConfig.IllusionUpStarEffect[star_effect]
        -- 计算下一颗星星位置
        local l_pos = self.TargetPos.transform.localPosition
        local next_star = (self.lv + 1) % 5
        next_star = next_star == 0 and 5 or next_star
        local dis = -9 + (self.add_y / 2) + (next_star - 1) * self.add_y
        self.TargetPos.transform.localPosition = Vector3(l_pos.x, l_pos.y + dis, l_pos.z)
        self.ObjTargetEffect.transform.position = self.TargetPos.transform.position
        if self.effect_ht then
            self.EffectTool:Stop(self.effect_ht)
        end
        self.ParabolaBoard:Play()
        self:ListPlayEffect()
        self.ObjBlock:SetActive(true)
    else
        if str then
            PublicPopupCtrl.Instance:Center(str)
        end
        if p1 == 2 and p2 then
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = p2})})
        end
    end
end

function IllusionLevelUpPanel:SendUp()
    IllusionCtrl.Instance:SendIllusionUp(IllusionConfig.IllusionUpReqType.LvUp, self.id)
end

function IllusionLevelUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

--------------IllusionLevelUpItem--------------
IllusionLevelUpItem = IllusionLevelUpItem or DeclareMono("IllusionLevelUpItem", UIWidgetBaseItem)
function IllusionLevelUpItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.now.attr_name)
    UH.SetText(self.TxtNowValue, data.now.attr_percent_value)
    self.ObjNext:SetActive(not data.is_max)
    UH.SetText(self.TxtNextValue, data.next.attr_percent_value)
end

function IllusionLevelUpItem:PlayEffect()
    if self.handle then
        UH.StopEffect(self.EffectTool, self.handle)
    end
    self.handle = UH.PlayEffect(self.EffectTool, "7165454")
end