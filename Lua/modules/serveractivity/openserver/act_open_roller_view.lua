ActOpenRollerView = ActOpenRollerView or DeclareMono("ActOpenRollerView", UIWFlushPanel)

function ActOpenRollerView:ActOpenRollerView()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.open_roller_data,func = self.FlushRollerView,init_call = true,keys = {"info"}},
        {data = self.data.open_roller_data,func = self.FlushIndexrView,init_call = false,keys = {"index"}},
    }
    self.index = 1
	self.cur_turn = 1                                       --当前位置
	self.turn_time = 0.5
    self.turn_timer = 0                                     --转盘计时器
end

function ActOpenRollerView:Awake()
    UIWFlushPanel.Awake(self)
    self:InitActOpenRollerView()
    self:SetButtonEnabled(true)
end

function ActOpenRollerView:InitActOpenRollerView()
    self.config = self.data:GetActivityOpenCfg("open_turntable","seq")
    for i=1,8 do
        self.ItemList[i]:SetData(self.config[i])
    end
    self:FlushTopDescView()
end

function ActOpenRollerView:FlushRollerView()
    local info = self.data.open_roller_data.info
    local OpenRoller = Language.OpenAct.OpenRoller
    local free_times = info.free_times or 0
    if free_times == 0 then
        self.Time:StampTime(info.free_stamp or 0,nil,OpenRoller.FreeTip,Format(OpenRoller.FreeTip,"00:00:00"))
    else
        self.Time:SetTime(OpenRoller.FreeTime)
    end
    UH.SetText(self.Num,Format(OpenRoller.TimesTip,info.have_times or 0))
    self.act_info = {free_times,info.have_times or 0}
end

function ActOpenRollerView:FlushIndexrView()
    self.index = self:GetSelectIndex()
end

function ActOpenRollerView:GetSelectIndex()
    local index = self.data.open_roller_data.index
    for k,v in pairs(self.config) do
        if v.seq == index then
            return k
        end
    end
end

function ActOpenRollerView:FlushTopDescView()
    local textdesc = Config.language_cfg_auto.textdesc[26] or {}
    UH.SetText(self.Tips,textdesc.desc or "")
end

--========================动画逻辑===========================
function ActOpenRollerView:OnPlayAnimation(type)
    if self.act_info[type] == 0 then
        PublicPopupCtrl.Instance:Center(Language.OpenAct.OpenRoller.NotTimesTip[type])
        return
    end
    self.onClick = true
    self.lottery_type = type
	self.updata_count = 0
    self:SetButtonEnabled(false)
    self.play_time = TimeHelper:AddCountDownTT(
        BindTool.Bind1(self.UpdataRollerTime,self),
        function()
            self:SetButtonEnabled(true)
        end
    ,8,0.03)
end

function ActOpenRollerView:UpdataRollerTime()
	if self.turn_timer >= self.turn_time then
        if self.onClick then
			self:FlushCellListView(self.turn_time / 1.5)
			self.updata_count = self.updata_count + 1
            if self.updata_count >= 45 then
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_OPEN_ROLLER,self.lottery_type)
				self.turn_time = 0.3
				self.onClick = false
			end
		elseif math.abs(self.cur_turn - self.index) ~= 0  then
			self:FlushCellListView(self.turn_time)
        else
            self.ItemList[self.cur_turn]:SendRewardReq()
            TimeHelper:CancelTimer(self.play_time)
			self:SetButtonEnabled(true)
		end
	end
	self.turn_timer = self.turn_timer + 0.03
end

function ActOpenRollerView:FlushCellListView(value)
	self.ItemList[self.cur_turn]:SetActive(false)
	self.cur_turn = self.cur_turn + 1
	if self.cur_turn > 8 then
		self.cur_turn = 1
	end
    self.ItemList[self.cur_turn]:SetActive(true)
	self.turn_time = math.max(value, 0.001)
	self.turn_timer = 0
end

function ActOpenRollerView:SetButtonEnabled(is_enable)
    for i=1,2 do
        self.InterList[i].Interactable = is_enable
    end
end

----------------------------ActOpenRollerItem----------------------------
ActOpenRollerItem = ActOpenRollerItem or DeclareMono("ActOpenRollerItem", UIWidgetBaseItem)

function ActOpenRollerItem:SetData(data)
    data.item = Item.New(data.reward_item)
    self.Call:SetData(data.item)
    UH.SetText(self.Name,"")
    UIWidgetBaseItem.SetData(self, data)
end

function ActOpenRollerItem:SetActive(active)
    self.Select:SetActive(active)
end

function ActOpenRollerItem:SendRewardReq()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_OPEN_ROLLER,3,self.data.seq)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_OPEN_ROLLER)
end