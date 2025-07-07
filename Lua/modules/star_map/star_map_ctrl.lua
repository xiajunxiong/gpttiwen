FILE_REQ("modules/star_map/star_map_data")

VIEW_REQ("modules/star_map/star_map_view")
VIEW_REQ("modules/star_map/star_map_supper_view")
VIEW_REQ("modules/star_map/star_map_reset_view")
VIEW_REQ("modules/star_map/star_map_attri_view")
VIEW_REQ("modules/star_map/star_map_sel_skill")

-- resetstarchart: 

StarMapCtrl = StarMapCtrl or BaseClass(BaseCtrl)

StarMapCtrl.OpType = {
	LevelUp = 0,		-- 星图星位升级	p1 星图类型，p2星图索引 * p3阶数
	LevelUpSuper = 1,	-- 超星系星位升级	p1超星系星位索引	*
	--Reset = 2,			-- 重置星图	p1 星图类型 *
	ResetSuper = 3,		-- 重置超星图	*	
	--UnlockSkill = 4,	-- 解锁技能孔位	p1 星图类型 p2 技能孔索引	>> 无用，自动解锁
	InstallSkill = 5,	-- 装备技能	p1 星图类型， p2技能孔索引 p3 技能在超星系的索引
	AutoUp = 6,			-- 一键升级 p1 星图类型 p2 星图索引 * p3阶数
	AutoUpSupper = 7,	-- 超星系一键升级 p1 超星系星位索引	*
}

StarMapCtrl.MapType =
{
	ROLE = 0,		--// 角色星图
	PET = 1,		--// 宠物星图
	PARTNER = 2,	--// 伙伴星图
	Super = 3,		-- 超星系
};

StarMapCtrl.SuperBgImg = {
	[0] = "ShuXingDiPuTong",
	[1] = "ShuXingDiTeShu",
	[2] = "JiNengDi108",
}

function StarMapCtrl:__init()
	if StarMapCtrl.Instance ~= nil then
		Debuger.LogError("[StarMapCtrl] attempt to create singleton twice!")
		return
	end
	StarMapCtrl.Instance = self
	self.data = StarMapData.New()
	self.data = StarMapData.Instance
	self:RegisterAllProtocols()
	
	RemindCtrl.Instance:Remind():Register(
        Mod.StarMap.Super,
        self.data.remind_data,
        function ()
            return self.data:RemindSuper()
        end
	)

	RemindCtrl.Instance:Remind():Register(
        Mod.StarMap.Role,
        self.data.remind_data,
        function ()
            return self.data:RemindRole()
        end
	)

	RemindCtrl.Instance:Remind():Register(
        Mod.StarMap.Pet,
        self.data.remind_data,
        function ()
            return self.data:RemindPet()
        end
	)

	RemindCtrl.Instance:Remind():Register(
        Mod.StarMap.Partner,
        self.data.remind_data,
        function ()
            return self.data:RemindPartner()
        end
	)
	
	BagData.Instance:ListenItems(self.data:RemindItemList(),function ()
        self.data:CheckRemind()
    end)
end

function StarMapCtrl:__delete()
    StarMapCtrl.Instance = nil
end

function StarMapCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function StarMapCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSStarMapReq)
	self:RegisterProtocol(SCStarChartAllInfo, "OnSCStarChartAllInfo")
	self:RegisterProtocol(SCStarMapInstallSkillNotice, "OnSCStarMapInstallSkillNotice")
end


function StarMapCtrl:SendReq(op_type, p1, p2, p3)
	local protocol = GetProtocol(CSStarMapReq)
	protocol.op_type = op_type or 0
	protocol.p1 = p1 or 0
	protocol.p2 = p2 or 0
	protocol.p3 = p3 or 0
	SendProtocol(protocol)
end

function StarMapCtrl:OnSCStarChartAllInfo(protocol)
	self.data:SetAllData(protocol)
	self.data:CheckRemind()
end

function StarMapCtrl:OnSCStarMapInstallSkillNotice(protocol)
	self.data:InstallSkillNotice(protocol)
end