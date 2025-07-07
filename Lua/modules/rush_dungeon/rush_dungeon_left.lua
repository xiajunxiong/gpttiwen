
MainRightRDTMenu = MainRightRDTMenu or DeclareMono("MainRightRDTMenu", UIWFlushPanel)

RushDungeonLeftPanel = RushDungeonLeftPanel or DeclareMono("RushDungeonLeftPanel", UIWFlushPanel)
function RushDungeonLeftPanel:RushDungeonLeftPanel()
    self.data = RushDungeonData.Instance
    self.data_cares = {
        {data = self.data:RankInfo(), func = self.FlushMyRank},
        {data = self.data.mop_up_info, func = self.FlushMyRank, init_call = false, keys = {"world_level"}}
    }
end

function RushDungeonLeftPanel:onFlush()
end
--[[ function RushDungeonLeftPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushMyRank()
end ]]
function RushDungeonLeftPanel:FlushMyRank()
    local data = self.data:RankInfo().rank_list
    local role_id = RoleData.Instance:GetRoleId()
    local cur_rank = 9999
    for _, v in ipairs(data) do
        if v.role_id == role_id then
            cur_rank = math.min(cur_rank, v.rank)
        end
        --[[ for i = 1, v.role_info_count do
            if v.role_info[i].role_id == role_id then
                cur_rank = math.min(cur_rank, v.rank)
            end
        end ]]
    end
    if cur_rank ~= 9999 then
        UH.SetText(self.MyRank, string.format(Language.RushDungeon.MyRank, cur_rank))
    else
        UH.SetText(self.MyRank, string.format(Language.RushDungeon.MyRank, Language.Common.Wu))
    end
   --[[  local level = self.data:GetOpenLevel()
    UH.SetText(self.OpenLevel, string.format(Language.RushDungeon.OpenLevel2, level)) ]]
end

function RushDungeonLeftPanel:OnClickRank()
    RushDungeonCtrl.Instance:SendBraveGroundReq({
		req_type = BRAVE_GROUND_REQ_TYPE.RANK_INFO
	})
    ViewMgr:OpenView(RushDungeonRank)
end

function RushDungeonLeftPanel:OnClickOut()
    PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips .. FightCtrl.GetWarnTips(SceneModeType.RushDungeon),function()
        RushDungeonCtrl.Instance:SendBraveGroundReq({
            req_type = BRAVE_GROUND_REQ_TYPE.INFO
        })
        SceneCtrl.Instance:RequestLeaveFb()
		LoginCtrl.Instance:SendBackToOriginServer()
	end)
end