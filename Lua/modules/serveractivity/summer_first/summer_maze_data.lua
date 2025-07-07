SummerMazeData = SummerMazeData or BaseClass()

SummerMazeMapType = {
    road = 0,
    boss = 1,
    monster = 2,
    shop = 3,
    question = 4,
}

SummerMazeMapStatus = {
    normal = 0,
    doing = 1,
    finish = 2,
}

SummerMazeMapLock = {
    lock = 0,
    unlock = 1,
}

function SummerMazeData:__init()
	if SummerMazeData.Instance ~= nil then
		Debuger.LogError("[SummerMazeData] attempt to create singleton twice!")
		return
	end
    SummerMazeData.Instance = self
    -- 行和列从0开始算
    self.col = 3
    self.row = 5
    -- 算上路算10行
    self.row2 = 10
    self.total = self.row * self.col
    self.total2 = self.row2 * self.col
    self.move_speed = 8
    -- 河道种类
    self.river = 3
    -- 这里存储了道路位置和房子位置，下标从0开始
    self.all_can_click_pos = nil
    -- 用于房子组跳转位置，下标从0开始
    self.all_house_group_list_pos = nil
    -- 所有地图格子信息下发，数组从0开始
    self.all_map_info = SmartData.New({info = {}})
    -- 当前角色位置，从0开始，到29
    self.now_role_pos_info = SmartData.New({index = 0, last = 0})
    -- 商店信息
    self.shop_info = SmartData.New({info = {}})
    -- 答题信息
    self.question_info = SmartData.New({id = 0, flag = false})
    -- 游戏状态
    self.game_info = SmartData.New({is_playing = 0, play_times = 0})

    -- 活动任务数据
    self.act_mission = SmartData.New({task_info = {}, type_info = {}})
    
    self:ResetData()
end

function SummerMazeData:__delete()
	SummerMazeData.Instance = nil
end

function SummerMazeData:ResetData()
    self.all_map_info.info = {}
    self.now_role_pos_info.index = 0
    self.now_role_pos_info.last = 0
    self.shop_info.info = {}
    self.question_info.id = 0
    self.game_info.is_playing = 0
    self.game_info.play_times = 0

    self.act_mission.task_info = {}
    self.act_mission.type_info = {}
    
    self.mission_tab = nil
    self.all_house_list = nil
    -- 记录房子和路对应的id房子mono的index（两边都是从0开始）
    self.find_house_mono_array = {}
    
    self:ClearData()
end

function SummerMazeData:ClearData()
    -- 房子组mono，从1开始
    self.house_group_mono = {}
    -- 房子mono，从0开始
    self.house_mono = {}
    -- 路径存放
    self.way_path = {}
    -- A*用到的节点信息
    self.all_path_node = {}
    self.is_walking = false
    -- 是否点击了探索
    self.is_click_explore = false
end

function SummerMazeData:GetActMissionData()
    self.mission_tab = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SUMMER_MAZE, Config.cool_summer_auto.maze_task, "type")
    for k, v in pairs(self.mission_tab) do
        v.now_param = self:GetActMissionNowParam(v.task)
        v.now_param = v.now_param > v.param_1 and v.param_1 or v.now_param
        v.task_index, v.is_receive = self:GetActMissionIsReceived(v.type)
    end
    table.sort(self.mission_tab, DataHelper.WeightSort("type", function(data)
        if data.is_receive == 1 then
            return 1000
        end
        if data.now_param == data.param_1 then
            return -1000
        end
    end))
    return self.mission_tab
end

-- 获取商店配置
function SummerMazeData:GetShopData()
    local t = {}
    for i, v in ipairs(self.shop_info.info) do
        for k, w in pairs(Config.cool_summer_auto.maze_shop_group) do
            if w.shop_type == v.shop_id and w.index == v.seq then
                v.shop_info = w
                v.server_index = i - 1
                table.insert(t, v)
            end
        end
    end
    table.sort(t, DataHelper.WeightSort("server_index", function(data)
		return (data.shop_info.buy_times - data.buy_times) > 0 and 0 or 1000
	end))
	-- 排序后要重新赋值item_index
	for i, v in ipairs(t) do
		v.item_index = i
	end
    return t
end

-- 根据任务类型获得对应进度
function SummerMazeData:GetActMissionNowParam(task)
    if next(self.act_mission.type_info) ~= nil then
        return self.act_mission.type_info[task] or 0
    end
    return 0
end

-- 根据任务id获得是否已领取奖励
function SummerMazeData:GetActMissionIsReceived(m_type)
    if next(self.act_mission.task_info) ~= nil then
        for k, v in pairs(self.act_mission.task_info) do
            if v.task_id == m_type then
                return k - 1, v.is_get_reward
            end
        end
    end
    return 0, 0
end

-- 任务红点
function SummerMazeData:GetActMissionRemind()
    if not self.mission_tab then
        self.mission_tab = self:GetActMissionData()
    end
    for k, v in pairs(self.mission_tab) do
        if v.now_param == v.param_1 and v.is_receive == 0 then
            return 1
        end
    end
    
    return 0
end

-- 是否可以入场红点
function SummerMazeData:StartRemind()
    -- 如果在游戏中则不显示红点
    if self.game_info.is_playing == 1 then
        return 0
    else
        local has_num = 0
        local consume_num = 0
        has_num, consume_num = self:GetHasAndConsume()
        return has_num >= consume_num and 1 or 0
    end
end

function SummerMazeData:TotalRemind()
    if self:GetActMissionRemind() == 1 then
        return 1
    elseif self:StartRemind() == 1 then
        return 1
    end
    return 0
end

-- 获取当前道具数量和消耗数量
function SummerMazeData:GetHasAndConsume()
    local has_num = Item.GetNum(Config.cool_summer_auto.other[1].consume_item)
    local consume_num = Config.cool_summer_auto.other[1].four_more_num
    if self.game_info.play_times == 0 then
        consume_num = Config.cool_summer_auto.other[1].first_num
    elseif self.game_info.play_times == 1 then
        consume_num = Config.cool_summer_auto.other[1].sec_num
    elseif self.game_info.play_times == 2 then
        consume_num = Config.cool_summer_auto.other[1].third_num
    end
    return has_num, consume_num
end

function SummerMazeData:GetAllHouseListData()
    if not self.all_house_list then
        self.all_house_list = {}
        for i = 0, self.row - 1 do
            self.all_house_list[i + 1] = {}
            self.all_house_list[i + 1].row = i
            self.all_house_list[i + 1].house = {}
            for j = 0, self.col - 1 do
                local house = {}
                house.index = self:RCToIndex(i, j)
                house.col = j
                house.row = i
                house.road_id = (2 * i) * self.col + j
                house.house_id = house.road_id + self.col
                self.all_house_list[i + 1].house[j + 1] = house
                self.find_house_mono_array[house.road_id] = self.col * i + j
                self.find_house_mono_array[house.house_id] = self.col * i + j
            end
        end
    end
    return self.all_house_list
end

-- 存储房子组和房子的mono
function SummerMazeData:DealAllHouseList(AllHouseList)
    for k, v in pairs(AllHouseList.item_list) do
        self.house_group_mono[AllHouseList.total_count - k + 1] = v
        for i, w in pairs(v.HouseList.item_list) do
            self.house_mono[self.col * w:GetData().row + w:GetData().col] = w
        end
    end
end

-- 刷新所有房子信息
function SummerMazeData:FlushAllMapInfo()
    for k, v in pairs(self.house_mono) do
        v:FlushAll()
    end
end

-- 重置所有房子显示
function SummerMazeData:ResetAllHouse()
    for k, v in pairs(self.house_mono) do
        v:ResetHouse()
    end
end

-- 刷新指定房子信息
function SummerMazeData:FlushSingleMapInfo(index)
    local mono_id = self.find_house_mono_array[index]
    if self.house_mono[mono_id] then
        self.house_mono[mono_id]:FlushAll()
    end
end

-- 根据房子id或者路id获取到对应的房子mono
function SummerMazeData:GetHouseMonoById(index)
    local mono_id = self.find_house_mono_array[index]
    return self.house_mono[mono_id]
end

-- 根据房子id或者路id获取到对应的房子data
function SummerMazeData:GetHouseDataById(index)
    return self:GetHouseMonoById(index):GetData()
end

-- 用于A*的节点，放入open_list和close_list的节点，用到的都会存起来
function SummerMazeData:PathNode(now, last, cost)
    local path_node = {now = now, last = last or -1, cost = cost or 0}
    self.all_path_node[now] = path_node
    return path_node
end

-- 获取周围上下左右的有效节点
function SummerMazeData:GetAroundId(now)
    local t = {}
    local now_row, now_col = self:IndexToRC(now)
    local dir = {}
    dir[1] = self:RCToIndex(now_row + 1, now_col)
    dir[2] = self:RCToIndex(now_row - 1, now_col)
    dir[3] = self:RCToIndex(now_row, now_col - 1)
    dir[4] = self:RCToIndex(now_row, now_col + 1)

    -- 在房子里不能左右
    if self.all_map_info.info[now].map_type ~= SummerMazeMapType.road then
        dir[3] = -1
        dir[4] = -1
        -- 房子未完成，只能往下
        if self.all_map_info.info[now].map_status ~= SummerMazeMapStatus.finish then
            dir[1] = -1
        end
    else
        if dir[2] ~= -1 then
            -- 路要能往下需要下面房子完成了
            if self.all_map_info.info[dir[2]].map_status ~= SummerMazeMapStatus.finish then
                dir[2] = -1
            end
        end
    end
    for i, v in pairs(dir) do
        if v ~= -1 then
            t[#t + 1] = v
        end
    end
    return t
end

-- 计算路径（用A*算法）
function SummerMazeData:CalculatePath()
    self.way_path = {}
    self.open_list = {}
    self.close_list = {}
    self.all_path_node = {}
    local is_success = false
    local open_node_num = 1
    -- 起点没有上一个，所以用-1
    self.open_list[self.now_role_pos_info.last] = self:PathNode(self.now_role_pos_info.last)
    while open_node_num > 0 do
        local min_node = nil
        local min_index = 0
        -- 找到open_list中权重最小的节点
        for k, v in pairs(self.open_list) do
            if not min_node then
                min_node = v
                min_index = k
            elseif v.cost < min_node.cost then
                min_index = k
                min_node = v
            end
        end
        if min_node.now ~= self.now_role_pos_info.index then
            self.open_list[min_index] = nil
            open_node_num = open_node_num - 1
            self.close_list[min_index] = min_node
            for k, v in pairs(self:GetAroundId(min_index)) do
                local v_info = self.all_map_info.info[v]
                if v_info.map_lock == SummerMazeMapLock.unlock then
                    -- 每走一格权重+1
                    local now_cost = min_node.cost + 1
                    if self.open_list[v] then
                        if now_cost < self.open_list[v].cost then
                            self.open_list[v].cost = now_cost
                            self.open_list[v].last = min_index
                        end
                    elseif self.close_list[v] then
                        if now_cost < self.close_list[v].cost then
                            self.close_list[v].cost = now_cost
                            self.close_list[v].last = min_index
                            self.open_list[v] = self.close_list[v]
                            open_node_num = open_node_num + 1
                            self.close_list[v] = nil
                        end
                    else
                        self.open_list[v] = self:PathNode(v, min_index, now_cost)
                        open_node_num = open_node_num + 1
                    end
                end
            end
        else
            is_success = true
            break
        end
    end
    if is_success then
        local last_node = self.all_path_node[self.now_role_pos_info.index]
        local reverse_list = {}
        table.insert(reverse_list, {index = last_node.now})
        while last_node.last ~= -1 do
            table.insert(reverse_list, {index = last_node.last})
            last_node = self.all_path_node[last_node.last]
        end
        self.way_path = DataHelper.ReverseList(reverse_list)
    end
end

-- 计算所有可以移动的点的位置，因为这里层级不相同，只能通过硬算的方式获得位置
-- 计算房子组所在位置用于跳转
-- 如果位置修改了，这里也要改
function SummerMazeData:DealAllCanMovePosAndGroupPos()
    if not self.all_can_click_pos then
        self.all_can_click_pos = {}
        for i = 0, self.row - 1 do
            for j = 0, self.col - 1 do
                local index = (2 * i) * self.col + j
                local offset_x = 418 * j
                local offset_y = 508 * i
                self.all_can_click_pos[index] = Vector2.New(-418 + offset_x, -1196 + offset_y)
                self.all_can_click_pos[index + self.col] = Vector2.New(-418 + offset_x, -960 + offset_y)
            end
        end
    end
    if not self.all_house_group_list_pos then
        self.all_house_group_list_pos = {}
        for i = 0, self.row - 1 do
            self.all_house_group_list_pos[i] = Vector2.New(0, -750 - 497.5 * i)
        end
    end
end

function SummerMazeData:GetCanMovePos(index)
    return index and self.all_can_click_pos[index] or self.all_can_click_pos
end

function SummerMazeData:GetSingleMapInfo(index)
    return self.all_map_info.info[index]
end

-- 判断是否能点击移动
function SummerMazeData:JudgeIsCanWalk()
    if self.is_walking then
        PublicPopupCtrl.Instance:Center(Language.SummerMaze.MoveTip)
        return false
    end
    if self:JudgeIsLockPos() then
        PublicPopupCtrl.Instance:Center(Language.SummerMaze.LockTip)
        return false
    end
    return true
end

-- 判断角色是否被锁在房间里
function SummerMazeData:JudgeIsLockPos()
    if self.all_map_info.info[self.now_role_pos_info.index] then
        return self.all_map_info.info[self.now_role_pos_info.index].map_status == SummerMazeMapStatus.doing
    end
    return true
end

-- 夏日迷宫信息请求
function SummerMazeData:SendInfo(opera_type, param_1, param_2)
    -- opera_type
    -- 0：下发全部地图信息
    -- 1：开始游戏消耗门票
    -- 2：移动到哪格，param_1：对应x，param_2：对应y
    -- 3：进行特殊玩法，答题，打怪，商店，需要请求
    -- 4：任务下发
    -- 5：领取任务奖励，param_1：对应type
    -- 6：答题，param_1：对应1234
    -- 7：放弃本轮
    -- 8：购买，param_1：服务端发过来的数组下标
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SUMMER_MAZE, opera_type, param_1, param_2)
end

-- 夏日迷宫整个迷宫信息下发
function SummerMazeData:SetTotalMapInfo(protocol)
    self.now_role_pos_info.last = self:RCToIndex(protocol.role_y, protocol.role_x)
    self.now_role_pos_info.index = self.now_role_pos_info.last
    self.all_map_info.info = protocol.map_infos
end

-- 夏日迷宫单个迷宫格子信息下发
function SummerMazeData:SetSingleMapInfo(protocol)
    local change_index = self:RCToIndex(protocol.change_y, protocol.change_x)
    self.all_map_info.info[change_index] = protocol.map_info
    if ViewMgr:IsOpen(SummerMazeView) then
        self:FlushSingleMapInfo(change_index)
    end
end

-- 夏日迷宫商店下发
function SummerMazeData:SetShopInfo(protocol)
    self.shop_info.info = protocol.shop_info
end

-- 夏日迷宫答题下发
function SummerMazeData:SetQuestionInfo(protocol)
    self.question_info.id = protocol.question_id
    self.question_info.flag = not self.question_info.flag
end

-- 夏日迷宫任务下发
function SummerMazeData:SetMissionInfo(protocol)
    self.act_mission.task_info = protocol.task_info
    self.act_mission.type_info = protocol.type_info
end

-- 夏日迷宫进行状态下发
function SummerMazeData:SetStatusInfo(protocol)
    self.game_info.is_playing = protocol.is_playing
    self.game_info.play_times = protocol.play_times
end

-- 夏日迷宫玩法结算下发
function SummerMazeData:SetFinishInfo(protocol)
    self.all_map_info.info = {}
    self.now_role_pos_info.last = 0
    self.now_role_pos_info.index = 0
    self.shop_info.info = {}
end

-- 夏日迷宫位置改变下发
function SummerMazeData:SetPosInfo(protocol)
    local pro_index = self:RCToIndex(protocol.role_y, protocol.role_x)
    if self.now_role_pos_info.index ~= pro_index then
        self.now_role_pos_info.last = self.now_role_pos_info.index
        self.now_role_pos_info.index = pro_index
    end
end

-- 行列转换值（从0开始）
function SummerMazeData:RCToIndex(row, col)
    -- 这里加个范围判断限制
    if row < 0 or col < 0 or row >= self.row2 or col >= self.col then
        return -1
    end
    return row * self.col + col
end

-- 值转换行列（从0开始）
function SummerMazeData:IndexToRC(index)
    return math.floor(index / self.col), index % self.col
end