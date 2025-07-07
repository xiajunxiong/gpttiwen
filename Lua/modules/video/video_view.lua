
local video_fit = {
	[5] = {2, 50}
}

VideoView = VideoView or DeclareView("VideoView", VideoConfig.ResPath .. "video", 0, ViewLayer.Top - 2)
VIEW_DECLARE_MASK(VideoView, ViewMask.Block)

function VideoView:VideoView()
	self.data = VideoCtrl.Instance:Data()
	self.video = self.data.cur_video

	local fit_co = video_fit[self.video.info.id]
	if fit_co and DeviceData.ScreenAspect() > fit_co[1] then
		self.VideoRect.offsetMin = Vector2.New(self.VideoRect.offsetMin.x, fit_co[2])
	end
end

function VideoView:LoadCallback()
	AudioMgr:SetBgFlag(false)
	AudioMgr:SetSingletonListenerEnable(true)
	if self.data.cur_video.info.mask == 1 then
		self.FullScreenMask:SetActive(true)
		self.FullScreenBg:SetActive(false)
		self.mask_delay_timer = TimeHelper:AddDelayTimer(function ()
			self.data.video_smart:SetDirty("fade_in_end")
			self.FullScreenBg:SetActive(true)
		end, 1.5)
	end
	if self.data.cur_video.info.id == 4 then
		RoleCreateData.Instance:LockUpDirector(true)
	elseif self.data.cur_video.info.id == 7 then
		self.FullScreenBg:SetActive(false)
	end
end

function VideoView:CloseCallback()
	local flag = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_MUSIC_VOLUME) > 0
	AudioMgr:SetBgFlag(flag)
	AudioMgr:ResetSingletonListenerEnable()
	if self.mask_delay_timer then
		TimeHelper:CancelTimer(self.mask_delay_timer)
		self.mask_delay_timer = nil
	end
	self.FullScreenMask:SetActive(false)
	if self.data.cur_video.info.id == 4 then
		RoleCreateData.Instance:UnLockDirector()
	end
end
--------------------------------------------
VideoViewF = VideoViewF or DeclareMono("VideoViewF", UIWFlushPanel)
function VideoViewF:VideoViewF()
	self.data = VideoCtrl.Instance:Data()
	self.video = self.data.cur_video
	
	self.end_func = BindTool.Bind(self.video.EndVideo, self.video)
	self.VideoPlayer.loopPointReached = self.VideoPlayer.loopPointReached + self.end_func
	--要先有遮幕的先播放遮幕动画
	if self.video.info.mask == 1 then
		self.is_loaded = true
		self.data_cares = {
			{data = self.data.video_smart, func = self.LoadResObj, keys = {"fade_in_end"}, init_call = false}
		}
	else
		self.is_loaded = false
		self:LoadResObj()
	end
	self.hide_bullet = GLOBAL_CONFIG:PkgInfo().agent_id == "ma1" and CHILD_CHANNEL_ID == "200510001"

	self.destroyed = false
	if self.hide_bullet then
		self.BtnBulletObj:SetActive(false)
	end
end

function VideoViewF:LoadResObj()
	ResMgr:LoadObject(self.video:ResPath(), BindTool.Bind(self.OnLoaded, self))
			self.timer_wait = TimeHelper:AddDelayTimer(function ()
				PublicPopupCtrl.Instance:ShowWait(Language.Video.WaitShow)
			end, VideoConfig.WAIT_TIME)
		self.anim_handle = self.SkipShowAnim:ListenEvent("view_state", function (eve_param)
			if eve_param == "hided" then
				self.SkipShowAnim:SetObjActive(false)
			end
		end)
end

function VideoViewF:OnDestroy()
	self.destroyed = true
	UIWFlushPanel.OnDestroy(self)
	self.VideoPlayer.loopPointReached = self.VideoPlayer.loopPointReached - self.end_func
	self.SkipShowAnim:UnlistenEvent("view_state", self.anim_handle)
	TimeHelper:CancelTimer(self.timer_dt)
	TimeHelper:CancelTimer(self.timer_wait)
	TimeHelper:CancelTimer(self.bullet_runner)
	BattleData.Instance:BaseData():SetDirty("clear_bullets")
	BattleData.Instance:BaseData().bullet_on = false
	self.VideoPlayer.targetTexture:Release()
end

function VideoViewF:OnLoaded(obj)
	TimeHelper:CancelTimer(self.timer_wait)
	if self.destroyed then return end
	if self.video.info.mask == 1 then
		self.MaskAnimation:Play("fadeOut")
	end
	PublicPopupCtrl.Instance:HideWait()
	self.VideoPlayer.clip = obj
	self.VideoPlayer:Play()
	self.is_loaded = true
	self.bullet_data, self.custom_bullet_str_t , self.custom_bullet_nums = self.video:GetBulletData()
	if self.bullet_data ~= nil and not self.hide_bullet then
		self.cur_bullet_seq = 1
		BattleData.Instance:BaseData().bullet_on = true
		self.bullet_runner = TimeHelper:AddRunTimer(function ()
			local bullet_str = ""
			--有没有当前新发的
			if StringIsEmpty(self.next_bullet) then
				--有没有以前发送的记录
				if self.custom_bullet_str_t[self.cur_bullet_seq] ~= nil then
					bullet_str = string.format("<color=#%s>%s</color>", COLORSTR.Blue11, ChatFilter.Instance:Filter(self.custom_bullet_str_t[self.cur_bullet_seq]))
				elseif self.bullet_data[self.cur_bullet_seq] then
					bullet_str = string.format("<color=#%s>%s</color>", COLORSTR.White, self.bullet_data[self.cur_bullet_seq] or "")
				end
				self.cur_bullet_seq = self.cur_bullet_seq + 1
			else
				self.custom_bullet_nums = self.custom_bullet_nums + 1
				local playerPrefKey = PrefKeys.StoryCustomBullet()..self.video.info.id..self.custom_bullet_nums
				UnityPlayerPrefs.SetString(playerPrefKey,self.next_bullet.."_"..self.cur_bullet_seq)
				bullet_str = string.format("<color=#%s>%s</color>", COLORSTR.Blue11, ChatFilter.Instance:Filter(self.next_bullet))
			end
			BattleData.Instance.bullet_comments:Insert({text = bullet_str})
			self.next_bullet = nil
		end,0.4, #self.bullet_data + self.custom_bullet_nums)
		
	end

	if self.video.info.id == 7 then
		self.video:EndVideo()
	end
end

function VideoViewF:IsLoaded()
	return self.is_loaded
end

function VideoViewF:OnClickBlock()
	self.SkipShowAnim:SetObjActive(true)
	self.SkipShowAnim:SetBool(APH("is_show"), true)
	if self.hide_bullet then
		self.BtnBulletObj:SetActive(false)
	elseif self.bullet_data ~= nil then
		self.BtnBulletObj:SetActive(not self.BtnBulletObj.activeSelf)
	end
	TimeHelper:CancelTimer(self.timer_dt)
	self.timer_dt = TimeHelper:AddDelayTimer(function ()
		self.SkipShowAnim:SetBool(APH("is_show"), false)
    end, VideoConfig.SKIP_SHOW_TIME)
end

function VideoViewF:OnClickSkip()
	self.video:EndVideo()
end

function VideoViewF:OnClickHideBullet()
	BattleData.Instance:BaseData().bullet_on = not BattleData.Instance:BaseData().bullet_on
	UH.SpriteName(self.HideBtnImg, BattleData.Instance:BaseData().bullet_on and "DanMu" or "PingBiDanMu")
end

function VideoViewF:OnClickSendBullet()
	local str = self.BulletInput.text
	if StringIsEmpty(str) then
		PublicPopupCtrl.Instance:Center(Language.Video.BulletStringIsEmpty)
		return
	elseif string.len(str) > 50 then
		PublicPopupCtrl.Instance:Center(Language.Video.BulletStringMuchLong)
		return
	end
	UH.SetText(self.BulletInput, "")
	self.next_bullet = str
end