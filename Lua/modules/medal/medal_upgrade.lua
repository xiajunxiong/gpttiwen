MedalUpgradeView = MedalUpgradeView or DeclareView("MedalUpgradeView", "medal/medal_upgrade")
function MedalUpgradeView:MedalUpgradeView()end

MedalUpgradePanel = MedalUpgradePanel or DeclareMono("MedalUpgradePanel", UIWFlushPanel)
function MedalUpgradePanel:MedalUpgradePanel()
    self.data = MedalData.Instance

    self.data_cares = {
        {data = self.data.NoticeInfo,func = self.ShowNotice ,keys = {"is_show"}},
        {data = self.data.cost_medal,func = self.FlushRankTips},
        {data = BagData.Instance.item_grid, func = self.onFlush, init_call = false},
    }

    self:SortDesk()
    self:FlushNone()
    self.wait_notice = false
end 

function MedalUpgradePanel:OnClickUpgrade()
    local data = self.data:GetCostMedal()
    if data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.SmeltViewNotSelect)
        return 
    end 

    local operate = self.data:GetOperateMedal()
    local oper_ = self.data.upgrading_medal


    local function send_msg()

        local param_t = { 
            req_type = oper_.oper_type == MedalData.OperaType.Bag and MEDAL_REQ_TYPE.UPGRADE or MEDAL_REQ_TYPE.UPGRADE_EQUIP,
            param1 = oper_.oper_type == MedalData.OperaType.Bag and operate.column or operate.vo.grid_index, 
            param2 = oper_.oper_type == MedalData.OperaType.Bag and operate.index or 0, 
            param3 = data.column_type, 
            param4 = data.index,
        }
    
        self.data:SetCostMedal()
        self.data:MarkMedalInfo()
        if MedalData.OperaType.Wear == oper_.oper_type then self.data:SetMarkAttrView(true) end 
        BagCtrl.Instance:SendMedalReq(param_t)
    
        self.MedalList:CancelSelect()
        self.wait_notice = true
		ViewMgr:CloseView(DialogTipsView)
    end

    if data.param.grade == nil then
        data:Check()
    end

    if operate.param.grade == nil then 
        operate:Check()
    end 

    if data.param.grade <= operate.param.grade then
        send_msg()
        return 
    end 

	local info = {
		content = string.format(Language.Medal.UpGradeCostMedal,  self.data:GetMedalFullName(data)),
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = send_msg
		}
	}
	PublicPopupCtrl.Instance:DialogTips(info)
end 

-- 手动矫正
function MedalUpgradePanel:SortDesk()
    self.Desk:SetParent(self.y_mark,true)
    local x = self.Desk.transform.anchoredPosition.x
    local y = self.Desk.transform.anchoredPosition.y
    UH.AnchoredPosition(self.Desk:GetComponent(typeof(UnityEngine.RectTransform)),
        Vector2.New(x,0))
    self.Desk:SetParent(self.x_mark,true)
    UH.AnchoredPosition(self.Desk:GetComponent(typeof(UnityEngine.RectTransform)),
        Vector2.New(0,y))
end

function MedalUpgradePanel:FlushNone()
    local str = string.format(Language.Medal.MedalUpgradeWithoutCost,RichTextHelper.LinkStr(ColorStr(Language.Medal.MedalUpgradeNoneTips,COLORSTR.Green6),"_ul",COLORSTR.Green6))
    UH.SetText(self.EmptyListTips,str)
end

function MedalUpgradePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.wait_handle ~= nil then
        TimeHelper:CancelTimer(self.wait_handle)
        self.wait_handle = nil
    end

    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end

    self.data:SetCostMedal()
end

function MedalUpgradePanel:OnClickClose()
    ViewMgr:CloseView(MedalUpgradeView)
end

function MedalUpgradePanel:onFlush()
    if self.wait_notice then return end 
    local bag_list = self.data:GetMedalUpgradeMaterial()
    self.MedalList:SetData(bag_list)

    local operate_item = self.data:GetOperateMedal()
    UH.SetIcon(self.OperIcon,operate_item:BigIcon())

    self.AttrList:SetData(self.data:GetOperMedalAttr())
    local effect_list = self.data:GetOperMedalEffect()
    self.EffectList:SetData(effect_list)

    self.EmptyList:SetActive(#bag_list == 0)
    self.EmptyEffect:SetActive(#effect_list == 0)

    UH.SetText(self.MedalName,self.data:GetMedalFullName())

    if operate_item.param.grade > 4 then 
        local pos_ = operate_item.param.grade - 4
        if self.flush_left_item_handle ~= nil then
            TimeHelper:CancelTimer(self.flush_left_item_handle)
            self.flush_left_item_handle = nil
        end
        self.flush_left_item_handle = TimeHelper:AddDelayFrameTimer(function()
            self.flush_left_item_handle = nil
 
            local force_heigh = 0
            local jump_num = pos_ < #effect_list - 1 and pos_ or #effect_list - 1
            for i = 1 , jump_num - 1 do 
                local obj = self.EffectList ~= nil and self.EffectList:GetItem(i) or nil
                if obj ~= nil then force_heigh = force_heigh + math.floor(obj.RectTrans.rect.height) end 
            end 
            if self.EffectList ~= nil then self.EffectList:ForceJumpVertical(force_heigh) end 
        end,5+math.floor(#effect_list/self.EffectList.StepCount))
    end 
end

function MedalUpgradePanel:FlushRankTips()
    if self.wait_notice then return end 
    local data = self.data:GetRankUpTips()
    self.UpgradeTips:SetActive(not data.is_max)
    self.BtnUpgrade:SetActive(not data.is_max)
    self.MaxTips:SetActive(data.is_max)

    UH.SpriteName(self.SuccTips,MedalConfig.MedalRankIcon..data.succ_rank)
    UH.SpriteName(self.FailTips,MedalConfig.MedalRankIcon..data.fail_rank)
    UH.SetText(self.RateTips,data.rate)
end

function MedalUpgradePanel:OnClickNoneBuy()
    local operate = self.data:GetOperateMedal()
    local cfg = MedalData.Instance:GetMedalMarketCfg(operate.param.grade)
    if cfg == nil then return end 
    TradeData.Instance:SetSearchType(cfg.market_filtrate_seq)
    TradeData.Instance:SetSelectBigType(15)
    ViewMgr:OpenViewByKey(Mod.Mall.Trade)
end

function MedalUpgradePanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[90].desc})
end

function MedalUpgradePanel:OnClickNameTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[91].desc})
end

function MedalUpgradePanel:ShowNotice()
    if self.data.NoticeInfo.is_show then 
        ViewMgr:OpenView(MedalUpgradeNoticeView)
        self.data:ClearUpgradeNotice()


        if self.wait_handle ~= nil then
            TimeHelper:CancelTimer(self.wait_handle)
            self.wait_handle = nil
        end

        self.wait_handle = TimeHelper:AddDelayTimer(function()
            self.wait_handle = nil
            self.wait_notice = false
            self:onFlush()
            self:FlushRankTips()
        end,1.2)
    end 

end

MedalUpgradeShowPanel = MedalUpgradeShowPanel or DeclareMono("MedalUpgradeShowPanel", UIWFlushPanel)
function MedalUpgradeShowPanel:MedalUpgradeShowPanel()
end 

function MedalUpgradeShowPanel:OnDestroy()
end

MedalEffectDescItem = DeclareMono("MedalEffectDescItem", UIWidgetBaseItem)
function MedalEffectDescItem:MedalEffectDescItem()

end

function MedalEffectDescItem:SetData(data)
    self.Canvas.alpha = data.is_dark and 0.5 or 1 
    self.CurBg:SetActive(data.is_cur)
    local _name = string.format( Language.Medal.MedalRank,DataHelper.GetWord(data.cfg.sp_rank))..data.cfg.reward_name..":"
    UH.SetText(self.Name,_name)

    local cur_medal = MedalData.Instance:GetOperateMedal()
    local _desc = (data.is_cur or data.is_pass) and data.cfg.reward_description or data.cfg.rank_up_description
    UH.SetText(self.Desc, _desc )
    if self.flush_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_handle)
        self.flush_handle = nil
    end

    self.flush_handle = TimeHelper:AddDelayFrameTimer(function()
        if self.DescRect ~= nil and self.NameRect ~= nil and self.DescRect.sizeDelta ~= nil then 
            local desc_h = self.DescRect.rect.height

            local num_desc = DataHelper.GetStringWordNum(data.cfg.reward_description)
            local num_limited = (490 - self.NameRect.rect.width ) / 22
            local test_h = math.floor(num_desc % num_limited) > 0 and math.floor(num_desc / num_limited) + 1 or math.floor(num_desc / num_limited)
            desc_h = test_h * 22 + (test_h - 1) * 11

            -- -- 这里的22是字号 11换行
            -- if len_test < MedalConfig.UpgradeEffectCellSingleLen then 
            --     desc_h = 22
            -- elseif len_test > MedalConfig.UpgradeEffectCellSingleLen and len_test < MedalConfig.UpgradeEffectCellTwiceLen then 
            --     desc_h = 55
            -- elseif len_test > MedalConfig.UpgradeEffectCellTwiceLen then 
            --     desc_h = 88
            -- end 
            self.DescRect.sizeDelta = Vector2.New(490 - self.NameRect.rect.width, desc_h)
        end
    end)
end

function MedalEffectDescItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    if self.flush_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_handle)
        self.flush_handle = nil
    end
end

MedalUpgradeAttrItem = DeclareMono("MedalUpgradeAttrItem", UIWidgetBaseItem)
function MedalUpgradeAttrItem:MedalUpgradeAttrItem()

end

function MedalUpgradeAttrItem:SetData(data)
    UH.SetText(self.type,Language.Common.AttrList[data.attr_type])
    local str_change = data.change > 0 and data.change or ""
    UH.SetText(self.attr,data.attr_value)
    UH.SetText(self.change,str_change..")")
end

MedalUpgradeCostCell = DeclareMono("MedalUpgradeCostCell", UIWidgetBaseItem)
function MedalUpgradeCostCell:MedalUpgradeCostCell()
    self.medal_data = MedalData.Instance
end

function MedalUpgradeCostCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local _item = Item.Create(data)
    self.Item:SetData(_item)
end

function MedalUpgradeCostCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function MedalUpgradeCostCell:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
    end
end

function MedalUpgradeCostCell:OnClickItem()
    self.medal_data:SetCostMedal(self.data)
end
