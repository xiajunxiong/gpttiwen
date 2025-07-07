TigerGreatLuckView = TigerGreatLuckView or DeclareView("TigerGreatLuckView", "serveractivity/tiger_year/tiger_great_luck", Mod.TigerYear.TigerGreatLuck)
function TigerGreatLuckView:TigerGreatLuckView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_TIGER_GREAT_LUCK, TimeType.Type_Special_4)
end

function TigerGreatLuckView:LoadCallback()
    
end

function TigerGreatLuckView:OnCloseCllick()
    ViewMgr:CloseView(TigerGreatLuckView)
end

function TigerGreatLuckView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[219].desc)
end

--------------------TigerGreatLuckPanel--------------------
TigerGreatLuckPanel = TigerGreatLuckPanel or DeclareMono("TigerGreatLuckPanel", UIWFlushPanel)
function TigerGreatLuckPanel:TigerGreatLuckPanel()
    self.Data = TigerGreatLuckData.Instance
    self.reward_config = self.Data:GetRewardConfig()
    self.reward_group_config = self.Data:GetRewardGroupConfig()
    self.other = Config.huxingdayun_auto.other[1]
    self.language = Language.TigerGreatLuck
    self.data = self.Data.tiger_great_luck_data
    self.data_cares = {
        {data = self.data, func = self.FlushShow, keys = {"info"}, init_call = false},
        {data = self.data, func = self.FlushRightDownBtn, keys = {"invited_info"}, init_call = false},
    }
end

function TigerGreatLuckPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushShow()
end

function TigerGreatLuckPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.person_handler)
    Runner.Instance:RemoveRunUntilTrue(self.time_handler)
end

function TigerGreatLuckPanel:FlushShow()
    self:FlushRoadAndHouse()
    self:FlushPerson()
    self:FlushLeftDownBtn()
    self:FlushRightDownBtn()
end

function TigerGreatLuckPanel:FlushRoadAndHouse()
    for i = 1, self.HouseList:Length() do
        self.HouseList[i]:SetData(self.reward_config[i])
    end
    for i = 1, self.RoadPosAct:Length() do
        self.RoadPosAct[i]:SetActive(self.data.info.finish_pos >= (i - 1))
    end
end

function TigerGreatLuckPanel:FlushPerson()
    TimeHelper:CancelTimer(self.person_handler)
    if self.data.info.is_can_receive > 0 then
        -- 这里加+2是因为起点也算进去了
        UH.LocalPosG(self.ObjPerson, self.Target[self.data.info.finish_pos + 2].anchoredPosition)
        self.ObjStay:SetActive(true)
        self.ObjPerson.transform.localRotation = Vector3.New(0, 0, 0)
    else
        if self.data.info.finish_pos > self.other.invite_times then
            LogError("配置有问题",self.data)
            return
        end
        -- 判断是否正在走
        if self.data.info.invite_time > 0 then
            self.now_use_road = self.Data:GetRoadInfo(self.data.info.finish_pos + 1, self["Path" .. self.data.info.finish_pos + 1])
            self.person_handler = TimeHelper:AddCountDownTT(
                BindTool.Bind(self.UpdatePerson, self),
                function()
                    self.ObjStay:SetActive(true)
                    UH.LocalPosG(self.ObjPerson, self.now_use_road.end_pos)
                    self.ObjPerson.transform.localRotation = Vector3.New(0, 0, 0)
                end,
                self.data.info.invite_time,
                1)
        else
            self.ObjStay:SetActive(true)
            UH.LocalPosG(self.ObjPerson, self.Target[self.data.info.finish_pos + 1].anchoredPosition)
            self.ObjPerson.transform.localRotation = Vector3.New(0, 0, 0)
        end
    end
end

function TigerGreatLuckPanel:UpdatePerson()
    self.ObjStay:SetActive(false)
    local now_time = math.floor(TimeManager.GetServerTime())
    local start_time = self.data.info.invite_time - self.other.consume_times
    local percent = (now_time - start_time) / self.other.consume_times
    for i = 1, #self.now_use_road.path do
        local v = self.now_use_road.path[i]
        if percent > v.start_percent and percent <= v.end_percent then
            percent = (percent - v.start_percent) / v.total_percent
            local now_pos = Vector2.New((v.end_pos.x - v.start_pos.x) * percent + v.start_pos.x, (v.end_pos.y - v.start_pos.y) * percent + v.start_pos.y)
            UH.LocalPosG(self.ObjPerson, now_pos)
            local rotation = Vector3.New(0, 180, 0)
            if v.start_pos.x <= v.end_pos.x then
                rotation.y = 0
            end
            self.ObjPerson.transform.localRotation = rotation
            break
        end
    end
    self.now_use_path = 1
end

-- 用于测试路径
-- function TigerGreatLuckPanel:TestMove(path)
--     self.pos_index = 1
--     self.now_path = self["Path"..path]
--     UH.LocalPosG(self.ObjPerson, self.now_path[self.pos_index].anchoredPosition)
--     self.ObjStay:SetActive(false)
--     self:Move()
-- end

-- function TigerGreatLuckPanel:Move()
--     Runner.Instance:RemoveRunUntilTrue(self.time_handler)
--     self.pos_index = self.pos_index + 1
--     if self.pos_index <= self.now_path:Length() then
--         local rotation = Vector3.New(0, 180, 0)
--         if self.ObjPerson.transform.localPosition.x <= self.now_path[self.pos_index].anchoredPosition.x then
--             rotation.y = 0
--         end
--         self.ObjPerson.transform.localRotation = rotation
--         -- 设置目标移动位置
--         self.time_handler = UH.MoveToTarget(self.ObjPerson, self.now_path[self.pos_index].anchoredPosition, 1, BindTool.Bind(self.Move, self))
--     else
--         self.ObjStay:SetActive(true)
--     end
-- end

function TigerGreatLuckPanel:FlushLeftDownBtn()
    self.InviteTime:CloseCountDownTime()
    if self.data.info.invite_time > 0 then
        self.InviteTime:StampTime(self.data.info.invite_time)
    end
    self.InviteTime:SetObjActive(self.data.info.invite_time > 0)
    self.InviteRemind:SetNum(self.Data:DownLeftBtnRemind())
    local can_invite_times = self.other.invite_times - self.data.info.finish_pos
    if self.data.info.invite_time > 0 or self.data.info.is_can_receive > 0 then
        can_invite_times = can_invite_times - 1
    end
    UH.SetText(self.TxtInviteTimes, Format(self.language.Times, can_invite_times > 0 and can_invite_times or 0, self.other.invite_times))
    self.IHandlerInvite.Interactable = can_invite_times > 0 or self.data.info.invite_time > 0

    if ViewMgr:IsOpen(TigerGreatLuckInviteView) and self.Data:DownLeftBtnRemind() == 0 then
        ViewMgr:CloseView(TigerGreatLuckInviteView)
    end
end

function TigerGreatLuckPanel:FlushRightDownBtn()
    self.InvitedTime:CloseCountDownTime()
    if self.data.info.invited_time > 0 then
        self.InvitedTime:StampTime(self.data.info.invited_time)
    end
    self.InvitedTime:SetObjActive(self.data.info.invited_time > 0)
    self.InvitedRemind:SetNum(self.Data:DownRightBtnRemind())
    local can_invited_times = self.other.be_invite_times - self.data.info.invited_times
    UH.SetText(self.TxtInvitedTimes, Format(self.language.Times, can_invited_times > 0 and can_invited_times or 0, self.other.be_invite_times))

    UH.SetText(self.TxtInvitedFriend, self.data.info.is_can_receive_invited == 1 and self.language.Receive or self.language.InvitedFriend)
    self.IHandlerInvited.Interactable = can_invited_times > 0 or self.data.info.invited_time > 0 or self.data.info.is_can_receive_invited > 0

    if ViewMgr:IsOpen(TigerGreatLuckInvitedView) and self.Data:DownRightBtnRemind() == 0 then
        ViewMgr:CloseView(TigerGreatLuckInvitedView)
    end
end

function TigerGreatLuckPanel:OnInviteClick()
    if self.data.info.invite_time > 0 then
        PublicPopupCtrl.Instance:Center(self.language.Tip[1])
        return
    end
    if self.data.info.is_can_receive > 0 then
        PublicPopupCtrl.Instance:Center(self.language.Tip[2])
        return
    end
    local can_invite_times = self.other.invite_times - self.data.info.finish_pos
    can_invite_times = self.data.info.invite_time > 0 and can_invite_times - 1 or can_invite_times
    if can_invite_times <= 0 then
        PublicPopupCtrl.Instance:Center(self.language.Tip[3])
        return
    end
    ViewMgr:OpenView(TigerGreatLuckInviteView)
end

function TigerGreatLuckPanel:OnInvitedClick()
    if self.data.info.invited_time > 0 then
        PublicPopupCtrl.Instance:Center(self.language.Tip[1])
        return
    end
    if self.data.info.is_can_receive_invited > 0 then
        TigerGreatLuckCtrl.Instance:Send(4)
        return
    end
    local can_invited_times = self.other.be_invite_times - self.data.info.invited_times
    if can_invited_times <= 0 then
        PublicPopupCtrl.Instance:Center(self.language.Tip[3])
        return
    end
    ViewMgr:OpenView(TigerGreatLuckInvitedView)
end

--------------------TigerGreatLuckHouseItem--------------------
TigerGreatLuckHouseItem = TigerGreatLuckHouseItem or DeclareMono("TigerGreatLuckHouseItem", UIWidgetBaseItem)
function TigerGreatLuckHouseItem:TigerGreatLuckHouseItem()
    self.is_first = true
end

function TigerGreatLuckHouseItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.is_first then
        self.is_first = false
        UH.SetText(self.TxtName, data.place_name)
    end
    local info = TigerGreatLuckData.Instance:GetInfo()
    local remind = info.finish_pos == data.seq and info.is_can_receive > 0
    self.Remind:SetNum(remind and 1 or 0)
end

function TigerGreatLuckHouseItem:OnHouseClick()
    local info = TigerGreatLuckData.Instance:GetInfo()
    if info.finish_pos == self.data.seq and info.is_can_receive > 0 then
        TigerGreatLuckCtrl.Instance:Send(3)
    else
        ViewMgr:OpenView(TigerGreatLuckRewardView, self.data)
    end
end

-------------------TigerGreatLuckRewardView-------------------
TigerGreatLuckRewardView = TigerGreatLuckRewardView or DeclareView("TigerGreatLuckRewardView", "serveractivity/tiger_year/tiger_great_luck_reward")
VIEW_DECLARE_MASK(TigerGreatLuckRewardView, ViewMask.BgBlock)
function TigerGreatLuckRewardView:TigerGreatLuckRewardView()
    
end

function TigerGreatLuckRewardView:LoadCallback(param)
    local reward_config = TigerGreatLuckData.Instance:GetHouseShowReward(param)
    for i = 1, self.List:Length() do
        self.List[i]:SetData(reward_config[i])
    end
    self.ObjGeted:SetActive(TigerGreatLuckData.Instance:GetFirstFlag(param.seq) > 0)
    UH.SetText(self.TxtName, param.place_name)
end

function TigerGreatLuckRewardView:OnCloseClick()
    ViewMgr:CloseView(TigerGreatLuckRewardView)
end

--------------------TigerGreatLuckCellItem--------------------
TigerGreatLuckCellItem = TigerGreatLuckCellItem or DeclareMono("TigerGreatLuckCellItem", UIWidgetBaseItem)
function TigerGreatLuckCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.item))
    UH.SpriteName(self.ImgSetItemTip, "BiaoQian" .. data.m_type)
    UH.SetText(self.TxtItemTip, Language.TigerGreatLuck.ItemTip[data.m_type])
end

-------------------TigerGreatLuckInviteView-------------------
TigerGreatLuckInviteView = TigerGreatLuckInviteView or DeclareView("TigerGreatLuckInviteView", "serveractivity/tiger_year/tiger_great_luck_invite")
VIEW_DECLARE_MASK(TigerGreatLuckInviteView, ViewMask.BgBlock)
function TigerGreatLuckInviteView:TigerGreatLuckInviteView()
    
end

function TigerGreatLuckInviteView:LoadCallback()
    self.List:SetData(SocietyData.Instance:GetFriendListByType(SocietyConfig.FriendGroupType.friend, true, true))
end

function TigerGreatLuckInviteView:OnCloseClick()
    ViewMgr:CloseView(TigerGreatLuckInviteView)
end

-------------------TigerGreatLuckInvitedView-------------------
TigerGreatLuckInvitedView = TigerGreatLuckInvitedView or DeclareView("TigerGreatLuckInvitedView", "serveractivity/tiger_year/tiger_great_luck_invited")
VIEW_DECLARE_MASK(TigerGreatLuckInvitedView, ViewMask.BgBlock)
function TigerGreatLuckInvitedView:TigerGreatLuckInvitedView()
    self.data = TigerGreatLuckData.Instance.tiger_great_luck_data
    self.ht = self.data:Care(BindTool.Bind(self.FlushShow, self), "invited_info")
end

function TigerGreatLuckInvitedView:LoadCallback()
    self:FlushShow()
end

function TigerGreatLuckInvitedView:FlushShow()
    self.List:SetData(TigerGreatLuckData.Instance:GetInvitedInfo().list)
end

function TigerGreatLuckInvitedView:OnCloseClick()
    ViewMgr:CloseView(TigerGreatLuckInvitedView)
end

function TigerGreatLuckInvitedView:CloseCallback()
    self.data:Uncare(self.ht)
end

-------------------TigerGreatLuckInviteItem-------------------
TigerGreatLuckInviteItem = TigerGreatLuckInviteItem or DeclareMono("TigerGreatLuckInviteItem", UIWidgetBaseItem)
function TigerGreatLuckInviteItem:TigerGreatLuckInviteItem()
    self.is_first = true
end

function TigerGreatLuckInviteItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Avatar:SetAvatar(data.info.avatar_type, data.info.avatar_id)
    UH.SetText(self.TxtName, data.info.name)
    UH.SpriteName(self.ImgSetProf, RoleData.GetProfSp(data.info.prof))
    self:FlushBtn()
    self.is_first = false
end

function TigerGreatLuckInviteItem:FlushBtn()
    self.IHandler.Interactable = self.is_first
    UH.SetText(self.TxtInvite, Language.TigerGreatLuck.Invite[self.is_first and 1 or 2])
end

function TigerGreatLuckInviteItem:OnInviteClick()
    TigerGreatLuckCtrl.Instance:Send(1, self.data.info.uid)
    self:FlushBtn()
end

-------------------TigerGreatLuckInvitedItem-------------------
TigerGreatLuckInvitedItem = TigerGreatLuckInvitedItem or DeclareMono("TigerGreatLuckInvitedItem", UIWidgetBaseItem)
function TigerGreatLuckInvitedItem:TigerGreatLuckInvitedItem()
    self.is_first = true
end

function TigerGreatLuckInvitedItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Avatar:SetAvatar(data.avatar_type, data.headshot_id)
    UH.SetText(self.TxtName, data.role_name)
    UH.SpriteName(self.ImgSetProf, RoleData.GetProfSp(data.prof_id))
    self:FlushBtn()
    self.is_first = false
end

function TigerGreatLuckInvitedItem:FlushBtn()
    self.IHandler.Interactable = self.is_first
    UH.SetText(self.TxtInvited, Language.TigerGreatLuck.Invited[self.is_first and 1 or 2])
end

function TigerGreatLuckInvitedItem:OnInvitedClick()
    TigerGreatLuckCtrl.Instance:Send(2, self.data.index)
    self:FlushBtn()
end