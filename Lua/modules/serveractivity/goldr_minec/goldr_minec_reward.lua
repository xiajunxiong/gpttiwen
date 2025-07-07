
GoldrMinecRewardView = GoldrMinecRewardView or DeclareView("GoldrMinecRewardView", "serveractivity/goldr_minec/goldr_minec_reward_view")
VIEW_DECLARE_MASK(GoldrMinecRewardView, ViewMask.BgBlockClose)
function GoldrMinecRewardView:LoadCallback(param_t) 
    self.param_t = param_t
    local fix_item_list = BagData.Instance:ItemListBuild(param_t.item_list or {})
    self.panel.list:SetData(fix_item_list)

    self.handle_close = TimeHelper:AddDelayTimer(function ()
        -- ViewMgr:CloseView(GoldrMinecRewardView)
    end, 5)

    local commit_list = {}
    for k,v in pairs(fix_item_list) do 
        local flag_with = false 
        for i,j in pairs(commit_list) do 
            if j.item_id == v.item_id then 
                flag_with = true
                j.num = v.num + j.num
                break
            end 
        end 
        if not flag_with then 
            table.insert(commit_list,v)
        end 
    end 

    self.handle_wait = TimeHelper:AddDelayTimer(function ()
        for k,v in pairs(commit_list) do 

            PublicPopupCtrl.Instance:CenterI({item_id = v.item_id, num = v.num,
            color_str = v:ColorStr(),item = v,icon_id = v:IconId()})
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = v, num = v.num}}
        end 
    end, 1)
end 

function GoldrMinecRewardView:CloseCallback()
    TimeHelper:CancelTimer(self.handle_close)
    TimeHelper:CancelTimer(self.handle_wait)

    if self.param_t.close_func then
        Call(self.param_t.close_func)
    end
end

--===========================GoldrMinecRewardPanel===========================
GoldrMinecRewardPanel = GoldrMinecRewardPanel or DeclareMono("GoldrMinecRewardPanel", UIWFlushPanel)
function GoldrMinecRewardPanel:GoldrMinecRewardPanel() end 

GoldrMinecRewardCell = GoldrMinecRewardCell or DeclareMono("GoldrMinecRewardCell",UIWidgetBaseItem)
function GoldrMinecRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data) 

    self.call:SetData(data)
    UH.SetText(self.name ,data:QuaName(true))
end 
