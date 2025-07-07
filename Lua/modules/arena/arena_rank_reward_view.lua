
ArenaRankRewardView = ArenaRankRewardView or DeclareView("ArenaRankRewardView","arena/arena_rank_reward")
VIEW_DECLARE_LEVEL(ArenaRankRewardView,ViewLevel.L1)

function ArenaRankRewardView:ArenaRankRewardView()
    self.total_reward_list:SetData(ArenaData.Instance:GetFlipConfig())
    self.count.text = (ArenaData.Instance.total_challenge_times - ArenaData.Instance.RoleInfo.today_fight_count) .. "/" .. ArenaData.Instance.total_challenge_times
    self.hightest_rank.text = ArenaData.Instance.history_highest_rank
    local reward_data_list = ArenaData.Instance:GotRewardList()
    self.got_reward_list:SetData(reward_data_list)
    self.reward_img:SetActive(#reward_data_list ~= 0)
    self.total_reward:SetActive(#reward_data_list ~= 0)
end

function ArenaRankRewardView:OnClose()
    ViewMgr:CloseView(ArenaRankRewardView)
end

function ArenaRankRewardView:OnClickRank()
    ViewMgr:OpenView(ArenaRankView)
end

ArenaRankRewardViewFlush = ArenaRankRewardViewFlush or DeclareMono("ArenaRankRewardViewFlush",UIWFlushPanel)
function ArenaRankRewardViewFlush:ArenaRankRewardViewFlush()
    self.data_cares = {
        {data = ArenaData.Instance.rank_reward_fetch_falg,func = self.OnFlush,keys = {"is_change"}}
    }
end

function ArenaRankRewardViewFlush:OnFlush()
    self.total_reward_list:SetData(ArenaData.Instance:GetFlipConfig())
    local reward_data_list = ArenaData.Instance:GotRewardList()
    self.got_reward_list:SetData(reward_data_list)
    self.reward_img:SetActive(#reward_data_list ~= 0)
    self.total_reward:SetActive(#reward_data_list ~= 0)
end

ArenaRankRewardItem = ArenaRankRewardItem or DeclareMono("ArenaRankRewardItem",UIWidgetBaseItem)
function ArenaRankRewardItem:SetData(data)
    self.rank_txt.text = string.format(Language.Arena.rank_desc,data.max_rank)
    table.insert(data.reward,{item_id = CommonItemId.Coin ,num = data.bind_coin})
    self.reward_list:SetData(data.reward)

    local rink = ArenaData.Instance:GetRink()
    local is_reward = ArenaData.Instance:IsRewardFetchFalg(data.seq)
    self.btnOJ:SetActive(not is_reward)
    self.mask:SetActive(is_reward)
    --检查并显示红点
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.btnOJ, Vector3.New(76,20,0))
    end
    if rink ~= 0 and rink <= data.max_rank and not is_reward then
        self.BtnInterActor.Interactable = true
        self.red_point:SetNum(1)
    else
        self.BtnInterActor.Interactable = false
        self.red_point:SetNum(0)
    end
    UIWidgetBaseItem.SetData(self, data)  
end

function ArenaRankRewardItem:OnGetRewardClick()
    ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_GET_AWARD,self.data.seq)
end

ArenaRewardItem = ArenaRewardItem or DeclareMono("ArenaRewardItem",UIWidgetBaseItem)
function ArenaRewardItem:SetData(data)
    self.data = data
    local item_data = Item.Init(data.item_id, data.num, data.is_bind)
	self.cell:SetData(item_data)
end

ArenaGotRewardItem = ArenaGotRewardItem or DeclareMono("ArenaGotRewardItem",UIWidgetBaseItem)
function ArenaGotRewardItem:SetData(data)
    local item_data = Item.Init(data.item_id, data.num, data.is_bind)
	self.cell:SetData(item_data)
end