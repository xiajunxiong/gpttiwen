-- 造化装备·分解获得
MarbasResolveReward = MarbasResolveReward or DeclareView("MarbasResolveReward", "marbas/marbas_resolve_reward")
VIEW_DECLARE_MASK(MarbasResolveReward, ViewMask.BgBlockClose)
function MarbasResolveReward:LoadCallback(param_t) 

    if param_t.item_list == nil then
        ViewMgr:CloseView(MarbasResolveReward)
        return 
    end 

    local fix_item_list = BagData.Instance:ItemListBuild(param_t.item_list or {})
    self.list:SetData(fix_item_list)

    self.handle_close = TimeHelper:AddDelayTimer(function ()
        ViewMgr:CloseView(MarbasResolveReward)
    end, 5)

    self.handle_wait = TimeHelper:AddDelayTimer(function ()
        for k,v in pairs(fix_item_list) do 

            PublicPopupCtrl.Instance:CenterI({item_id = v.item_id, num = v.num,
            color_str = v:ColorStr(),item = v,icon_id = v:IconId()})
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = v, num = v.num}}
        end 
    end, 1)
end 

function MarbasResolveReward:CloseCallback()
    TimeHelper:CancelTimer(self.handle_close)
    TimeHelper:CancelTimer(self.handle_wait)
end

MarbasResolveRewardCell = MarbasResolveRewardCell or DeclareMono("MarbasResolveRewardCell",UIWidgetBaseItem)
function MarbasResolveRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data) 

    self.call:SetData(data)
    UH.SetText(self.name ,data:QuaName())
end 
