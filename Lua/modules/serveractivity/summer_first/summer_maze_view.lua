SummerMazeView = SummerMazeView or DeclareView("SummerMazeView", "serveractivity/summer_first/summer_maze", Mod.SummerFirst.SummerMaze)
VIEW_DECLARE_LEVEL(SummerMazeView, ViewLevel.L1)
VIEW_DECLARE_MASK(SummerMazeView, ViewMask.BgBlock)
function SummerMazeView:SummerMazeView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SUMMER_MAZE, TimeType.Type_Time_3)
end

function SummerMazeView:OnCloseClick()
    self.ObjPerson:SetActive(false)
    ViewMgr:CloseView(SummerMazeView)
end

function SummerMazeView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[134].desc)
end

function SummerMazeView:OnItemClick()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.SummerMazeTicket]})})
end


----------------------------SummerMazePanel----------------------------
SummerMazePanel = SummerMazePanel or DeclareMono("SummerMazePanel", UIWFlushPanel)
function SummerMazePanel:SummerMazePanel()
    self.Data = SummerMazeData.Instance
    self.game_info = self.Data.game_info
    self.other = Config.cool_summer_auto.other[1]
    self.has_num = 0
    self.consume_num = 0
    self.language = Language.SummerMaze
    self.Data.is_walking = false
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushByItem, init_call = true},
        {data = self.Data.game_info, func = self.FlushByItem, keys = {"is_playing"}, init_call = false},
        {data = self.Data.all_map_info, func = self.UpdateAllMapInfo, init_call = false},
        {data = self.Data.now_role_pos_info, func = self.MoveFunc, keys = {"index"}, init_call = false},
        {data = self.Data.question_info, func = self.StartQuestion, keys = {"flag"}, init_call = false},
        {data = self.Data.shop_info, func = self.OpenShop, init_call = false},
    }

    -- 这块只是把item创建好
    self.AllHouseList:SetCompleteCallBack(BindTool.Bind(function()
        self.Data:DealAllHouseList(self.AllHouseList)
        self.Data:DealAllCanMovePosAndGroupPos()
        UH.LocalPosG(self.ObjPerson, self.Data:GetCanMovePos(self.Data.now_role_pos_info.index))
    end))
    local t = self.Data:GetAllHouseListData()
    -- 设置数据的时候倒序设置，之后通过id拿到对应的item也要经过转换
    self.AllHouseList:SetData(DataHelper.ReverseList(t))
    self.RectHouseGroupItems.sizeDelta = Vector2.New(0, 508 * #t)

    -- 格子信息不足就要请求一遍全部信息，切换格子也会重置
    if table.nums(self.Data.all_map_info.info) ~= self.Data.row2 * self.Data.col then
        self.Data:SendInfo(0)
    else
        TimeHelper:CancelTimer(self.update_delay_handle)
        self.update_delay_handle = TimeHelper:AddDelayTimer(function()
            self:UpdateAllMapInfo()
        end, 0.1)
    end
    TimeHelper:CancelTimer(self.person_delay_handle)
    self.person_delay_handle = TimeHelper:AddDelayTimer(function()
        self.ObjPerson:SetActive(true)
    end, 0.5)
end

function SummerMazePanel:Awake()
    UIWFlushPanel.Awake(self)
end

function SummerMazePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Data:ClearData()
    Runner.Instance:RemoveRunUntilTrue(self.time_handler)
    TimeHelper:CancelTimer(self.update_delay_handle)
    TimeHelper:CancelTimer(self.person_delay_handle)
end

function SummerMazePanel:JudgeIsPlaying()
    self.IHStartBtn.Interactable = self.Data.game_info.is_playing == 0 and self.has_num >= self.consume_num
    self.StartRemind:SetNum(self.Data:StartRemind())
    self.ObjBlock:SetActive(self.Data.game_info.is_playing == 0)
    self.ObjStart:SetActive(self.Data.game_info.is_playing == 0)
    self.ObjGiveUp:SetActive(self.Data.game_info.is_playing == 1)
    self.TaskPanel:OnTaskCaseClick(self.Data.game_info.is_playing == 1)
end

-- 根据服务端下发的所有地图信息才来更新全部
function SummerMazePanel:UpdateAllMapInfo()
    if next(self.Data.all_map_info.info) == nil then
        self.Data:ResetAllHouse()
        return
    end
    self.Data:FlushAllMapInfo()
    self.ScrollRectHouseList:StopMovement()
    local row = self.Data:GetHouseDataById(self.Data.now_role_pos_info.index).row
    UH.AnchoredPosition(self.RectContent, self.Data.all_house_group_list_pos[row])
end

-- 消耗门票入场开始
function SummerMazePanel:OnStartClick()
    if self.has_num >= self.consume_num then
        self.Data:SendInfo(1)
    else
        PublicPopupCtrl.Instance:Center(self.language.ItemNotEnought)
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.SummerMazeTicket]})})
    end
end

-- 放弃玩法退出
function SummerMazePanel:OnGiveUpClick()
    if self.Data.game_info.is_playing == 1 then
        PublicPopupCtrl.Instance:AlertTip(
            self.language.ExitTip, function()
                self.Data:SendInfo(7)
                self.Data:ResetAllHouse()
                self.ScrollRectHouseList:StopMovement()
                UH.AnchoredPosition(self.RectContent, Vector2.New(0, -750))
            end)
    end
end

-- 角色位置改变开始移动
function SummerMazePanel:MoveFunc()
    if self.Data.is_walking then
        return
    end
    if self.Data.now_role_pos_info.last == self.Data.now_role_pos_info.index then
        UH.LocalPosG(self.ObjPerson, self.Data:GetCanMovePos(self.Data.now_role_pos_info.index))
        self.AnimPerson:SetBool("walk", false)
    else
        self:StartMove()
    end
end

function SummerMazePanel:StartMove()
    self.Data.is_walking = true
    self.AnimPerson:SetBool("walk", true)
    -- 计算路径
    self.Data:CalculatePath()
    if next(self.Data.way_path) == nil then
        LogError("无法移动到该格")
        self.Data.is_walking = false
        self.AnimPerson:SetBool("walk", false)
        return
    end

    -- 这里还要把第一个去掉，第一个是当前格子
    table.remove(self.Data.way_path, 1)
    -- 设置目标移动位置
    self:MoveNext()
end

function SummerMazePanel:MoveNext()
    Runner.Instance:RemoveRunUntilTrue(self.time_handler)
    if next(self.Data.way_path) ~= nil then
        local next_index = self.Data.way_path[1].index
        table.remove(self.Data.way_path, 1)
        local rotation = Vector3.New(0, 180, 0)
        if self.ObjPerson.transform.localPosition.x <= self.Data.all_can_click_pos[next_index].x then
            rotation.y = 0
        end
        self.ObjPerson.transform.localRotation = rotation
        Runner.Instance:RemoveRunUntilTrue(self.time_handler)
        -- 设置目标移动位置
        self.time_handler = UH.MoveToTarget(self.ObjPerson, self.Data.all_can_click_pos[next_index], self.Data.move_speed, BindTool.Bind(self.MoveNext, self))
    else
        if self.Data.is_click_explore then
            self.Data:GetHouseMonoById(self.Data.now_role_pos_info.index):SetForwardDoorClose()
            self.Data.is_click_explore = false
        end
        self.Data.is_walking = false
        self.AnimPerson:SetBool("walk", false)
    end
end

-- 收到协议开始答题
function SummerMazePanel:StartQuestion()
    if not ViewMgr:IsOpen(SummerMazeQuestionView) then
        ViewMgr:OpenView(SummerMazeQuestionView)
    end
end

-- 收到协议后再打开商店
function SummerMazePanel:OpenShop()
    if next(self.Data.shop_info.info) == nil then
        return
    end
    if not ViewMgr:IsOpen(SummerMazeShopView) then
        ViewMgr:OpenView(SummerMazeShopView)
    end
end

function SummerMazePanel:FlushByItem()
    self.has_num, self.consume_num = self.Data:GetHasAndConsume()
    UH.SetText(self.TxtConsume, DataHelper.ConsumeNum(self.has_num, self.consume_num, true))
    UH.SetText(self.TxtItemHas, self.has_num)
    self:JudgeIsPlaying()
end


----------------------------SummerMazeHourseGroup----------------------------
SummerMazeHourseGroup = SummerMazeHourseGroup or DeclareMono("SummerMazeHourseGroup", UIWidgetBaseItem)
function SummerMazeHourseGroup:SummerMazeHourseGroup(data)
    self.is_first = true
end

function SummerMazeHourseGroup:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.is_first then
        self:ShowRiver()
        self.is_first = false
    end
    self.HouseList:SetData(data.house)
end

function SummerMazeHourseGroup:ShowRiver()
    for i = 1, self.LeftRivers:Length() do
        local is_show = (self.data.row % SummerMazeData.Instance.river) == (i - 1)
        self.LeftRivers[i]:SetActive(is_show)
    end
end


----------------------------SummerMazeHourse----------------------------
SummerMazeHourse = SummerMazeHourse or DeclareMono("SummerMazeHourse", UIWidgetBaseItem)
function SummerMazeHourse:SummerMazeHourse(data)
    self.Data = SummerMazeData.Instance
    self.is_first = true
end

function SummerMazeHourse:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.is_first = self.Data.game_info.is_playing == 1
end

function SummerMazeHourse:OnDestroy()
    
end

function SummerMazeHourse:FlushAll()
    local house_info = self.Data:GetSingleMapInfo(self.data.house_id)
    local road_info = self.Data:GetSingleMapInfo(self.data.road_id)
    if house_info and road_info then
        -- 路解锁了就能点
        self.ObjRoadPos:SetActive(road_info.map_lock == SummerMazeMapLock.unlock)
        -- 房子解锁了并且还没进去挑战的时候才有挑战按钮
        self.ObjExplore:SetActive(house_info.map_lock == SummerMazeMapLock.unlock and house_info.map_status == SummerMazeMapStatus.normal)
        -- 房子是boss就不显示云了
        if house_info.map_type == SummerMazeMapType.boss then
            self.ObjCloudClose:SetActive(false)
            self.ObjCloudOpen:SetActive(false)
        else
            -- 房子正常就有云
            self.ObjCloudClose:SetActive(house_info.map_status == SummerMazeMapStatus.normal)
            if not self.is_first then
                self.ObjCloudOpen:SetActive(house_info.map_status ~= SummerMazeMapStatus.normal)
            end
        end
        if not self.Data.is_click_explore then
            self.AnimForwardDoor:SetBool("state", house_info.map_status == SummerMazeMapStatus.finish)
        end
        -- 后门如果是最后一行就永久不开
        if self.data.row ~= self.Data.row then
            self.AnimBackDoor:SetBool("state", house_info.map_status == SummerMazeMapStatus.finish)
        end
        UH.SetText(self.TxtMissionName, Language.SummerMaze.MissionType[house_info.map_type])
        UH.SpriteName(self.ImgSetMission, "Mission_" .. house_info.map_type)
        -- 商店即使完成了也不置灰
        if house_info.map_lock == SummerMazeMapLock.unlock then
            if house_info.map_type == SummerMazeMapType.shop then
                self.IHandleMission.Interactable = not (house_info.map_status == SummerMazeMapStatus.normal)
            else
                self.IHandleMission.Interactable = house_info.map_status == SummerMazeMapStatus.doing
            end
        else
            self.IHandleMission.Interactable = false
        end
    else
        self:ResetHouse()
    end
    self.is_first = false
end

function SummerMazeHourse:ResetHouse()
    self.ObjRoadPos:SetActive(false)
    self.ObjExplore:SetActive(false)
    self.ObjCloudClose:SetActive(true)
    self.ObjCloudOpen:SetActive(false)
    self.AnimForwardDoor:SetBool("state", false)
    self.AnimBackDoor:SetBool("state", false)
    UH.SetText(self.TxtMissionName, Language.SummerMaze.MissionType[1])
    UH.SpriteName(self.ImgSetMission, "Mission_1")
    self.IHandleMission.Interactable = false
end

function SummerMazeHourse:OnRoadClick()
    if not self.Data:JudgeIsCanWalk() then
        return
    end
    local row, col = self.Data:IndexToRC(self.data.road_id)
    self.Data:SendInfo(2, col, row)
end

function SummerMazeHourse:SetForwardDoorClose()
    self.AnimForwardDoor:SetBool("state", false)
    self.Data.is_click_explore = false
end

function SummerMazeHourse:OnExploreClick()
    if not self.Data:JudgeIsCanWalk() then
        return
    end
    self.AnimForwardDoor:SetBool("state", true)
    self.ObjExplore:SetActive(false)
    -- 商店是特殊的，不会关门
    if self.Data:GetSingleMapInfo(self.data.house_id).map_type ~= SummerMazeMapType.shop then
        self.Data.is_click_explore = true
    end
    local row, col = self.Data:IndexToRC(self.data.house_id)
    self.Data:SendInfo(2, col, row)
end

function SummerMazeHourse:OnMissionClick()
    if self.Data.is_walking then
        PublicPopupCtrl.Instance:Center(Language.SummerMaze.MoveTip)
        return
    end
    local map_type = self.Data:GetSingleMapInfo(self.data.house_id).map_type
    -- 组队状态下不能玩
    if TeamCtrl.Instance:Data():InTeam() then
        PublicPopupCtrl.Instance:Center(Language.SummerMaze.InTeamTip)
        return
    end
    -- 商店是可以点的
    if map_type == SummerMazeMapType.shop then
        -- 如果在锁着的房间就不能点商店
        local now_house_info = self.Data:GetSingleMapInfo(self.Data.now_role_pos_info.index)
        if now_house_info.map_status == SummerMazeMapStatus.doing then
            PublicPopupCtrl.Instance:Center(Language.SummerMaze.LockTip)
        else
            local row, col = self.Data:IndexToRC(self.data.house_id)
            self.Data:SendInfo(2, col, row)
        end
    end
    if self.data.house_id == self.Data.now_role_pos_info.index then
        self.Data:SendInfo(3)
    end
end


----------------------------SummerMazeTaskPanel----------------------------
SummerMazeTaskPanel = SummerMazeTaskPanel or DeclareMono("SummerMazeTaskPanel",UIWFlushPanel)
function SummerMazeTaskPanel:SummerMazeTaskPanel()
    self.Data = SummerMazeData.Instance
    self.act_mission = self.Data.act_mission
    self.task_info_off = false
    self.data_cares = {
        {data = self.act_mission, func = self.FlushPanel, init_call = true},
    }
end

function SummerMazeTaskPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function SummerMazeTaskPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function SummerMazeTaskPanel:FlushPanel()
    self.TaskList:SetData(self.Data:GetActMissionData())
    local remind_num = self.Data:GetActMissionRemind()
    self.RedPoint:SetNum(remind_num)
end

function SummerMazeTaskPanel:OnTaskCaseClick(value)
    if value ~= nil then
        if self.task_info_off == value then
            return
        end
        self.task_info_off = not value
    end
    if not self.task_info_off then
        self.TaskAnimation:Play("flower_lamp_task_info_off")
        self.task_info_off = true
    else
        self.task_info_off = false
        self.TaskAnimation:Play("flower_lamp_task_info_on")
    end
end


----------------------------SummerMazeTaskItem----------------------------
SummerMazeTaskItem = SummerMazeTaskItem or DeclareMono("SummerMazeTaskItem",UIWidgetBaseItem)
function SummerMazeTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.can_receive = data.now_param == data.param_1
    self.is_receive = data.is_receive > 0
    local color = self.can_receive and COLORSTR.Green9 or COLORSTR.Red20
    local progress_str = ColorStr(string.format("(%s/%s)", data.now_param, data.param_1), color)
    UH.SetText(self.Desc, string.format("%s %s", data.task_desc, progress_str))
    self.GetBtnObj:SetActive(not self.is_receive)
    self.GetedObj:SetActive(self.is_receive)
    self.Interactor.Interactable = self.can_receive
    self.RewardCell:SetData(Item.Create(data.reward_item))
end

function SummerMazeTaskItem:GetClick()
    SummerMazeData.Instance:SendInfo(5, self.data.task_index)
end