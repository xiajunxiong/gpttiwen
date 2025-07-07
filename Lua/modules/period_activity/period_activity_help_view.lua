PeriodActivityHelpView = PeriodActivityHelpView or DeclareView("PeriodActivityHelpView", "period_activity/period_activity_help")
PeriodActivityHelpView.IconList = {
    [Mod.PeriodActivity.LoversMatch] = "HelpIcon1"
}

VIEW_DECLARE_MASK(PeriodActivityHelpView,ViewMask.BgBlock)

function PeriodActivityHelpView:PeriodActivityHelpView()
    self.page_list = {}
    self.page_left = 1
end

function PeriodActivityHelpView:LoadCallback(param_t)
    self.tips_str = param_t.tips_str
    self.mod_key = param_t.mod_key
    self.close_func = param_t.close_func
    self.other_param = param_t.param_t

    self:HandeText()
    self:FlushPanel()
    UH.SpriteName(self.ArtImage, PeriodActivityHelpView.IconList[self.mod_key])
end

function PeriodActivityHelpView:CloseCallback()
    Call(self.close_func)
end

function PeriodActivityHelpView:OnClickClose()
    --阻止关闭窗口。 
    if self.other_param.custom_close_func ~= nil then
        Call(self.other_param.custom_close_func)
        return
    end
    ViewMgr:CloseView(PeriodActivityHelpView)
end

function PeriodActivityHelpView:HandeText()
    local help_text = self.tips_str
    for i = 1, 10 do
        help_text = "\n" .. help_text
    end
    self.page_list = self:ParseText(help_text).pages
    self.max_page_count = #self.page_list
end

function PeriodActivityHelpView:ParseText(text)
    local tab = RichTextHelper.ColorStr("ttttt", COLORSTR.Transparent)
    text = tab .. string.gsub(text, "\n", "\n" .. tab)

    local info = {
        lines_count_all = 1, -- 总行数
        lines_count_page = 15, -- 每页行数
        pages = {} -- 每页内容
    }
    self.TextLeft.text = text
    self.FullText.text = text
    -- UnityTextGenerator.Populate ==== 将使用给定设置为给定字符串生成顶点和其他数据。
    -- GetGenerationSettings ===== 绘制文本范围
    TextExtend.Populate(self.TextLeft.cachedTextGenerator, text, self.TextLeft:GetGenerationSettings(Vector2.New(425, 485)))
    TextExtend.Populate(self.FullText.cachedTextGenerator, text, self.FullText:GetGenerationSettings(Vector2.New(425, 485)))
    local lines_page = TextExtend.lines(self.TextLeft.cachedTextGenerator) -- 注意text组件需设置成能显示全部内容的
    local lines_all = TextExtend.lines(self.FullText.cachedTextGenerator)
    info.lines_count_all = lines_all.Count
    -- info.lines_count_page = lines_page.Count --防止一页铺不满。

    local characterCount = TextExtend.characterCount(self.FullText.cachedTextGenerator) --字符数量
    local startCharIdxs = {}
    for i = 0, lines_all.Count - 1 do
        table.insert(startCharIdxs, TextExtend.startCharIdx(lines_all[i])) --首个字符在该线上的索引
    end
    local lines = math.ceil(info.lines_count_all / info.lines_count_page)
    for i = 1, lines do
        local a_index = (i - 1) * info.lines_count_page + 1
        local b_index = i * info.lines_count_page + 1
        local a_char = startCharIdxs[a_index]
        local b_char = startCharIdxs[b_index] ~= nil and startCharIdxs[b_index] or (characterCount - 1)
        table.insert(info.pages, System.String.New(text):Substring(a_char, b_char - a_char))
    end
    return info
end

function PeriodActivityHelpView:FlushPanel()
    -- self.ArtObj:SetActive(self.page_left == 1)
    UH.SetText(self.TextLeft, self.page_list[self.page_left] or "")
    UH.SetText(self.TextRight, self.page_list[self.page_left + 1] or "")
end

-- value小于0向前翻页，value大于0向后翻页
function PeriodActivityHelpView:OnChangeEvent(value)
    local add_num = value < 0 and -2 or 2
    local page_left = self.page_left + add_num
    local page_right = self.page_left + add_num + 1
    if page_left > 0 and page_right <= self.max_page_count then
        self.page_left = page_left
        self:FlushPanel()
    end
end
