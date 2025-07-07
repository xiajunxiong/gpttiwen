WaitView = WaitView or DeclareView("WaitView", "public_popup/wait", nil, ViewLayer.Top + 2)
VIEW_DECLARE_MASK(WaitView,ViewMask.Block)
VIEW_DECLARE_CACHE(WaitView)

-----Str======Text

function WaitView:WaitView()
	self.care_handle = nil
end

function WaitView:LoadCallback()
	self.care_handle = PublicPopupData.Instance:Base():Care(BindTool.Bind(self.onStrChange,self),"wait_str")
	self:onStrChange()
end


function WaitView:CloseCallback()
	PublicPopupData.Instance:Base():Uncare(self.care_handle)
	self.care_handle = nil
	local state = LoginData.Instance:LoginState()
	if (state == LoginStateType.Logined or state == LoginStateType.SceneLogining) and self.cache_str == Language.Login.Reconnecting then
		local flag = FightData.Instance:GetMeetMonsterFlag()
		if flag == 0 then
			local time = UnityPlayerPrefs.GetInt(PrefKeys.GoPatrol())
			local offset = math.floor(Time.time - time)
			if offset >=0 and offset <= 50 then
				BehaviorCtrl.ExeByCfgVo({behavior_type = 102})
			end
		end
	end
	if ViewMgr:IsOpen(GuildTrialNew) then
		ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GUILD_BOSS)
	end
end

function WaitView:onStrChange()
	self.Str.text = PublicPopupData.Instance:Base().wait_str
	if not StringIsEmpty(PublicPopupData.Instance:Base().wait_str) then
		self.cache_str = PublicPopupData.Instance:Base().wait_str
	end
end

function WaitView:OnClickBlock()
	-- PublicPopupCtrl.Instance:HideWait()
end