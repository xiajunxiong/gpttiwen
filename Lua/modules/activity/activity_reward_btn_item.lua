UIActivityRewardBtn = UIActivityRewardBtn or BaseClass()

local reward_btn_path =  "ui/views/activity/activity_reward_btn_item.prefab"
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        GameObjPool:RegisterPool(
            reward_btn_path,
            function(error)
                if not StringIsEmpty(error) then
                    Debuger.LogError("reward_btn register error:%s", error)
                end
                PreloadCtrl.UnregisterLoading()
            end,
            false)
    end)

function UIActivityRewardBtn:__init(root_node, activity_type, click_func)
    self.root_obj = GameObjPool:Get(reward_btn_path, root_node)
    self.mono_com = self.root_obj:GetLuaComponent("ActivityTimeLimitRewardBtn")
    self.mono_com:SetActType(activity_type)
    self.mono_com.click_func = click_func
end

function UIActivityRewardBtn:__delete()
    GameObjPool:Release(self.root_obj)
    self.root_obj = nil
end

function UIActivityRewardBtn:ShowEffect(show)
    self.mono_com:ShowEffect(show)
end
----------------------------------
ActivityTimeLimitRewardBtn = ActivityTimeLimitRewardBtn or DeclareMono("ActivityTimeLimitRewardBtn")
function ActivityTimeLimitRewardBtn:ActivityTimeLimitRewardBtn()
end

function ActivityTimeLimitRewardBtn:OnDestroy()
    self.effect_handle = nil
    if self.care_handle and self.care_data then
        self.care_data:Uncare(self.care_handle)
        self.care_handle = nil
    end
end

function ActivityTimeLimitRewardBtn:SetActType(activity_type)
    self.activity_type = activity_type

    self.care_data = ActivityData.Instance.time_limit_reward_btn_effect
    if self.care_data ~= nil then
        self.care_handle = self.care_data:KeysCare({activity_type},BindTool.Bind1(self.ShowEffect, self))
    end

    local btn_name = Language.Activity.RewardBtnName[self.activity_type] or Language.Common.Reward
    UH.SetText(self.BtnText, btn_name)
end

function ActivityTimeLimitRewardBtn:OnClickPreview()
    if self.click_func ~= nil then
        self.click_func()
    end
end

function ActivityTimeLimitRewardBtn:ShowEffect(show)
    if show ~= nil and type(show) == "table" then
        show = self.care_data[self.activity_type] or false
    end
    if self.effect_handle ~= nil then
        self.EffParObj:SetActive(show)
    elseif show then
        self.effect_handle = self.EffectTool:Play(100108205)
    end
end