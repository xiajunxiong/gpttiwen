FILE_REQ("modules/pet_stamp/pet_stamp_cfg")
FILE_REQ("modules/pet_stamp/pet_stamp")
FILE_REQ("modules/pet_stamp/pet_stamp_data")

VIEW_REQ("modules/pet_stamp/pet_stamp_view")
VIEW_REQ("modules/pet_stamp/pet_stamp_child_view")
VIEW_REQ("modules/pet_stamp/pet_stamp_intensify")
VIEW_REQ("modules/pet_stamp/pet_stamp_blueprint_view")
VIEW_REQ("modules/pet_stamp/pet_stamp_pray_view")
VIEW_REQ("modules/pet_stamp/pet_stamp_item_info")
VIEW_REQ("modules/pet_stamp/pet_stamp_change_view")
VIEW_REQ("modules/pet_stamp/pet_stamp_resolve_new")
VIEW_REQ("modules/pet_stamp/pet_stamp_swallow_view")
PetStampCtrl = PetStampCtrl or BaseClass(BaseCtrl)
function PetStampCtrl:__init()
    if PetStampCtrl.Instance ~= nil then
		Debuger.LogError("[PetStampCtrl] attempt to create singleton twice!")
		return
	end
	PetStampCtrl.Instance = self
    self.data = PetStampData.New()
	self.data = PetStampData.Instance
	self:RegisterAllProtocol()

end

function PetStampCtrl:__delete()
    
end

function PetStampCtrl:OnUnloadGameLogic()
    
end

function PetStampCtrl:RegisterAllProtocol()
	self:RegisterProtocol(SCPetStampBagInfo, "OnSCPetStampBagInfo")
	self:RegisterProtocol(SCPetStampInfo, "OnSCPetStampInfo")
	self:RegisterProtocol(SCPetStampExpInfo, "OnSCPetStampExpInfo")
	self:RegisterProtocol(SCPetStanpBluePrintInfo, "OnSCPetStanpBluePrintInfo")
	self:RegisterProtocol(SCPetStampPrayLogInfo, "OnSCPetStampPrayLogInfo")
	self:RegisterProtocol(SCPetStampPrayLogContent, "OnSCPetStampPrayLogContent")
	self:RegisterProtocol(SCPetStampPrayRewardInfo, "OnSCPetStampPrayRewardInfo")
	self:RegisterProtocol(CSPetStampIntensifyReq)
	self:RegisterProtocol(CSPetStampSetBluePrint)
end

-- 神印背包与方案数据
function PetStampCtrl:OnSCPetStampBagInfo(protocol)
	--LogError("神印背包数据", protocol)
	local info = protocol.info
	local pet_stamp_list = info.pet_stamp_list
	self.data:SetPetStampList(pet_stamp_list)

	local blue_print_list = info.blue_print_list
	--LogError("方案列表",blue_print_list)
	self.data:SetBlueprintList(blue_print_list)

	self.data.pray_pos_index = protocol.info.pray_pos_index + 1
	--LogError("祈神位置", self.data.pray_pos_index)
	self.data.suit_new_falg_data = {}

	self.data:SetExpInfo({exp = protocol.info.exp},true)
end

function PetStampCtrl:OnSCPetStampInfo(protocol)
	--LogError("单个神印数据", protocol)
	local info = protocol.info
	local stamp = info.pet_stamp
	local old_stamp = self.data:GetPetStampByIndex(info.index)
	local old_stamp_info = old_stamp and old_stamp.info
	local need_open_swallow_succ = false
	if old_stamp and stamp.info.item_id ~= 0 and stamp.info.swallow_num > old_stamp_info.swallow_num then
		need_open_swallow_succ = true
	end
	self.data:SetPetStamp(info.index, info.pet_stamp,info.put_reason or 0)
	if not info.pet_stamp:PetIndex() then
		local suit_id = info.pet_stamp:SuitId()
		self.data:SetSuitNewFlag(suit_id,true)
	end
	self.data:SetExpInfo({exp = protocol.exp})
	if need_open_swallow_succ then
		ViewMgr:CloseView(PetStampSwallowView)
		self.data.swallow_smart.show_succ_index = info.index
		ViewMgr:OpenView(PetStampSwallowSucc,{index = info.index})
	end
end

function PetStampCtrl:OnSCPetStampExpInfo(protocol)
	local info = protocol.info
	-- local exp = self.data:GetExp()
	-- LogError("神印经验信息",exp, protocol)
	self.data:SetExpInfo({exp = protocol.info.exp})
end

function PetStampCtrl:OnSCPetStanpBluePrintInfo(protocol)
	--LogError("方案单个下发",protocol)
	self.data:SetBlueprint(protocol.info.blueprint, protocol.info.index)
end

-- 祈神日志列表
function PetStampCtrl:OnSCPetStampPrayLogInfo(protocol)
	--LogError("日志列表",protocol)
	self.data:SetPrayLogData(protocol)
end

-- 祈神日志信息
function PetStampCtrl:OnSCPetStampPrayLogContent(protocol)
	--LogError("日志详细信息",protocol)
	local pet_stamp = protocol.info.pet_stamp
	local info = {
        item_list = {pet_stamp}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
	-- 打开神印信息
end

-- 神印祈神奖励下发
function PetStampCtrl:OnSCPetStampPrayRewardInfo(protocol)
	--LogError("祈神奖励信息协议", protocol)
	--LogError("祈神服务端下发的总消耗", protocol.info.consume_coin, "结果：",protocol.info.result_seq_list)
	self.data.pray_pos_index = protocol.info.end_seq
	self.data:SetPrayData(protocol.info)
end

-- 更新神印方案
function PetStampCtrl:SetBlueprint(blueprint)
	local protocol = GetProtocol(CSPetStampSetBluePrint)
	protocol.name = blueprint:Name() 
	protocol.index = blueprint:Index()
	protocol.pet_index = blueprint:PetIndex() or -1
	protocol.bag_index_list = {}
	for i = 1, PetStampCfg.StampMaxPos do
		local stamp_index = 0
		if blueprint.stamp_index_list[i] then
			stamp_index = blueprint.stamp_index_list[i] < 0 and 0 or blueprint.stamp_index_list[i]
		end
		protocol.bag_index_list[i] = stamp_index
	end
	--LogError("请求神印方案",protocol)
	SendProtocol(protocol)
end


-- ============ 功能 ==========
-- pet_stamp:PetStamp

-- 替换       pet_stamp_1:被替换的那个
function PetStampCtrl:Replace(pet_stamp_1, pet_stamp_2)

end

-- 穿戴
function PetStampCtrl:OnUse(pet_stamp)
	local pet = PetData.Instance:GetCurSelect()
	if not pet then
		return
	end
	local pet_index = pet.index
	--LogError("穿戴请求",PET_OP_TYPE.PetStampUse, pet_index, pet_stamp.index)
	PetCtrl.SendPetOp(PET_OP_TYPE.PetStampUse, pet_index, pet_stamp.index)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.EquipEquipment)
end

-- 卸下
function PetStampCtrl:Unfix(pet_stamp)
	local pet_index = pet_stamp:PetIndex()
	if not pet_index then
		return
	end
	local pos_index = pet_stamp:PosIndex()
	--LogError("卸下请求",PET_OP_TYPE.PetStampUnfix, pet_index, pos_index)
	PetCtrl.SendPetOp(PET_OP_TYPE.PetStampUnfix, pet_index, pos_index)
end

-- 分享
function PetStampCtrl:Share(pet_stamp)

end

-- 分解
function PetStampCtrl:Resolve(pet_stamp)
	local pet_index = pet_stamp:PetIndex()
	if pet_index then
		PublicPopupCtrl.Instance:Center(Language.PetStamp.Error2)
		return
	end

	-- if pet_stamp:IsLock() then
	-- 	PublicPopupCtrl.Instance:Center(Language.PetStamp.Error3)
	-- 	return
	-- end
	local qua = pet_stamp:QuaLevel()
	if self.data:GetResloveData(qua) then
		PetCtrl.SendPetOp(PET_OP_TYPE.PetStampReslove, pet_stamp.index)
	else
		ViewMgr:OpenView(PetStampResloveDialog,{item = pet_stamp, func = function ()
			PetCtrl.SendPetOp(PET_OP_TYPE.PetStampReslove, pet_stamp.index)
		end})
	end
	
end

-- 升级
function PetStampCtrl:IntensifyByItem(pet_stamp, stuff_list)
	stuff_list = stuff_list or {}
	local protocol = GetProtocol(CSPetStampIntensifyReq)
	protocol.stamp_index = pet_stamp:Index()
	local num = 0
	protocol.index_list = {}
	for item,v in pairs(stuff_list) do
		if v then
			num = num + 1
			protocol.index_list[num] = item:Index()
		end
	end
	protocol.num = num
	for i = num + 1, PetStampCfg.IntensifyStuffMaxCount do
		protocol.index_list[i] = 0
	end
	SendProtocol(protocol)
	--LogError("请求了使用神印list升级",protocol)
end

function PetStampCtrl:IntensifyByExp(pet_stamp, exp)
	--LogError("请求了使用经验升级",PET_OP_TYPE.PetStampIntensify, pet_stamp:Index(), exp)
	PetCtrl.SendPetOp(PET_OP_TYPE.PetStampIntensify, pet_stamp:Index(), exp)
end

function PetStampCtrl:IntensifyByExpItem(pet_stamp, exp_stuff_list)
	exp_stuff_list = exp_stuff_list or {}
	for i,num in pairs(exp_stuff_list) do
		PetCtrl.SendPetOp(PET_OP_TYPE.PetStampExpStuffIntensify, pet_stamp:Index(), i.column_type, i.index, num)
		-- LogError("请求了使用经验材料升级：：：",PET_OP_TYPE.PetStampExpStuffIntensify, pet_stamp:Index(), i.column_type, i.index, num)
	end
end

-- 穿戴方案里面的神印
function PetStampCtrl:UseBlueprint(blueprint_index, pet_index)
	pet_index = pet_index or PetData.Instance:GetCurSelect().index
	--LogError("请求了直接使用方案",pet_index, blueprint_index)
	PetCtrl.SendPetOp(PET_OP_TYPE.PetStampUseBlueprint, pet_index, blueprint_index)
end

-- 分解背包里所有经验材料
function PetStampCtrl:ResloveAllExpStuff(stuff_list)
	stuff_list = stuff_list or BagData.Instance:GetPetStampExpStuffList()
	if TableIsEmpty(stuff_list) then
		return
	end
	for i,v in pairs(stuff_list) do
		if self.data.stamp_stuff_cfg[v.item_id] then
			PetCtrl.SendPetOp(PET_OP_TYPE.PetStampExpStuffReslove, v.column_type, v.index, v.num)
			--LogError("请求分解强化材料",PET_OP_TYPE.PetStampExpStuffReslove, v.column_type, v.index, v.num)
		end
	end
end

-- 执行分解设置界面的分解事件
function PetStampCtrl:ExcuteResloveSetEvent(stamp_list, stuff_list)
	stamp_list = stamp_list or self.data:GetPetStampList() or {}
	-- 自动分解设置
    for i,v in pairs(stamp_list) do
        if v:Type() == ItemType.PetStamp then
            if v.item_id > 0 and self.data:GetResloveData(v:QuaLevel()) and not v:IsLock() and not v:PetIndex() then
                PetStampCtrl.Instance:Resolve(v)
            end
        end
    end
    if self.data:GetResloveData(0) then
        PetStampCtrl.Instance:ResloveAllExpStuff(stuff_list)
    end
end

-- 释放自动分解所产生的经验
function PetStampCtrl:ReleaseAutoGetExp()
	--LogError("释放了",self.data.auto_get_exp)
	if self.data.auto_get_exp and self.data.auto_get_exp > 0 then
		PublicPopupCtrl.Instance:Center(string.format(Language.PetStamp.Tip7,self.data.auto_get_exp))
	end
	self.data.auto_get_exp = 0
end