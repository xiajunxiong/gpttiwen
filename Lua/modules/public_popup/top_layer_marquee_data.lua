
TopLayerMarqueeSetting = {
    timer_interval = 0,
    per_move_x = 120,
    distance = 500,
    bg_width = 954,
    float_vector = Vector2.New(588,35),
}

TopLayerMarqueeData = TopLayerMarqueeData or BaseClass()

function TopLayerMarqueeData:__init()
    if TopLayerMarqueeData.Instance ~= nil then
        Debuger.LogError("[TopLayerMarqueeData] attempt to create singleton twice!")
        return
    end
    TopLayerMarqueeData.Instance = self

    self.caption_list = {}
    self.caption_switch = SmartData.New({switch = false})
    self.show_special_effect = false

    self.item_cache_list = {}
    self.rolling_item_num = 0
end

function TopLayerMarqueeData:AddCaptionInfo(info, delay_time)
    if delay_time ~= nil and delay_time ~= 0 then
        TimeHelper:AddDelayTimer(function ()
            self.caption_switch.switch = true
            table.insert(self.caption_list, 1, info)
        end, delay_time)
    else
        self.caption_switch.switch = true
        table.insert(self.caption_list, 1, info)
    end
end

function TopLayerMarqueeData:ClearCaptionList()
    self.caption_list = {}
    self.caption_switch.switch = false
end

function TopLayerMarqueeData:NextCaptionInfo(t)
    if next(self.caption_list) then
        t.content = table.remove(self.caption_list, #self.caption_list)
        return true
    else
        self.caption_switch.switch = false
        return false
    end
end

function TopLayerMarqueeData:GetContentItem(ContentItem, ContentParent)
    self.rolling_item_num = self.rolling_item_num + 1
    if next(self.item_cache_list) then
        return table.remove(self.item_cache_list, #self.item_cache_list)
    else
        local obj = ContentItem:Clone(ContentParent)
        return {
            item_obj = obj,
            item_text = obj:GetComponent(typeof(UnityUI.Text)),
            item_pos = obj:GetComponent(typeof(UnityEngine.RectTransform))
        }
    end
end

function TopLayerMarqueeData:RecycleContentItem(item)
    item.item_pos.anchoredPosition = Vector2.New(0, 50)
    item.item_obj:SetActive(false)
    table.insert(self.item_cache_list, item)
end

function TopLayerMarqueeData:NeedDelay(str)
    --寻宝的结果由于客户端需要播放特效等画面，需要跑马灯延迟播放
    if string.find(str,"mod_key;31001") then
        return 8            --直接返回需要延迟的秒数
    end
end