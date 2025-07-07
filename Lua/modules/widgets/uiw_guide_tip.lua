-------------------------------------------------
--ui_guide_tip 控件，通用指引提示
-------------------------------------------------
UiGuideTip = UiGuideTip or BaseClass()

local guide_tip_path = "ui/widgets/guide_tip.prefab"
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        GameObjPool:RegisterPool(
            guide_tip_path,
            function(error)
                if not StringIsEmpty(error) then
                    Debuger.LogError("guide_tip register error:%s", error)
                end
                PreloadCtrl.UnregisterLoading()
            end,
            false
        )
    end
)

--创建对象实例,父对象,位置,箭头方向和位置,描述,延迟显示，销毁时间
-- data = {
--     desc = ,
--     pos = ,
--     show =,
--     hide =,
--     arrow = 2,
--     arrow_pos = Vector2.New(-77,28),
--     not_destroy = false,
-- }
function UiGuideTip:__init(root_node, data)
    self.root_obj = GameObjPool:Get(guide_tip_path, root_node)
    self.root_obj:SetActive(false)
    self.mono_com = self.root_obj:GetLuaComponent("UIWGuideTip")
    self.mono_com:SetData(data)
    --设定显示时间和销毁时间
    self.handle_delay1 =
        TimeHelper:AddDelayTimer(
        function()
            self.root_obj:SetActive(true)
            if data.not_destroy then return end
            self.handle_delay2 =
                TimeHelper:AddDelayTimer(
                function()
                    self:DeleteMe()
                end,
                data.hide or 5
            )
        end,
        data.show or 1
    )
end

function UiGuideTip:__delete()
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_delay2)
    GameObjPool:Release(self.root_obj)
    self.root_obj = nil
end

UIWGuideTip = UIWGuideTip or DeclareMono("UIWGuideTip")
function UIWGuideTip:UIWGuideTip()
end
function UIWGuideTip:SetData(data)
    UH.SetText(self.Desc, data.desc)
    UH.AnchoredPosition(self.Pos, data.pos)
    for i = 1, 2 do
        self.Arrow[i]:SetActive((i == (data.arrow or 1)))
        if data.arrow_pos and ((i == (data.arrow or 1))) then
            UH.AnchoredPosition(self.ArrowPos[i], data.arrow_pos)
        end
    end
end
