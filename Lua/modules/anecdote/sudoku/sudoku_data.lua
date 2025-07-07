SudokuData = SudokuData or BaseClass()
SudokuData.BOTTOM_ELEMENT_POS = {
	LEFT = 0 ,
	CENTER = 1,
	RIGHT = 2,
}
function SudokuData:__init()
	if SudokuData.Instance ~= nil then
		Debuger.LogError("[SudokuData] attempt to create singleton twice!")
		return
	end	
	SudokuData.Instance = self
	self.config = Config.sudoku_book_auto
	self:Reset()
	self.enter_bottom_data = nil
	self.is_opened_desc = false
end

function SudokuData:Clear()
	self.enter_bottom_data = nil
	self.is_opened_desc = false
end

function SudokuData:__delete()
	SudokuData.Instance = nil
end

function SudokuData:GetNpcId()
	return self.config.other[1].npc_id
end

function SudokuData:GetFailTalkId()
	return self.config.other[1].fail_talk
end

function SudokuData:GetRestartTalkId( ... )
	return self.config.other[1].restart_talk
end

function SudokuData:GetTaskId()
	return self.config.other[1].task_id
end

function SudokuData:Reset()
	self.a_select_data = nil
	self.b_select_data = nil
	self.a_element_pos_list = TableCopy(self.config.lattice)
	self.b_element_pos_list = {}
	local initial_nums = {}
	for i,v in pairs(self.a_element_pos_list) do
		self.b_element_pos_list[i] = {}
		self.b_element_pos_list[i].seq = v.seq
		self.b_element_pos_list[i].value = 0
		self.a_element_pos_list[i].value = v.initial_num
		if v.initial_num > 0 then
			table.insert(initial_nums,v.initial_num)
		end
	end
	local get_value = function ()
		for i = 1 , 9 do
			local last_i = i
			for _,v in pairs(initial_nums) do
				if i == v then
					i = i + 1
				end
			end
			if last_i == i then
				table.insert(initial_nums,i)
				return i
			end
		end
	end
	for i,v in pairs(self.a_element_pos_list) do
		if v.initial_num <= 0 then
			local num = get_value()
			self.b_element_pos_list[num].value = num
		end
	end
end

-- 记录安放的data
function SudokuData:SetSelectMonoA(data)
	self.a_select_data = data
end

function SudokuData:GetSelectMonoA()
	return self.a_select_data
end

-- 记录拖拽的data
function SudokuData:SetSelectMonoB(data)
	self.b_select_data = data
end

function SudokuData:GetSelectMonoB()
	return self.b_select_data
end

function SudokuData:GetTopElements()
	return self.a_element_pos_list
end

function SudokuData:GetBottomElements()
	return self.b_element_pos_list
end

-- 获取底部剩余几个
function SudokuData:GetRemainCount()
	local count = 0
	for _,v in pairs(self.b_element_pos_list) do
		if v.value > 0 then
			count = count + 1
		end
	end
	return count
end

--重排下面的棍子数据 data === 触发到底排棍子的数据 value === 托拽棍子的值，pos: 触发到的底部棍子的位置，trigger_type；从左边的触发器触发还是右边的触发器触发
function SudokuData:ResetSortData(data,value,pos,trigger_type)
	-- local list = self.b_element_pos_list
	-- local len = #list
	-- local dir = -1 
	-- for i = len , 1,-1 do
	-- 	local v = list[i]
	-- 	if v.value == 0 then
	-- 		if v.seq <= data.seq then
	-- 			dir = 0
	-- 		else
	-- 			dir = 1
	-- 		end
	-- 	end 
	-- end
	-- if dir < 0 then
	-- 	return
	-- end
	-- len = dir == 0 and 1 or len
	-- local step = dir == 0 and -1 or 1
	-- local T
	-- local TT
	-- for i = data.seq,len,step do
	-- 	if i == data.seq then
	-- 		T = list[i].value
	-- 		list[i].value = value
	-- 	else
	-- 		TT = list[i].value
	-- 		list[i].value = T
	-- 		T = TT
	-- 		if TT == 0 then
	-- 			return list
	-- 		end
	-- 	end
	-- end

	-- ============ 
	if data.value <= 0 then
		self.b_element_pos_list[1].value = value
		return
	end
	--LogError(pos,trigger_type)
	pos = pos or SudokuData.BOTTOM_ELEMENT_POS.CENTER
	trigger_type = trigger_type or "right"
	if trigger_type == "right" then
		if pos == SudokuData.BOTTOM_ELEMENT_POS.RIGHT then
			pos = SudokuData.BOTTOM_ELEMENT_POS.RIGHT
		else
			pos = SudokuData.BOTTOM_ELEMENT_POS.CENTER
		end
	else
		if pos == SudokuData.BOTTOM_ELEMENT_POS.LEFT then
			pos = SudokuData.BOTTOM_ELEMENT_POS.LEFT
		else
			pos = SudokuData.BOTTOM_ELEMENT_POS.CENTER
		end
	end
	local list = TableCopy(self.b_element_pos_list)
	local len = #list
	local new_list = {}
	data = TableCopy(data)
	data.value = value
	--LogError(pos)
	if pos == SudokuData.BOTTOM_ELEMENT_POS.LEFT then
		new_list[1] = data
		new_list[1].seq = 1
		for i=1,#list do
			local v = self.b_element_pos_list[i]
			if v.value > 0 then
				local seq = #new_list + 1
				new_list[seq] = v
				new_list[seq].seq = seq
			    list[i].value = 0
			end
		end
		for i = len,1,-1 do
			if new_list[i] == nil then
				new_list[i] = list[i]
				new_list[i].seq = i
			end
			local v = new_list[i]
			if v.value > 0 then
				break
			end
		end
	elseif pos == SudokuData.BOTTOM_ELEMENT_POS.CENTER then
		local target_seq = data.seq
		for i=1,#list do
			local v = self.b_element_pos_list[i]
			if v.value > 0 then
				local seq = #new_list + 1
				if v.seq == target_seq then
					new_list[seq + 1] = data
					new_list[seq + 1].seq = seq + 1
				end
				new_list[seq] = v
				new_list[seq].seq = seq
			    list[i].value = 0
			end
		end
		for i = len,1,-1 do
			if new_list[i] == nil then
				new_list[i] = list[i]
				new_list[i].seq = i
			end
			local v = new_list[i]
			if v.value > 0 then
				break
			end
		end
	elseif pos == SudokuData.BOTTOM_ELEMENT_POS.RIGHT then
		for i=1,#list do
			local v = self.b_element_pos_list[i]
			if v.value > 0 then
				local seq = #new_list + 1
				new_list[seq] = v
				new_list[seq].seq = seq
			    list[i].value = 0
			end
		end
		for i = len,1,-1 do
			if new_list[i] == nil then
				new_list[i] = list[i]
				new_list[i].seq = i
			end
			local v = new_list[i]
			if v.value > 0 then
				new_list[i+1] = data
				new_list[i+1].seq = i + 1
				break
			end
		end
	end
	self.b_element_pos_list = new_list
end

-- 判断下排的棍子的位置：左，中，右
function SudokuData:GetBottomElementPos(data)
	local min_seq = #self.b_element_pos_list
	local max_seq = 1
	for i,v in pairs(self.b_element_pos_list) do
		if v.value > 0 then
			if v.seq < min_seq then
				min_seq = v.seq
			end
			if v.seq > max_seq then
				max_seq = v.seq
			end
		end
	end
	if data.seq <= min_seq then
		return SudokuData.BOTTOM_ELEMENT_POS.LEFT
	elseif data.seq >= max_seq then
		return SudokuData.BOTTOM_ELEMENT_POS.RIGHT
	else
		return SudokuData.BOTTOM_ELEMENT_POS.CENTER
	end
end

-- 设置上面的的棍子拖回到桌子的时候触发棍子的数据
function SudokuData:SetEnterBottomData(data)
	self.enter_bottom_data = data
end
function SudokuData:GetEnterBottomData()
	return self.enter_bottom_data
end