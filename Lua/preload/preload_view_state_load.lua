PreloadViewStateLoad = PreloadViewStateLoad or DeclarePreloadView("preload/update")

-- PreloadViewStateLoadInst = nil
function PreloadViewStateLoad:__init()
    -- PreloadViewStateLoadInst = self
    self.hint_text = nil
    self.prog_slider = nil
    self.is_first = true
    self.is_need_check_version = true
end

function PreloadViewStateLoad:LoadCallback()
    self.hint_txt_tran = self:Obj().Find("hint_text")
    self.hint_text = self.hint_txt_tran:GetComponent(typeof(UnityUI.Text))
    self.hint_txt_tran:SetActive(false)

    self.audit_hint_text = self:Obj().Find("audit_hint_text"):GetComponent(typeof(UnityUI.Text))
    self.audit_hint_text.text = ""
    self.prog_slider_tran = self:Obj().Find("prog_slider")
    self.prog_slider = self.prog_slider_tran:GetComponent(typeof(UnityUI.Slider))
    self.prog_slider_tran:SetActive(false)

    self.AgeTipImgTran = self:Obj().Find("AgeTipImg")
    self.AgeTipImgTran:SetActive(false)
    self.restore_btn_tran = self:Obj().Find("restore_btn")
    local click_eve = self.restore_btn_tran:GetComponent(typeof(Game.LuaUIEventClick))
    if IS_MICRO_PLAYER() then
        self.restore_btn_tran:SetActive(false)
    end
    local btn_confirm_tran = self:Obj().Find("btn_confirm")
    local click_eve2 = btn_confirm_tran:GetComponent(typeof(Game.LuaUIEventClick))
    local btn_abandon_confirm_tran = self:Obj().Find("btn_abandon_confirm")
    local click_eve5 = btn_abandon_confirm_tran:GetComponent(typeof(Game.LuaUIEventClick))
    click_eve.OverrideFunc = function() self:OnClickRestore() end
    click_eve2.OverrideFunc = function() self:OnClickReinit() end
    click_eve5.OverrideFunc = function () self:OnClickAbandonConfirm() end
    self.dialog_tips = self:Obj().Find("dialog_tips").gameObject
    self.dialog_tips:SetActive(false)
    local abandon_dia_tran = self:Obj().Find("dialog_abandon_tips")
    self.abandon_dialog_tips = abandon_dia_tran.gameObject
    self.abandon_text = abandon_dia_tran.Find("abandon_content"):GetComponent(typeof(UnityUI.Text))
    self.abandon_dialog_tips:SetActive(false)
    self.wait_obj = self:Obj().Find("wait")
    self.special_loop = self:Obj().Find("special_loop")
    self.special_loop:SetActive(false)
    self.normal_loop = self:Obj().Find("normal_loop")
    self.wait_obj:SetActive(false)
    -- 版本检测所需 --
    local version_content_tran = self:Obj().Find("version_content")
    self.version_content = version_content_tran:GetComponent(typeof(UnityUI.Text))
    local version_richtext_tran = self:Obj().Find("version_richtext")
    self.obj_version_richtext = version_richtext_tran.gameObject
    
    local click_eve3 = version_richtext_tran:GetComponent(typeof(Game.LuaUIEventClick))
    local btn_version_confirm_tran = self:Obj().Find("btn_version_confirm")
    local click_eve4 = btn_version_confirm_tran:GetComponent(typeof(Game.LuaUIEventClick))
    click_eve3.OverrideFunc = function() self:OnRichTextClick() end
    click_eve4.OverrideFunc = function() self:OnVersionConfirmClick() end

    local txt_version_confirm_tran = self:Obj().Find("txt_version_confirm")
    self.txt_version_confirm = txt_version_confirm_tran:GetComponent(typeof(UnityUI.Text))
    self.txt_version_confirm.text = PreLanguage.Confirm
    self.dialog_version_tips = self:Obj().Find("dialog_version_tips").gameObject
    self.dialog_version_tips:SetActive(false)
    -- 版本检测所需 --

    local res_path, is_stream_asset = ChannelAgentConsts.UpdateBgPath()
    if is_stream_asset then
        self.bg_raw_image = self:Obj().Find("RawBg"):GetComponent(typeof(UnityUI.RawImage))
        -- local tex = ResourceManager.LoadStreamingTexture(res_path)

        local tex = UnityEngine.Texture2D.New(2,2,UnityEngine.TextureFormat.RGBA32,false)
        local pic_data = Nirvana.StreamingAssets.ReadAllBytes(res_path)
        UnityEngine.ImageConversion.LoadImage(tex, pic_data)

        self.bg_raw_image.texture = tex
        self.bg_raw_image:SetNativeSize()
        self.bg_raw_image.enabled = true
        self:Obj().Find("10010015"):SetActive(false)
        self:Obj().Find("10010814"):SetActive(false)
    end
    local res_path, is_stream_asset = ChannelAgentConsts.GetUIStremingAsset("SpecialLoop")
    if is_stream_asset then
        local special_loop_img = self.special_loop:GetComponent(typeof(UnityUI.RawImage))
        ChannelAgentConsts.SetRawImg(special_loop_img,res_path)
    end
    self.special_loop_show = is_stream_asset
    self.hint_text.text = PreLanguage.UpdateChecking
end

local function kbBytesToStr(kb_bytes)
    if kb_bytes > 1024 then
        return string.format("%.2fMB",kb_bytes/1024)
    else
        return string.format("%dKB",kb_bytes)
    end
end

function PreloadViewStateLoad:Update()
    PreloadViewState.Update(self)
    if self.is_first then
        self:CheckVersion()
        self.is_first = false
    end
    if self.is_need_check_version then
        return
    end
    local logic_state = PreloadData.Instance:GetState()
    if logic_state == PreloadData.State.Require then
        self.hint_text.text = PreLanguage.RequireLuaHint
        local req_per = PreloadData.Instance:GetScriptRequirePer()
        self.prog_slider.value = req_per
        if req_per >= 1 then
            self:Stop()
        end
    elseif logic_state == PreloadData.State.Update then
        if PreloadData.Instance:GetUpdateError() then
            self.hint_text.text = PreLanguage.AssetsUpdateError
        else
            local updated_bytes = PreloadData.Instance:GetUpdatedBytes()
            local total_update_byte = PreloadData.Instance:GetUpdateTotalBytes()
            local update_speed = PreloadData.Instance:GetUpdateSpeed()
            local per = total_update_byte ~= 0 and updated_bytes/total_update_byte or 0
            self.prog_slider.value = per
            local hint_txt = string.format(PreLanguage.UpdateHint,
                per*100,kbBytesToStr(updated_bytes),kbBytesToStr(total_update_byte),kbBytesToStr(update_speed))
            self.hint_text.text = hint_txt
        end
    elseif logic_state == PreloadData.State.Connect then
        local retry_count = PreloadData.Instance:GetConnectRetryCount()
        local max_retry_count = PreloadData.Instance:GetConnectMaxRetryCount()
        if retry_count > max_retry_count then
            self.hint_text.text = PreLanguage.ConnectFailed
            self.dialog_tips:SetActive(true)
        -- elseif GLOBAL_CONFIG:Flags().abandoned_device then
        --     self.hint_text.text = PreLanguage.ConnectFailed
        --     self.abandon_text.text = string.format(PreLanguage.AbandonedDevice, GLOBAL_CONFIG:Urls().abandoned_device_id)-- MainProber.device_id)
        --     self.abandon_dialog_tips:SetActive(true)
        else
            self.hint_text.text = string.format(PreLanguage.ConnectGetConfig,retry_count,max_retry_count)
        end
    elseif logic_state == PreloadData.State.UpdateCheck then
        if PreloadData.Instance:GetUpdateError() then
            self.hint_text.text = PreLanguage.UpdateCheckError
        else
            self.hint_text.text = PreLanguage.UpdateChecking
        end
    elseif logic_state == PreloadData.State.ExtluaUpdate then
        self.prog_slider.value = PreloadData.Instance:GetExtluaUpdatePer()
        self.hint_text.text = PreLanguage.ExtluaUpdate
    elseif logic_state == PreloadData.State.UpdateLuaBundle then
        self.prog_slider.value = PreloadData.Instance:GetUpdateLuabundlePer()
        self.hint_text.text = PreLanguage.UpdateLuaBundle
    end
    if IS_AUDIT_VERSION or GLOBAL_CONFIG:PkgInfo().single then
        if not IS_IOS() then
            self.AgeTipImgTran:SetActive(true)
        end
        if self.prog_slider_tran then
            self.prog_slider_tran:SetActive(false)
        end
        if self.hint_txt_tran then
            self.hint_txt_tran:SetActive(false)
        end
        if self:AuditSpecialAgentId() then
            self:SetAuditHintText()
            if self.wait_obj then
                self.wait_obj:SetActive(false)
            end
        else
            if self.wait_obj then
                self.wait_obj:SetActive(true)
                self.normal_loop:SetActive(not self.special_loop_show)
                self.special_loop:SetActive(self.special_loop_show)
            end
        end
        if GLOBAL_CONFIG:Flags().restore_state then
            self.restore_btn_tran:SetActive(false)
        end
    else
        if self.prog_slider_tran then
            self.prog_slider_tran:SetActive(true)
        end
        if self.hint_txt_tran then
            self.hint_txt_tran:SetActive(true)
        end
        if self.wait_obj then
            self.wait_obj:SetActive(false)
        end
    end
end

function PreloadViewStateLoad:SetAuditHintText()
    local num = math.floor(Time.realtimeSinceStartup * 2) % 4
    local str = ""
    for i = 0, num do
        str = str .. PreLanguage.RequireLuaHintPoint
    end
    self.audit_hint_text.text = PreLanguage.RequireLuaHintAudit .. str
end

function PreloadViewStateLoad:AuditSpecialAgentId()
    if GLOBAL_CONFIG:PkgInfo().agent_id == "nky" then
        return true
    else
        return false
    end
end

function PreloadViewStateLoad:OnClickRestore()
    -- UnityEngine.Debug.LogError("OnClickRestore")
    MainCleanClientPreOperation()
end

function PreloadViewStateLoad:OnClickAbandonConfirm()
    GameUtil.StopGame()
end

function PreloadViewStateLoad:OnClickReinit()
    self.dialog_tips:SetActive(false)
    PreloadData.Instance:SetConnectRetryCount(1)
end

-- 检查版本号
-- up_version    版本号上限
-- down_version  版本号下限
-- version_tip   版本号不足文案提示
-- version_url   去下载的url
function PreloadViewStateLoad:CheckVersion()
    if IS_EDITOR then
        self.is_need_check_version = false
        return
    end
    -- 将版本号变成数字
    local get_version_num = function(version)
        local num_version = string.gsub(version, "%.", "")
        num_version = string.gsub(num_version, "%s", "")
        return num_version == "" and 0 or tonumber(num_version)
    end
    self.now_version = get_version_num(GLOBAL_CONFIG:PkgInfo().version or "")
    self.up_version = get_version_num(GLOBAL_CONFIG:ReminderInfo().up_version or "")
    self.down_version = get_version_num(GLOBAL_CONFIG:ReminderInfo().down_version or "")
    if StringIsEmpty(self.now_version) or StringIsEmpty(self.up_version) or StringIsEmpty(self.down_version) then
        self.is_need_check_version = false
        LogError("版本号有问题，或者是该渠道没有设置版本更新上下限")
        return
    end

    self.version_content.text = GLOBAL_CONFIG:ReminderInfo().version_tip or ""
    self.obj_version_richtext:SetActive(not StringIsEmpty(GLOBAL_CONFIG:ReminderInfo().version_url))
    self.dialog_version_tips:SetActive(self.up_version > self.now_version)
    self.is_need_check_version = self.up_version > self.now_version
end

function PreloadViewStateLoad:OnRichTextClick()
    if not StringIsEmpty(GLOBAL_CONFIG:ReminderInfo().version_url) then
        UnityApp.OpenURL(GLOBAL_CONFIG:ReminderInfo().version_url)
    end
end

function PreloadViewStateLoad:OnVersionConfirmClick()
    self.is_need_check_version = false
    if self.down_version > self.now_version then
        UnityApp.Quit()
    else
        self.dialog_version_tips:SetActive(false)
    end
end

return PreloadViewStateLoad