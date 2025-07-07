MengChongJiaDaoGetView = MengChongJiaDaoGetView or DeclareView("MengChongJiaDaoGetView", "serveractivity/mengchongjiadao/mengchongjiadao_get")
VIEW_DECLARE_MASK(MengChongJiaDaoGetView, ViewMask.BgBlockClose)

function MengChongJiaDaoGetView:LoadCallback(param_t)
    local reward_cfg = MengChongJiaDaoData.Instance:GetRewardByMonsterId(param_t.monster)
    local reward_list = DataHelper.FormatItemList(reward_cfg.reward_item)
    self.List:SetData(reward_list)
end
