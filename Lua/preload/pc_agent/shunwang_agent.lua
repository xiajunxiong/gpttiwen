local shun_wang_agent = {}
--by agent_channel_agent
function shun_wang_agent.PCMai(role_id, role_name, amount, server_id, callback, gift_type)
	if VersionTag.HasTag(VersionTag.GameAgentPC) then
		if StringIsEmpty(GLOBAL_CONFIG:Urls().pc_recharge_url) then
			PublicPopupCtrl.Instance:Center(Format(Language.PCNotice.RechargeError,"-1"))
			return
		end
		local spid = GLOBAL_CONFIG:PkgInfo().agent_id
		local orderNo = spid.."_".. os.time() .."_"..gift_type
		local uid = LoginData.Instance:GetAccountKey()
		local swtag = Nirvana.GameAgent.GetExtraInfo()
		local otherData = ""
		local url_format = "%s?guid=%s&orderNo=%s&rmb=%s&time=%s&otherData=%s&swTag=%s&roleId=%s&size=140"
		local send_url = Format(url_format, 
			GLOBAL_CONFIG:Urls().pc_recharge_url,
			uid,
			orderNo,
			amount,
			os.time(),
			otherData,
			swtag,
			role_id
		)
		-- LogError("ShunWang getQRCode url", send_url)
		WebRequester:Request(send_url, function (data, error)
			if StringIsEmpty(error) then
				local ret, suc = TryParseJson(data)
				if suc then
					-- LogError("ShunWang getQRCode",data)
					if ret.response.code == 0 and GLOBAL_CONFIG:PkgInfo().agent_id == "ps1" then
						ViewMgr:OpenView(PCWebView,{title_str = Language.Recharge.Recharge, qrcode = ret.response.qrCodeUrl, amount = amount})
					end
				else
					PublicPopupCtrl.Instance:Center(Format(Language.PCNotice.RechargeError, "-3"))
				end
			else
				PublicPopupCtrl.Instance:Center(Format(Language.PCNotice.RechargeError, "-2"))
			end
		end)
	end
end

return shun_wang_agent