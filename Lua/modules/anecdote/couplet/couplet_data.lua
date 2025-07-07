-- 天下绝对类，可用于天下绝对相关界面相关
-- 当前使用到的界面有：天下绝对、宝箱解密、诗词歌赋、宝箱解密2、宝箱解密3、天下绝对2、天下绝对3、诗词歌赋2
CoupletData = CoupletData or BaseClass()

-- 上方Item的状态：空格，被选中，普通字，空格有字
CoupletData.TopItemState = {
	Space = 0,
	Selected = 1,
	NormalWord = 2,
	SpaceWord = 3,
}

function CoupletData:__init()
	if CoupletData.Instance ~= nil then
		Debuger.LogError("[CoupletData] attempt to create singleton twice!")
		return
	end
	CoupletData.Instance = self

	self.anecdote_type = nil
	self.config = nil
	self.npc_id = nil
	self.top_list_data = {}
	self.down_list_data = {}
	-- 用于记录需要填充的文字列表id
	self.can_select_id = {}
	-- 组最大数，该值会因为不同的奇闻而不同
	self.GROUP_MAX = 1
	-- 当前是第几组
	self.now_group = 1
	-- 可以选择的item的mono
	self.can_select_mono = {}
	-- 上方Item全部创建完成回调
	self.top_list_callback = nil
	-- 上方Item全部创建完成需要toplist回调几次，每次初始化数据只用于计数一次
	self.top_list_need_callback_count = 0
	-- 是否完成
	self.is_finish = false
	-- 当前选中格子的id
	self.now_select_id = -1
end

function CoupletData:__delete()
	CoupletData.Instance = nil
end

-- 初始化/清除数据，调用前必须初始化
function CoupletData:DataReset(anecdote_type, config,now_group)
	self.top_list_data = {}
	self.down_list_data = {}
	self.can_select_id = {}
	self.can_select_mono = {}
	self.top_list_callback = nil
	self.is_finish = false
	self.now_select_id = -1
	self.config = nil
	self.npc_id = nil
	self.GROUP_MAX = 3
	if config then
		self.config = config
		self.anecdote_type = anecdote_type
		self.now_group = now_group or AnecdoteData.Instance:GetSpecialInfo(anecdote_type)
		if self.config.other then
			self.npc_id = self.config.other[1].npc_id
			self.GROUP_MAX = self.config.other[1].group
		end
		if config.group_max then
			self.GROUP_MAX = config.group_max
		end

		self:CheckNowGroup()
		self:SetTopListData()
		self:SetDownListData()
	end
end

-- 每个小toplist创建完需要调用一次
function CoupletData:OneTopListCallBack()
	self.top_list_need_callback_count = self.top_list_need_callback_count - 1
	if self.top_list_need_callback_count <= 0 then
		if self.top_list_callback then
			self.top_list_callback()
			-- 设置第一个为选中格子
			self:SetTopListSelectFirst()
		end
	end
end

function CoupletData:GetNpcId()
	return self.npc_id
end

function CoupletData:GetNpcName()
	local config = Cfg.NpcById(self:GetNpcId(),false) or Cfg.NpcBySeq(self:GetNpcId(),false)
	return config.name or ""
end

function CoupletData:GetFailTalkId()
	return self.config.other[1].fail_talk
end

function CoupletData:GetSubjectTalkId(num)
	return self.config.other[1]["subject_talk_"..num] or 0
end

function CoupletData:GetTopListData()
	return self.top_list_data
end

function CoupletData:GetDownListData()
	return self.down_list_data
end

function CoupletData:GetCanSelectIdList()
	return self.can_select_id
end

function CoupletData:SetCanSelectMono(can_select_mono)
	self.can_select_mono = can_select_mono
end

function CoupletData:GetCanSelectMono(index)
	return index and self.can_select_mono[index] or self.can_select_mono
end

function CoupletData:SetTopListCallback(func)
	self.top_list_callback = func
end

function CoupletData:GetIsFinish()
	return self.is_finish
end

function CoupletData:GetFinishWord(type)
	for k,v in pairs(self.config.lattice) do
		if v.group == self.now_group and v.type == type then
			return v.word
		end
	end
end

-- 获取上方列表data
function CoupletData:SetTopListData()
	local data = {}
	for i, v in ipairs(self.config.lattice) do
		if v.group == self.now_group then
			local data1 = {}
			data1.list = {}
			data1.type = v.type
			-- 表中的字是从0开始的
			local index = 0
			local str = "word_"..index
			while(v[str]) do
				if v[str] ~= 0 then
					local word = {}
					-- 这里面的index百位数为第几个大组，余100的为大组中的第几个
					word.index = index + 1 + (v.type + 1) * 100
					if tonumber(v[str]) ~= nil then
						word.right_pos = v[str]
						word.now_select = -1
						word.is_can_select = true
						table.insert(self.can_select_id, {pos = word.index})
					else
						word.word = v[str]
						word.is_can_select = false
					end
					table.insert(data1.list, word)
				else
					break
				end
				index = index + 1
				str = "word_"..index
			end
			table.insert(data, data1)
		end
	end
	table.sort(data, DataSorter.KeyLowerSorter("type"))
	self.top_list_data = data
	self.top_list_need_callback_count = #self.top_list_data
end

-- 获取下方列表data
function CoupletData:SetDownListData()
	local data = {}
	for i, v in ipairs(self.config.word_group) do
		if v.group == self.now_group then
			local data2 = {}
			data2.seq = v.seq
			data2.word = v.word
			-- 这里记录的id是根据self.can_select_mono记录的顺序id
			data2.select_id = -1
			table.insert(data, data2)
		end
	end
	table.sort(data, DataSorter.KeyLowerSorter("seq"))
	self.down_list_data = data
end

-- 检查now_group是否合法
function CoupletData:CheckNowGroup()
	if self.now_group <= 0 or self.now_group > self.GROUP_MAX then
		self.now_group = 1
		self:SendTaskSpecialReq()
	end
end

function CoupletData:GetNowGroup()
	return self.now_group
end

function CoupletData:SetNowGroup(value)
	self.now_group = value
	self:CheckNowGroup()
end

-- 设置已经轮到第几副对联
function CoupletData:SendTaskSpecialReq()
	if self.anecdote_type then
		AnecdoteCtrl.Instance:SendSpecialData(self.anecdote_type, self.now_group)
	end
end

-- 点击上方Item
function CoupletData:OnTopItemClick(mono)
	if self.is_finish then
		return
	end
	local mono_data = mono:GetData()
	if mono_data.is_can_select then
		-- 如果当前有选中框，并且选中框不是当前点击框
		if self.now_select_id ~= -1 and self.now_select_id ~= mono_data.index then
			-- 先取消下方对应字的使用，并且当前选中框变成空格
			local mono_now = self:GetCanSelectMonoBySeq(mono_data.now_select)
			if mono_now then
				mono_now.down_mono:SetUsedItemActive(false)
			end
			self:SetTopItemSpaceOrWord(self.now_select_id, "", -1, nil)
		end
		-- 然后在设置当前点击Item变成选中框
		self:SetTopItemSelect(mono_data.index)
	end
end

-- 点击下方Item
function CoupletData:OnDownItemClick(mono)
	if self.is_finish then
		return
	end
	local mono_data = mono:GetData()
	
	if mono_data.select_id ~= -1 then
		-- 如果当前字格已被使用
		self:SetTopItemSpaceOrWord(self.now_select_id, "", -1, nil)
		self:SetTopItemSelect(mono_data.select_id)
		mono:SetUsedItemActive(false)
	elseif self.now_select_id ~= -1 then
		-- 否则给上方选中格子赋字，并且跳到下一个空格中被选中
		mono:SetUsedItemActive(true, self.now_select_id)
		self:SetTopItemSpaceOrWord(self.now_select_id, mono_data.word, mono_data.seq, mono)
		self:SetTopListSelectFirst()
	else
		-- 没有选中格子的飘字提示
		PublicPopupCtrl.Instance:Center(Language.Anecdote.CoupletFullTip)
	end
end

-- 设置上方index（或者mono）的格子为选中状态
function CoupletData:SetTopItemSelect(index, mono_now)
	-- 防止一直选同一个格子
	if self.now_select_id ~= index then
		local mono =  mono_now or self:GetCanSelectMonoByIndex(index)
		if not mono then
			return
		end
		-- 如果这个格子本身有字，就要去下方字库取消选中
		if mono.down_mono then
			mono.down_mono:SetUsedItemActive(false)
			mono.down_mono = nil
		end
		mono.top_mono:SetWordText("")
		self:SetTopItemState(mono.top_mono, CoupletData.TopItemState.Selected)
		self.now_select_id = index
	end
end

-- 设置上方index_now的格子为空格有字状态或者为空格状态
function CoupletData:SetTopItemSpaceOrWord(index_now, str, index_change, down_mono)
	local mono = self:GetCanSelectMonoByIndex(index_now)
	if not mono then
		return
	end
	mono.top_mono:SetWordText(str, index_change)
	mono.down_mono = down_mono
	if index_change ~= -1 then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
		self:SetTopItemState(mono.top_mono, CoupletData.TopItemState.SpaceWord)
	else
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
		self:SetTopItemState(mono.top_mono, CoupletData.TopItemState.Space)
	end
end

-- 通过上方index返回对应可选的can_select_mono和所在数组位置
function CoupletData:GetCanSelectMonoByIndex(index)
	if #self.can_select_id <= 0 then
		LogError("没有可选格子")
		return
	end
	if index < 100 then
		return
	end
	for i, v in ipairs(self.can_select_id) do
		if v.pos == index then
			return self.can_select_mono[i], i
		end
	end
end

-- 通过下方seq返回对应可选的can_select_mono
function CoupletData:GetCanSelectMonoBySeq(seq)
	if #self.can_select_id <= 0 then
		LogError("没有可选格子")
		return
	end
	if seq == -1 then
		return
	end
	for i, v in ipairs(self.can_select_mono) do
		if v.down_mono then
			if v.down_mono:GetData().seq == seq then
				return v
			end
		end
	end
end

-- 设置上方对应Item图片状态
function CoupletData:SetTopItemState(mono, state)
	mono.WordObj:SetActive(false)
	mono.EmptyObj:SetActive(false)
	mono.SelectObj:SetActive(false)
	mono.FillObj:SetActive(false)
	if state == CoupletData.TopItemState.Space then
		mono.EmptyObj:SetActive(true)
	elseif state == CoupletData.TopItemState.Selected then
		mono.SelectObj:SetActive(true)
	elseif state == CoupletData.TopItemState.NormalWord then
		mono.WordObj:SetActive(true)
	elseif state == CoupletData.TopItemState.SpaceWord then
		mono.FillObj:SetActive(true)
	end
end

-- 设置上方所有Item为字状态
function CoupletData:SetAllItemWordState()
	for i, v in ipairs(self.can_select_mono) do
		self:SetTopItemState(v.top_mono, CoupletData.TopItemState.NormalWord)
	end
end

-- 判断是否都填对
function CoupletData:JudgeIsFinish()
	if #self.can_select_mono > 0 then
		for k, v in ipairs(self.can_select_mono) do
			if v.top_mono:GetData().now_select ~= v.top_mono:GetData().right_pos then
				return false
			end
		end
		self.is_finish = true
		return true
	end
	return true
end

-- 主线任务需要快速完成
function CoupletData:FastFinish()
	-- 将id和文字设置正确
	for k, v in pairs(self.can_select_mono) do
		v.top_mono:SetWordText(self.down_list_data[v.top_mono:GetData().right_pos].word, v.top_mono:GetData().right_pos)
	end
end

-- 判断是否有没填的空格，如果有，返回can_select_mono，否则返回nil
function CoupletData:JudgeIsHasSpace()
	if #self.can_select_mono > 0 then
		for k, v in ipairs(self.can_select_mono) do
			if v.top_mono:GetData().now_select == -1 then
				return v
			end
		end
		return false
	end
	return false
end

-- 设置上方列表中的第一个空格为选中状态，如果已经填完则不设置
function CoupletData:SetTopListSelectFirst()
	local mono_first_space = self:GetNextSpace()
	if mono_first_space then
		self:SetTopItemSelect(mono_first_space.top_mono:GetData().index, mono_first_space)
	else
		self.now_select_id = -1
	end
end

-- 获取下一个空格的位置
function CoupletData:GetNextSpace()
	if self.now_select_id == -1 then
		return self:JudgeIsHasSpace()
	end
	local mono, index = self:GetCanSelectMonoByIndex(self.now_select_id)
	local next_id = index % #self.can_select_mono + 1
	while next_id ~= index do
		if self.can_select_mono[next_id].top_mono:GetData().now_select == -1 then
			return self.can_select_mono[next_id]
		end
		next_id = next_id % #self.can_select_mono + 1
	end
end