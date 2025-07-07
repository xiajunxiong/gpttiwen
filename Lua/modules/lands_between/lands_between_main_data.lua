function LandsBetweenData:TestAddRoleInfo()
    local vo = {}
    vo.uid = "64572" -- MsgAdapter.ReadInt() -- 角色id
    vo.plat_type = "ka1"--MsgAdapter.ReadInt()  -- 渠道号
    vo.user_name = "内测人员α"--MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) -- 角色名
	vo.server_id = 106--MsgAdapter.ReadShort() -- 服务器ID
	vo.row = 49--MsgAdapter.ReadShort() -- 行
	vo.column = 48--MsgAdapter.ReadShort() -- 列
	vo.faction = 3--MsgAdapter.ReadShort() -- 玩家所属阵营[0,4)
	vo.avatar_type = 2--MsgAdapter.ReadShort() -- 模型
	vo.profession = 500--MsgAdapter.ReadInt() -- 职业

	local list = {}
	table.insert(list,vo)

	self:SetAllUserInfo({users = list})

	self:SetUserInfo({
		my_info = {
			plat_type = 555,
			row = 11,
			column = 56,
			faction = 0,
		}
	})
end 

