PuzzleData = PuzzleData or BaseClass()

-- 不打乱拼图碎片的奇闻任务id
PuzzleData.NotRondomTaskId = {
    [65184] = false
}
PuzzleData.PuzzleImgType = {
    Raw = 0, -- 纹理拼图
    Sprite = 1, -- 精灵拼图
}

function PuzzleData:__init()
    if PuzzleData.Instance ~= nil then
		Debuger.LogError("[PuzzleData] attempt to create singleton twice!")
		return
    end
    PuzzleData.Instance = self
end

-- 打乱结构 注意list那个layout_group是从左下角开始生成item
-- 7   8   9
-- 4   5   6
-- 1   2   3
-- is_random 是否打乱
-- img_type == 1 时，img_name必须要有
function PuzzleData:GetListData(row,col,img_name,is_random,img_type)
    is_random = is_random == nil and true or is_random
    row = row or 3
    col = col or 3
    local w = 1 / col
    local h = 1 / row
    local list_data = {}
    for i = 1,col do
        for j = 1,row do
            local data = {}
            data.w = w
            data.h = h
            data.x = (1 / col) * (j - 1)
            data.y = (1 / row) * (i - 1)
            data.raw_img_name = img_name
            data.img_type = img_type
            table.insert(list_data,data)
        end
    end
    local rand_t = {}
    if is_random then
        rand_t = DataHelper.GetRamdonList(1,#list_data)
    else
        for i = 1,#list_data do
            rand_t[i] = i
        end
    end
    local rand_list_data = {}
    for i,v in pairs(rand_t) do
        list_data[v].index = i
        list_data[v].num = v
        table.insert(rand_list_data,list_data[v])
    end
    return rand_list_data
end

function PuzzleData:IsPass(list_data)
    local num = 0
    for k,v in pairs(list_data or {}) do
        if v.index and v.index == v.num or ( v.pos_index and v.pos_index == v.num) then
            num = num + 1
        end
    end
    if list_data == nil then
        return false
    end
    return num == #list_data
end

-- 判断奇闻任务是否需要打乱拼图碎片
function PuzzleData:IsNeedRandom(task_id)
    return PuzzleData.NotRondomTaskId[task_id] == nil and true or PuzzleData.NotRondomTaskId[task_id]
end

--相邻条件判断，默认3*3的格子
function PuzzleData:IsAdjoin(param1,param2,row,col)
	row = row or 3
    col = col or 3
    if math.abs(param1 - param2) == 1 then 
        return (param1 + param2) % col ~= 1
    else
        return math.abs(param1 - param2) == row
    end
end

-- 获取拼图背面纹理的位置 默认 3*3 且是正方形
function PuzzleData:GetPuzzleBackUV(pos_index,default_size)
	if pos_index and pos_index <= 0 then
		return
	end
	local default_size = default_size or 3
	-- 因为纹理的xy的圆点是在左下角，而拼图碎片的list圆点在左上角
	if pos_index >= ((default_size - 1) * default_size) + 1 then
		pos_index = pos_index - (default_size - 1) * default_size
	end
	if pos_index <= default_size then
		pos_index = pos_index + (default_size - 1) * default_size
	end
	local row = (pos_index - 1) / default_size + 1  -- 第几行
	local col = pos_index % default_size            -- 第几列
	local uv = {}
	uv.w = 1 / default_size
	uv.h = 1 / default_size
	uv.x = (1 / default_size) * (col - 1)
	uv.y = (1 / default_size) * (row - 1)
	return uv
end

-- 播放拼图替换动效 -- a_icon_obj == 第一次点击的格子，b == 第二次点击的格子，t_a 和 t_b分别是他们交叉移动虚假的格子 -- t_a 和a_icon 的transform需要一样的属性
function PuzzleData:PlayPuzzleMoveAnima(a_icon_obj,b_icon_obj,t_a_obj,t_b_obj,end_call_back_func)
	local step = 10
	local EndElementAnimationFunc = function()
		if self.run_element ~= nil then
			TimeHelper:CancelTimer(self.run_element)
			self.run_element = nil
		end
		t_a_obj:SetActive(false)
		t_b_obj:SetActive(false)
		a_icon_obj:SetActive(true)
		b_icon_obj:SetActive(true)
		if end_call_back_func then
			end_call_back_func()
		end
	end
	local UpdateElementPosFunc = function (dir,speed)
		if dir == 0 then
			local a_pos = t_a_obj:GetLocalPosition()
			local b_pos = t_b_obj:GetLocalPosition() 
			a_pos.x = a_pos.x + speed
			b_pos.x = b_pos.x - speed
			t_a_obj:SetLocalPosition(a_pos)
			t_b_obj:SetLocalPosition(b_pos)
		elseif dir == 1 then
			local a_pos = t_a_obj:GetLocalPosition()
			local b_pos = t_b_obj:GetLocalPosition()
			a_pos.y = a_pos.y + speed
			b_pos.y = b_pos.y - speed
			t_a_obj:SetLocalPosition(a_pos)
			t_b_obj:SetLocalPosition(b_pos)
		end
		step = step - 1
		if step <= 0 then
			EndElementAnimationFunc()
		end
	end

    a_icon_obj:SetActive(false)
    b_icon_obj:SetActive(false)
    t_a_obj:SetActive(true)
    t_b_obj:SetActive(true)
    local a_pos = t_a_obj:GetLocalPosition()
    local b_pos = t_b_obj:GetLocalPosition()
    -- dir = 0 则左右移动，= 1 则上下移动
    local dir = math.abs(a_pos.x - b_pos.x) < 10 and 1 or 0
    local speed
    if dir == 0 then
        speed = math.abs(a_pos.x - b_pos.x) / step
        speed = a_pos.x > b_pos.x and -speed or speed
    else
        speed = math.abs(a_pos.y - b_pos.y) / step
        speed = a_pos.y > b_pos.y and -speed or speed
    end
    self.run_element = TimeHelper:AddRunFrameTimer(BindTool.Bind(UpdateElementPosFunc, dir , speed), 1, step)
	return self.run_element
end