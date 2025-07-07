RandHeartSkillView = RandHeartSkillView or DeclareView("RandHeartSkillView","serveractivity/rand_heart_skill_view",Mod.RandActivity.HeartSkill)

VIEW_DECLARE_LEVEL(RandHeartSkillView, ViewLevel.L1)

function RandHeartSkillView:RandHeartSkillView()
    self.data = ServerActivityData.Instance
    self.care_handle = self.data.heart_skill_data:Care(BindTool.Bind(self.FlusnItemView,self))
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_HEART_SKILL,TimeType.Type_Special_4)
end

function RandHeartSkillView:LoadCallback()
    self.toggle_list = self.data:GetHeartSkillList()
    local index = self:GetIndex()
    UH.SetToggle(self.Toggle[index],true)
    self:OnClickItem(self.toggle_list[index])
    self:FlushItemList()
end

function RandHeartSkillView:GetIndex()
    for i,v in ipairs(self.toggle_list) do
        if self.data:GetHeartSkillFlag(v.seq) == 0 then
            return i
        end
    end
end

function RandHeartSkillView:CloseCallback()
	self.data.heart_skill_data:Uncare(self.care_handle)
	self.care_handle = nil
end

function RandHeartSkillView:FlusnItemView()
    self:OnClickItem(self.select_data)
    self:FlushItemList()
end

function RandHeartSkillView:FlushItemList()
    for i=1,self.ItemList:Length() do
        if self.toggle_list[i] then
            self.ItemList[i]:SetData(self.toggle_list[i])
        end
        self.ItemList[i]:SetCancel(self.toggle_list[i] == nil)
    end
end

function RandHeartSkillView:OnClickItem(data)
    local flag = self.data:GetHeartSkillFlag(data.seq)
    local recharge_num = self.data:GetHeartSkillRechargeNum()
    local target_num,Tips = data.acc_price,Language.RandHeartSkill.Tips
    local can_reward = flag == 0 and recharge_num >= target_num

    self.item = Item.New(data.gift_item)
    UH.SetText(self.Name,self.item:Name() .. "x" .. data.gift_item.num)
    UH.SpriteName(self.Icon,[[GfitItem]] .. self.item:Color())

    local target_tip = Format(Tips[can_reward and 3 or 2],target_num - recharge_num)
    self.Grid:SetData(DataHelper.FormatItemList(data.reward_item,1))
    UH.SetText(self.Tips,flag == 0 and target_tip or Format(Tips[1],target_num))
    self.select_data = data
end

function RandHeartSkillView:OnClickClose()
    ViewMgr:CloseView(RandHeartSkillView)
end

function RandHeartSkillView:OnClickRecharge()
    ViewMgr:OpenViewByKey(Mod.Store.Recharge)
end

function RandHeartSkillView:OnClickGift()
    CellClicks.BagGridNoButton({GetData = function()
        return self.item
    end})
end

----------------------------RandHeartSkillItem----------------------------
RandHeartSkillItem = RandHeartSkillItem or DeclareMono("RandHeartSkillItem", UIWidgetBaseItem)

function RandHeartSkillItem:SetData(data)
    self.item = Item.New(data.gift_item)
    UH.SpriteName(self.Icon,[[GfitItem]] .. self.item:Color())
    UH.SetText(self.Name,Format(Language.RandHeartSkill.RechargeNumTip,data.acc_price))

    local flag = ServerActivityData.Instance:GetHeartSkillFlag(data.seq)
    local recharge_num = ServerActivityData.Instance:GetHeartSkillRechargeNum()
    local can_reward = flag == 0 and recharge_num >= data.acc_price
    self.Block:SetActive(can_reward)
    self.RedPiont:SetNum(can_reward and 1 or 0)
    UIWidgetBaseItem.SetData(self, data)
end

function RandHeartSkillItem:SetCancel(flag)
    self.Image:SetActive(flag)
    self.Panel:SetActive(not flag)
end

function RandHeartSkillItem:OnClickEffect()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_HEART_SKILL,1,self.data.seq)
end

function RandHeartSkillItem:OnClickGift()
    CellClicks.BagGridNoButton({GetData = function()
        return self.item
    end})
end