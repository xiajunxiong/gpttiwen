-- 游历诗人类，可用于游历诗人相关玩法界面
-- 当前使用到的界面有：游历诗人、寻味美食
PoetryData = PoetryData or BaseClass()

function PoetryData:__init()
	if PoetryData.Instance ~= nil then
		Debuger.LogError("[PoetryData] attempt to create singleton twice!")
		return
	end
	PoetryData.Instance = self
	-- 配置，需要赋值
	self.config = nil
	-- 当前使用的item_data
	self.item_data = {}
	-- 存储诗句的RectTransfrom
	self.pos_list = {}
	-- NPCid
	self.npc_id = nil
	-- 当前拖拽中的item
	self.drag_item = nil
	-- 当前拖拽中下面需要交换的item
	self.drag_down_item = nil
	-- 是否可以拖拽，刚开始播动画的时候无法拖拽，完成排序后也无法拖拽
	self.is_can_drag = false
	-- 是否在拖拽中
	self.is_draging = false
	-- 是否成功
	self.is_succeed = false
	-- 全部匹配成功回调，用于播放不同的特效
	self.finish_callback = nil
	-- 条子动画控制器
	self.list_anim = nil
	-- 用于检查所有的诗句是否对的上
	self.check_flag = SmartData.New({flag = false})
end

function PoetryData:__delete()
	PoetryData.Instance = nil
end

function PoetryData:SetFinishCallback(callback)
    self.finish_callback = callback
end

-- 初始化/清除数据，调用前必须初始化
function PoetryData:DataReset(config)
	self.is_can_drag = false
	self.is_draging = false
	self.drag_item = nil
	self.drag_down_item = nil
	self.is_succeed = false
	self.finish_callback = nil
	self.list_anim = nil
	self.config = nil
	self.npc_id = nil
	if config then
		self.config = config
		if self.config.other then
			self.npc_id = self.config.other[1].npc_id
		end
		self:SetItemData()
	end
end

function PoetryData:SetItemData()
	for i = 1, #self.config.initial do
		local t = {}
		t.seq = self.config.initial[i].seq
		t.point = self.config.initial[i].point
		t.initial_number = self.config.initial[i].initial_number
		t.correct_number = self.config.initial[i].correct_number
		t.poetry_word = self.config.poetry_number[i].poetry_word
		t.now_pos = t.initial_number
		self.item_data[i] = t
	end
	table.sort(self.item_data, DataSorter.KeyLowerSorter("initial_number"))
end

function PoetryData:SetPos(PosList)
	for i = 1, PosList:Length() do
		self.pos_list[i] = PosList[i].anchoredPosition
	end
end

function PoetryData:GetPos(index)
	return index and self.pos_list[index] or self.pos_list
end

function PoetryData:GetItemData(index)
	return index and self.item_data[index] or self.item_data
end

function PoetryData:SetDragItem(item)
	self.drag_item = item
end

function PoetryData:GetDragItem()
	return self.drag_item
end

function PoetryData:SetDragDownItem(item)
	self.drag_down_item = item
end

function PoetryData:GetDragDownItem()
	return self.drag_down_item
end

function PoetryData:GetIsCanDrag()
	return self.is_can_drag
end

function PoetryData:SetIsCanDrag(value)
	self.is_can_drag = value
end

function PoetryData:GetIsDraging()
	return self.is_draging
end

function PoetryData:SetIsDraging(value)
	self.is_draging = value
end

function PoetryData:SetIsSucceed(value)
	self.is_succeed = value
end

function PoetryData:GetIsSucceed()
	return self.is_succeed
end

function PoetryData:GetNPCId()
	return self.npc_id
end

function PoetryData:SetListAnim(anim)
	self.list_anim = anim
end

function PoetryData:GetListAnim()
	return self.list_anim
end

function PoetryData:ExchangeItem()
	if self.is_can_drag and self.is_draging and self.drag_item ~= nil then
		if self.drag_down_item ~= nil then
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
			local index = self.drag_down_item.data.now_pos
			self:SetItemPos(self.drag_down_item, self.drag_item.data.now_pos)
			self:SetItemPos(self.drag_item, index)
			self.drag_down_item.SelectObj:SetActive(false)
			self.check_flag.flag = not self.check_flag.flag
		else
			UH.AnchoredPosition(self.drag_item.Rect, self:GetPos(self.drag_item.data.now_pos))
		end
	end
end

-- 这里的pos是index
function PoetryData:SetItemPos(item, pos)
	item.data.now_pos = pos
	UH.AnchoredPosition(item.Rect, self:GetPos(pos))
end

function PoetryData:GetLastTaskId()
	return AnecdoteData.GetConfig(AnecdoteType.Poetry).last_task_id
end