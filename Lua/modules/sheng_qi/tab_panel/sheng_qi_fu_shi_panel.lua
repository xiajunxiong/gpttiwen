ShengQiFuShiPanel = ShengQiFuShiPanel or DeclareMono("ShengQiFuShiPanel", UIWFlushPanel)
function ShengQiFuShiPanel:ShengQiFuShiPanel()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sel_sheng_qi, func = self.FlushPanel, init_call = true},
    }
    self.show_att_type = 2  -- 1 显示符石属性，2显示角色属性
end

function ShengQiFuShiPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function ShengQiFuShiPanel:FlushPanel()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    self.ShengQiCell:SetData(sheng_qi)
    for i = 1, self.FuShiList:Length() do
        local fushi = sheng_qi:GetFuShi(i - 1)
        self.FuShiList[i]:SetData(fushi or {})
    end

    self.AttriShow:SetActive(sheng_qi:IsHasFushi() or self.show_att_type == 1)
    self.Tip:SetObjActive(not sheng_qi:IsHasFushi() and self.show_att_type ~= 1)
    UH.SetText(self.AttriTitle, self.show_att_type == 1 and Language.ShengQi.Text19 or Language.ShengQi.Text20)
    if sheng_qi:IsHasFushi() or self.show_att_type == 1 then
        if self.show_att_type == 1 then
            local fushi_attri_list = sheng_qi:AllFuShiAttriList()
            local show_attri_list = {}
            for i,v in pairs(fushi_attri_list) do
                table.insert(show_attri_list, ShengQiData.Instance:FormatAttriInfo(v.attri_type, v.attri_value))
            end
            self.AttriList:SetData(show_attri_list)
            self.RoleAttriList:SetData({})
        else
            self.AttriList:SetData({})
            local role_attri_list = sheng_qi:AllFuShiRoleAttriList(true)
            self.RoleAttriList:SetData(role_attri_list)
        end
    else
        self.AttriList:SetData({})
        self.RoleAttriList:SetData({})
    end  

end

function ShengQiFuShiPanel:OnOpenBookClick()
    ViewMgr:OpenView(FuShiBookView)
end

function ShengQiFuShiPanel:OnSwitchClick()
    self.show_att_type = self.show_att_type == 1 and 2 or 1
    self:FlushPanel()
end

function ShengQiFuShiPanel:OnDetailAttriClick()
    local sheng_qi = self.data:GetSelShengQi()
    ViewMgr:OpenView(FuShiAttriPreview, {sheng_qi = sheng_qi})
end

-- >>>>> 符石 fu_shi_item
FuShiItem = FuShiItem or DeclareMono("FuShiItem", UIWidgetBaseItem)
function FuShiItem:FuShiItem()
    self.effect_cfg = {
        [0] = {
            [2] = 3165188,
            [3] = 3161005,
            [4] = 3161010,
            [5] = 3161015,
            [6] = 3165193,
        },
        [1] = {
            [2] = 3165189,
            [3] = 3161001,
            [4] = 3161006,
            [5] = 3161011,
            [6] = 3165194,
        },
        [2] = {
            [2] = 3165190,
            [3] = 3165177,
            [4] = 3165178,
            [5] = 3165179,
            [6] = 3165195,
        },
        [3] = {
            [2] = 3165191,
            [3] = 3165180,
            [4] = 3165181,
            [5] = 3165182,
            [6] = 3165196,
        },
        [4] = {
            [2] = 3165192,
            [3] = 3161004,
            [4] = 3161009,
            [5] = 3161014,
            [6] = 3165197,
        }
    }
end

function FuShiItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_ht)
end

function FuShiItem:SetData(data) -- data ==== Fushi
    UIWidgetBaseItem.SetData(self, data)

    if self.effect_ht then
        self.EffectTool:Stop(self.effect_ht)
        self.effect_ht = nil
    end

    local item_id = data.item_id or 0
    self.Show:SetActive(item_id > 0)
    if item_id > 0 then
        UH.SetIcon(self.Icon, data:IconId(), ICON_TYPE.ITEM)
        UH.SetText(self.ItemName, data:Name())
        local qua = Item.GetColor(item_id)
        local effect_cfg = self.effect_cfg[self.HoleIndex][qua]
        if effect_cfg then
            TimeHelper:CancelTimer(self.time_ht)
            self.time_ht = Invoke(function ()
                self.effect_ht = self.EffectTool:Play(effect_cfg)
            end,0.1)
        end
    end


    local sheng_qi = data:ShengQi()
    if sheng_qi and self.HoleIndex >= 0 then
        self.Lock:SetActive(true)
        local flag,lock_level = FuShiData.Instance:FuShiCheckLock(sheng_qi, self.HoleIndex)
        self.LockImg:SetActive(flag >= 0)
        self.LockInfo:SetActive(flag == 0)
        self.UnlockInfo:SetActive(flag == 1)
        if flag == 0 then
            UH.SetText(self.LockTip, string.format(Language.ShengQi.Tip9,lock_level))
        end
    else
        self.LockInfo:SetActive(false)
        self.UnlockInfo:SetActive(false)
        self.Lock:SetActive(false)
    end

end

function FuShiItem:UnlockEffect()
    self.EffectTool:Play("3165167")
end

function FuShiItem:OnIconClick()

    local func_list = {}
    table.insert(func_list,ItemInfoFunc.FuShiUnfix)
    if self.data:Level() > 1 then
        table.insert(func_list,ItemInfoFunc.FuShiReset)
    end
    if not self.data:IsMax() then
        table.insert(func_list,ItemInfoFunc.FuShiUpLevel)
    end

    local info = {
        item_list = {self.data},
		func = function ()
			return func_list
		end
		}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function FuShiItem:OnItemUnlockClick()
    local hole_index = self.HoleIndex
    local sheng_qi = ShengQiData.Instance:GetSelShengQi()
    local cfg = Cfg.FuShiHoleCfg(hole_index)
    if TableIsEmpty(cfg) then
        return
    end
    local item_id = cfg.unlock_cost
    local has_num = Item.GetNum(item_id)
    local item = Item.Create({item_id = item_id, num = DataHelper.ConsumeNum(has_num, cfg.num,true)})
    local item_flush_func = function (item)
        local num = Item.GetNum(item_id)
        num = DataHelper.ConsumeNum(num,cfg.num,true)
        item.num = num
    end
    local param = {
        show_toggle = false,
        context = string.format(Language.ShengQi.Tip10, cfg.num, Item.GetQuaName(item_id)),
        item = item,
        item_flush_func = item_flush_func,
        r_func = function ()
            local has_num = Item.GetNum(item_id)
            if has_num >= cfg.num then
                self:UnlockEffect()
                ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_UNLOCK_FUSHI_HOLE, sheng_qi.index, hole_index)
            else
                local name = Item.GetQuaName(item_id)
                PublicPopupCtrl.Instance:Error(string.format(Language.ShengQi.Tip11, name))
                local item = Item.Create({item_id = item_id, jump_need_num = cfg.num - has_num})
                MainOtherCtrl.Instance:GetWayView({item = item})
            end
        end,
    }
    ViewMgr:OpenView(PetStuffBuyDialog,param)
end

-- =========== 符石背包 ============
FuShiBag = FuShiBag or DeclareMono("FuShiBag", ShengQiBag)
function FuShiBag:FuShiBag()
    self.data = FuShiData.Instance
    self.data_cares = {
        {data = self.data.fu_shi_list, func = self.FlushPanel, init_call = false},
        {data = ShengQiData.Instance.sel_sheng_qi, func = self.FlushPanel, init_call = false}
    }

    self.type_list = {
        {text = Language.ShengQi.FuShiTypeName2[0], type = FU_SHI_TYPE.Common, qua = ItemColor.None},
        {text = Language.ShengQi.FuShiTypeName2[1], type = FU_SHI_TYPE.QingLong, qua = ItemColor.None},
        {text = Language.ShengQi.FuShiTypeName2[2], type = FU_SHI_TYPE.BaiHu, qua = ItemColor.None},
        {text = Language.ShengQi.FuShiTypeName2[3], type = FU_SHI_TYPE.ZhuQu, qua = ItemColor.None},
        {text = Language.ShengQi.FuShiTypeName2[4], type = FU_SHI_TYPE.XuanWU, qua = ItemColor.None},
        {text = Language.ShengQi.FuShiTypeName2[5], type = FU_SHI_TYPE.QiLing, qua = ItemColor.None},
    }
end

function FuShiBag:GetList()
    local list_data = self.data:GetBagShowList(self.cur_type_data.qua,self.cur_type_data.type)
    return list_data
end

function FuShiBag:GetMinCount(num)
    if num < self.MinCount then
        return self.MinCount
    else
        local n = num % 4
        return num + 4 - n
    end
end



FuShiAttriPreview = FuShiAttriPreview or DeclareView("FuShiAttriPreview", "sheng_qi/fu_shi_attri_preview")
function FuShiAttriPreview:FuShiAttriPreview()
    self.Board:SetData(self:GetType(), Language.ShengQi.Title10, Vector2.New(426, 484))
end

function FuShiAttriPreview:LoadCallback(param_t)
    param_t = param_t or {}
    local sheng_qi = param_t.sheng_qi
    if not sheng_qi then
        return
    end
    local fushi_attri_list = sheng_qi:AllFuShiAttriList()
    local show_attri_list = {}
    for i,v in pairs(fushi_attri_list) do
        table.insert(show_attri_list, ShengQiData.Instance:FormatAttriInfo(v.attri_type, v.attri_value))
    end
    self.AttriList:SetData(show_attri_list)
end


FuShiQuickBuy = FuShiQuickBuy or DeclareView("FuShiQuickBuy", "sheng_qi/fushi_quick_buy")
function FuShiQuickBuy:FuShiQuickBuy()
    
end

function FuShiQuickBuy:LoadCallback(param)
    local context = param.context or ""
    UH.SetText(self.Context, context)
    self.ok_func = param.ok_func
    local text_h = self.Context.preferredHeight
    self.BgRect:SetSizeWithCurrentAnchors(1, 272 + text_h)
    if param.item then
        self.ItemCell:SetData(param.item)
    end
end

function FuShiQuickBuy:OnCloseClick()
    ViewMgr:CloseView(FuShiQuickBuy)
end

function FuShiQuickBuy:OnCancelClick()
    ViewMgr:CloseView(FuShiQuickBuy)
end

function FuShiQuickBuy:OnOkClick()
    if self.ok_func then
        self.ok_func()
    else
        ViewMgr:CloseView(FuShiQuickBuy)
    end
end