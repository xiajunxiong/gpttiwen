RoleCreateVideoView = RoleCreateVideoView or DeclareView("RoleCreateVideoView", "login/role_create_video_view", 0, ViewLayer.Normal)
---RoleList====UIWidgetList

function RoleCreateVideoView:RoleCreateVideoView()
    self.click_times = 5
    self.data = RoleCreateData.Instance
end

function RoleCreateVideoView:LoadCallback()
    self.data:LockUpDirector(true)
    AudioMgr:StopBg()
    self.RawImage.mainTexture:DiscardContents()
    PublicPopupCtrl.Instance:HideWait()
    self.close_view_func = function ()
        self:JumpToSecond()
    end
    self.delay_close = TimeHelper:AddDelayTimer(self.close_view_func, 66)
end

function RoleCreateVideoView:JumpToSecond()
    if self.VideoImgObj2.activeSelf then
            ViewMgr:CloseView(RoleCreateVideoView)
    else
        self.VideoImgObj:SetActive(false)
        self.VideoImgObj2:SetActive(true)

        if self.delay_close ~= nil then
            TimeHelper:CancelTimer(self.delay_close)
        end
        self.delay_close = TimeHelper:AddDelayTimer(function ()
            ViewMgr:CloseView(RoleCreateVideoView)
        end,10)
    end
end

function RoleCreateVideoView:OnDestroy()
    if self.delay_close ~= nil then
        TimeHelper:CancelTimer(self.delay_close)
        self.delay_close = nil
    end
    if self.hideSkipBtnDelay ~= nil then
        TimeHelper:CancelTimer(self.hideSkipBtnDelay)
        self.hideSkipBtnDelay = nil
    end
    if self.render_texture ~= nil then
        RenderTexture.ReleaseTemporary(self.render_texture)
        self.render_texture = nil
    end
    self.data:UnLockDirector()
    self.VideoImgObj:SetActive(false)
end

function RoleCreateVideoView:CloseCallback()
    AudioMgr:SetBgFlag(true)
end

function RoleCreateVideoView:OnClickPass()
    local dialog_info = {
        content = Language.RoleCreate.SkipStoryVideo,
        confirm = {func = function ()
            if self.hideSkipBtnDelay ~= nil then
                TimeHelper:CancelTimer(self.hideSkipBtnDelay)
                self.hideSkipBtnDelay = nil
            end
            self.SkipBtnObj:SetActive(false)
            self:JumpToSecond()
            ViewMgr:CloseView(DialogTipsView)
        end},
        -- cancel = {func = function ()
        --     ViewMgr:CloseView(DialogTipsView)
        -- end},
        tnr = DTTodayNotRemind.RoleCreateVideo
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_info)
end

function RoleCreateVideoView:OnClickBg()
    if not self.VideoImgObj2.activeSelf then
        self.SkipBtnObj:SetActive(true)
        self.CanvasG.alpha = 1
        if self.hideSkipBtnDelay ~= nil then
            TimeHelper:CancelTimer(self.hideSkipBtnDelay)
            self.hideSkipBtnDelay = nil
        end
        self.hideSkipBtnDelay = TimeHelper:AddDelayTimer(function ()
            self.Animation:Play()
        end,5)
    end
end
-----------------------------------------------------
RoleCreateVideoApiView = RoleCreateVideoApiView or DeclareView("RoleCreateVideoApiView", "login/role_create_video_api_view", 0, ViewLayer.Normal)
function RoleCreateVideoApiView:RoleCreateVideoApiView()
    self.click_times = 5
    self.data = RoleCreateData.Instance
end

function RoleCreateVideoApiView:LoadCallback()
    self.data:LockUpDirector(true)--锁定场景中的点击事件
    AudioMgr:StopBg()
    self.close_view_func = function ()
        self:JumpToSecond()
    end
    self.VideoImgObj:SetActive(true)
    self.VideoPlayer.loopPointReached = self.VideoPlayer.loopPointReached + self.close_view_func
    self.VideoPlayer2.loopPointReached = self.VideoPlayer2.loopPointReached + self.close_view_func
    Runner.Instance:AddRunObj(self, 1)
end

function RoleCreateVideoApiView:JumpToSecond()
    if self.VideoImgObj2.activeSelf then
            ViewMgr:CloseView(RoleCreateVideoApiView)
    else
        self.VideoImgObj:SetActive(false)
        self.VideoImgObj2:SetActive(true)
    end
end

function RoleCreateVideoApiView:Update()
    if self.VideoImgObj.activeSelf and self.RawImage.texture == nil then
        self.RawImage.texture = self.VideoPlayer.texture
        self.RawImage.enabled = true
    end
    if self.VideoImgObj2.activeSelf then
        if self.RawImage2.texture == nil then
            self.RawImage2.texture = self.VideoPlayer2.texture
        else
            self.RawImage2.enabled = true
        end
    end
end

function RoleCreateVideoApiView:OnDestroy()
    if self.hideSkipBtnDelay ~= nil then
        TimeHelper:CancelTimer(self.hideSkipBtnDelay)
        self.hideSkipBtnDelay = nil
    end
    self.VideoPlayer.loopPointReached = self.VideoPlayer.loopPointReached - self.close_view_func
    self.VideoPlayer2.loopPointReached = self.VideoPlayer2.loopPointReached - self.close_view_func
    
    self.data:UnLockDirector()--解锁场景中的点击事件
    self.VideoImgObj:SetActive(false)
end

function RoleCreateVideoApiView:CloseCallback()
    AudioMgr:SetBgFlag(true)
    Runner.Instance:RemoveRunObj(self, 1)
end

function RoleCreateVideoApiView:OnClickPass()
    local dialog_info = {
        content = Language.RoleCreate.SkipStoryVideo,
        confirm = {func = function ()
            if self.hideSkipBtnDelay ~= nil then
                TimeHelper:CancelTimer(self.hideSkipBtnDelay)
                self.hideSkipBtnDelay = nil
            end
            self.SkipBtnObj:SetActive(false)
            self.RawImage.enabled = false
            self:JumpToSecond()
            ViewMgr:CloseView(DialogTipsView)
        end},
        cancel = {func = function ()
            self.VideoPlayer:Play()
            ViewMgr:CloseView(DialogTipsView)
        end},
        tnr = DTTodayNotRemind.RoleCreateVideo
    }
    if VersionTag.HasTag(VersionTag.ShowCGSupport) then
        self.VideoPlayer:Pause()
    end
    PublicPopupCtrl.Instance:DialogTips(dialog_info)
end

function RoleCreateVideoApiView:OnClickBg()
    if not self.VideoImgObj2.activeSelf then
        self.SkipBtnObj:SetActive(true)
        self.CanvasG.alpha = 1
        if self.hideSkipBtnDelay ~= nil then
            TimeHelper:CancelTimer(self.hideSkipBtnDelay)
            self.hideSkipBtnDelay = nil
        end
        self.hideSkipBtnDelay = TimeHelper:AddDelayTimer(function ()
            self.Animation:Play()
        end,5)
    end
end

