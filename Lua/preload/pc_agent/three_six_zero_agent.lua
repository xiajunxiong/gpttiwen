local three_six_zero_agent = {}
--by dev_channel_agent
function three_six_zero_agent.PCMai(role_id, role_name, money, server_id, callback, gift_type)
	if VersionTag.HasTag(VersionTag.GameAgentPC) then
		--计算order_id
		local spid = InstallPackageInfo:AgentId()
		local channel_id = LoginData.Instance:AccountData().spid
		if GetLocalType() == LocalType.CN then
			if GLOBAL_CONFIG:Flags().get_child_spid then
				if channel_id and channel_id ~= "0" then
					spid = channel_id
				end
			end
		end
		local uid = LoginData.Instance:GetAccountKey()
		local order_id = tostring(server_id .. "_"..role_id.."_"..spid.."_".. uid .. "_" .. os.time().."_0_"..gift_type)
		if GLOBAL_CONFIG:Flags().short_order_id then
			order_id = tostring(server_id .. "_"..role_id.."_"..spid.."_".. uid .. "_0_0_"..gift_type)
		end
		if GLOBAL_CONFIG:Flags().log_print then
			print("order_id "..tostring(order_id))
		end
		--计算order_id结束

		local url = three_six_zero_agent.PC360Mai(role_id, role_name, money, server_id, callback, gift_type, uid, order_id)
		if GLOBAL_CONFIG:PkgInfo().agent_id == "pa1" then
			ViewMgr:OpenView(PCWebView,{title_str = Language.Recharge.Recharge, url_str = url})
		end
	end
end

function three_six_zero_agent.PC360Mai(role_id, role_name, money, server_id, callback, gift_type, uid, order_id)
	if StringIsEmpty(GLOBAL_CONFIG:Urls().pc_recharge_url) or StringIsEmpty(GLOBAL_CONFIG:Urls().mai_key) then
		return
	end
	local sign_key = GLOBAL_CONFIG:Urls().mai_key or "SkQJtl9PNggBwZaLcfavBeQjMNbCWNhf"
	local skey = "S"..server_id
	local sign = GameUtil.StrMd5(uid.."wlshzjpc"..skey..money..role_id.."1"..order_id..sign_key)
	local url_format = "%s?uid=%s&skey=%s&amount=%s&rid=%s&sign=%s&exts_cp=%s&platform=wan&gkey=wlshzjpc&pid=1"
	local url = string.format(url_format, 
		GLOBAL_CONFIG:Urls().pc_recharge_url,
		uid,
		skey,
		money,
		role_id,
		sign,
		order_id)
	return url
end

function three_six_zero_agent.PCPlatBehaviourTrigger(behaviour)
	if behaviour == BehaviourType.LevelUp then
		if RoleLevel() <= 2 or RoleLevel() >= 30 then
			three_six_zero_agent.PC360Trigger(behaviour)
		end
	else
		three_six_zero_agent.PC360Trigger(behaviour)
		three_six_zero_agent.PC360Trigger1(behaviour)
	end
end

function three_six_zero_agent.PC360Trigger(behaviour)
	local url_head = GLOBAL_CONFIG:Urls().plat_report_url
	local app_key = GLOBAL_CONFIG:Urls().app_key
	if StringIsEmpty(url_head) or StringIsEmpty(app_key) then
		return
	end
	local gkey = "wlshzjpc"
	local qid = LoginData.Instance:GetAccountKey()
	local server_id = LoginData.Instance:GetLastLoginServer()
	local level = RoleLevel()
	local role_id = RoleId()
	local role_name = RoleData.Instance:GetRoleName()
	local sign = GameUtil.StrMd5(gkey..level..role_name..role_id..qid..server_id..app_key)
	local form = {}
    form.qid = qid
    form.role_id = role_id
    form.server_id = server_id
    form.level = level
	form.gkey = gkey
	form.name = WebRequester:UrlEncode(role_name)
    -- LogError("signstr","game_id="..game_id.."&level="..form.level.."&open_time="..form.open_time.."&pay_total="..form.pay_total.."&role_id="..form.role_id.."&server_no="..form.server_no.."&time="..form.time.."|"..app_key)
    form.sign = sign
    -- LogError("form",form)
    WebRequester:Post(url_head,function (data,error)
        if StringIsEmpty(error) then
            if GLOBAL_CONFIG:Flags().log_print then
                print("PlatReport callback data",data)
            end
        end
    end,form)
	print("behaviour = "..tostring(behaviour))
end

function three_six_zero_agent.PC360Trigger1(behaviour)
	if behaviour ~= BehaviourType.CreateRole and behaviour ~= BehaviourType.RoleLogin then
		return
	end
	local url_head = "https://dd.mgame.360.cn/t/gameinfo/log"
	local interface = behaviour == BehaviourType.RoleLogin and "login" or "create_role"
	local ip = GLOBAL_CONFIG:Urls().client_ip
	local gid = 1139
	local sid = "S"..LoginData.Instance:GetLastLoginServer()
	local user = LoginData.Instance:GetAccountKey()
	local roleid = RoleId()
	local dept = 38
	local rolename = RoleData.Instance:GetRoleName()
	local level = RoleLevel()
	local url = Format("%s?interface=%s&gid=%s&sid=%s&oldsid=%s&user=%s&roleid=%s&dept=%s&time=%s&ip=%s&gname=wlshzjpc",
		url_head,
		interface,
		gid,
		sid,
		sid,
		user,
		roleid,
		dept,
		os.time(),
		ip)

	if behaviour == BehaviourType.CreateRole then
		url = url .. Format("&rolename=%s&prof=%s&channel=0&poster=0&site=0",
			rolename,
			RoleData.Instance:GetRoleProfId()
		)
	elseif behaviour == BehaviourType.RoleLogin then
		url = url .. Format("&level=%s&map_id=%s",
			level,
			SceneData.Instance:SceneId()
		)
	end
	WebRequester:Request(url,
		function(data, error)
            if StringIsEmpty(error) then
                if GLOBAL_CONFIG:Flags().log_print then
                    print("PC360Trigger1 " .. tostring(data))
                end
            end
        end)
end

function three_six_zero_agent:OpenPCRealNameSignView()
	ViewMgr:OpenView(SanLiuLingFcmView)
end

return three_six_zero_agent