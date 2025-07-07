-- 游历诗人类，可用于游历诗人相关玩法界面
-- 当前使用到的界面有：游历诗人2
Poetry2Data = Poetry2Data or BaseClass()

function Poetry2Data:__init()
	if Poetry2Data.Instance ~= nil then
		Debuger.LogError("[Poetry2Data] attempt to create singleton twice!")
		return
	end
	Poetry2Data.Instance = self
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

function Poetry2Data:__delete()
	Poetry2Data.Instance = nil
end

function Poetry2Data:SetFinishCallback(callback)
    self.finish_callback = callback
end

-- 初始化/清除数据，调用前必须初始化
function Poetry2Data:DataReset(config)
	self.is_can_drag = false
	self.is_draging = false
	self.drag_item = nil
	self.drag_down_item = nil
	self.is_succeed = false
	self.finish_callback = nil
	self.list_anim = nil
	if config then
		self.config = config
		self.npc_id = self.config.other[1].npc_id
		self:SetItemData()
	else
		self.config = nil
		self.npc_id = nil
	end
end

function Poetry2Data:SetItemData()
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

function Poetry2Data:SetPos(PosList)
	for i = 1, PosList:Length() do
		self.pos_list[i] = PosList[i].anchoredPosition
	end
end

function Poetry2Data:GetPos(index)
	return index and self.pos_list[index] or self.pos_list
end

function Poetry2Data:GetItemData(index)
	return index and self.item_data[index] or self.item_data
end

function Poetry2Data:SetDragItem(item)
	self.drag_item = item
end

function Poetry2Data:GetDragItem()
	return self.drag_item
end

function Poetry2Data:SetDragDownItem(item)
	self.drag_down_item = item
end

function Poetry2Data:GetDragDownItem()
	return self.drag_down_item
end

function Poetry2Data:GetIsCanDrag()
	return self.is_can_drag
end

function Poetry2Data:SetIsCanDrag(value)
	self.is_can_drag = value
end

function Poetry2Data:GetIsDraging()
	return self.is_draging
end

function Poetry2Data:SetIsDraging(value)
	self.is_draging = value
end

function Poetry2Data:SetIsSucceed(value)
	self.is_succeed = value
end

function Poetry2Data:GetIsSucceed()
	return self.is_succeed
end

function Poetry2Data:GetNPCId()
	return self.npc_id
end

function Poetry2Data:SetListAnim(anim)
	self.list_anim = anim
end

function Poetry2Data:GetListAnim()
	return self.list_anim
end

function Poetry2Data:ExchangeItem()
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
function Poetry2Data:SetItemPos(item, pos)
	item.data.now_pos = pos
	UH.AnchoredPosition(item.Rect, self:GetPos(pos))
end

function Poetry2Data:GetLastTaskId()
	return AnecdoteData.GetConfig(AnecdoteType.Poetry2).last_task_id
end