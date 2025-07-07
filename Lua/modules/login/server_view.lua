
ServerView = ServerView or DeclareView("ServerView","login/server",0,ViewLayer.Top)
VIEW_DECLARE_MASK(ServerView,ViewMask.Block)
---ServerTypeList=======UIWidgetList[LoginServerTypeItem]
---ServerList===========UIWidgetList[LoginServerListItem]

function ServerView:LoadCallback()
	self.data = LoginCtrl.Instance:Data()			--data === LoginData
	self.Anim:ListenEvent("show_type_list", BindTool.Bind(self.FlushServerListSelect, self))
	
	PublicPopupCtrl.Instance:CloseActTips()
	if IS_AUDIT_VERSION then
		for i = 1, self.AuditHide:Length() do
			self.AuditHide[i]:SetActive(GLOBAL_CONFIG:PkgInfo().is_youyan)
		end
		for i = 1, self.AuditShow:Length() do
			self.AuditShow[i]:SetActive(not GLOBAL_CONFIG:PkgInfo().is_youyan)
		end
	else
		for i = 1, self.AuditHide:Length() do
			self.AuditHide[i]:SetActive(true)
		end
		for i = 1, self.AuditShow:Length() do
			self.AuditShow[i]:SetActive(false)
		end
	end
end

function ServerView:FlushServerListSelect()
	local server_list_config = GLOBAL_CONFIG:ServerInfoOther()
	if server_list_config[1] ~= nil then
		self.ServerListSelectList:SetData(server_list_config)
		-- LogError("server_list_index ",LoginData.Instance.view_sel_server_id.server_list_index)
		self.ServerListSelectList:ClickItem(LoginData.Instance.view_sel_server_id.server_list_index)
	else
		self:FlushTypeList()
	end
end

function ServerView:FlushTypeList()
	local server_data = LoginCtrl.Instance:Data():GetServerData()
	self.ServerTypeList:SetData(server_data)
	self.ServerTypeList:ClickItem(1)
	self.ListAnim.enabled = true
end

function ServerView:OnCloseServerListBtnClick()
	ViewMgr:CloseView(ServerView)
end

function ServerView:OnSelectServerType(selected_type_data)
	if selected_type_data == nil then
		return
	end
	local line1_data = {}
	local line2_data = {}
	local line3_data = {}
	for i,v in ipairs(selected_type_data) do
		local lineI = i%3
		if lineI == 1 then
			table.insert(line1_data,v)
		elseif lineI == 2 then
			table.insert(line2_data,v)
		else
			table.insert(line3_data,v)
		end
	end
	self.ListLine1:SetData(line1_data)
	self.ListLine2:SetData(line2_data)
	self.ListLine3:SetData(line3_data)
end

function ServerView:OnSelectServer(selected_data)
	local old_server = LoginData.Instance.view_sel_server_id.server_id > 0 
			and LoginData.Instance.view_sel_server_id.server_id or LoginData.Instance:GetLastLoginServer()
	LoginData.Instance.view_sel_server_id.server_id = selected_data.id
	ViewMgr:FlushView(WelcomeView,WelcomeView.FK.Server)
	ViewMgr:CloseView(ServerView)
	-- LogError("OnSelectServer ",LoginData.Instance.view_sel_server_id.account_spid)
	if SceneCtrl.Instance:CurScene():GetClassType() ~= LoginScene 
		and old_server ~= LoginData.Instance.view_sel_server_id.server_id then
			self:StartConnect()
	end
end
function ServerView:StartConnect()
	local curr_server_id = self.data:GetLastLoginServer()
	if self.data.view_sel_server_id.server_id > 0 then
		self.data:SaveLastLoginServer(self.data:GetAccountKey(),self.data.view_sel_server_id.server_id)
	end
	local servers_info = GLOBAL_CONFIG:ServerInfoOther(self.data.view_sel_server_id.server_list_index)
	local sel_server_info = self.data:GetServerInfoById(self.data:GetLastLoginServer(), self.data:GetLastLoginAccountSpid())
	local now_server_time = servers_info.server_time + (Time.realtimeSinceStartup - GLOBAL_CONFIG.client_time)
	--保险点取完就重置？
	self.data:SaveLastLoginServer(self.data:GetAccountKey(), curr_server_id)
	--非超级用户才判断开服时间
	if self.data.account.account_type ~= PLAT_ACCOUNT_TYPE_TEST then
		if not StringIsEmpty(sel_server_info.open_time) and not StringIsEmpty(sel_server_info.ahead_time) and now_server_time < sel_server_info.open_time - sel_server_info.ahead_time then
		-- if true then
			local t_time = os.date("*t", sel_server_info.open_time)
			-- 提示不要了，改成弹窗
			-- local tip_str =string.format(Language.Login.RushDungeonips, t_time.month, t_time.day, t_time.hour, t_time.min)
			-- tip_str = string.format(Language.Login.ServerOpenTips2, tip_str)
			-- PublicPopupCtrl.Instance:Center(tip_str)
			local dialog_info = {
				content = Format(Language.Login.ServerOpenTips3, t_time.year, t_time.month, t_time.day, t_time.hour, t_time.min),
				confirm = {
					func = function()
						self.data:SaveLastLoginServer(self.data:GetAccountKey(), curr_server_id)
						ViewMgr:CloseView(DialogTipsView)
					end
				},
				no_cancel = true,
			}
			PublicPopupCtrl.Instance:DialogTips(dialog_info)
			return
		end
		-- 删档计费弹窗
		local account_data = LoginData.Instance:AccountData()
		if account_data.fc_recharge > 0 then
			local dialog_info = {
				content = Format(Language.Login.ReturnRechargeTip, account_data.fc_recharge, account_data.login_day, account_data.fill_gold),
				confirm = {
					func = function()
						self:RequestServerAccountLogin()
						ViewMgr:CloseView(DialogTipsView)
					end
				},
			}
			PublicPopupCtrl.Instance:DialogTips(dialog_info)
		else
			self:RequestServerAccountLogin()
		end
	-- end
	-- LogError("need_reverify",self.data.need_reverify)
	-- if self.data.need_reverify then
	-- 	LoginCtrl.Instance:SendReverify(self.data:GetPlatformName(), function ()
	-- 		self:RequestServerAccountLogin()
	-- 	end)
	else
		-- 删档计费弹窗
		local account_data = LoginData.Instance:AccountData()
		if account_data.fc_recharge > 0 then
			local dialog_info = {
				content = Format(Language.Login.ReturnRechargeTip, account_data.fc_recharge, account_data.login_day, account_data.fill_gold),
				confirm = {
					func = function()
						self:RequestServerAccountLogin()
						ViewMgr:CloseView(DialogTipsView)
					end
				},
			}
			PublicPopupCtrl.Instance:DialogTips(dialog_info)
		else
			self:RequestServerAccountLogin()
		end
	end
end
function ServerView:RequestServerAccountLogin()
	LoadingCtrl.Instance:AutoPercent(function()
		local judge_scene = SceneCtrl.Instance:CurScene() ~= nil and SceneCtrl.Instance:CurScene() or SceneCtrl.Instance:NextScene()
		return judge_scene and judge_scene:LoadingComplete()
	end)
	-- ViewMgr:OpenView(LoadingView)
	Runner.Instance:RunUntilTrue(function ()
		if ViewMgr:IsOpen(LoadingView) then
			LoginData.Instance:SaveLastLoginServer(LoginData.Instance:GetAccountKey(),LoginData.Instance.view_sel_server_id.server_id)
			LoginCtrl.Instance:ReloginQuietly()
			LoginData.Instance:SetLoginBackType(RELOGIN_TYPE.ROLE_SELECT)
			return true
		else
			return false
		end
	end)
end

function ServerView:OnClickSelectListBtn(data)
	LoginData.Instance.view_sel_server_id.server_list_index = data.index
	LoginData.Instance.view_sel_server_id.account_spid = data.account_spid
	-- LogError("server_list_index",data.index)
	self:FlushTypeList()
end

---------ServerListSelectItem------------
ServerListSelectItem = ServerListSelectItem or DeclareMono("ServerListSelectItem", UIWidgetBaseItem)
function ServerListSelectItem:ServerListSelectItem()
end

function ServerListSelectItem:SetData(data)
	self.data = data
	local title_str = StringIsEmpty(data.title) and string.format(Language.Login.ServerListBtnFormat, data.index + 1) or data.title
	UH.SetText(self.ServerListBtnText, title_str)
end

function ServerListSelectItem:Click()
	self.Toggle.isOn = true
end