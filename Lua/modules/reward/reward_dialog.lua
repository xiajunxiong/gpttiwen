RewardDialog = RewardDialog or DeclareView("RewardDialog", "reward/reward_dialog")

function RewardDialog:RewardDialog()
    self.default_param_t = {
        context = "",                   -- 对话的文字
        left_btn_text = Language.Reward.Dialog.LeftBtnText,         -- 左边按钮文字
        right_btn_text = Language.Reward.Dialog.RightBtnText,        -- 右边按钮文字
        remind_key = nil,                   -- 提醒key === DTNewTodayNotRemind
        item_list = {},                     -- 物品列表
        left_btn_func = function ()         -- 左按钮事件
            ViewMgr:CloseView(RewardDialog)
        end,
        right_btn_func = function ()        -- 右按钮事件
            ViewMgr:CloseView(RewardDialog)
        end,
    }
end

-- param_t : 参考self.default_param_t
function RewardDialog:LoadCallback(param_t)
    param_t = param_t or {}
    self.paramt = setmetatable(param_t,{
        __index = function (t, key)
            return self.default_param_t[key]
        end
    })

    UH.SetText(self.Content, self.paramt.context)
    UH.SetText(self.LeftBtnText, self.paramt.left_btn_text)
    UH.SetText(self.RightBtnText, self.paramt.right_btn_text)

    self.TodayNotRemind:SetObjActive(self.paramt.remind_key ~= nil)
    if self.paramt.remind_key then
        local result = DialogTipsData.Instance:TodayIsTrue(self.paramt.remind_key)
        self.TodayNotRemind.isOn = result
    end

    self.ItemList:SetData(self.paramt.item_list)
end

function RewardDialog:CloseCallback()
    if self.paramt.remind_key and self.TodayNotRemind.isOn == true then
        DialogTipsData:SetTodayRemind(self.paramt.remind_key)
    end
end

function RewardDialog:OnLeftClick()
    self.paramt.left_btn_func()
end

function RewardDialog:OnRightClick()
    self.paramt.right_btn_func()
end
