-- 数字华容道类，可用于华容道相关界面相关
-- 华容道的位置是从左到右，从下到上，从0开始算
-- 使用时先调用一遍GetStruct
KlotskiData = KlotskiData or BaseClass()

function KlotskiData:__init()
    if KlotskiData.Instance ~= nil then
		Debuger.LogError("[KlotskiData] attempt to create singleton twice!")
		return
    end
    KlotskiData.Instance = self
    -- 存储所有的华容道数据
    self.klotski_datas = {}
    -- 当前正在使用的华容道的key
    self.now_key = nil
    -- 当前正在使用的华容道数据
    self.now_data = nil
    -- 点击道具回调
    self.click_callback = nil
    -- 交换后成功匹配回调
    self.finish_callback = nil
    -- 是否可以点击
    self.is_can_click = false
    -- item图片初始的Rect
    self.item_rect = UnityEngine.Rect.New(0, 0, 1, 1)
    -- 用于显示移动的道具A的RawImage
    self.RImgExchangeA = nil
    -- 用于显示移动的道具B的RawImage
    self.RImgExchangeB = nil
    -- 用于显示移动的道具A的RectTransfrom
    self.RectExchangeA = nil
    -- 用于显示移动的道具B（就是空格）的RectTransfrom
    self.RectExchangeB = nil
    -- 拼图名字，用于加载图片
    self.icon_name = nil
    -- 空格道具
    self.space_mono = nil
    -- 当前道具
    self.now_mono = nil
    -- 交换移动总帧数
    self.total_frame = 10
end

function KlotskiData:__delete()
	KlotskiData.Instance = nil
end

function KlotskiData:GetNowKey(key)
    return self.now_key
end

function KlotskiData:GetNowData()
    return self.now_data
end

function KlotskiData:GetRow()
    return self.now_data.row
end

function KlotskiData:GetCol()
    return self.now_data.col
end

function KlotskiData:GetSpacePos()
    return self.now_data.space_pos
end

function KlotskiData:GetRect()
    return self.item_rect
end

function KlotskiData:SetNowKey(key)
    self.now_key = key
end

function KlotskiData:GetIconName()
    return self.icon_name
end

function KlotskiData:SetSpaceMono(mono)
    self.space_mono = mono
end

function KlotskiData:GetIsCanClick()
    return self.is_can_click
end

function KlotskiData:SetIsCanClick(is_can_click)
    self.is_can_click = is_can_click
end


function KlotskiData:SetTotalFrame(total_frame)
    self.total_frame = total_frame
end

function KlotskiData:SetClickCallback(callback)
    self.click_callback = callback
end

function KlotskiData:SetFinishCallback(callback)
    self.finish_callback = callback
end

-- 关闭界面时必须调用
function KlotskiData:Clear()
    self.now_key = nil
    self.now_data = nil
    self.click_callback = nil
    self.finish_callback = nil
    self.RImgExchangeA = nil
    self.RImgExchangeB = nil
    self.RectExchangeA = nil
    self.RectExchangeB = nil
    self.icon_name = nil
    self.space_mono = nil
    self.now_mono = nil
    self.is_can_click = false
    self:ReleaseTime()
end

-- 销毁计时器
function KlotskiData:ReleaseTime()
    if self.time_handler then
        TimeHelper:CancelTimer(self.time_handler)
        self.time_handler = nil
    end
end

-- 设置用于交换的道具A、B和其他相关的参数
function KlotskiData:SetExchangeItem(RImgExchangeA, RImgExchangeB, RectExchangeA, RectExchangeB, icon_name)
    self.RImgExchangeA = RImgExchangeA
    self.RImgExchangeB = RImgExchangeB
    self.RectExchangeA = RectExchangeA
    self.RectExchangeB = RectExchangeB
    self.icon_name = icon_name
    -- 根据图片名字给交换的道具设置图片
    UH.SetIconToRawImage(self.RImgExchangeA, self.icon_name, ICON_TYPE.ANECDOTE)
    UH.SetIconToRawImage(self.RImgExchangeB, self.icon_name, ICON_TYPE.ANECDOTE)
end

-- 这个key是view模块号，华容道的数据用这个来区分
-- is_reset：是否重置数据，tab：重置数据需要的表
-- pos_tab：格子对应位置表，如果有就按照这个生成，否则随机
-- 获取数据时就会设置一遍当前正在使用的华容道数据
function KlotskiData:GetStruct(key, is_reset, tab, pos_tab)
    if not key then
        LogError("获取华容道数据必须带有key")
        return
    end
    key = key or self.now_key
    self:SetNowKey(key)
    if self.klotski_datas[key] then    -- 有数据，是否创建，并返回
        if is_reset then
            self.klotski_datas[key] = self:CreateStruct(tab, pos_tab)
        end
        self.now_data = self.klotski_datas[key]
        return self.klotski_datas[key]
    elseif is_reset and tab then    -- 没有数据，创建，并返回
        self.klotski_datas[key] = self:CreateStruct(tab, pos_tab)
        self.now_data = self.klotski_datas[key]
        return self.klotski_datas[key]
    end
end

-- 创建一个华容道数据并返回
function KlotskiData:CreateStruct(tab, pos_tab)
    local t = {}
    t.row = tab.row
    t.col = tab.col
    t.total = tab.row * tab.col
    t.space_pos = tab.space_pos

    self.item_rect = UnityEngine.Rect.New(0, 0, 1 / t.col, 1 / t.row)

    local rand_list = {}
    local list = {}
    -- 这里需要一个while循环确保随机出来的数组一定是乱序的
    local is_all_match = true
    while is_all_match do
        rand_list = pos_tab or DataHelper.GetRamdonList(1, t.total)
        for i, v in ipairs(rand_list) do
            list[i] = {}
            list[i].right_pos = v - 1
            list[i].now_pos = i - 1
            -- 随机后记录space_pos的位置
            if tab.space_pos == v - 1 then
                t.space_pos = list[i].now_pos
            end
        end
        t.list = list

        is_all_match = self:IsAllMatch(list)
        if pos_tab then
            is_all_match = false
        end
    end
    return t
end

-- 交换数据，只是交换正确的位置数据
function KlotskiData:ExchangeData(i, j)
    local index_right_pos = self.now_data.list[i + 1].right_pos
    self.now_data.list[i + 1].right_pos = self.now_data.list[j + 1].right_pos
    self.now_data.list[j + 1].right_pos = index_right_pos
    self.now_data.space_pos = self.now_data.list[i + 1].now_pos
end

-- 物体交换动画开始
-- 物体a、b的RectTransfrom组件，交换完成回调
function KlotskiData:ExchangeItemStart()
    local pos_a = self.RectExchangeA.gameObject:GetLocalPosition()
    local pos_b = self.RectExchangeB.gameObject:GetLocalPosition()
    -- dir = 0 则左右移动，= 1 则上下移动
    local dir = math.abs(pos_a.x - pos_b.x) > 1 and 0 or 1
    local speed = 0
    -- 用于计数
    self.step_count = self.total_frame
    if dir == 0 then
        speed = math.abs(pos_a.x - pos_b.x) / self.total_frame
        speed = pos_a.x > pos_b.x and -speed or speed
    else
        speed = math.abs(pos_a.y - pos_b.y) / self.total_frame
        speed = pos_a.y > pos_b.y and -speed or speed
    end
    self.time_handler = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.UpdateExchangeItem, self, pos_a, pos_b, dir, speed), 1, self.total_frame)
end

-- 交换动画执行帧函数
function KlotskiData:UpdateExchangeItem(target_a, target_b, dir, speed)
    local pos_a = self.RectExchangeA.gameObject:GetLocalPosition()
    local pos_b = self.RectExchangeB.gameObject:GetLocalPosition()
    if dir == 0 then
        pos_a.x = pos_a.x + speed
        pos_b.x = pos_b.x - speed
    else
        pos_a.y = pos_a.y + speed
        pos_b.y = pos_b.y - speed
    end
    self.RectExchangeA.gameObject:SetLocalPosition(pos_a)
    self.RectExchangeB.gameObject:SetLocalPosition(pos_b)
    self.step_count = self.step_count - 1
    if self.step_count <= 0 then
        if self.time_handler ~= nil then
            TimeHelper:CancelTimer(self.time_handler)
            self.time_handler = nil
        end
        self:ExchangeItemEnd()
    end
end

-- 物体交换动画结束
function KlotskiData:ExchangeItemEnd()
    self:ExchangeData(self.now_mono.data.now_pos, self.space_mono.data.now_pos)
    -- 更新图片显示
    self.now_mono:UpdateImage()
    self.space_mono:UpdateImage()
    
    self.RectExchangeA.gameObject:SetActive(false)
    self.RectExchangeB.gameObject:SetActive(false)
    self.is_can_click = true
    if self:IsAllMatch() then
        self.finish_callback()
    end
end

-- 点击道具回调
function KlotskiData:OnItemClick(mono)
    if not self.is_can_click then
        return
    end
    self.now_mono = mono
    self.click_callback()
    if self:IsNear(mono.data.now_pos) then
        -- 显示用于交换的两个物体，并取消激活真正物体中的RawImage组件
        self.RectExchangeA.gameObject:SetActive(true)
        self.RectExchangeB.gameObject:SetActive(true)
        self.RImgExchangeA.uvRect = mono.RImage.uvRect
        self.RImgExchangeB.uvRect = self.space_mono.RImage.uvRect
        self.RectExchangeA.gameObject:SetLocalPosition(mono.RectItem.gameObject:GetLocalPosition())
        self.RectExchangeB.gameObject:SetLocalPosition(self.space_mono.RectItem.gameObject:GetLocalPosition())
        mono.RImage.enabled = false
        self.space_mono.RImage.enabled = false
        self.is_can_click = false

        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ToggleIn)
        -- 开始交换物体
        self:ExchangeItemStart()
    else
        PublicPopupCtrl.Instance:Center(Language.Anecdote.KlotskiSelectTip)
    end
end

-- 判断是否与空格相邻，即是否可以交换
function KlotskiData:IsNear(i)
    local i_row, i_col = self:PosToRC(i)
    local j_row, j_col = self:PosToRC(self.now_data.space_pos)
    -- 判断左右
    if i_row == j_row and math.abs(i_col - j_col) == 1 then
        return true
    elseif i_col == j_col and math.abs(i_row - j_row) == 1 then
        return true
    end
    return false
end

-- 是否全部匹配
function KlotskiData:IsAllMatch(list)
    list = list or self.now_data.list
    for i, v in ipairs(list) do
        if v.now_pos ~= v.right_pos then
            return false    
        elseif i == #list then
            return true
        end
    end
end

-- 根据pos获取该item对应行列，行从0开始，列从0开始
function KlotskiData:PosToRC(pos)
    return math.floor(pos / self.now_data.col), pos % self.now_data.col
end

-- 根据item的行列获取对应pos，行从0开始，列从0开始
function KlotskiData:RCToPos(row, col)
    return row * self.now_data.col + col
end