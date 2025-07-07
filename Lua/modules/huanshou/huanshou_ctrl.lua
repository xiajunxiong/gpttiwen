FILE_REQ("modules/huanshou/huanshou")
FILE_REQ("modules/huanshou/huanshou_data")
VIEW_REQ("modules/huanshou/huanshou_view")
VIEW_REQ("modules/huanshou/huanshou_attri_panel")
VIEW_REQ("modules/huanshou/huanshou_skill_panel")
VIEW_REQ("modules/huanshou/huanshou_star_panel")
VIEW_REQ("modules/huanshou/huanshou_egg_view")
VIEW_REQ("modules/huanshou/huanshou_skill_study")
VIEW_REQ("modules/huanshou/huanshou_gain_view")
VIEW_REQ("modules/huanshou/huanshou_soul_resolve")
VIEW_REQ("modules/huanshou/huanshou_fushen")
VIEW_REQ("modules/huanshou/huanshou_equip_view")

-- =========== 幻兽 ============
HuanShouCtrl = HuanShouCtrl or BaseClass(BaseCtrl)

HuanShouReqType = {
	Info = 0,		-- 请求下发所有信息     *
	OpenEgg = 1,	-- //!< 请求打开幻兽蛋 p1:bag_column p2:bag_index   *
	UpLevel = 2,	-- //!< 请求强化幻兽 p1:huanshou_id                 *
	StudySkill = 3,	-- //!< 请求学习技能 p1:huanshou_id p2:bag_index p3:lock_flag
	SaveSkill = 4,	-- //!< 请求保存技能 p1:huanshou_id p2:is_save(0:清除 非0:保存)
	UpStar = 5,		-- //!< 请求学习潜能点位 p1:huanshou_id p2:point_index      *
	LianDan = 6,	-- //!< 请求炼丹(潜能果)            *
	FuShen = 7,		-- //!< 请求附身 p1:huanshou_id     *
	CancelFuShen = 8,	-- //!< 请求取消附身            *
	EggRecord = 9,		-- //!< 请求公告信息            *
    HuanShouInfo = 10,	-- //!< 请求下发单个幻兽信息 p1.huanshou_id     *
	ShowModel = 11,     -- 请求是否使用幻兽形象 0不使用，1使用			  *
	EquipLevelUp = 12,	-- 请求升级幻兽装备 p1:幻兽id, p2:装备idx
}


HuanShouNoticeType =
{
	NOTICE_TYPE_ACTIVE = 0,				--// 幻兽激活 p1.huanshou_id *
	NOTICE_TYPE_ACTIVE_CONVERT = 1,		--// 幻兽激活转化 p1.huanshou_id
	NOTICE_TYPE_STR_LEVEL = 2,			--// 幻兽强化(进化) p1.huanshou_id p2.str_level     *
	NOTICE_TYPE_LEARN_SKILL = 3,		--// 幻兽学习技能 p1.huanshou_id p2.new_skill_grid_idx p3.new_skill_id
	NOTICE_TYPE_CLEAR_LEARN_SKILL = 4,	--// 幻兽清除准备学习技能 p1.huanshou_id	*
	NOTICE_TYPE_SAVE_LEARN_SKILL = 5,	--// 幻兽保存学习技能 p1.huanshou_id p2.new_skill_grid_idx p3.new_skill_id
	NOTICE_TYPE_LEARN_POINT = 6,		--// 幻兽学习点位 p1.huanshou_id p2.point_index p3.point_level  *
	NOTICE_TYPE_EGG_OPEN_COUNT = 7,		-- 幻兽蛋稀有保底， p1:egg_id, p2: egg_times
}

MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM = 10;			--//幻兽技能格子数量(当前)
MAX_HUAN_SHOU_GRID_NUM = 200 					-- 幻兽背包格子

function HuanShouCtrl:__init()
	if HuanShouCtrl.Instance ~= nil then
		Debuger.LogError("[HuanShouCtrl] attempt to create singleton twice!")
		return
	end
	HuanShouCtrl.Instance = self
    HuanShouData.New()
    self.data = HuanShouData.Instance
	self:RegisterAllProtocols()
	
	RemindCtrl.Instance:Remind():Register(
        Mod.HuanShou.Attri,
        self.data.remind_data,
        function ()
            return self.data:GetAttriTabRemind()
        end
	)

	RemindCtrl.Instance:Remind():Register(
        Mod.HuanShou.Star,
        self.data.remind_data,
        function ()
            return self.data:GetStarTabRemind()
        end
	)

	RemindCtrl.Instance:Remind():Register(
        Mod.HuanShou.Egg,
        self.data.remind_data,
        function ()
            return self.data:GetEggTabRemind()
        end
	)
	
	BagData.Instance:ListenItems(self.data:RemindItemList(),function ()
        self.data:CheckRemind()
    end)
end

function HuanShouCtrl:__delete()
    HuanShouCtrl.Instance = nil
end

function HuanShouCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function HuanShouCtrl:OnInit()
end

function HuanShouCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSHuanShouReq)
    self:RegisterProtocol(CSHuanShouDecomposeReq)
    self:RegisterProtocol(SCHuanShouAllInfo, "OnSCHuanShouAllInfo")
    self:RegisterProtocol(SCHuanShouInfo, "OnSCHuanShouInfo")
    self:RegisterProtocol(SCHuanShouNoticeInfo, "OnSCHuanShouNoticeInfo")
    self:RegisterProtocol(SCHuanShouOtherInfo, "OnSCHuanShouOtherInfo")
    self:RegisterProtocol(SCHuanShouBagInfo, "OnSCHuanShouBagInfo")
    self:RegisterProtocol(SCHuanShouBagItemInfo, "OnSCHuanShouBagItemInfo")
    self:RegisterProtocol(SCHuanShouEggRecordInfo, "OnSCHuanShouEggRecordInfo")
end

function HuanShouCtrl:SendReq(req_type, p1, p2, p3)
    local protocol = GetProtocol(CSHuanShouReq)
    protocol.req_type = req_type
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    protocol.p3 = p3 or 0
    SendProtocol(protocol)
end

function HuanShouCtrl:SendResolve(list, column_type)
    local protocol = GetProtocol(CSHuanShouDecomposeReq)
	protocol.list = list
	protocol.column_type = column_type or 0
    SendProtocol(protocol)
end


function HuanShouCtrl:OnSCHuanShouAllInfo(protocol)
	local info_list = protocol.huan_shou_info_list
	for i,v in pairs(info_list) do
		local huanshou = HuanShou.New(v)
		self.data:SetHuanShou(huanshou)
	end
	self.data.open_egg_times_list:Set(protocol.open_egg_times_list or {})
end

function HuanShouCtrl:OnSCHuanShouInfo(protocol)
	local info = protocol.info
	local huanshou = HuanShou.New(info)
	self.data:SetHuanShou(huanshou)
end

function HuanShouCtrl:OnSCHuanShouNoticeInfo(protocol)
	self.data:HandleNotice(protocol)
	self.data:CheckRemind()
end

function HuanShouCtrl:OnSCHuanShouOtherInfo(protocol)
	local info = protocol.info
	if self.data.other_data.fushen_id and info.fushen_id ~= -1 and info.fushen_id ~= self.data.other_data.fushen_id then
		local huanshou = self.data:GetHuanShou(info.fushen_id)
		if huanshou then
			self.data:ShowFuShenView(huanshou)
		end
	end
	self.data.other_data:Set(info)
end

function HuanShouCtrl:OnSCHuanShouBagInfo(protocol)
	local item_list = protocol.item_list
	self.data.bag_item_list:Set(item_list)
end

function HuanShouCtrl:OnSCHuanShouBagItemInfo(protocol)
	self.data:SetBagItem(protocol)
end

function HuanShouCtrl:OnSCHuanShouEggRecordInfo(protocol)
    self.data:SetEggRecord(protocol.list)
end