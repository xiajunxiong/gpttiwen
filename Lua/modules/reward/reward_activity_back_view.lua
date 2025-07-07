
RewardActivityBackView = RewardActivityBackView or DeclareMono("RewardActivityBackView", UIWFlushPanelGroup)

function RewardActivityBackView:RewardActivityBackView()
    self.flush_events = {EventProtocol.ToEventKey(SCFindRewardInfo)}
    for i=1,self.ChildPanels:Length() do
        self.ChildPanels[i]:Flush()
    end
end

RewardActivityBackViewUp = RewardActivityBackViewUp or DeclareMono("RewardActivityBackViewUp", UIWFlushPanel)

function RewardActivityBackViewUp:RewardActivityBackViewUp()
    self.data = RewardCtrl.Instance:Data()
    self.data.reward_activity_back_type_cur = 1
end

function RewardActivityBackViewUp:onFlush()
    local reward_activity_back_type_list = self.data:GetRewardActivityBackTypeList()
    self.UpList:SetData(reward_activity_back_type_list)
    self.UpList:ClickItemData(reward_activity_back_type_list[self.data.reward_activity_back_type_cur])

    if self.data.activity_back_red_point.val then
        RewardCtrl.Instance:SendNotifyCancelReq()
    end
end

RewardActivityBackViewUpItem = RewardActivityBackViewUpItem or DeclareMono("RewardActivityBackViewUpItem", UIWidgetBaseItem)

function RewardActivityBackViewUpItem:RewardActivityBackViewUpItem()
	self.reward_data = RewardCtrl.Instance:Data()
	self.language = Language.Reward.RewardActivityBack.up_desc
end

function RewardActivityBackViewUpItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = self.language[data.type]
end

function RewardActivityBackViewUpItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function RewardActivityBackViewUpItem:OnClickItem(data)
    self.reward_data.reward_activity_back_type_cur = data.type
    ViewMgr:FlushView(RewardView, "RewardView/RewardActivityBackView/RewardActivityBackViewMid")
    ViewMgr:FlushView(RewardView, "RewardView/RewardActivityBackView/RewardActivityBackViewDown")
end

RewardActivityBackViewMid = RewardActivityBackViewMid or DeclareMono("RewardActivityBackViewMid", UIWFlushPanel)

function RewardActivityBackViewMid:RewardActivityBackViewMid()
    self.data = RewardCtrl.Instance:Data()
    
    self.data_cares = {
        {data = RoleData.Instance:GetBaseData(), func = self.onFlush, init_call = false, keys = {"level"}},
    }
end

function RewardActivityBackViewMid:onFlush()
    local reward_activity_back_list = {}
    -- 特殊插入，境界领取奖励
    table.insert(reward_activity_back_list, {find_type = FindType.OfflineReward})
    for i, v in ipairs(self.data:GetRewardActivityBackList()) do
        table.insert(reward_activity_back_list, v)
    end
    self.MidList:SetData(reward_activity_back_list)
    self.EmptyObj:SetActive(0 == #reward_activity_back_list)
end

RewardActivityBackViewMidItem = DeclareMono("RewardActivityBackViewMidItem", UIWidgetBaseItem)

function RewardActivityBackViewMidItem:RewardActivityBackViewMidItem()
    self.reward_data = RewardCtrl.Instance:Data()
    self.language = Language.Reward.RewardActivityBack.Mid
end

function RewardActivityBackViewMidItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- 超特殊找回客户端自定义处理
    self.ObjNodeSp:SetActive(self.data.find_type > 100)
    self.ObjNode:SetActive(self.data.find_type <= 100)
    if self.data.find_type > 100 then
        self.sp_data = self.reward_data:ActivityBackSpData(self.data.find_type)
        UH.SetText(self.TxtNameSp, self.language.SpName[self.data.find_type])
        self.CellSp:SetData(Item.Init(CommonItemId.Exp, 1, true))
        self:FlushSp()
        if not TableIsEmpty(self.sp_data) then
            if self.sp_data.handle then
                self.handler = self.sp_data.handle:Care(BindTool.Bind(self.FlushSp, self))
            end
        end
    else
        self.Name.text = data.name
        self.SumNum.text = string.format(self.language.sum_desc, data.sum)
        self.BtnBackInter.Interactable = data.sum > 0
        self.MaskObj:SetActive(data.sum == 0)
        local item_data = (data.reward_list[self.reward_data.reward_activity_back_type_cur - 1] or {})[1]
        self.RewardCell:SetData(item_data or {})
        self.RewardNum.text = item_data and item_data:Num() or 0
        self.GetFlagObj:SetActive(self.data.get_flag)
        self:FlushNum()
        self:FlushSpecial()
        -- self.Label:SetActive(data.islimit == 1)
        local active_system_type = FindTypeToActivitySystemType[self.data.find_type]
        local corner_type = active_system_type ~= nil and 2 or data.islimit
        UH.SetText(self.LimitText, Language.Reward.FindCorner[corner_type])
        if data.sp_reward then
            self.SpList:SetObjActive(true)
            self.SpList:SetData(DataHelper.FormatItemList({data.sp_reward}))
        else
            self.SpList:SetObjActive(false)
        end
    end
end

function RewardActivityBackViewMidItem:FlushSp()
    if not TableIsEmpty(self.data) and self.data.find_type > 100 then
        if not TableIsEmpty(self.sp_data) then
            local num = self.sp_data.num_fun()
            UH.SetText(self.TxtDescSp, self.sp_data.desc_fun())
            UH.SetText(self.TxtNumSp, num)
            self.IHBackSp.Interactable = num > 0
            self.RemindSp:SetNum(self.sp_data.remind_fun())
            if self.sp_data.sp_reward then
                self.SpList:SetObjActive(true)
                self.SpList:SetData(DataHelper.FormatItemList(self.sp_data.sp_reward()))
            else
                self.SpList:SetObjActive(false)
            end
        end
    end
end

function RewardActivityBackViewMidItem:FlushNum()
    self.CurNum.text = self.data.cur_num
    -- self.CostNum.text = self.data.cur_num * self.data.back_price[self.reward_data.reward_activity_back_type_cur]
    local price = self.reward_data:GetRewardActivityBackFindPrice(self.data.find_type)
    self.CostNum.text = (0 == self.data.cur_num and self.data.finish or self.data.cur_num) * price
end

-- 特殊的找回在这里进行刷新
function RewardActivityBackViewMidItem:FlushSpecial()
    if self.data.find_type > 100 then return end
    -- 对四象幻境进行特殊处理
    if self.data.find_type == 7 then 
        local cur_num,info_paramt = FightData.Instance:GetFantasyBackRewardInfo(self.data.sum)
        local item_paramt = FightData.Instance:ChangeFantasyBackRewardInfo(info_paramt)
        self.SumNum.text = string.format(self.language.sum_desc, cur_num)
        self.BtnBackInter.Interactable = cur_num > 0
        if item_paramt[1] ~= nil then 
            self.RewardCell:SetData(item_paramt[1])
        end 
        self.RewardNum.text = item_paramt[1] and item_paramt[1]:Num() or 0
        -- self.CostNum.text = cur_num * self.data.back_price[self.reward_data.reward_activity_back_type_cur]
        local price = self.reward_data:GetRewardActivityBackFindPrice(self.data.find_type)
        self.CostNum.text = cur_num * price
        self.special_cur_num = cur_num
        self.MaskObj:SetActive(cur_num == 0)
    elseif self.data.find_type == FindType.FightEnergy then
        local cur_num = self.data.cur_num
        local finish = self.data.finish
        local num = 0 == cur_num and finish or cur_num
        self.SumNum.text = string.format(self.language.sum_energy_desc, cur_num)
        self.CostNum.text = self.reward_data:ConfigRewardFindCopperCoinPrice()
        self.RewardCell:SetData(Item.Init(CommonItemId.Exp))
        self.RewardNum.text = num * FightData.Instance:GetEnergyExpNum()
    else
        self.special_cur_num = nil
    end 
end

function RewardActivityBackViewMidItem:OnClickAdd()
    if self.data.find_type > 100 then return end
	if self.data.sum > self.data.cur_num then
        self.data.cur_num = self.data.cur_num + 1
        self:FlushNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.tips.MaxNum)
	end
end

function RewardActivityBackViewMidItem:OnClickSub()
    if self.data.find_type > 100 then return end
	if 1 < self.data.cur_num then
        self.data.cur_num = self.data.cur_num - 1
        self:FlushNum()
	else
		PublicPopupCtrl.Instance:Center(self.language.tips.MinNum)
	end
end

function RewardActivityBackViewMidItem:OnClickBack()
    if self.data.find_type > 100 then
        if not TableIsEmpty(self.sp_data) then
            self.sp_data.receive_fun()
        end
        return
    end
    -- local need = self.data.cur_num * self.data.back_price[self.reward_data.reward_activity_back_type_cur]
    local price = self.reward_data:GetRewardActivityBackFindPrice(self.data.find_type)
    local need = self.data.cur_num * price
    if self.data.find_type == 7 then   
        local cur_num,info_paramt = FightData.Instance:GetFantasyBackRewardInfo(self.data.sum)
        need = cur_num 
    end 
    local have = RoleData.Instance:GetCurrency(self.reward_data.reward_activity_back_type_cur == 1 and CurrencyType.CoinBind or CurrencyType.Gold)
    if need > have then
        PublicPopupCtrl.Instance:Center(self.language.tips.NotEnough[self.reward_data.reward_activity_back_type_cur])
        return
    end
    RewardCtrl.Instance:SendFindRewardReq{
        type = self.data.find_type,
	    consume_type = self.reward_data.reward_activity_back_type_cur,
	    count = self:GetIsSpecialBackNum(),
    }
    self.reward_data:SetMainFindNotShow()
end

function RewardActivityBackViewMidItem:GetIsSpecialBackNum()
    if self.data.find_type > 100 then return end
    if self.special_cur_num ~= nil then 
        return self.special_cur_num
    end 
    return self.data.cur_num
end

function RewardActivityBackViewMidItem:OnDestroy()
    if self.handler then
        if not TableIsEmpty(self.sp_data) then
            self.sp_data.handle:Uncare(self.handler)
        end
    end
end

RewardActivityBackViewDown = RewardActivityBackViewDown or DeclareMono("RewardActivityBackViewDown", UIWFlushPanel)

function RewardActivityBackViewDown:RewardActivityBackViewDown()
    self.data = RewardCtrl.Instance:Data()
    self.language = Language.Reward.RewardActivityBack
    GuideManager.Instance:RegisterGetGuideUi("ActiveActivityBackBtn", function()
		if self.ActiveActivityBackBtn then
			return self.ActiveActivityBackBtn
		end
	end)
end

function RewardActivityBackViewDown:onFlush()
    local reward_activity_back_type_list = self.data:GetRewardActivityBackTypeList()
    self.DownTips.text = string.format(self.language.down_desc, reward_activity_back_type_list[self.data.reward_activity_back_type_cur].back_scale)
end

function RewardActivityBackViewDown:OnClickBack()
    local all_need = self.data:GetRewardActivityBackAllNeedByType(self.data.reward_activity_back_type_cur)
    if 0 == all_need then
        -- 相当于普通找回已经没有了，那这里过一遍特殊找回，就不弹窗了
        sp_remind = self.data:ActivityBackSpRemind()
        if 0 == sp_remind then
            PublicPopupCtrl.Instance:Center(self.language.NoCanBack)
        else
            self.data:ActivityBackSpReceive()
        end
        return
    end
    local have = RoleData.Instance:GetCurrency(self.data.reward_activity_back_type_cur == 1 and CurrencyType.CoinBind or CurrencyType.Gold)
    local info = {
        content = string.format(self.language.Mid.tips.AllBack[self.data.reward_activity_back_type_cur], all_need),
        confirm = {
            func = function()
                if all_need > have then
                    PublicPopupCtrl.Instance:Center(self.language.Mid.tips.NotEnough[self.data.reward_activity_back_type_cur])
                    ViewMgr:CloseView(DialogTipsView)
                    return
                end
                local reward_activity_back_list = self.data:GetRewardActivityBackList()
                for _, info in pairs(reward_activity_back_list) do
                    RewardCtrl.Instance:SendFindRewardReq{
                        type = info.find_type,
                        consume_type = self.data.reward_activity_back_type_cur,
                        count = info.find_type == 7 and FightData.Instance:GetFantasyBackRewardInfo(info.sum) or info.sum,
                    }
                end
                -- 一键找回的时候过一遍所有的超特殊的找回
                self.data:ActivityBackSpReceive()
                ViewMgr:CloseView(DialogTipsView)
                self.data:SetMainFindNotShow()
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end