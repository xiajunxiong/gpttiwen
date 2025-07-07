

ChallengeFubenWin = ChallengeFubenWin or DeclareView("ChallengeFubenWin", "challenge/challenge_fuben_win")
VIEW_DECLARE_MASK(ChallengeFubenWin,ViewMask.None)
--RewardItems=====UIWidgetList[UIWItemCell]
--ScrollTranRect==RectTransform


function ChallengeFubenWin:LoadCallback()
	local reward_data = ChallengeData.Instance:GetFbReward()
	local list_data = {}
	for _,v in pairs(reward_data) do
		table.insert(list_data,Item.Init(v.item_id,v.item_num))
	end

    if #list_data > 8 then --三个以下居中
        self.ScrollTranRect.pivot = Vector2.New(0.5,1)
    else
        self.ScrollTranRect.pivot = Vector2.New(0.5,0.5)
    end

	self.RewardItems:SetData(list_data)
end

function ChallengeFubenWin:OnCloseClick()
	ViewMgr:CloseView(ChallengeFubenWin)
	SceneCtrl.Instance:RequestLeaveFb()
end