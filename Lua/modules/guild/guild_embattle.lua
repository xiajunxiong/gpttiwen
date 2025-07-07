GuildEmbattle = GuildEmbattle or DeclareView("GuildEmbattle", "guild/guild_embattle")
VIEW_DECLARE_MASK(GuildEmbattle, ViewMask.Block)
function GuildEmbattle:GuildEmbattle()
end
function GuildEmbattle:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Guild.Embattle, Vector2.New(1378, 659))
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end
function GuildEmbattle:OnFlush()
end
function GuildEmbattle:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end
GuildEmbattlePanel = GuildEmbattlePanel or DeclareMono("GuildEmbattlePanel", UIWFlushPanel)
function GuildEmbattlePanel:GuildEmbattlePanel()
    -- 监听列表变化
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.embattle, func = self.FlushZoneInfo, keys = {"flushview"}}
    }
end
function GuildEmbattlePanel:onFlush()
    -- body
end
function GuildEmbattlePanel:FlushZoneInfo()
    local data1 = self.data:GetHonorRoleInfo(0)
    data1 = data1 or {}
    self.AreaList0:SetData(data1)
    local data2 = self.data:GetHonorRoleInfo(self.data.select_zone_id)
    data2 = data2 or {}
    self.AreaList1:SetData(data2)
end
function GuildEmbattlePanel:OnClickArea(area)
    -- LogError("分配区域",area)
    self.data.select_zone_id = area
    local data2 = self.data:GetHonorRoleInfo(self.data.select_zone_id)
    data2 = data2 or {}
    self.AreaList1:SetData(data2)
end
function GuildEmbattlePanel:OnClickAllot()
    --分配
    local data = self.AreaList0:GetSelectedData()
    if table.nums(data) == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[84])
        return
    end
    for _, value in pairs(data) do
        GuildCtrl.GuildHonorReq(
            {req_type = GUILD_EMBATTLE_REQ.ASSIGN, p1 = self.data.select_zone_id, p2 = value.role_id}
        )
    end
end
function GuildEmbattlePanel:OnClickAuto()
    GuildCtrl.GuildHonorReq({req_type = GUILD_EMBATTLE_REQ.AUTO_ASSIGN})
    --自动分配
end

GuildEmbattleItem = GuildEmbattleItem or DeclareMono("GuildEmbattleItem", UIWidgetBaseItem)
function GuildEmbattleItem:GuildEmbattleItem()
    self.lang = Language.Guild
end
function GuildEmbattleItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    --#919435 LuaException
    if data == nil then
        return
    end
    -- UH.SetText(self.Name, data.role_name)
    UH.SetText(self.Level, data.level)
    UH.SpriteName(self.ProfSP, RoleData.GetProfSp(math.floor(data.prof / 100)))
    --自己特殊显示
    local my_name = RoleData.Instance:GetBaseData().name
    UH.SetText(self.MyName, my_name == data.role_name and my_name or "")
    UH.SetText(self.Name, my_name ~= data.role_name and data.role_name or "")
end
function GuildEmbattleItem:OnClickCha()
    GuildCtrl.GuildHonorReq({req_type = 0, p1 = 0, p2 = self.data.role_id})
end

--家族荣誉战 右侧图标
--===================================GuildTimeFlushView===================================
GuildTimeFlushView = GuildTimeFlushView or DeclareMono("GuildTimeFlushView", UIWFlushPanel)
function GuildTimeFlushView:GuildTimeFlushView()
    --监听可进入时间进行显隐
    --MainOtherData.Instance:SetRightMenuData({key = "Chief",is_active = true})
end
function GuildTimeFlushView:OnEnable()
    self.Root:SetActive(true)
    --[[ if not ChiefCtrl.Instance:GetData():IsLeaveFlag() then
        local end_time = ChiefCtrl.Instance:GetData().info_data.end_time
        if end_time ~= 0 then
            self.Time:SetCallBack(function()self.Root:SetActive(false)end)
            self.Time:StampTime(end_time,TimeType.Type_Special_0)
        else
            self.Root:SetActive(false)
        end
    else
        self.Root:SetActive(false)
    end ]]
end

function GuildTimeFlushView:OnClickEnter()
    ViewMgr:OpenView(GuildEmbattleEnter)
end

-- 对手信息
GuildEmbattleInfoView =
    GuildEmbattleInfoView or DeclareView("GuildEmbattleInfoView", "guild_embattle/guild_embattle_info")
function GuildEmbattleInfoView:GuildEmbattleInfoView()
    self.data = GuildEmbattleData.Instance
    self.Board:SetData(self:GetType(), Language.GuildEmbattle.InfoTitle, Vector2.New(813, 419))
end
function GuildEmbattleInfoView:OnFlush()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    --显示对战双方信息
    local guild_id = GuildData.Instance:GetGuildID()
    local info = self.data:OpponetInfo()
    local temp
    if info and info[1] then
        temp = info[1]
        UH.SetText(self.LeftInfo[1], temp.banner)
        UH.SetText(self.LeftInfo[2], guild_id == temp.guild_id and self.data:GuildName(temp.name) or temp.name)
        UH.SetText(self.LeftInfo[3], self.data:ServerName(temp.server_id))
        UH.SetText(self.LeftInfo[4], temp.rank)
        UH.SetText(self.LeftInfo[5], temp.role_name)
    else
        UH.SetText(self.LeftInfo[1], "")
        UH.SetText(self.LeftInfo[2], "")
        UH.SetText(self.LeftInfo[3], "")
        UH.SetText(self.LeftInfo[4], "")
        UH.SetText(self.LeftInfo[5], "")
    end
    if info and info[2] then
        temp = info[2]
        UH.SetText(self.RightInfo[1], temp.banner)
        UH.SetText(self.RightInfo[2], guild_id == temp.guild_id and self.data:GuildName(temp.name) or temp.name)
        UH.SetText(self.RightInfo[3], self.data:ServerName(temp.server_id))
        UH.SetText(self.RightInfo[4], temp.rank)
        UH.SetText(self.RightInfo[5], temp.role_name)
    else
        UH.SetText(self.RightInfo[1], "")
        UH.SetText(self.RightInfo[2], "")
        UH.SetText(self.RightInfo[3], "")
        UH.SetText(self.RightInfo[4], "")
        UH.SetText(self.RightInfo[5], "")
    end
end
function GuildEmbattleInfoView:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

--战斗开始
GuildEmbattleFight = GuildEmbattleFight or DeclareView("GuildEmbattleFight", "guild_embattle/guild_embattle_fight")
VIEW_DECLARE_MASK(GuildEmbattleFight, ViewMask.BlockClose)
function GuildEmbattleFight:GuildEmbattleFight()
    self.data = GuildEmbattleData.Instance
end
function GuildEmbattleFight:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.TaskFinish)
    local guild_id = GuildData.Instance:GetGuildID()
    local info = self.data:OpponetInfo()
    local temp
    if info and info[1] then
        temp = info[1]
        UH.SetText(self.LeftInfo[1], guild_id == temp.guild_id and self.data:GuildName(temp.name) or temp.name)
        UH.SetText(self.LeftInfo[2], self.data:ServerName(temp.server_id))
        UH.SetText(self.LeftInfo[3], temp.role_name)
        UH.SetText(self.LeftInfo[4], temp.banner)
        -- UH.SetText(self.LeftInfo[5], temp.rank)
    else
        UH.SetText(self.LeftInfo[1], "")
        UH.SetText(self.LeftInfo[2], "")
        UH.SetText(self.LeftInfo[3], "")
        UH.SetText(self.LeftInfo[4], "")
        -- UH.SetText(self.LeftInfo[5], "")
    end
    if info and info[2] then
        temp = info[2]
        UH.SetText(self.RightInfo[1], guild_id == temp.guild_id and self.data:GuildName(temp.name) or temp.name)
        UH.SetText(self.RightInfo[2], self.data:ServerName(temp.server_id))
        UH.SetText(self.RightInfo[3], temp.role_name)
        UH.SetText(self.RightInfo[4], temp.banner)
        -- UH.SetText(self.RightInfo[5], temp.rank)
    else
        UH.SetText(self.RightInfo[1], "")
        UH.SetText(self.RightInfo[2], "")
        UH.SetText(self.RightInfo[3], "")
        UH.SetText(self.RightInfo[4], "")
        -- UH.SetText(self.RightInfo[5], "")
    end
    self.ShowTimer = Invoke(function()
        ViewMgr:CloseView(GuildEmbattleFight)
    end,5)
end

function GuildEmbattleFight:CloseCallback()
	TimeHelper:CancelTimer(self.ShowTimer)
end

-- 绿9 绿14 红1
--[[ condition
-- 赛程界面

赛程界面包括 1.赛程 2.竞猜 3.上届前三
1.赛程显示
4个阶段 16-8 8-4 4-2 2-1
首先判断 服务端下发的赛程根据参与家族数量显示
对战表 X 进 Y
显示内容: 名字 服务器名 家族族徽 上届状况 是否胜利 当前对战状态
2.竞猜
参与家族不可竞猜
显示内容: 名字 服务器端名称 家族族徽 赛果 支持人数 ]]
