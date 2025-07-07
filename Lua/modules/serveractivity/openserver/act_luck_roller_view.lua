ActLuckRollerView = ActLuckRollerView or DeclareMono("ActLuckRollerView", UIWFlushPanel)

function ActLuckRollerView:ActLuckRollerView()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.luck_roller_data,func = self.FlushRollerView,init_call = false,keys = {"info"}},
        {data = self.data.luck_roller_data,func = self.FlushIndexrView,init_call = false,keys = {"val"}},
    }
    self.index = 1
	self.cur_turn = 1                                       --当前位置
	self.turn_time = 0.5
    self.turn_timer = 0                                     --转盘计时器
end

function ActLuckRollerView:Awake()
    UIWFlushPanel.Awake(self)
    self:InitActLuckRollerView()
    self:SetButtonEnabled(true)
    self.BtnCheck:SetActive(self.data:GetLuckyTurntableSave())
    self:FlushRollerView()
end

function ActLuckRollerView:InitActLuckRollerView()
    local config = Config.lucky_turntable_auto.turntable_rewards
    local item_list = Config.lucky_turntable_auto.other[1].show_reward_item
    self.act_config = ServerActivityData.Instance:GetActOpenDayCfgSixStart(ACTIVITY_TYPE.KF_LUCK_ROLLER,config,"seq")
    for i=1,8 do
        self.ItemList[i]:SetData(self.act_config[i])
    end
    self.Grid:SetData(DataHelper.FormatItemList(item_list))
    self.Time:CreateActTime(ACTIVITY_TYPE.KF_LUCK_ROLLER,TimeType.Type_Special_4)
end

function ActLuckRollerView:FlushRollerView()
    if not self.BtnInter.Interactable then
        return
    end
    self.last_num = self.data:GetLuckyTurntableInfo().last_num or 0
    UH.SetText(self.Times,Format(Language.OpenAct.LuckRollerTimes,self.last_num))
    if self.task_config == nil then
        self:InitTaskListData()
    end
    self.TaskList:SetData(self.task_config)
end

function ActLuckRollerView:InitTaskListData()
    self.task_config = {}
    for k,v in pairs(Config.lucky_turntable_auto.turntable_parameter) do
        if RoleData.Instance:GetRoleLevel() >= v.level_appear then
            table.insert(self.task_config,v)
        end
    end
    table.sort(self.task_config,function(a,b) return a.level_appear < b.level_appear end)
end

function ActLuckRollerView:FlushIndexrView()
    local index,config = self:GetSelectIndex()
    self.item_list = {Item.New(config.reward_item)}
    if self.BtnCheck.activeSelf then
        self.ItemList[self.index]:SetActive(false)
        self.ItemList[index]:SetActive(true)
        local angle = -45 * index + 22.5
        local ang = Quaternion.AngleAxis(angle, Vector3.up)
        ang.z = ang.y
        ang.y = 0
        UH.LocalRotate(self.Pointer,ang)
        self.cur_turn = index
        self:ShowCustom()
    end
    self.index = index
end

function ActLuckRollerView:GetSelectIndex()
    local index = self.data.luck_roller_data.index or 0
    for k,v in pairs(self.act_config) do
        if v.seq == index then
            return k,v
        end
    end
end

function ActLuckRollerView:OnClickCheck()
    self.BtnCheck:SetActive(not self.BtnCheck.activeSelf)
    self.data:SetLuckyTurntableSave(self.BtnCheck.activeSelf)
end

--========================动画逻辑===========================
function ActLuckRollerView:OnPlayAnimation()
    self.item_list = nil
    if self.last_num == 0 then
        PublicPopupCtrl.Instance:Error(Language.OpenAct.OpenRoller.NotTimesTip[2])
        return
    end
    if self.BtnCheck.activeSelf then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_LUCK_ROLLER,3)
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

function ActLuckRollerView:UpdataRollerTime()
	if self.turn_timer >= self.turn_time then
        if self.onClick then
			self:FlushCellListView(self.turn_time / 1.5)
			self.updata_count = self.updata_count + 1
            if self.updata_count >= 45 then
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_LUCK_ROLLER,1)
				self.turn_time = 0.3
				self.onClick = false
			end
		elseif math.abs(self.cur_turn - self.index) ~= 0  then
			self:FlushCellListView(self.turn_time)
        else
            self:ShowCustom()
            TimeHelper:CancelTimer(self.play_time)
            self:SetButtonEnabled(true)
            self:FlushRollerView()
		end
	end
	self.turn_timer = self.turn_timer + 0.03
end

function ActLuckRollerView:FlushCellListView(value)
	self.ItemList[self.cur_turn]:SetActive(false)
	self.cur_turn = self.cur_turn + 1
	if self.cur_turn > 8 then
		self.cur_turn = 1
    end
    self.Pointer:Rotate(0,0,-45)
    self.ItemList[self.cur_turn]:SetActive(true)
	self.turn_time = math.max(value, 0.001)
	self.turn_timer = 0
end

function ActLuckRollerView:SetButtonEnabled(is_enable)
    self.BtnInter.Interactable = is_enable
    self.CheckInter.Interactable = is_enable
end

function ActLuckRollerView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.play_time)
    self:ShowCustom()
end

function ActLuckRollerView:ShowCustom()
    if self.item_list then
        PublicPopupCtrl.Instance:ShowCustom(self.item_list)
        self.item_list = nil
    end
    MainOtherCtrl.Instance.pause = false
    MainOtherCtrl.Instance:RenewKeyUseView()
end

function ActLuckRollerView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(164)
end

----------------------------ActLuckRollerItem----------------------------
ActLuckRollerItem = ActLuckRollerItem or DeclareMono("ActLuckRollerItem", UIWidgetBaseItem)

function ActLuckRollerItem:SetData(data)
    self.Call:SetData(Item.New(data.reward_item))
    UIWidgetBaseItem.SetData(self, data)
end

function ActLuckRollerItem:SetActive(active)
    self.Select:SetActive(active)
end

----------------------------ActLuckRollerTaskItem----------------------------
ActLuckRollerTaskItem = ActLuckRollerTaskItem or DeclareMono("ActLuckRollerTaskItem", UIWidgetBaseItem)

function ActLuckRollerTaskItem:SetData(data)
    local progress = ServerActivityData.Instance:GetLuckyTurntableProgress(data.type)
    progress = progress > data.parameter and data.parameter or progress

    if progress == data.parameter then
        local progress_str = ColorStr(Format("(%s/%s)",progress,data.parameter),COLORSTR.Green11)
        UH.SetText(self.Desc,data.describe .. progress_str)
    else
        UH.SetText(self.Desc,data.describe .. Format("(%s/%s)",ColorStr(progress,COLORSTR.Green11),data.parameter))
    end

    self.BtnInter.Interactable = RoleData.Instance:GetRoleLevel() >= data.level_appear
    if not self.BtnInter.Interactable then
        local act_cfg = ActivityData.GetConfig(data.act_type)
        UH.SetText(self.Tips,Format(Language.Activity.ActItem.LevelLimit,data.level_appear))
    else
        self.Complete:SetActive(ServerActivityData.Instance:GetLuckyTurntableFlag(data.type) == 1)
    end
    self.Tips:SetObjActive(not self.BtnInter.Interactable)
    UIWidgetBaseItem.SetData(self, data)
end

function ActLuckRollerTaskItem:OnClickEnter()
    if not self.BtnInter.Interactable or RoleData.Instance:GetRoleLevel() < self.data.level_appear then
        PublicPopupCtrl.Instance:Center(Format(Language.WST.level_tips,self.data.level_appear))
        return
    end
    if self.data.act_type ~= "" then
        if ActivityData.Instance:GetActivityInfo(self.data.act_type) then
            ActivityData.Instance:GetActivityInfo(self.data.act_type):DoClickHandle()
        end
    else
        if self.data.open_panel ~= "" then
            ViewMgr:OpenViewByKey(self.data.open_panel)
        end
    end
    ViewMgr:CloseView(ServerActivityView)
end