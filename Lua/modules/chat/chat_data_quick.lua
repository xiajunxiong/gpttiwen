
function ChatData:ChatConfigQuick()
	self.config_quick_send = self.config.quick_send
end

function ChatData:ChatDataQuick()
	self:ChatDataQuickType()
	self:ChatDataQuickEmoj()

	self.chat_quick_input = SmartData.New()
	self.simple_records = SmartData.New()
	self.simple_records_switch = SmartData.New{val = false}
end

function ChatData:ChatDataQuickType()
	self.chat_quick_send_list = {}
	self.chat_quick_list = {}
	for _, v in pairs(self.config_quick_send) do
		if 1 == v.is_show then
			self.chat_quick_send_list[v.seq] = v
			self.chat_quick_list[v.seq] = {}
		end
	end
end

function ChatData:GetChatQuickTypeList()
	local list = {}
	for i,v in pairs(self.chat_quick_send_list or {}) do
		if v.seq ~= 7 or ShengQiData.Instance:IsOpen() then
			table.insert(list, v)
		end
	end
	return list
end

function ChatData:ChatDataQuickEmoj()
	for i = 1, ChatConfig.EMOJ_NUM_MAX do
		self.chat_quick_list[ChatConfig.ChatQuickType.emoj][i] = {
			res_id = i,
			quick_msg =  string.format("/%se", i),
			is_dy = false,
		}
	end
	for i = 1, ChatConfig.EMOJD_NUM_MAX do
		self.chat_quick_list[ChatConfig.ChatQuickType.emoj][i + ChatConfig.EMOJ_NUM_MAX] = {
			res_id = i .. "_",
			quick_msg =  string.format("/%se", i + 100),
			is_dy = true,
			pic_num = ChatConfig.EmojDPicNum[i],
			speed = ChatConfig.EmojDPicSpeed[i]
		}
	end
end

function ChatData:ChatDataQuickItem()
	local bag_data = BagCtrl.Instance:Data()
	local list = {}
	local function insert_list(items, key, val)
		for _, v in pairs(items) do
			if key then
				v.chat_quick_show = true
				v[key] = v[val]
			end
			table.insert(list, v)
		end
	end
	insert_list(bag_data:GetEquipList(), "chat_quick_show_equip", "index")
	insert_list(bag_data:GetJewelryList(), "chat_quick_show_jewelry", "jewelry_type")
	insert_list(bag_data:GetMedalItemList(), "chat_quick_show_medal", "grid_index")
	insert_list(bag_data:GetColumnList(ItemColumnType.Item))
	insert_list(bag_data:GetColumnList(ItemColumnType.Material))
	insert_list(bag_data:GetColumnList(ItemColumnType.Task))
	insert_list(bag_data:GetColumnList(ItemColumnType.Supply))
	insert_list(bag_data:GetColumnList(ItemColumnType.Illustration))
	self.chat_quick_list[ChatConfig.ChatQuickType.item] = list
end

function ChatData:ChatDataQuickPet()
	self.chat_quick_list[ChatConfig.ChatQuickType.pet] = self:GetSortedList()
end

function ChatData:ChatDataQuickShengQi()
	local list = ShengQiData.Instance.sheng_qi_list
	list = table.rearrange(list)
	ShengQiData.Instance:BagSort(list)
	self.chat_quick_list[ChatConfig.ChatQuickType.sheng_qi] = list
end

function ChatData:GetSortedList()
	local list = PetData.Instance:GetPetList()
    list = table.rearrange(list)
    local function sort_func(a, b)
        local order_a = 100000
        local order_b = 100000
        --排序优先级（出战等状态>评分>等级>品质>强化等级>序号）
        if a:State() < b:State() then
            order_a = order_a + 100000
        elseif a:State() > b:State() then
            order_b = order_b + 100000
        end
        if a:Quality() > b:Quality() then
            order_a = order_a + 100
        elseif a:Quality() < b:Quality() then
            order_b = order_b + 100
        end
        if a:Level() > b:Level() then
            order_a = order_a + 1000
        elseif a:Level() < b:Level() then
            order_b = order_b + 1000
        end
        if a:GetScore() > b:GetScore() then
            order_a = order_a + 10000
        elseif a:GetScore() < b:GetScore() then
            order_b = order_b + 10000
        end
        if a:StrLevel() > b:StrLevel() then
            order_a = order_a + 10
        elseif a:StrLevel() < b:StrLevel() then
            order_b = order_b + 10
        end
        if a.index < b.index then
            order_a = order_a + 1
        elseif a.index > b.index then
            order_b = order_b + 1
        end
        return order_a > order_b
    end
    table.sort(list, sort_func)
	return list
end

function ChatData:GetChatQuickListBySeq(seq)
	return self.chat_quick_list[seq]
end

function ChatData:SetChatSimpleRecordInfo(protocol)
	local simple_records = {}
	for i = 1, ChatConfig.SIMPLE_RECORD_NUM_MAX do
		local simple_record = protocol.records[i]
        simple_records[i] = {
			index = i,
			content = StringIsEmpty(simple_record) and Language.Chat.SimpleRecords[i] or simple_record,
		}
	end
	self.simple_records:Set(simple_records)
end


function ChatData:SetMutePlayerDeleteChatRecord(protocol)
	local role_id = protocol.role_id
	if RoleData.Instance:GetRoleId() == role_id then return end
	--频道
	for _, info in pairs(self.chat_channel_list) do
		info:ClearChatInfoByUid(role_id)
	end
	--好友
	for _, info in pairs(self.chat_role_list) do
		info:ClearChatInfoByUid(role_id)
	end
	--群组
	for _, info in pairs(SocietyData.Instance.chat_group_channels) do
		info:ClearChatInfoByUid(role_id)
	end
end

function ChatData:ChatSimpleSwitch()
	self.simple_records_switch.val = not self.simple_records_switch.val
end