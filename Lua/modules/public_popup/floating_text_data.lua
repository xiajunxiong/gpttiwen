FloatingTextData = FloatingTextData or BaseClass()
function FloatingTextData:__init()
    if FloatingTextData.Instance ~= nil then
        Debuger.LogError("[FloatingTextData] attempt to create singleton twice!")
        return
    end
    FloatingTextData.Instance = self

    -- 有一个再存过来的字体
    self.floating_data = SmartData.New()
    self.floating_frame = nil
    self.floating_item_cache = {}
    self.floating_quene = {}
    self.floating_icon_cache = {}
end
--add飘字缓存(全部信息)
function FloatingTextData:GetFloatText()
    return self.floating_data.center
end
--飘字物体储存池（飘字结束后放入）
function FloatingTextData:GetItemCache()
    return self.floating_item_cache
end
--当前正在飘的文字（超过上限时要按序渐出）
function FloatingTextData:GetFloatQuene()
    return self.floating_quene
end
--飘的获得物品格子（飘动结束后放入）
function FloatingTextData:GetFloatIconCache()
    return self.floating_icon_cache
end
