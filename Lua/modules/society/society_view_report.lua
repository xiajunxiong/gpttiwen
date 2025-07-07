
SocietyViewReport = SocietyViewReport or DeclareView("SocietyViewReport", SocietyConfig.ResPath .. "society_report")

function SocietyViewReport:SocietyViewReport()
	self.data = SocietyCtrl.Instance:Data()
	self.chat_data = ChatCtrl.Instance:Data()
	self.language = Language.Society
end
 
function SocietyViewReport:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.SocietyReport, Vector2.New(644, 536))

	self.info = self.data.cur_sel_role.info
	self.Name.text = self.info.name
	
	self.ReasonList:SetData(self.chat_data.config_report)
end

function SocietyViewReport:OnClickReport()
	-- *	zone_id	平台ID: alw
	-- *	server_id	区服ID:
	-- *	user_id	用户ID
	-- *	role_id	角色ID
	-- *	role_name	角色名字
	-- *	role_level	角色等级
	-- *	role_vip	角色vip
	-- *	role_gold	角色当前剩余元宝(可以暂时传0)
	-- *	role_scene	角色当前场景ID
	-- *	user_device	用户手机信息
	-- *	user_ver	客户端版本信息
	-- *	issue_type	问题类型 1/2/3/4 提交bug/投诉/建议/举报角色
	-- *	issue_subject	问题标题
	-- *	issue_content	问题正文
	-- *  issue_role_id   被举报的角色id
	-- *  issue_role_name   被举报的角色名

	local role_base_data = RoleData.Instance:GetBaseData()
	local login_data = LoginData.Instance
	local report_info = {
		zone_id = LoginData.Instance:GetAccountSpid() or 0,
		server_id = LoginData.Instance:GetLastLoginServer() or 0,
		user_id = 0,
		role_id = role_base_data.role_id or 0,
		role_name = role_base_data.name or 0,
		role_level = role_base_data.level or 0,
		role_vip = 0,
		role_gold = RoleData.Instance:GetCurrency(CurrencyType.Gold) or 0,
		role_scene = SceneData.Instance:SceneId() or 0,
		user_device = UnityEngine.SystemInfo.deviceModel or 0,
		user_ver = GLOBAL_CONFIG:PkgInfo().assets_version or 0,
		issue_type = 4,
		issue_subject = 0,
		issue_content = self.InputField.text or 0,
		issue_role_id = self.info.uid or 0,
		issue_role_name = self.info.name or 0,
	}

	report_info.user_id = string.format("%s_%s",tostring(login_data:GetPlatformName()), tostring(login_data:GetLastLoginServer())) or 0

	local issue_subject = ""
	for _, data in pairs(self.ReasonList:GetSelectedData()) do
		issue_subject = string.format("%s|%s", issue_subject, data.report_type)
	end
	report_info.issue_subject = string.sub(issue_subject, 2, #issue_subject) or 0

	local function tourl(url)
		for key, val in pairs(report_info) do
			url = string.format("%s%s=%s&", url, key, val)
		end
		return string.sub(url, 0, #url - 1)
	end
	
	WebRequester:Request(tourl(GLOBAL_CONFIG:Urls().gm_report_url .. "?"),function(data,error)
		PublicPopupCtrl.Instance:Center(Language.Society.Report.Succ)
		ViewMgr:CloseView(SocietyViewReport)
	end)
end

SocietyViewReportItem = DeclareMono("SocietyViewReportItem", UIWidgetBaseItem)

function SocietyViewReportItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Reason.text = data.report_type
end

function SocietyViewReportItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
end