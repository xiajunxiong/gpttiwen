LuckyMazeData = LuckyMazeData or BaseClass()
LuckyMazeData.CtrlType = {
	Info = 0, 			-- 请求下发信息
	GetCard = 1,    	-- 翻盘 p1 == 哪张牌的id
	Play = 2,       	-- 拼图位移 p1 == 哪张图的id,p2、p3分别是操作的两张碎片位置
	EnterScene = 3,   	-- 进入副本
	SetPuzzleIndex = 4, -- 设置拼图当前操作的大图id
	PuzzleInfo = 5, 	-- 拼图信息下发
}
LuckyMazeData.ImgNames = {
	[0] = "BaiNv",
	[1] = "DaNv",
	[2] = "LuoLi",
	[3] = "NeZha",
	[4] = "QingNv",
	[5] = "ShuNv",
	[6] = "XuanNv",
}
function LuckyMazeData:__init()
    if LuckyMazeData.Instance ~= nil then
		Debuger.LogError("[LuckyMazeData] attempt to create singleton twice!")
		return
	end
	LuckyMazeData.Instance = self
	ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoLuckyMaze,function() 
			self.cfg = Config.lucky_maze_auto
		end,true)
	self.puzzle_index = 0 			-- 当前操作的大图id -- 这个是服务端记录的 == img_id
	self.change_index = 0 			-- 这个是翻牌随机获得的碎片id
	self.act_data = SmartData.New()
	self.puzzle_data = SmartData.New()
	self.puzzle_img_data = {}
	self.default_img_sort_list = {3,1,2,0,4,5,6}
	self.scene_reward_info = SmartData.New({}) -- 用于其他场景显示奖励信息
	self.is_all_finish = false 		           -- 是否全部拼图完成，并且包括迷宫场景玩法也完成了
	self.remind_data = {}
end

function LuckyMazeData:__delete()
    LuckyMazeData.Instance = nil
end

function LuckyMazeData:SetActData(protocol)
	UnityPlayerPrefs.SetInt(PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(),ACTIVITY_TYPE.RAND_LUCKY_MAZE),0)
	self.act_data:Set(protocol.info)
end

function LuckyMazeData:SetPuzzleData(protocol)
	self:SetChangeIndex(protocol)
	self.puzzle_data:Set(protocol.info)
	self.puzzle_index = protocol.info.mark_index
	self:InitPuzzleAllData()
	self:CheckIsAllFinish()
end

function LuckyMazeData:GetActData()
	return self.act_data
end
-- 获取拼图全部信息
function LuckyMazeData:GetPuzzleData()
	return self.puzzle_data
end

function LuckyMazeData:GetPuzzleIndex()
	return self.puzzle_index
end

-- 设置随机获得的拼图id
function LuckyMazeData:SetChangeIndex(protocol,pos_index)
	if pos_index then
		self.change_index = pos_index
		return
	end
	if self.puzzle_data.puzzle_info == nil then
		self.change_index = 0
		return
	end
	-- 检查与旧数据的变化
	local old_data = self.puzzle_data.puzzle_info[self.puzzle_index]
	for i,v in pairs(protocol.info.puzzle_info[self.puzzle_index] or {}) do
		if old_data[i] == 0 and v > 0 then
			self.change_index = i
			return
		end
	end
end

function LuckyMazeData:GetChangeIndex()
	return self.change_index or 0
end

function LuckyMazeData:GetBoxData(box_id)
	local box_reward_data = {}
	local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_LUCKY_MAZE, self.cfg.reward) or {}
	local data = {item_list = {}}
	for i,v in pairs(cfg) do
		local reward_group_id = v.reward_group_id % 3
		reward_group_id = reward_group_id == 0 and 3 or reward_group_id
		if reward_group_id == box_id then
			data.item_list = DataHelper.FormatItemList(v.reward_item)
			return data
		end
	end
	return data
end

function LuckyMazeData:GetTaskData()
	self.task_cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_LUCKY_MAZE,self.cfg.task)
	local task_data = {}
	for i,v in pairs(self.task_cfg) do
		table.insert(task_data,v)
		task_data[#task_data].flag = self:GetTaskFlag(v)
		local count = self:GetTaskCount(v.seq1)
		task_data[#task_data].count = count > v.parameter and v.parameter or count
	end
	local sort_func = function(a,b)
		if a.flag == b.flag then
			return a.seq < b.seq
		end
		return a.flag < b.flag
	end
	table.sort(task_data,sort_func)
	return task_data
end

-- 0 是未达标，1是已经达标
function LuckyMazeData:GetTaskFlag(data)
	return self:GetTaskCount(data.seq1) >= data.parameter and 1 or 0
end

function LuckyMazeData:GetTaskCount(seq)
	return self.act_data.task_info[seq]
end

-- 红点逻辑
function LuckyMazeData:GetRemindNum()
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_LUCKY_MAZE) then
		return 0
	end
	-- 有时候协议较晚下发
	if not self.act_data.can_enter_fb or not self.act_data.rand_count then
		return 0
	end
	local last_num = self.remind_data[RoleData.Instance:GetRoleId()] or 0
	if self.act_data.can_enter_fb > 0 or self.act_data.rand_count > last_num then
		return 1
	end
    return 0
end

-- 真理拼图数据
function LuckyMazeData:InitPuzzleAllData()
	self.puzzle_img_data = {}
	for i,v in pairs(self.puzzle_data.puzzle_info) do
		self.puzzle_img_data[i] = self:InitPuzzleOneData(i)
	end
end

function LuckyMazeData:InitPuzzleOneData(img_id)
	local data = self.puzzle_data.puzzle_info[img_id]
	local puzzle_data = {}
	puzzle_data.img_id = img_id
	puzzle_data.child_data = {}
	for i,v in ipairs(data) do
		puzzle_data.child_data[i] = {}
		puzzle_data.child_data[i].pos_index = i
		puzzle_data.child_data[i].num = v
		puzzle_data.child_data[i].img_name = LuckyMazeData.ImgNames[img_id]
		puzzle_data.child_data[i].img_id = img_id
	end
	return puzzle_data
end

function LuckyMazeData:GetPuzzleOneData(img_id)
	return self.puzzle_img_data[img_id]
end

-- 获取该张拼图是不是都抽完了1 == 满
function LuckyMazeData:GetPuzzleChildFull(img_id)
	local data = self:GetPuzzleOneData(img_id)
	if not data then
		return
	end
	for i,v in pairs(data.child_data) do
		if v.num == 0 then
			return 0
		end
	end
	return 1
end

function LuckyMazeData:GetImgSortList(img_id)
	for i,v in pairs(self.default_img_sort_list) do
		if v == img_id then
			return self:MoveTElement(self.default_img_sort_list,-(i - 4))
		end
	end
end

function LuckyMazeData:SetSceneRewardInfo(info)
	self.scene_reward_info:Set(info)
end

function LuckyMazeData:GetSceneRewardInfo()
	return self.scene_reward_info
end

function LuckyMazeData:CheckIsAllFinish()
	if self.act_data.can_enter_fb and self.act_data.can_enter_fb == 1 then
		self.is_all_finish = false
		return
	end
	for i,v in pairs(self.puzzle_img_data) do
		if not self:IsPass(v.child_data) then
			self.is_all_finish = false
			return
		end
	end
	self.is_all_finish = true
end

function LuckyMazeData:IsAllFinish()
	return self.is_all_finish
end

function LuckyMazeData:IsPass(list_data)
    return PuzzleData.Instance:IsPass(list_data)
end

function LuckyMazeData:IsPassById(img_id)
	if img_id == nil or self.puzzle_img_data[img_id] == nil then
		return false
	end
    local pass_flag = self:IsPass(self:GetPuzzleOneData(img_id).child_data)
	return pass_flag
end

-- 移动表里面的元素 n > 0 向右移几位， n < 0 向左移几位 , 表的key是需要连续的 first_i == 起始下标，默认为1
-- 例如：t = {1,2,3,4,5,6} MoveTElement(t,2) 后就等于 = {5,6,1,2,3,4}
function LuckyMazeData:MoveTElement(t,n,first_i)
	local list = {}
	local index
	first_i = first_i or 1
	for i,v in ipairs(t) do
		index = i + n
		index = index > #t and (index - #t) + first_i - 1 or index
		index = index < first_i and #t - (first_i - index) + 1 or index
		list[index] = v
	end
	return list
end

-- 获取一个未通关的拼图id
function LuckyMazeData:GetNotPassId()
	local cur_id = self:GetPuzzleIndex()
	if not self:IsPassById(cur_id) or self.act_data.can_enter_fb == 1 then
		return
	end
	local list = self:GetImgSortList(cur_id)
	local first_index = 1
	for i,v in pairs(list) do
		if v == cur_id then
			first_index = i
		end
	end
	for i = first_index + 1, #list do
		local v = list[i]
		if not self:IsPassById(v) then
			return v
		end
	end
	for i = 1, first_index - 1 do
		local v = list[i]
		if not self:IsPassById(v) then
			return v
		end
	end
end

-- 设置一个没有通关的拼图
function LuckyMazeData:SetNotPassId()
	local not_pass_id = self:GetNotPassId()
	if not_pass_id then
		LuckyMazeView.SendReq(LuckyMazeData.CtrlType.SetPuzzleIndex,not_pass_id)
	end
end