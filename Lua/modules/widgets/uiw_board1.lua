------Title====UI.Text
------Tabbar===UIWidgetList
-----Type====0, 1 light
UIWBoard1 = UIWBoard1 or DeclareMono("UIWBoard1")
UIWBoard1.Effect = {
    LBLight = 1,    --左下角的光
    FengLing = 2,   --左上角风铃
}
local crossPos1 = Vector3.New(-300, 100, 0)
local crossPos2 = Vector3.New(-62, 240, 0)
function UIWBoard1:UIWBoard1()
    self.title_str = ""
    self.view_type = nil

    self.money_effect_list = {}
    self.calc_step_key_list = {}
    self.shoot_curve_list = {}
    self.start_frame_list = {}
    self.start_step_key_list = {}
    self.effect_act_cache = {}
end

function UIWBoard1:OnClickClose()
    if self.view_type ~= nil then
        if ViewMgr:IsOpen(self.view_type) then
            self:CloseFunc()
        end
        ViewMgr:CloseView(self.view_type)
    end
end
function UIWBoard1:onCurrencyChange(old)
    if self:Destroyed() then 
        if self.curr_handle then
            RoleData.Instance:GetCurrencyData():Uncare(self.curr_handle)
            self.curr_handle = nil
        end
        return 
    end
    if not self:IsActiveAndEnabled() then
        return
    end

    --0元宝2铜币
    local count = 3
    local diff = 0
    if old then
        for key, value in pairs(old) do
            diff = (tonumber(RoleData.Instance:GetCurrency(key))or 0) - (tonumber(value) or 0)
            if key == 0 and diff > 0 then
                if diff > 3000 then
                    count = count + math.floor(diff / 3000)
                end
                count = math.min(count, 20)
                -- self:PlayCurrEffect(Vector3.New(0, -130, 0), Vector3.New(-317.6, 353, 0), self.YuanBao, 10010264, count)
            elseif key == 2 and diff > 0 then
                if diff > 3000 then
                    count = count + math.floor(diff / 3000)
                end
                count = math.min(count, 20)
                self:PlayCurrEffect(Vector3.New(0, -130, 0), Vector3.New(-584, 349.5, 0), self.TongBi, 10010265, count)
            end
        end
    end
end
-- 起点 终点 复制对象 特效key 需要多少个
function UIWBoard1:PlayCurrEffect(startPos, endPos, playObj, key, count)
    self:ClearEffect(key)
    for i = 1, count, 1 do
        self.shoot_curve_list[key][i] =
            Game.CurveParabola(startPos, endPos, i % 2 == 0 and crossPos1 or crossPos2, GameMath.Ramdon())
    end
    local index = 0
    --[[ local update_fun = function()
        local time = Time.frameCount - self.start_frame
        --帧一帧
        if time % 2 == 0 then
            -- 播特效
            index = index + 1
            if index <= count then
                self.money_effect_list[key][index] = UnityGameObj.Instantiate(playObj, self.RootNode)
                self.start_frame_list[key][index] = Time.frameCount
                self.calc_step_key_list[key][index] =
                    TimeHelper:AddRunFrameTimer(BindTool.Bind(self.UpdatePos, self, key, index, count), 1, 20)
                self.EffectTool:Play(key, self.money_effect_list[key][index].gameObject)
            end
        end
    end
    self.start_frame = Time.frameCount
    self.start_step_key_list[key] = TimeHelper:AddRunFrameTimer(update_fun, 1, count * 2) ]]

    self.start_time = Time.realtimeSinceStartup
    self.start_step_key_list[key] = TimeHelper:AddCountDownTT(function ()
        local time = Time.realtimeSinceStartup - self.start_time
        if time % (2 / 24) <= (1 / 24) then
            index = index + 1
            if index <= count then
                self.money_effect_list[key][index] = UnityGameObj.Instantiate(playObj, self.RootNode)
                self.start_frame_list[key][index] = Time.realtimeSinceStartup
                self.calc_step_key_list[key][index] =
                    TimeHelper:AddCountDownTT(BindTool.Bind(self.UpdatePos, self, key, index, count, true),function ()
                    end, (20 / 24), (1 / 24))
                self.EffectTool:Play(key, self.money_effect_list[key][index].gameObject)
            end
        end
    end,function ()
    end,(count * 2 / 24), (1 / 24))
end
function UIWBoard1:ClearEffect(key, ignore)
    if self.money_effect_list[key] == nil then
        self.money_effect_list[key] = {}
    end
    if self.calc_step_key_list[key] == nil then
        self.calc_step_key_list[key] = {}
    end
    if self.start_frame_list[key] == nil then
        self.start_frame_list[key] = {}
    end
    if self.start_step_key_list[key] == nil then
        self.start_step_key_list[key] = {}
    end
    TimeHelper:CancelTimer(self.start_step_key_list[key])
    if self.calc_step_key_list[key] then
        for key, value in ipairs(self.calc_step_key_list[key]) do
            TimeHelper:CancelTimer(value)
        end
        if not ignore then
            for _, v in ipairs(self.money_effect_list[key]) do
                UnityGameObj.Destroy(v.gameObject)
            end
        end
        self.shoot_curve_list[key] = {}
        self.calc_step_key_list[key] = {}
        self.money_effect_list[key] = {}
    end
end
function UIWBoard1:UpdatePos(key, index, count, usetime)
    if index <= count then
        local time = 0
        if usetime then
            time = (Time.realtimeSinceStartup - self.start_frame_list[key][index]) / (20/24)
        else
            time = (Time.frameCount - self.start_frame_list[key][index]) / 20
        end
        time = time > 1 and 1 or time
        UH.LocalPos(self.money_effect_list[key][index], self.shoot_curve_list[key][index]:GetPos(time))
        if time == 1 and (index == 1 or index == 7 or index == 14 or index == 20) then --index == 1 7 14 20才有资格播放特效
            if key == 10010264 then
                self.EffectTool:Play(10010266,self.EffectRoot[1])
            else
                self.EffectTool:Play(10010266,self.EffectRoot[2])
            end
        end
    end
end
function UIWBoard1:SetData(view_type, title_str)
    if self.curr_handle ~= nil then
        RoleData.Instance:GetCurrencyData():Uncare(self.curr_handle)
        self.curr_handle = nil
    end
    self.curr_handle = RoleData.Instance:GetCurrencyData():Care(BindTool.Bind(self.onCurrencyChange, self))
    self.view_type = view_type
    self.title_str = title_str
    self.Title.text = self.title_str
    self.TitleBg:SetActive(not StringIsEmpty(title_str))
    self.AuditBg:SetActive(false)
    self.BtnComp:SetActive(false)

    if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
		local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset("Board1AuditBg")
		if streaming_has then
			self.AuditBg:SetActive(true)
		end
	end
end

function UIWBoard1:SetTabbarVisible(visible)
    self.Tabbar:SetObjActive(visible)
end

function UIWBoard1:OnRelease()
    if self.curr_handle then
        RoleData.Instance:GetCurrencyData():Uncare(self.curr_handle)
        self.curr_handle = nil
    end
    self.Tabbar:SetObjActive(false)
    self:ClearEffect(10010264)
    self:ClearEffect(10010265)
    --还原已经隐藏的特效
    for t,act in pairs(self.effect_act_cache) do
        if act == false then
            self.effect_act_cache[t] = true
            self.EffectObj[t]:SetActive(true)
        end
    end
end
function UIWBoard1:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
    if self.curr_handle then
        RoleData.Instance:GetCurrencyData():Uncare(self.curr_handle)
        self.curr_handle = nil
    end
    self:ClearEffect(10010264, true)
    self:ClearEffect(10010265, true)
end
--这里设置隐藏需要的特效
--type===UIWBoard1.Effect
--show===bool
function UIWBoard1:SetEffectActive(type, show)
    local act_cache = self.effect_act_cache[type] or true
    if act_cache ~= show then
        self.effect_act_cache[type] = show
        self.EffectObj[type]:SetActive(show)
    end
end

function UIWBoard1:SetDecsActive(show)
    for i=1,2 do
        self.Decs[i]:SetActive(show)
    end
end

function UIWBoard1:SetCloseFunc(func)
    self.close_func = func
end

function UIWBoard1:CloseFunc()
    TimeHelper:CancelTimer(self.handle_delay)
    if self.close_func ~= nil then
        self.close_func()
        self.close_func = nil
    end
end

function UIWBoard1:SetAuditBgShow()
    self.AuditBg:SetActive(true)
end
--新服比拼显示 rank_type
function UIWBoard1:SetCompetitionRank(rank_type)
    CompetitionRanklCtrl.Instance:SendCompetitionRankType(rank_type)
    --[[ local config_list = CompetitionRanklData.Instance:GetBtnList()
    local config = nil
    for k, v in pairs(config_list) do
        if v.type == rank_type then
            config = v
        end
    end
    if config then ]]
        local is_open = CompetitionRanklData.Instance:GetIsOpenEnd(rank_type)
        -- if is_open then
            self.Effect2:SetActive(not is_open)
        -- end
        self.BtnComp:SetActive(is_open)
        UH.SetText(self.CompDesc, Language.CompetitionRank.RankName[rank_type])
        TimeHelper:CancelTimer(self.handle_delay)
        self.handle_delay = TimeHelper:AddDelayFrameTimer(function ()
            self.Fitter.enabled = false
            self.Fitter.enabled = true
        end, 2)
    --[[ end ]]
end
function UIWBoard1:CompetitionRank()
    ViewMgr:OpenViewByKey(Mod.RandActivity.CompetitionRank)
end
--------------------------------
UIBoard1AuditBg = UIBoard1AuditBg or DeclareMono("UIBoard1AuditBg",UIWFlushPanel)
function UIBoard1AuditBg:UIBoard1AuditBg()
    self.setted = false
end

function UIBoard1AuditBg:OnEnable()
    if not self.setted then
        local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset("Board1AuditBg")
        if streaming_has then
            ChannelAgentConsts.SetRawImg(self.RawImg,path)
        else
            local bg_path = "ui/textures/uiwboard1/Board1AuditBg.png"
            ResMgr:LoadObject(bg_path,function(tex)
                if not self:Destroyed() then
                    if self.RawImg then
                        self.RawImg.texture = tex
                        self.setted = true
                    end
                end
            end,false)
        end
    end
end