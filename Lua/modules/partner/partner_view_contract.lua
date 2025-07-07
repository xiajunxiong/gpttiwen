
PartnerViewContract = PartnerViewContract or DeclareMono("PartnerViewContract", UIWFlushPanel)

function PartnerViewContract:PartnerViewContract()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Contract

    self.data_cares = {
        {data = self.data.item_flush, func = self.onFlush, init_call = false},
        {data = self.data.partner_pact_up_effect, func = self.FlushEffect, init_call = false},
    }
    for i = 1, self.EffectList:Length() do 
        self.EffectListT[i]:SetData({mono = self,end_func = function ()
            self.EffectList[i]:Play(4165058)
            self.EffectList[i]:Play(4165056)
            self:onFlush()
            self.EffectListT[i]:AllHandleCancel()
        end})
    end 

end

function PartnerViewContract:OnEnable()
    self.data.cur_sel_tabbar.val = PartnerConfig.PartnerViewTabs.Contract
    self:onFlush()
end

function PartnerViewContract:onFlush()
    if self.data.partner_pact_up_effect.partner > 0 then return end
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local vo = data.vo
    local pact_items = data.pact_items
    local not_empty = false
    for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
        local pact_item = pact_items[i]
        self.ShowItems[i]:SetData(pact_item, i - 1)
        self.ShowCells[i]:SetData(pact_item.item)
        not_empty = not_empty or pact_item.item_id > 0
        self.CellEmptys[i]:SetActive(0 == pact_item.item_id)
    end
    if not_empty then
        local attr_list = self.data:GetPartnerPactAttrShowAll(pact_items)
        for i = 1, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX do
            local attr = attr_list[i - 1]
            self.Attrs[i].text = string.format(self.language.AttrShow, Language.Common.AttrList[attr.attr_type], DataHelper.IsPercent(attr.attr_type) and Percent(attr.attr_value) or attr.attr_value)
        end
    end
    self.AttrEmpty:SetActive(not not_empty)
    self.AttrShow:SetActive(not_empty)

    self:FlushModelShow(data)

    self.red_point_equip_items = self.red_point_equip_items or {}
    for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
        self.red_point_equip_items[i] = self.red_point_equip_items[i] or UiRedPoint.New(self.PactEmptys[i], Vector3.New(200,80,0))
        local rpn = 0
        if PartnerConfig.LockState.lock == vo.is_unlocked or PartnerConfig.FightState.rest == vo.fight_state then
            rpn = 0
        else
            local pact_item = data.pact_items[i]
            if data.pact_equip_red_point_show and 0 == pact_item.item_id then
                if PartnerData.Instance:GetPartnerPactItemsEquipCheck(data.id) then
                    rpn = 1
                end
            end
        end
        self.red_point_equip_items[i]:SetNum(rpn)
    end
    self.red_point_up_items = self.red_point_up_items or {}
    for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
        self.red_point_up_items[i] = self.red_point_up_items[i] or UiRedPoint.New(self.PactUps[i], Vector3.New(50,20,0))
        local rpn = 0
        if PartnerConfig.LockState.lock == vo.is_unlocked or PartnerConfig.FightState.rest == vo.fight_state then
            rpn = 0
        else
            local pact_item = data.pact_items[i]
			if pact_item.item_id > 0 then
				if PartnerData.Instance:GetPartnerPactItemsUpCheck(pact_item.item) then
					rpn = 1
				end
			end
        end
        self.red_point_up_items[i]:SetNum(rpn)
    end
end

function PartnerViewContract:FlushEffect()
    if 0 == self.data.partner_pact_up_effect.partner then return end
    local pos = self.data.partner_pact_up_effect.pos
    local type = self.data.partner_pact_up_effect.type
    self.data:SetPartnerPactUpEffect(0,0,0)
    if 1 == type then
        self.EffectListT[pos]:AllHandleCancel()
        self.EffectTool:Play(4165057)
        self.EffectListT[pos]:Play(3165007)
    elseif 2 == type then
        self.EffectList[pos]:Play(4165058)
        self.EffectList[pos]:Play(4165056)
        self.EffectTool:Play(4165057)
        self:onFlush()
    end
end

function PartnerViewContract:FlushModelShow(data)
    local skin_id = (0 == data.vo.skin_id and self.data.cur_sel_skin.skin_tab) and data.skin_id_try or data.vo.skin_id
    local res_path = ""
    if skin_id > 0 and self.data.partner_skin_ids[skin_id] then
        local res_id = self.data.partner_skin_ids[skin_id].res_id
        res_path = string.format("actor/npc/%s.prefab", res_id)
    else
        local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
        res_path = string.format("actor/npc/%s.prefab", data.info.res_id)
    end
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData(res_path)
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
            }
        )
    else
        self.ui_obj:SetData(res_path)
    end
end

function PartnerViewContract:OnClickTips()
	ViewMgr:OpenView(PartnerContractAttrView)
end

function PartnerViewContract:OnClickBag(pos)
    if PartnerCtrl.Instance:UnLockTips() then return end
    self.data.partner_pact_show_info.sel_pos = pos
	ViewMgr:OpenView(PartnerContractView)
    
    if -1 == self.data.cur_sel_partner then return end
    local partner = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if partner.pact_equip_red_point_show then
        partner.pact_equip_red_point_show = false
        self.data.item_flush.val = not self.data.item_flush.val
        self.data:PartnerPactEquipRedPoint()
    end
end

function PartnerViewContract:OnClickStore()
    ViewMgr:OpenView(TreasureHuntView,{auto_select = 5})
end

PartnerViewContractItem = PartnerViewContractItem or DeclareMono("PartnerViewContractItem", UIWidgetBaseItem)

function PartnerViewContractItem:PartnerViewContractItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Contract
    self.index = 0
end

function PartnerViewContractItem:OnDestroy()
    self:uncareData()
end

function PartnerViewContractItem:SetData(data, index)
    UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
    self.care_handles = {}
    self:careData(self.partner_data.partner_pact_items, BindTool.Bind(self.FlushNum, self))

    self.index = index
    local is_empty = 0 == data.item_id
    self.EmptyObj:SetActive(is_empty)
    self.InfoObj:SetActive(not is_empty)
    if is_empty then return end
    local item = data.item
    local is_max = item:MaxAttrUp()
    self.NextObj:SetActive(not is_max)
    self.MaxObj:SetActive(is_max)
    self.Title.text = item:Name()
    self.Name.text = item:Name()
    self.Desc1.text = string.format(self.language.DescShow1, item:PartnerName(), item:AttrUp())
    self.Desc2.text = string.format(self.language.DescShow2, item:AttrUpNext())
    self.MaxDesc.text = string.format(self.language.DescShow3, item:PartnerName(), item:AttrUp())
    self.Cell:SetData(item)
end

function PartnerViewContractItem:FlushNum()
    if 0 == self.data.item_id then return end
    local item = self.data.item
    local have_num = self.partner_data:GetPatnerPactHaveNum(item:PromoteExpens())
    local need_num = item:ExpensNum()
    self.Num.text = string.format(self.language.NumShow, RichTextHelper.ColorStr(have_num, need_num > have_num and COLORSTR.Red5 or COLORSTR.Green6), need_num)
end

function PartnerViewContractItem:OnClickBag()
    if PartnerCtrl.Instance:UnLockTips() then return end
    self.partner_data.partner_pact_show_info.sel_pos = self.index
    ViewMgr:OpenView(PartnerContractView)

    if -1 == self.partner_data.cur_sel_partner then return end
    local partner = self.partner_data:GetPartnerInfoById(self.partner_data.cur_sel_partner)
    if partner.pact_equip_red_point_show then
        partner.pact_equip_red_point_show = false
        self.partner_data.item_flush.val = not self.partner_data.item_flush.val
        self.partner_data:PartnerPactEquipRedPoint()
    end
end

function PartnerViewContractItem:OnClickUp()
    local item = self.data.item
    local star_level = item:StarLevel()
    local have_num = self.partner_data:GetPatnerPactHaveNum(item:PromoteExpens())
    local need_num = item:ExpensNum()
    if need_num > have_num then
        PublicPopupCtrl.Instance:Center(self.language.UpNotEnoughTips)
        return
    end
    if 0 == star_level then
        PublicPopupCtrl.Instance:DialogTips{
            content = self.language.Star0UpTips,
            confirm = {
                func = function ()
                    ViewMgr:CloseView(DialogTipsView)
                    PartnerCtrl.Instance:SendPartnerContractUpStar(self.partner_data.cur_sel_partner, self.index)
                end
            }
        }
    else
        PartnerCtrl.Instance:SendPartnerContractUpStar(self.partner_data.cur_sel_partner, self.index)
    end
end