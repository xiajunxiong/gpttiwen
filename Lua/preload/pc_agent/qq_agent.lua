local qq_agent = {}
--by dev_channel_agent
function qq_agent.PCMai(role_id, role_name, money, server_id, callback, gift_type)

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

	local get_midashi_url = "http://"..GLOBAL_CONFIG:GetUrlHead().."/"..GLOBAL_CONFIG:PkgInfo().agent_id.."/buy_goods.php?"
	local openid = LoginData.Instance:GetAccountKey()
	local openkey = Nirvana.GameAgent.GetToken()
	local appid = 1112173951
	local pfKey = Nirvana.GameAgent.GetExtraInfo()

	local param = WebRequester:UrlEncode(GameUtil.EncodeB64("price=" .. money .. "&num=1&oid=" .. order_id))
	get_midashi_url = get_midashi_url .. "openid=" .. openid .."&openkey=" .. openkey .. "&pfkey="..pfKey.."&pf=qqgame&appid="..appid.."&param="..param

	WebRequester:Request(get_midashi_url, function (data, error)
		if StringIsEmpty(error) then
			local ret, status = TryParseJson(data)
			if status then
				if ret.ret == 0 then
					local url = "qqgameprotocol:///openembedwebdialog Caption=支付 Width=800 Height=680 New=1&Url=https://qqgame.qq.com/midaspay/?"
					local param = {}
					param.openid = openid
					param.openkey = openkey
					param.sandbox = ret.sand_box
					param.goodstokenurl = ret.url_params
					param.action="buy"
					param.appid=1450043355
					param = cjson.encode(param)
					param = WebRequester:UrlEncode(param)
					url = url.."param="..param
					Nirvana.QQPay.OpenUrlPay(url)
				else
					LogError("QQPCMai Failed ret:",ret)
				end
			end
		else
			LogError("QQPCMai Error",error)
		end
	end) 
end

function qq_agent.PCPlatBehaviourTrigger(behaviour)
	local url_head = GLOBAL_CONFIG:Urls().plat_report_url
	if StringIsEmpty(url_head) then
		return
	end
	local roleid = RoleId()
	local roleName = RoleData.Instance:GetRoleName()
	roleName = WebRequester:UrlEncode(roleName)
	local serverid = LoginData.Instance:GetLastLoginServer()
	local roleLevel = RoleLevel()
	local accountId = LoginData.Instance:GetAccountKey()
	local real_url = nil
	local deviceModel = WebRequester:UrlEncode(UnityEngine.SystemInfo.deviceModel)
	local appkey = "wlshzjpc"
	local version = string.gsub(GLOBAL_CONFIG:PkgInfo().version or '', '%.', '')
	local channel_id = GLOBAL_CONFIG:PkgInfo().agent_id
	if behaviour == BehaviourType.CreateRole then --角色创建
		real_url = string.format("%s?appkey=%s&version=%s&serverid=%s&accountid=%s&roleid=%s&rolename=%s&td_type=%s&channel_id=%s", url_head, appkey, version, serverid, accountId, roleid, roleName,"register", channel_id)
	elseif behaviour == BehaviourType.RoleLogin then --角色登录
		real_url = string.format("%s?appkey=%s&version=%s&serverid=%s&accountid=%s&roleid=%s&rolename=%s&level=%s&DeviceModel=%s&imei=&accname=%s&td_type=%s&channel_id=%s", url_head, appkey, version, serverid, accountId, roleid, roleName, roleLevel, deviceModel, accountId, "loggedin", channel_id)
	elseif behaviour == BehaviourType.RoleLogout then --角色退出
		real_url = string.format("%s?appkey=%s&version=%s&serverid=%s&accountid=%s&roleid=%s&rolename=%s&level=%s&accname=%s&td_type=%s&channel_id=%s", url_head, appkey, version, serverid, accountId, roleid, roleName, roleLevel, accountId, "logout_td", channel_id)
	elseif behaviour == BehaviourType.LevelUp then --角色升级
		real_url = string.format("%s?appkey=%s&version=%s&serverid=%s&accountid=%s&roleid=%s&rolename=%s&level=%s&level1=%s&td_type=%s&channel_id=%s", url_head, appkey, version, serverid, accountId, roleid, roleName, roleLevel, roleLevel-1,"level_up", channel_id)
	end
	if real_url ~= nil then
		WebRequester:Request(real_url, function(data, error)
			if StringIsEmpty(error) then
				-- print("QQTrigger ret==" .. data)
			end
		end)
	end
end

return qq_agent