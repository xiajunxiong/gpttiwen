LoadingView = LoadingView or DeclareView("LoadingView", "loading", 0, ViewLayer.Top)
VIEW_DECLARE_MASK(LoadingView, ViewMask.None)
----BgSetter====UIWTextureSetter
----HintTxt=====Text

function LoadingView:LoadCallback()
    --Debuger.LogError
    -- local rnd = GameMath.Ramdon(1, 2)
    -- local path
    -- if rnd == 2 then
    --     path = "loading/loading2.png"
    -- else
    --     path = "loading/loading3.png"
    -- end
    -- self:SetBg()
	PublicPopupCtrl.Instance:HideWait()
    -- local value = 0
    -- self.handle_time = TimeHelper:AddRunFrameTimer(
	-- 	function()
    --         self.Prog.fillAmount = value / 100
	-- 		self.ProgPer.text = string.format("%s%%", value)
	-- 		if SceneCtrl.Instance:CurScene() and SceneCtrl.Instance:CurScene():LoadingComplete() then
	-- 			value = value + 2
	-- 		else
	-- 			value = value + 1
	-- 		end
	-- 		if value >= 100 then
	-- 			self.Prog.fillAmount = 1
	-- 			self.ProgPer.text = "100%"
	-- 			ViewMgr:CloseView(LoadingView)
	-- 		end
    --     end
    -- ,1,101)
    if GuildHongBaoPanel.Instance then GuildHongBaoPanel.Instance.hide.count = 0 end
    self.care_handle = LoadingData.Instance:ViewData():Care(BindTool.Bind(self.OnDataChange,self))
    self:OnDataChange()
    for i = 1, self.CornerImgs:Length() do
        self.CornerImgs[i]:SetActive(not GLOBAL_CONFIG:Flags().block_loading_anim)
    end
    self.ProgObj:SetActive(not (IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan))
    self.WaitObj:SetActive(IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan)
end

function LoadingView:SetBg()
    local bg_path, is_stream_asset = ChannelAgentConsts.LoadingBgPath()
    if not is_stream_asset then
        ResMgr:LoadObject(bg_path,function(tex)
                if self.BgRawImg then
                    self.BgRawImg.texture = tex
                end
            end,false)
        -- if is_stream_asset == false then
            self.LogoObj:SetActive(false)
        -- end
    else
        local tex = ResourceManager.LoadStreamingTexture(bg_path)
        self.BgRawImg.texture = tex
        self.LogoObj:SetActive(false)
    end
end

function LoadingView:OnDataChange()
    local per = LoadingData.Instance:Percent()
    self.Prog.fillAmount = per
    local desc = IS_AUDIT_VERSION and PreLanguage.RequireLuaHint or LoadingData.Instance:Desc()
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
        UH.SetText(self.HintTxt,"")
        UH.SetText(self.ProgPer,"")
    else
        UH.SetText(self.HintTxt, StringIsEmpty(desc) and PreLanguage.RequireLuaHint or desc)
        UH.SetText(self.ProgPer, string.format("%s%%", math.floor(per * 100)))
    end
    if per >= 1 then
        ViewMgr:CloseView(LoadingView)
        if GuildHongBaoPanel.Instance then GuildHongBaoPanel.Instance.hide.count = 1 end
        --[[ if LoginData.Instance:IsNewChara() then
            EventSys:Fire(GlobalEventKeys.RoleCreateEnterScene)
            LoginData.Instance:ClearIsNewChara()
        end ]]
    end
end

function LoadingView:CloseCallback()
    LoadingData.Instance:ViewData():Uncare(self.care_handle)
    -- self.Prog.fillAmount = 1
	-- self.ProgPer.text = "100%"
	-- TimeHelper:CancelTimer(self.handle_time)
end

LoadingBgPanel = LoadingBgPanel or DeclareMono("LoadingBgPanel",UIWFlushPanel)
function LoadingBgPanel:LoadingBgPanel()
    self.is_loaded = false
end

function LoadingBgPanel:Awake()
    local bg_path, is_stream_asset = ChannelAgentConsts.LoadingBgPath()
    if not is_stream_asset then
        ResMgr:LoadObject(bg_path,function(tex)
                self.is_loaded = true
                if self.BgRawImg then
                    self.BgRawImg.texture = tex
                end
            end,false)
        -- if is_stream_asset == false then
            self.LogoObj:SetActive(false)
        -- end
    else
        local tex = ResourceManager.LoadStreamingTexture(bg_path)
        self.BgRawImg.texture = tex
        self.LogoObj:SetActive(false)
        self.is_loaded = true
    end
    UIWFlushPanel.Awake(self)
end

function LoadingBgPanel:IsLoaded()
    return self.is_loaded
end