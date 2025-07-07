LingBaoStrUpView = LingBaoStrUpView or DeclareView("LingBaoStrUpView", "ling_bao/ling_bao_str_up")
VIEW_DECLARE_MASK(LingBaoStrUpView, ViewMask.BlockClose)
function LingBaoStrUpView:LingBaoStrUpView()
    self.Data = LingBaoData.Instance
end

function LingBaoStrUpView:LoadCallback(param_t)
end

function LingBaoStrUpView:CloseCallback()
    self.Data.ling_bao_flag:Notify()
end

----------------------------LingBaoStrUpPanel----------------------------
LingBaoStrUpPanel = LingBaoStrUpPanel or DeclareMono("LingBaoStrUpPanel", UIWFlushPanel)
function LingBaoStrUpPanel:LingBaoStrUpPanel()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:LingBaoRemindItemList(), BindTool.Bind(self.FlushAll, self))
    end

    self.ParabolaBoard:SetData({x_offset = -2, speed_time = 3, complete_func = function()
        self.ObjTargetEffect:SetActive(false)
        self.ObjTargetEffect:SetActive(true)
        self:SendUp()
    end})
end

function LingBaoStrUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.now_select_shenghen = self.Data:MainShengHenType()
    self.now_select_lingbao = self.Data:MainLingBaoType()
    self.equip_data = self.Data:EquipList(self.now_select_shenghen, self.now_select_lingbao)
    self:FlushAll()
end

function LingBaoStrUpPanel:FlushAll()
    if TableIsEmpty(self.equip_data.item) then
        return
    end
    self.LingBaoItem:SetData(self.equip_data.item)
    local item = LingBao.New(self.equip_data.item)
    local str_lv = self.Data:LingBaoStrLv(self.now_select_shenghen, self.now_select_lingbao)
    local str_max = self.Data:LingBaoMaxStrLv()
    local str_next = (str_lv + 1) <= str_max and (str_lv + 1) or str_max
    UH.SetText(self.TxtName, item:QuaName())
    UH.SetText(self.TxtStrLv, Format(self.language.LingBaoStrLv, str_lv))

    UH.SetText(self.TxtStrLvNow, Format(self.language.Lv, str_lv))
    UH.SetText(self.TxtStrLvNext, Format(self.language.Lv, str_next))
    self.ObjStrLvNext:SetActive(str_lv ~= str_max)

    local can_up, str, p1, p2, p3 = self.Data:LingBaoIsCanStr(self.now_select_shenghen, self.now_select_lingbao)
    if (not can_up) and p1 == 4 then
        UH.SetText(self.TxtTip, str)
        self.ObjTip:SetActive(true)
    else
        self.ObjTip:SetActive(false)
    end

    local now_str_cfg = self.Data:LingBaoLvAttrCfg(self.now_select_shenghen, self.now_select_lingbao, str_lv)
    local next_str_cfg = self.Data:LingBaoLvAttrCfg(self.now_select_shenghen, self.now_select_lingbao, str_lv + 1)
    local next_spend_cfg = self.Data:LingBaoStrengthSpendCfg(str_lv + 1)
    if (not TableIsEmpty(next_str_cfg)) and (not TableIsEmpty(next_spend_cfg)) then
        local has_num = self.Data:LingBaoStrItemNum()
        local need_num = next_spend_cfg.quintessence
        local color = has_num >= need_num and COLORSTR.Green4 or COLORSTR.Red8
        local num_str = Format(Language.Common.Cstr, color, has_num, need_num)
        local spend_item = Item.Init(self.Data:LingBaoStrItem(), num_str,true)
        self.Cell:SetData(spend_item)
        UH.SetText(self.TxtSpendName, spend_item:Name())
    end
    
    self.ObjMaxShow:SetActive(str_lv == str_max)
    
    self.OneRemind:SetNum(can_up and 1 or 0)
    self.AllRemind:SetNum(can_up and 1 or 0)

    local play_effect = false
    if not self.now_str_lv then
        self.now_str_lv = str_lv
    else
        if self.now_str_lv ~= str_lv then
            play_effect = true
            self.now_str_lv = str_lv
        end
    end
    self.List:SetData(self:GetShowData(now_str_cfg, next_str_cfg, play_effect))
end

function LingBaoStrUpPanel:GetShowData(now_str_cfg, next_str_cfg, play_effect)
    local list = {}
    local item_config = Item.GetConfig(self.equip_data.item.item_id)
    local base_attr = self.Data:GetAttrList(item_config)
    local now_str_attr = self.Data:GetAttrList(now_str_cfg)
    local next_str_attr = self.Data:GetAttrList(next_str_cfg)
    local is_max = TableIsEmpty(next_str_attr)
    if TableIsEmpty(now_str_attr) then
        -- 0级
        now_str_attr = self.Data:GetAttrList(item_config, true)
    end
    if TableIsEmpty(next_str_attr) then
        -- 满级
        next_str_attr = self.Data:GetAttrList(item_config, true)
    end
    for i, v in ipairs(base_attr) do
        local t = {}
        t.now = CommonStruct.AttrStruct(v.attr_type, v.attr_value + now_str_attr[i].attr_value)
        t.next = CommonStruct.AttrStruct(v.attr_type, v.attr_value + next_str_attr[i].attr_value)
        t.is_max = is_max
        t.play_effect = play_effect
        table.insert(list, t)
    end
    return list
end

function LingBaoStrUpPanel:OnUpClick(num)
    if self.now_num then
        return
    end
    local can_up, str, p1, p2, p3 = self.Data:LingBaoIsCanStr(self.now_select_shenghen, self.now_select_lingbao)
    if can_up then
        self.now_num = num
        self.StartPos.transform.position = self.CellPos.transform.position
        self.TargetPos.transform.position = self.ItemPos.transform.position
        self.ParabolaBoard:Play()
    else
        PublicPopupCtrl.Instance:Center(str)
        if p1 == 5 and p2 and p3 then
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = p2, jump_need_num = p3})})
        end
    end
end

function LingBaoStrUpPanel:SendUp()
    if self.now_num == 1 then
        LingBaoCtrl.Instance:SendReq(LingBaoReqType.LingBaoStrUp, self.now_select_shenghen, self.now_select_lingbao)
    else
        LingBaoCtrl.Instance:SendReq(LingBaoReqType.LingBaoStrUpAll2, self.now_select_shenghen, self.now_select_lingbao)
    end
    self.now_num = nil
end

function LingBaoStrUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

--------------LingBaoStrUpItem--------------
LingBaoStrUpItem = LingBaoStrUpItem or DeclareMono("LingBaoStrUpItem", UIWidgetBaseItem)
function LingBaoStrUpItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.now.attr_name)
    UH.SetText(self.TxtNowValue, data.now.attr_percent_value)
    self.ObjNext:SetActive(not data.is_max)
    UH.SetText(self.TxtNextValue, data.next.attr_percent_value)
    if data.play_effect then
        self:PlayEffect()
    end
end

function LingBaoStrUpItem:PlayEffect()
    if self.handle then
        UH.StopEffect(self.EffectTool, self.handle)
    end
    self.handle = UH.PlayEffect(self.EffectTool, "3165223")
end