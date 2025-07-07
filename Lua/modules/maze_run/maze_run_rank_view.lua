MazeRunRankView = MazeRunRankView or DeclareView("MazeRunRankView", "maze_run/maze_run_rank")

function MazeRunRankView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.MazeRun.Title)
    self.Board:SetTabbarVisible(true)
    local tabbar = self.Board.Tabbar
    tabbar:SetData(
        {
            -- {name = Language.MazeRun.Tab1,tab_objs = {self.TabRound}},
            {name = Language.MazeRun.Tab2, tab_objs = {self.TabWorld}}
        }
    )
    tabbar:ClickItem(1)
    self.Currency:DefaultSet()
end

MazeRunRankWorld = MazeRunRankWorld or DeclareMono("MazeRunRankWorld", UIWFlushPanel)
---RankList=========UIWidgetList[MazeRunRankWorldItem]
---MazeIdToggles====Toggle[]
---PassTime=========Text
---RankNum==========Text
---EmtpyMask========GameObject

function MazeRunRankWorld:MazeRunRankWorld()
    self.data_cares = {
        --{data = MazeRunData.Instance:ViewData(), func = self.OnCurRankMazeIdChange, keys = {"rank_maze_id"}},
        {data = MazeRunData.Instance:AllRankData(), func = self.OnRankDataChange},
        {data = RoleData.Instance:GetBaseData(), func = self.OnMyPassRankChange, "name"},
        {data = MazeRunData.Instance:AllRankData(), func = self.OnMyPassRankChange}
    }
end

function MazeRunRankWorld:OnLvToggleClick(maze_id)
    --MazeRunData.Instance:ViewData().rank_maze_id = maze_id
end

function MazeRunRankWorld:getMyRankData()
    local rank_data = MazeRunData.Instance:AllRankData()
    local server_role_name = Format("%s_s%d", RoleData.Instance:GetRoleName(), LoginData.Instance:GetLastLoginServer())
    for _, rl in pairs(rank_data) do
        for _, rank_info in pairs(rl) do
            for _, mem_info in pairs(rank_info.mem_list) do
                if mem_info.role_name == RoleData.Instance:GetRoleName() or server_role_name == mem_info.role_name then
                    return rank_info
                end
            end
        end
    end
    return nil
end

function MazeRunRankWorld:OnMyPassRankChange()
    local my_rank_data = self:getMyRankData()
    if my_rank_data ~= nil and my_rank_data.rank ~= nil then
        self.RankNum.text = tostring(my_rank_data.rank)
        self.PassTime:SetObjActive(true)
        local t = TimeManager.FormatDHMS(my_rank_data.pass_time)
        self.PassTime.text = Format("%s:%s:%s",self:AddZero(t.hour),self:AddZero(t.min),self:AddZero(t.s))
    else
        self.RankNum.text = Language.MazeRun.NoRank
        self.PassTime:SetObjActive(false)
    end
end

function MazeRunRankWorld:OnCurRankMazeIdChange()
    local maze_id = MazeRunData.Instance:GetCurMazeId()
    local rank_list_data = MazeRunData.Instance:RankData(maze_id) or {}
    self.EmtpyMask:SetActive(#rank_list_data == 0)
    self.RankList:SetData(rank_list_data)
end

function MazeRunRankWorld:OnRankDataChange(change_keys)
    if change_keys == nil then
        self:OnCurRankMazeIdChange()
    else
        local cur_maze_id = MazeRunData.Instance:GetCurMazeId()
        if change_keys[cur_maze_id] ~= nil then --当前页面有数据有变化,刷新数据
            self:OnCurRankMazeIdChange()
        end
    end
end

function MazeRunRankWorld:OnClickInfo()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[11].desc)
end

function MazeRunRankWorld:OnClickPreview()
    ViewMgr:OpenView(MazeRunRewardPreview)
end

function MazeRunRankWorld:AddZero(time)
	return time < 10 and tostring("0"..time) or time
end

MazeRunRankWorldItem = MazeRunRankWorldItem or DeclareMono("MazeRunRankWorldItem", UIWidgetBaseItem)
---RankNumImg=====UIImageSpriteSet
---RankNumTxt=====Text
---PlayerName=====Text
---PlayerLv=====Text
---PlayerProf=====UIImageSpriteSet
---PassTime=====Text
---MemberList=====UIWidgetList[MazeRunRankWorldMemberItem]
---RankBg=====UIImageSpriteSet
--AvatarCell==AvatarCell

function MazeRunRankWorldItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.rank <= 3 then
        self.RankNumTxt:SetObjActive(false)
        self.RankNumImg:SetObjActive(true)
        self.RankNumImg.SpriteName = Format("WorldNum%d", data.rank)
        self.RankBg.SpriteName = Format("WorldBg%d", data.rank)
    else
        self.RankNumTxt:SetObjActive(true)
        self.RankNumImg:SetObjActive(false)
        self.RankNumTxt.text = tostring(data.rank)
        self.RankBg.SpriteName = "WorldBg4"
    end

    local t = TimeManager.FormatDHMS(data.pass_time)
    self.PassTime.text = Format("%s:%s:%s",self:AddZero(t.hour),self:AddZero(t.min),self:AddZero(t.s))

    local captain_info = data.mem_list[1]
    if captain_info ~= nil then
        self.PlayerName.text = captain_info.role_name
        self.PlayerLv.text = Language.LvDot(captain_info.level)
        self.PlayerProf.SpriteName = RoleData.GetProfSpByProfId(captain_info.prof, true)
        self.AvatarCell:SetAvatar(captain_info.avatar,captain_info.avata_id,captain_info.avatar_quality)
        self.role_uid = captain_info.role_uid
    else
        Debuger.LogError("MazeRunRankWorldItem:SetData===captain_info is NULL")
    end
    local mem_list = {}
    for i = 2, #data.mem_list do
        table.insert(mem_list, data.mem_list[i])
    end
    self.MemberList:SetData(mem_list)
end

function MazeRunRankWorldItem:AddZero(time)
	return time < 10 and tostring("0"..time) or time
end

function MazeRunRankWorldItem:OnClick()
    if self.role_uid ~= nil then
        RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.role_uid)
        RoleInfoCtrl.Instance:SendReferRoleEquipment(self.role_uid)
    end
end

MazeRunRankWorldMemberItem = MazeRunRankWorldMemberItem or DeclareMono("MazeRunRankWorldMemberItem", UIWidgetBaseItem)

function MazeRunRankWorldMemberItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.AvatarCell:SetAvatar(self.data.avatar,data.avata_id)
end

function MazeRunRankWorldMemberItem:OnClick()
    RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.role_uid)
    RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.role_uid)
end

MazeRunRankRound = MazeRunRankRound or DeclareMono("MazeRunRankRound", UIWFlushPanel)
---RankList=====UIWidgetList[MazeRunRankRoundItem]

function MazeRunRankRound:Awake()
    UIWFlushPanel.Awake(self)
    self:OnFlushList()
end

function MazeRunRankRound:OnFlushList()
    local list_data = {}
    for i = 1, 15 do
        table.insert(list_data, {rank = i})
    end
    self.RankList:SetData(list_data)
end

MazeRunRankRoundItem = MazeRunRankRoundItem or DeclareMono("MazeRunRankRoundItem", UIWidgetBaseItem)
---RankBg=====UIImageSpriteSet
---RankNumImg=====UIImageSpriteSet
---RankNumTxt=====Text
---PlayerName=====Text
---OnlineState=====Text
---PassTime=====Text

function MazeRunRankRoundItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.rank <= 3 then
        self.RankNumImg:SetObjActive(true)
        self.RankNumImg.SpriteName = Format("%d", data.rank)
    else
        self.RankNumTxt:SetObjActive(true)
        self.RankNumTxt.text = tostring(data.rank)
    end
    self.RankBg.SpriteName = Format("RoundBg%d", (2 - data.rank % 2))
    self.PlayerName.text = "Rouond_" .. tostring(data.rank)
end