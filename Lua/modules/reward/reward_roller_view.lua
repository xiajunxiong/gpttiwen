RewardRollerView = RewardRollerView or DeclareMono("RewardRollerView", UIWFlushPanel)

function RewardRollerView:RewardRollerView()
    self.data = RewardData.Instance
    self.data_cares = {
        {data = self.data.roller_data,func = self.FlushRollerView,init_call = true,keys = {"info"}},
        {data = self.data.roller_data,func = self.FlushIndexView,init_call = false,keys = {"change"}},
        {data = self.data.roller_data,func = self.FlushGetView,init_call = true,keys = {"val"}},
    }
    self.index = 1
	self.cur_turn = 1                                       --当前位置
	self.turn_time = 0.5
    self.turn_timer = 0                                     --转盘计时器
end

function RewardRollerView:Awake()
    UIWFlushPanel.Awake(self)
    self.config = ServerActivityData.Instance:GetActivityOpenCfg("consumer_carousel","seq")
    for i=1,12 do
        self.ItemList[i]:SetData(self.config[i])
    end
    self.gold_num = ServerActivityData.Instance:GetOtherConfig("spend_ingots_once")
    UH.SetText(self.TopTip,Format(Language.Reward.Roller.Tip,self.gold_num))
    self:SetButtonEnabled(true)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_CONSUMER_CAROUSEL)
end

function RewardRollerView:FlushRollerView()
    local info = self.data.roller_data.info
    UH.SetText(self.Today,info.consume_num_day or 0)
    UH.SetText(self.Consume,info.consume_count or 0)
    UH.SetText(self.Times,info.last_num or 0)
    self.last_num = info.last_num or 0
    self:CheckRedNum(self.last_num)
end

function RewardRollerView:FlushIndexView()
    local index,config = self:GetSelectIndex()
    self.index = index
    self.item_list = DataHelper.FormatItemList(config.reward_item)
end

function RewardRollerView:GetSelectIndex()
    local index = self.data.roller_data.index
    for k,v in pairs(self.config) do
        if v.seq == index then
            return k,v
        end
    end
end

function RewardRollerView:FlushGetView()
    if not self.InterList[1].Interactable then
        return
    end
    local get_name = ""
    local get_list = self.data:GetConsumeShowList()
    for i=1,#get_list do
        get_name = get_name .. get_list[i]
    end
    UH.SetText(self.GetName,get_name)
    self.Grid:SetData(get_list)
    self.NotName:SetActive(#get_list == 0)
end

function RewardRollerView:CheckRedNum(num)
    if self.red_one_point == nil then
        self.red_one_point = UiRedPoint.New(self.OneRedPos, Vector3.zero, false)
    end
    self.red_one_point:SetNum(num)

    if self.red_all_point == nil then
        self.red_all_point = UiRedPoint.New(self.AllRedPos, Vector3.zero, false)
    end
    self.red_all_point:SetNum(num)
end

--========================动画逻辑===========================
function RewardRollerView:OnClickRoller(type)
    self.item_list = nil
    if self.last_num <= 0 then
        PublicPopupCtrl.Instance:Error(Language.OpenAct.OpenRoller.NotTimesTip[2])
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
    if type == 1 then 
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_CONSUMER_CAROUSEL,2)
        return 
    end
    self.onClick = true
	self.updata_count = 0
    self:SetButtonEnabled(false)
    MainOtherCtrl.Instance.pause = true
    self.play_time = TimeHelper:AddCountDownTT(
        BindTool.Bind1(self.UpdataRollerTime,self),
        function()
            self:SetButtonEnabled(true)
        end
    ,8,0.03)
end

function RewardRollerView:UpdataRollerTime()
	if self.turn_timer >= self.turn_time then
        if self.onClick then
			self:FlushCellListView(self.turn_time / 1.5)
			self.updata_count = self.updata_count + 1
            if self.updata_count >= 60 then
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_CONSUMER_CAROUSEL,1)
				self.turn_time = 0.3
				self.onClick = false
			end
		elseif math.abs(self.cur_turn - self.index) ~= 0  then
			self:FlushCellListView(self.turn_time)
        else
            self:ShowCustom()
            self:FlushRollerView()
            TimeHelper:CancelTimer(self.play_time)
            self:SetButtonEnabled(true)
            self:FlushGetView()
		end
	end
	self.turn_timer = self.turn_timer + 0.03
end

function RewardRollerView:FlushCellListView(value)
	self.ItemList[self.cur_turn]:SetActive(false)
	self.cur_turn = self.cur_turn + 1
	if self.cur_turn > 12 then
		self.cur_turn = 1
	end
    self.ItemList[self.cur_turn]:SetActive(true)
	self.turn_time = math.max(value, 0.001)
	self.turn_timer = 0
end

function RewardRollerView:SetButtonEnabled(is_enable)
    for i=1,2 do
        self.InterList[i].Interactable = is_enable
    end
end

function RewardRollerView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.play_time)
    self:ShowCustom()
end

function RewardRollerView:ShowCustom()
    if self.item_list then
        PublicPopupCtrl.Instance:ShowCustom(self.item_list)
        self.item_list = nil
    end
    MainOtherCtrl.Instance.pause = false
    MainOtherCtrl.Instance:RenewKeyUseView()
end

function RewardRollerView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(163)
end

----------------------------RewardRollerItem----------------------------
RewardRollerItem = RewardRollerItem or DeclareMono("RewardRollerItem", UIWidgetBaseItem)

function RewardRollerItem:SetData(data)
    data.item = Item.New(data.reward_item[0])
    self.Call:SetData(data.item)
    UIWidgetBaseItem.SetData(self, data)
end

function RewardRollerItem:SetActive(active)
    self.Select:SetActive(active)
end

function RewardRollerItem:SendRewardReq()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_CONSUMER_CAROUSEL,3,self.data.seq)
end

----------------------------RewardRollerCell----------------------------
RewardRollerCell = RewardRollerCell or DeclareMono("RewardRollerCell", UIWidgetBaseItem)

function RewardRollerCell:SetData(data)
    UH.SetText(self.Name,data)
end