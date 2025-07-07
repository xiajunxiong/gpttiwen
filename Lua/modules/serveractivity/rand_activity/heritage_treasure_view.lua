HeritageTreasureView = HeritageTreasureView or DeclareView("HeritageTreasureView","serveractivity/heritage_treasure_view",Mod.Splendid.HeritageTreasure)
VIEW_DECLARE_LEVEL(HeritageTreasureView, ViewLevel.L1)

function HeritageTreasureView:HeritageTreasureView()
    self.Currency:DefaultSet()
end

function HeritageTreasureView:OnClickClose()
    ViewMgr:CloseView(HeritageTreasureView)
end
---------------------------------------HeritageTreasurePanel-------------------------------------
HeritageTreasurePanel = HeritageTreasurePanel or DeclareMono("HeritageTreasurePanel", UIWFlushPanel)

function HeritageTreasurePanel:HeritageTreasurePanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.heritage_treasure_data,func = self.FlushItemView},
    }
    self.bag_handle = BagData.Instance:Care(BindTool.Bind(self.FlushNumView, self))
end

function HeritageTreasurePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:UnCare(self.bag_handle)
    TimeHelper:CancelTimer(self.active_timer)
end

function HeritageTreasurePanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.Desc,Language.HeritageTreasure.TaskTip)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,TimeType.Type_Special_4)
    UH.SetText(self.TitleText, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE))

    local other_config = self.data:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,Config.treasure_activity_auto.other)
    local item_show = self.data:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,Config.treasure_activity_auto.item_show)
    self.RewardItem:SetData({reward_list = item_show,reward_item = other_config[1].reward_item})
    self.consume_id = other_config[1].consume_item_id

    self.active_timer = Invoke(function()
        self.CallEffect:SetActive(true)
    end, 0.2)
    self:FlushNumView()
end

function HeritageTreasurePanel:FlushNumView()
    self.Call:SetData(Item.New({item_id = self.consume_id}))
    UH.SetText(self.Num,DataHelper.ConsumeNum(Item.GetNum(self.consume_id),1,true))
end

function HeritageTreasurePanel:FlushItemView()
    local data = self.data:GetHeritageTreasureSelectData()
    local index = self:GetAutoIndex(data.index)
    if index ~= nil then
        self.ItemList[index]:Click()
    end
    self:FlushTankView(self.data:GetHeritageTreasurePieceData(index))
    self:FlushItemListView()
    self.Preinstall:SetActive(self:IsPreinstall()and self.data.heritage_treasure_data.raward_flag == 0 and self.clue_num and self.clue_num ~= 0)
    if self.data.heritage_treasure_data.raward_flag == 1 then
        self.RedPiont:SetActive(false)
    end
end

function HeritageTreasurePanel:GetAutoIndex(index)
    if index ~= nil then
        local data = self.data:GetHeritageTreasurePieceData(index)
        if data.clue <= self.TaskList:Length() then
            return index
        end
    end
    for i,v in ipairs(self.data:GetHeritageTreasurePieceList()) do
        if v.clue == self.TaskList:Length() then
            return i
        end
    end
    for i,v in ipairs(self.data:GetHeritageTreasurePieceList()) do
        if v.clue <= self.TaskList:Length() then
            return i
        end
    end
end

function HeritageTreasurePanel:OnClickItem(index)
    self:FlushTankView(self.data:GetHeritageTreasurePieceData(index))
end

--刷新左边激活状态
function HeritageTreasurePanel:FlushItemListView()
    local num = 0
    for k,v in pairs(self.data:GetHeritageTreasurePieceList()) do
        self.ItemList[k]:SetActive(v.clue > self.TaskList:Length())
        self.ItemList[k]:SetRedPiont(v.clue == self.TaskList:Length())
        if v.clue > self.TaskList:Length() then
            num = num + 1
        end
    end
    UH.SetText(self.Progress,Format("%s/%s",num,self.ItemList:Length()))
    self.RewardItem:SetEffect(self.data:GetHeritageTreasureGiftNum() ~= 0)
    self.clue_num = self.ItemList:Length() - num
end

--刷新右边任务线索
function HeritageTreasurePanel:FlushTankView(data)
    if data == nil then 
        self.BtnInter.Interactable = false
        return
    end
    local config = Config.treasure_activity_auto.heritage_treasure[data.type] or {}
    for i=1,self.TaskList:Length() do
        self.TaskList[i]:SetData({desc = data.clue >= i and config["clue"..i] or "????",index = i})
    end
    self.BtnInter.Interactable = data.clue <= self.TaskList:Length()
    self.RedPiont:SetActive(self.data.heritage_treasure_data.raward_flag == 0 and (data.clue == self.TaskList:Length() or (data.clue < self.TaskList:Length() and Item.GetNum(self.consume_id) > 0)))
    UH.SetText(self.BtnName,Language.HeritageTreasure.BtnName[data.clue == self.TaskList:Length() and 0 or(Item.GetNum(self.consume_id) == 0 and 1 or 2)])
    self.data:SetHeritageTreasureSelectData(data)
end

function HeritageTreasurePanel:OnClickRestore()
    local data = self.data:GetHeritageTreasureSelectData()
    if data.clue == self.TaskList:Length() then
        SceneLogic.Instance:AutoToNpc(self:GetNpcId(data.npc_id),nil,false,true)
        ActivityRandData.Instance:CheckRandChildData()
        ViewMgr:CloseView(HeritageTreasureView)
        ViewMgr:CloseView(PlanActivityView)
    else
        if Item.GetNum(self.consume_id) == 0 then
            PublicPopupCtrl.Instance:Center(Language.HeritageTreasure.ErrorLog[4])
        else
            PublicPopupCtrl.Instance:AlertTip(Format(Language.HeritageTreasure.ItemUseTip,Item.GetQuaName(self.consume_id)),function()
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,1,data.index-1)
            end)
        end
    end
end

--存在多个场景NPC时返回静态那个
function HeritageTreasurePanel:GetNpcId(npc_id)
    local npc_vo = VoMgr:NpcVoByNpcId(npc_id) or {}
    if #npc_vo > 1 then
        for k,v in pairs(npc_vo or {}) do
            if v.show_condition == 0 then
                return v.seq
            end
        end
    end
    return npc_id
end

function HeritageTreasurePanel:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[54].desc)
end

--是否到补齐开启时间
function HeritageTreasurePanel:IsPreinstall()
    local other_config = self.data:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,Config.treasure_activity_auto.other)
    if other_config[1].is_complement == 0 then
        return false
    end
	local activity_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE)
    local t = TimeHelper.FormatDHMS((activity_info.next_status_switch_time - TimeHelper.GetServerTime()))
    return t.day < other_config[1].time
end

function HeritageTreasurePanel:OnClickPreinstall()
    if Item.GetNum(self.consume_id) > 0 or self.RedPiont.activeSelf then
        PublicPopupCtrl.Instance:AlertTip(Language.HeritageTreasure.ContentTips,function()
            self:OnPreinstall()
        end)
        return
    end
    self:OnPreinstall()
end

function HeritageTreasurePanel:OnPreinstall()
    local other_config = self.data:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,Config.treasure_activity_auto.other)
    local config = other_config[1]
    ViewMgr:OpenView(ActDialogTipsBackView,{
        reward_list = DataHelper.FormatItemList({config.reward_item}),
        content = Format(Language.HeritageTreasure.BackContent,self.clue_num),
        cancel_name = RichTextHelper.SpriteStrAtlas(CommonItemId.Coin, 40, 40) .. config.coin_num * self.clue_num, 
        confirm_name = RichTextHelper.SpriteStrAtlas(CommonItemId.Gold, 40, 40) .. config.gold_num * self.clue_num, 
        cancel_func = function()
            ServerActivityCtrl.Instance:SendMakeUpReq(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,0)
        end,
        confirm_func = function()
            if MallCtrl.IsNotCurrency(CurrencyType.Gold, config.gold_num * self.clue_num) then
                ServerActivityCtrl.Instance:SendMakeUpReq(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,1)
            end
        end,
    })
end

function HeritageTreasurePanel:OnClickGetItem()
    if PlanActivityCtrl.Instance:ActGetWayViewById(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE) then
        ViewMgr:CloseView(HeritageTreasureView)
    end
end

----------------------------HeritageTreasureCall----------------------------
HeritageTreasureCall = HeritageTreasureCall or DeclareMono("HeritageTreasureCall", UIWidgetBaseItem)

function HeritageTreasureCall:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

function HeritageTreasureCall:SetActive(active)
    self.Image:SetActive(active)
    self.Selected.enabled = not active
end

function HeritageTreasureCall:SetRedPiont(active)
    self.RedPiont:SetActive(active)
end

function HeritageTreasureCall:Click()
	if self.Toggle then
		self.Toggle.isOn = true
	end
end

----------------------------HeritageTreasureReward----------------------------
HeritageTreasureReward = HeritageTreasureReward or DeclareMono("HeritageTreasureReward", UIWidgetBaseItem)

function HeritageTreasureReward:SetData(data)
    local item = Item.New(data.reward_item)
    self.Call:SetData(item)
    local reward_list = {}
    for i,v in ipairs(data.reward_list) do
        table.insert(reward_list,{reward_show = v.item,index = i})
    end
    self.RewardList:SetData(reward_list)
    self.RewardList:SetCompleteCallBack(function()
        if self.IsRewardAnim == nil then
            self.RewardAnim:Play()
            self.IsRewardAnim = true
        end
    end)
end

function HeritageTreasureReward:SetEffect(can_reward)
    self.Effect:SetActive(can_reward)
end

function HeritageTreasureReward:OnClickReward()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_HERITAGE_TREASURE,3)
end

----------------------------HeritageTreasureRewardCall----------------------------
HeritageTreasureRewardCall = HeritageTreasureRewardCall or DeclareMono("HeritageTreasureRewardCall", UIWidgetBaseItem)

function HeritageTreasureRewardCall:SetData(data)
    data.item = Item.New(data.reward_show)
    self.gameObject.name = "Item"..data.index
    self.Call:SetData(data.item)
end

----------------------------HeritageTreasureTaskItem----------------------------
HeritageTreasureTaskItem = HeritageTreasureTaskItem or DeclareMono("HeritageTreasureTaskItem", UIWidgetBaseItem)

function HeritageTreasureTaskItem:SetData(data)
    UH.SetText(self.Title,Format(Language.HeritageTreasure.Clue,DataHelper.GetWord(data.index)))
    UH.SetText(self.Desc,data.desc)
end