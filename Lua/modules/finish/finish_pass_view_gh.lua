local sp_bg = {
    [-1] = "_LocPinJu",
    [0] = "_LocPinJu",
    [2] = "_LocShengLi",
    [3] = "_LocShiBai"
}

FinishPassViewGH = FinishPassViewGH or DeclareView("FinishPassViewGH", "finish/finish_pass_gh")

function FinishPassViewGH:OnClickClose()
    ViewMgr:CloseView(FinishPassViewGH)
end

FinishPassViewGHDetail = FinishPassViewGHDetail or DeclareMono("FinishPassViewGHDetail", UIWFlushPanel)

function FinishPassViewGHDetail:FinishPassViewGHDetail()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.embattle, func = self.FlushRankInfo, keys = {"flushstate"}}
    }
end

function FinishPassViewGHDetail:FlushRankInfo()
    local battle = self.data:HonorBattleInfo()
    local is_first = false
    if battle then
        UH.SetText(self.MyTex, battle.my_guild_name)
        UH.SetText(self.EyTex, battle.ey_guild_name)
        local my_guild_info = GuildEmbattleData.Instance:GetScoreRankInfo(battle.my_guild)
        local ey_guild_info = GuildEmbattleData.Instance:GetScoreRankInfo(battle.ey_guild)
        if my_guild_info and ey_guild_info then
            if my_guild_info.index < ey_guild_info.index then
                is_first = true
            end
        end
    end
    local result_type = 0
    local win_count = 0
    local fail_cout = 0

    for i=1,3 do
        local value = self.data:GetZoneInfo(i)
        if tonumber(value) == 2 then
            win_count = win_count + 1
        end
        if tonumber(value) == 3 then
            fail_cout = fail_cout + 1
        end
    end

    if win_count == 3 then
        result_type = 1
    elseif win_count > fail_cout then
        result_type = 2
    elseif win_count == fail_cout then
        if is_first == true then
            result_type = 2
        else
            result_type = 3
        end
    elseif win_count < fail_cout then
        result_type = 3
    else
        result_type = 4
    end

    UH.SetText(self.ResTex, "")
    
    self.SuccObj:SetActive(result_type == 1)
    self.FailObj:SetActive(result_type == 3)
    self.ShengLiObj:SetActive(result_type == 2)
    self.PingJuObj:SetActive(result_type == 4)

    for i = 1, 3 do
        local value = self.data:GetZoneInfo(i)
        UH.SpriteName(self.TeamList[i], sp_bg[value])
    end
end