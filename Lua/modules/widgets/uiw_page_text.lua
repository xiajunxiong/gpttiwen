UIWPageText = UIWPageText or DeclareMono("UIWPageText")

-- 书页组件(文字像一本书一样展示用)
-- 伙伴传记可参考

-- LineHeight-----Int(线的高度)
-- TextOverflow---Text(不点亮)
-- TextLeft---Text
-- TextRight---Text
-- NumLeft---Text
-- NumRight---Text
-- LinesLeft---Object[Array]
-- LinesRight---Object[Array]
-- LayoutLeft---LayoutGroup
-- LayoutRight---LayoutGroup
-- ShowRect--Position

-- data结构
-- data = {
--     chapters = {
--         [1] = chapter1,     --string
--         [2] = chapter2,     --string
--         ..
--     }
-- }

function UIWPageText:UIWPageText()
    self.is_ready = false
    self.is_parse = false
    self.chapter_infos = {}
    self.mainCanvasCom = ViewMgr:GetMainCanvasCom()
    self.cur_info = {
        chapter = 1,
        page = 1,
    }
end

function UIWPageText:Awake()
    self.is_ready = true
    if self.data then
        -- Text刷新需要
        self.timer_dft = TimeHelper:AddDelayFrameTimer(function ()
            self:SetData(self.data)
        end)
    end
end

function UIWPageText:OnDestroy()
    TimeHelper:CancelTimer(self.timer_dft)
end

function UIWPageText:Parse()
    for key, chapter in ipairs(self.data.chapters) do
        self.chapter_infos[key] = self:ParseOne(chapter)
    end
    self.is_parse = true
end

function UIWPageText:ParseOne(chapter)
    local info = {
        lines_count_all = 1,      -- 总行数
        lines_count_page = 1,     -- 每页行数
        pages = {},         -- 每页内容
    }
    self.TextOverflow.text = chapter
    self.TextLeft.text = chapter
    TextExtend.Populate(self.TextOverflow.cachedTextGenerator, chapter, self.TextOverflow:GetGenerationSettings(Vector2.New(self.ShowRect.sizeDelta.x, self.ShowRect.sizeDelta.y)))
    TextExtend.Populate(self.TextLeft.cachedTextGenerator, chapter, self.TextLeft:GetGenerationSettings(Vector2.New(self.ShowRect.sizeDelta.x, self.ShowRect.sizeDelta.y)))
    local lines_all = TextExtend.lines(self.TextOverflow.cachedTextGenerator)
    local lines_page = TextExtend.lines(self.TextLeft.cachedTextGenerator)
    info.lines_count_all = lines_all.Count
    info.lines_count_page = lines_page.Count
    if 1 == info.lines_count_all then return end
    local topY_0 = TextExtend.topY(lines_all[0])
    local topY_1 = TextExtend.topY(lines_all[1])
    local space = (topY_0 - topY_1)
    local layoutSpace = space / self.mainCanvasCom.scaleFactor - (self.LineHeight or 0)
    self.LayoutLeft.spacing = layoutSpace
    self.LayoutRight.spacing = layoutSpace
    local characterCount = TextExtend.characterCount(self.TextOverflow.cachedTextGenerator)
    local startCharIdxs = {}
    for i = 0, lines_all.Count - 1 do
        table.insert(startCharIdxs, TextExtend.startCharIdx(lines_all[i]))
    end
    local startCharIdx, page_param = 0, info.lines_count_page
    for i = 1, #startCharIdxs do
        if 1 == i % page_param then
            startCharIdx = startCharIdxs[i]
        end
        if (0 == i % page_param) or (i == #startCharIdxs) then
            table.insert(info.pages, System.String.New(chapter):Substring(startCharIdx, (startCharIdxs[i + 1] or (characterCount - 1)) - startCharIdx))
        end
    end
    info.page_num = math.ceil(#startCharIdxs / page_param)
    info.page_num_show = 1 == info.page_num % 2 and info.page_num + 1 or info.page_num
    return info
end

-- 对外接口

-- 常用在构造函数
function UIWPageText:SetData(data)
    self.data = data
    if not self.is_ready then
        return
    end
    self:Parse()
    self:ToChapter(self.cur_info.chapter, self.cur_info.page)
end

function UIWPageText:ToChapter(chapter, page)
    chapter = chapter or 1
    page = page or 1
    if not self.is_parse then
        self.cur_info.chapter = chapter
        self.cur_info.page = page
        return false
    end
    local info = self.chapter_infos[chapter]
    if nil == info then return false end
    self.cur_info.chapter = chapter
    self.cur_info.page = page
    self.TextLeft.text = info.pages[page] or ""
    self.TextRight.text = info.pages[page + 1] or ""
    self.NumLeft.text = string.format("%s/%s", page, info.page_num_show)
    self.NumRight.text = string.format("%s/%s", page + 1, info.page_num_show)
    for i = 1, self.LinesLeft:Length() do
        self.LinesLeft[i]:SetActive(info.lines_count_page >= i)
    end
    if self.LinesRight then
        for i = 1, self.LinesRight:Length() do
            self.LinesRight[i]:SetActive(info.lines_count_page >= i)
        end
    end
    return true
end

function UIWPageText:ToLastPage()
    local info = self.chapter_infos[self.cur_info.chapter]
    local cur_page = self.cur_info.page
    if 1 == cur_page then return false end
    local page = cur_page - 2
    self.cur_info.page = page
    self.TextLeft.text = info.pages[page] or ""
    self.TextRight.text = info.pages[page + 1] or ""
    self.NumLeft.text = string.format("%s/%s", page, info.page_num_show)
    self.NumRight.text = string.format("%s/%s", page + 1, info.page_num_show)
    return true
end

function UIWPageText:ToNextPage()
    local info = self.chapter_infos[self.cur_info.chapter]
    local cur_page = self.cur_info.page
    if cur_page + 1 >= info.page_num then return false end
    local page = cur_page + 2
    self.cur_info.page = page
    self.TextLeft.text = info.pages[page] or ""
    self.TextRight.text = info.pages[page + 1] or ""
    self.NumLeft.text = string.format("%s/%s", page, info.page_num_show)
    self.NumRight.text = string.format("%s/%s", page + 1, info.page_num_show)
    return true
end

function UIWPageText:PageNumShow(chapter)
    local page_num_show = 0
    local info = self.chapter_infos[chapter]
    if nil ~= info then
        page_num_show = info.page_num_show
    end
    return page_num_show
end