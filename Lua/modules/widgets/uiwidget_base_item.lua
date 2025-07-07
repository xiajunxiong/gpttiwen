UIWidgetBaseItem = UIWidgetBaseItem or DeclareMono("UIWidgetBaseItem")
-----Selector======Toggle
function UIWidgetBaseItem:UIWidgetBaseItem()
    self.data = nil
    self.is_selected = false
    -- self.select_group = nil
    self._on_selected = nil
    self.smart_data_handle = nil
    self.care_handles = {}
end

function UIWidgetBaseItem:SetSmartData(data)
    if self.smart_data_handle ~= nil then
        if self.data ~= nil then
            self.data:Uncare(self.smart_data_handle)
        end
    end
    self:SetData(data)
    if SmartData.Check(self.data) then
        self.smart_data_handle = self.data:Care(BindTool.Bind(self.OnSmartDataFlush,self))
    else
        self.smart_data_handle = nil
        -- Debuger.LogErrorSD("UIWidgetBaseItem:SetSmartData ERROR,data is not SmartData:%s",Debuger.TableToString(self.data))
    end
end

function UIWidgetBaseItem:OnSmartDataFlush()
    self:SetData(self.data)
end

function UIWidgetBaseItem:SetData(data)
    self.data = data
end

function UIWidgetBaseItem:GetData()
    return self.data
end

function UIWidgetBaseItem:Click()
-- if self.select_group then
-- 	self.select_group:OnClick(self)
-- end
end

function UIWidgetBaseItem:GetMono()
    return self
end

--override 使用Selector无需重写
function UIWidgetBaseItem:SetSelected(selected)
    if self.Selector ~= nil then
        self.Selector.isOn = selected
    else
        if selected ~= self.is_selected then
            self.is_selected = selected
            self:OnSelect(selected)
        end
    end
end
--override
function UIWidgetBaseItem:OnSelect(selected)
    if self._on_selected ~= nil then
        self._on_selected(self, selected)
    end
end

--UIWScrollList 使用 OnDestroy
--UIWidgetList 使用 OnRelease
--注意 列表List 和 创建的Item 必须都要 勾选 OnDestroy
function UIWidgetBaseItem:OnRelease()
    if self.smart_data_handle ~= nil then
        if self.data ~= nil then
            self.data:Uncare(self.smart_data_handle)
        end
        self.smart_data_handle = nil
    end
    self.data = nil
    self._on_selected = nil
end
--override 使用Selector无需重写
function UIWidgetBaseItem:IsSelected()
    if self.Selector ~= nil then
        return self.Selector.isOn
    else
        return self.is_selected
    end
end

function UIWidgetBaseItem:AccordParentInfo()
    local name = ""
    if self.data and self.data.name then name = self.data.name end
    
    return { name = name, is_sel = self.Selector.isOn}
end

-- function UIWidgetBaseItem:IsLoaded()
--     if self.LoadWait ~= nil then
--         for i=1,self.LoadWait:Length() do
--             local wait = self.LoadWait[i]
--             if wait:GetObjActiveInScene() and wait:IsEnabled() and not wait:IsLoaded() then
--                 return false
--             end
--         end
--     end
--     return true
-- end

function UIWidgetBaseItem:SetupCompleted()
    return true
end
--override
function UIWidgetBaseItem:SetupAllComplete()
    
end
function UIWidgetBaseItem:careData(data,func,...)
	self.care_handles[data:Care(func,...)] = data
	func()
end

function UIWidgetBaseItem:uncareData()
    if nil == self.care_handles then return end
    for handle,data in pairs(self.care_handles) do
		data:Uncare(handle)
	end
    self.care_handles = {}
end

local click_block_items = {
    [65561] = true
}

RichTextItem = DeclareMono("RichTextItem", UIWidgetBaseItem)

function RichTextItem:OnClickLink(dict)
    if nil == dict then
        if self.OpenChat then ViewMgr:OpenView(ChatView) end
		return
    end
    local t = RichTextHelper.DictionarySSToTable(dict, true)
	local key = t[1]
	if "openLink" == key then
        local open_func = OpenLinks[t[2]][1]
        if open_func then
            Call(open_func(t[3]))
        end
    elseif "teamApply" == key then
        TeamCtrl.Instance:SendJoinTeam(tonumber(t[2]), tonumber(t[3]))
    elseif "societyShare" == key then
        local uid = RoleData.Instance:GetRoleId() == tonumber(t[5]) and tonumber(t[3]) or tonumber(t[5])
        SocietyData.Instance:SetSocietySendInfo(tonumber(t[2]), uid, tonumber(t[4]))
        ViewMgr:OpenView(SocietySendView)
    elseif "teamApplyTips" == key then
        ViewMgr:OpenView(TeamKnightlyView)
    elseif "teamSalary" == key then
        ViewMgr:OpenView(TeamSalaryView)
    elseif "antiFraud" == key then
        if ChannelAgent and ChannelAgent.ConnectPlatGM then
            ServiceData.Instance.msg_count_data.msg_count = 0
            ChannelAgent.ConnectPlatGM()
        end
    elseif "groupInvite" == key then
        if SocietyData.Instance.chat_group_list[t[4]] then
            PublicPopupCtrl.Instance:Center(Language.Society.GroupEnterTips)
        else
            PublicPopupCtrl.Instance:DialogTips{
                content = string.format(Language.Society.ChatGroupInviteDescTips, t[5]),
                confirm = {
                    func = function()
                        SocietyCtrl.Instance:SendEnterGroupReq(tonumber(t[2]), 0, tonumber(t[3]), SocietyData.GroupKey2Id(t[4]))
                        ViewMgr:CloseView(DialogTipsView)
                    end
                }
            }
        end
    elseif "randactivity" == key then
        if ActivityData.IsOpen(tonumber(t[2])) then
            ActivityRandData.SwitchView(tonumber(t[2]),true)
        else
            PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        end
    elseif "serveractivity" == key then
        if ActivityData.IsOpen(tonumber(t[2])) then
            ViewMgr:OpenView(ServerActivityView,{act_type = tonumber(t[2])})
        else
            PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        end
    elseif "hillSeaNotice" == key then
        ViewMgr:OpenViewByKey(Mod.FightFieldBoss.Shxy)
    elseif "paimaihangNotice" == key then
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_PAI_MAI_HANG) then
            PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error6)
            return
        end
        if not ServerActivityData.Instance:IsPaiMaiHangRecordCur(tonumber(t[2]), tonumber(t[3])) then
            PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error6)
            return
        end
        ViewMgr:OpenViewByKey(Mod.RandActivity.PaiMaiHang)
    elseif "duel_posi" == key then
        -- if not LoginData.Instance:IsOnCrossSever() then
        --     LoginCtrl.Instance:SendCrossStartReq()
        --     Runner.Instance:RunUntilTrue(function ()
        --         if LoginData.Instance:IsOnCrossSever() then
        --             SceneLogic.Instance:AutoFlyToScene(tonumber(t[2]), {x = tonumber(t[3]), y = tonumber(t[4])})
        --             return true
        --         end
        --     end)
        -- else
            SceneLogic.Instance:AutoFlyToScene(tonumber(t[2]), {x = tonumber(t[3]), y = tonumber(t[4])})
        -- end
        return
    elseif "weblink" == key then
        if RTParseType.SocietyMail == tonumber(t[3]) and self.link_mark[t[2]] ~= nil then 
            UnityApp.OpenURL(self.link_mark[t[2]])
        end 
    elseif "peak_token" == key then
        local end_time = tonumber(t[5]) or 0
        if end_time ~= 0 and TimeManager:GetServerTime() > end_time then
            PublicPopupCtrl.Instance:Center(Language.PeakToken.AutoToPosTip)
            return
        end
        local enter_func = function()
            local id = tonumber(t[5]) or 0
            local scene_id = tonumber(t[2]) or 0
            local pos = {x = tonumber(t[3] or 0),y = tonumber(t[4]) or 0}
            local beh_func = function()
                SceneCtrl.Instance:Objs():ForeachAllObj(function(scene_obj)
                    if scene_obj:Type() == SceneObjType.PeakToken then
                        local vo = scene_obj.vo or {}
                        if vo.info and vo.info.id == id then
                            SceneCtrl.Instance:Logic():ClickToObj(scene_obj, vo.is_server)
                            return true
                        end
                        if vo.x == pos.x and vo.y == pos.y then
                            SceneCtrl.Instance:Logic():ClickToObj(scene_obj, vo.is_server)
                            return true
                        end
                    end
                end)
            end
            if SceneData.Instance:SceneId() == scene_id then
                SceneLogic.Instance:AutoToPos(nil,pos,beh_func,true,true)
            else
                SceneLogic.Instance:AutoToPos(scene_id,pos,beh_func,true,true)
            end
        end
        if self.crosssever_care_handle then
            LoginData.Instance:BaseData():Uncare(self.crosssever_care_handle)
            self.crosssever_care_handle = nil
        end
        if not LoginData.Instance:IsOnCrossSever() then
            LoginCtrl.Instance:SendCrossStartReq()
            self.crosssever_care_handle = LoginData.Instance:BaseData():Care(function()
                TimeHelper:CancelTimer(self.crosssever_timer)
                self.crosssever_timer = Invoke(function()
                    enter_func()
                    LoginData.Instance:BaseData():Uncare(self.crosssever_care_handle)
                end,2)
            end,"is_on_crosssever")
        else
            enter_func()
        end
    elseif "conceont_knot_invite" == key then
        ConcentricKnotCtrl.Instance:SendInviteBack(t)
    elseif "guild_cooperate_task" == key then 
        -- 家族任务·特殊字段·点击生效
        GuildData.Instance:ShowHelpItem(tonumber(t[2]))
    elseif "landsbetween_share_pos" == key then 
        if PalacesData.Instance:IsInPalaces() or RuinsData.Instance:IsInRuins() then
            PublicPopupCtrl.Instance:Center(Language.GodAreasActs.DontGetOut)
            return 
        end
         
        -- 神域 地点分享
        if LandsBetweenData.Instance.operating_info.type ~= tonumber(t[4]) then 
            if tonumber(t[4]) == 1 then 
                PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.OutAreaStatusComplete)
            else 
                PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.InAreaStatusComplete)
            end 
            return 
        end 

        if ViewMgr:IsOpen(LandsBetweenMainView) then 
            LandsBetweenData.Instance:SetMapClick({x=tonumber(t[2]),y = tonumber(t[3])})
            LandsBetweenData.Instance:SetMapJumpClick({x=tonumber(t[2]),y = tonumber(t[3])})
        else
            LandsBetweenData.Instance:SetInitJumpCache({x=tonumber(t[2]),y = tonumber(t[3])})
            LandsBetweenCtrl.Instance:TryOpenMain()
        end 
        
    end

	if t.infoHandle then
		local data = RichTextHelper.FetchInfo(t.infoHandle)
		if nil == data or click_block_items[data.item_id] then
			ViewMgr:OpenView(ChatView)
			return
		else
			ItemInfoCtrl.Instance:ItemInfoView{item_list = {data}}
			return
		end
    end

    if t.ItemSignId then
		ChatCtrl.Instance:SendChatItemInfoReq(t.ItemSignId or "0s0")
    end
    if t.modKey then
		if tonumber(t.modKey) == Mod.Guild.GuildJiYuan then
			GuildData.Instance.guild_jiyuan_open_index_id = self.data.info.from_uid
        elseif tonumber(t.modKey) == Mod.GhostNight.Main then
            GhostNightCtrl.Instance:OnGhostNightScene()
            return
        end
        local config = ActivityRandData.Instance:GetConfigByKey(tonumber(t.modKey))
        if config ~= nil and config.act_type and not ActivityData.IsOpen(config.act_type) then
            PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
            return
        end
        ViewMgr:OpenViewByKey(tonumber(t.modKey))
    end
    if t.autoToNpc then
        SceneLogic.Instance:AutoToNpc(tonumber(t.autoToNpc), nil, nil, true)
    end
    if t.groupExit then
        local group = SocietyData.Instance.chat_group_list[t.groupExit]
        if group then
            local info = {
                content = string.format(Language.Society.ChatGroupTips.Exit, group:GroupName()),
                confirm = {
                    func = function()
                        SocietyCtrl.Instance:SendGroupLeaveReq(group:GroupId())
                        ViewMgr:CloseView(DialogTipsView)
                    end
                }
            }
            PublicPopupCtrl.Instance:DialogTips(info)
        end
    end
    if t.honorviewbattle then
        local info = GuildEmbattleData.Instance:OpponetInfo(tonumber(t.honorviewbattle))
        if info and not BattleData.Instance:IsServerFighting()
        and SceneData.Instance:GetMode() == 0 then
            GuildEmbattleData.Instance:WatchBattle(info)
        else
            PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.NoWatchBattle)
        end
    end
    if t.honornamelist then
        ViewMgr:OpenView(GuildEmbattleNameList)
    end
    if t.HongBaoId then
        ChatData.Instance:SelChannelId(ChatConfig.ChannelId.guild, true)
        ViewMgr:OpenView(ChatView)
    end
    if t.mail_survey then
        PublicPopupCtrl.Instance:Center("该功能施工中！")
    end
    if t.wedding_invite then
        MarryCtrl.Instance:SendWeddingFBReqWeddingShowInfo(tonumber(t.wedding_invite))
    end
end

function RichTextItem:OnDestroy()
    if self.crosssever_care_handle then
        LoginData.Instance:BaseData():Uncare(self.crosssever_care_handle)
        self.crosssever_care_handle = nil
    end
    TimeHelper:CancelTimer(self.crosssever_timer)
end