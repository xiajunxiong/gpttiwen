
ChatHelper = ChatHelper or BaseClass()

function ChatHelper:__init(param_t)
    self.input_field = param_t.input_field
    self.limit_num = param_t.limit_num
    self.input_field.onValueChanged:AddListener(BindTool.Bind(self.OnValueChanged, self))

    self:Empty()
end

function ChatHelper:PasteInfo(info)
    self.switch = info.switch
    self.info = TableCopy(info.info)
    self.input_field.text = self.info.text
end

function ChatHelper:CopyInfo()
    return {
        switch = self.switch,
        info = self.info
    }
end

function ChatHelper:Empty()
    self.switch = false

    self.info = {
        text = "",
        len_real = 0,
        len_show = 0,
    }
    self.input_field.text = ""
end

function ChatHelper:Switch()
    return self.switch
end

function ChatHelper:RealText()
    local real_text = self.info.text
    for i = #self.info, 1, - 1 do
        local info = self.info[i]
        local chat_item = info.ci
        local front = string.sub(real_text, 0, info.sl)
        local back = string.sub(real_text, info.el + 1, -1)
        real_text = front .. string.format("{%s;%s;%s}", chat_item.Mark(), i, chat_item:ParamShow()) .. back
    end
    return real_text
end

function ChatHelper:ShowText()
    return self.info.text
end

function ChatHelper:ItemList()
    local item_list = {}
    for i = 1, ChatConfig.CHAT_MSG_ITEM_INFO_NUM_MAX do
        local chat_item = self.info[i] and self.info[i].ci or ChatItem.EmptyItem()
        table.insert(item_list, chat_item)
    end
    return item_list
end

function ChatHelper:AddChatItem(chat_item)
    local switch = ChatConfig.ChatItemType.Emoj ~= chat_item:Type()
    local show_text = chat_item:ShowText()
    local word_show = DataHelper.GetStringWordNum(show_text)
    if switch then
        if 3 == #self.info then
            PublicPopupCtrl.Instance:Center(Language.Chat.ItemNumTips)
            return
        end
        local t = {
            ci = chat_item,
            ss = self.info.len_show + 1, es = self.info.len_show + word_show,
        }
        t.sl = #self.info.text
        t.el = t.sl + #show_text
        self.info.len_real = self.info.len_real + ChatItem.WordNum()
        self.info.len_show = t.es
        self.info.text = self.info.text .. show_text
        table.insert(self.info, t)
    else
        self.info.len_real = self.info.len_real + word_show
        self.info.len_show = self.info.len_show + word_show
        self.info.text = self.info.text .. show_text
    end
    self.switch = self.switch or switch
    self.input_field.text = self.info.text
end

function ChatHelper:RemoveChatItem(chat_item)
end

function ChatHelper:OnValueChanged()
    local t = {}
    t.word_new = self.input_field.text
    t.len_new = DataHelper.GetStringWordNum(self.input_field.text)
    if t.len_new == self.info.len_show then return end
    t.is_add = t.len_new > self.info.len_show

    if self.switch then
        if not self:CheckChange(t) then return end
    else
        self.info.len_real = t.len_new
        self.info.len_show = t.len_new
        self.info.text = t.word_new
    end
    self:IsLimitNum()
end

function ChatHelper:IsLimitNum()
    local value = self.info.len_real > self.limit_num
    if value then PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips) end
    return value
end

function ChatHelper:CheckChange(t)
    t.t_new = DataHelper.GetStringWordToTable(t.word_new)
    t.t_old = DataHelper.GetStringWordToTable(self.info.text)
    t.index = 0
    t.word = ""
    for i = 1, math.min(#t.t_new, #t.t_old) do
        if t.t_new[i] ~= t.t_old[i] then
            t.index = i
            t.word = t.is_add and t.t_new[i] or t.t_old[i]
            break
        end
    end
    if 0 == t.index then
        t.index = t.is_add and #t.t_new or #t.t_old
        t.word = t.is_add and t.t_new[t.index] or t.t_old[t.index]
        t.is_last = true
    end
    if t.is_add then
        return self:CheckChangeAdd(t)
    else
        return self:CheckChangeDel(t)
    end
end

function ChatHelper:CheckChangeAdd(t)
    for i = 1, #self.info do
        local info = self.info[i]
        if t.index > info.ss and t.index <= info.es then
            self.input_field.text = self.info.text
            return false
        end
    end
    for i = 1, #self.info do
        local info = self.info[i]
        local len = #t.word
        if t.index <= info.ss then
            info.ss = info.ss + 1
            info.es = info.es + 1
            info.sl = info.sl + len
            info.el = info.el + len
        end
    end
    t.len = 0
    if t.is_last then
        t.len = #t.word_new
    else
        for i = 1, t.index - 1 do
            t.len = t.len + #t.t_old[i]
        end
    end
    self.info.text = string.sub(self.info.text, 0, t.len) .. t.word .. string.sub(self.info.text, t.len + 1, -1)
    self.info.len_real = self.info.len_real + 1
    self.info.len_show = self.info.len_show + 1
    return true
end

function ChatHelper:CheckChangeDel(t)
    local index, len1, len2
    for i = #self.info, 1, -1 do
        local info = self.info[i]
        if t.index > info.ss and t.index <= info.es then
            index = i
            len1 = info.es - info.ss + 1
            len2 = info.el - info.sl
            local front = string.sub(self.info.text, 0, info.sl)
            local back = string.sub(self.info.text, info.el + 1, -1)
            self.info.text = front .. back
            self.info.len_real = self.info.len_real - ChatItem.WordNum()
            self.info.len_show = self.info.len_show - len1
            self.input_field.text = self.info.text
            table.remove(self.info, index)
            self.switch = #self.info > 0
            break
        end
    end
    if index then
        for i = index, #self.info do
            local info = self.info[i]
            info.ss = info.ss - len1
            info.es = info.es - len1
            info.sl = info.sl - len2
            info.el = info.el - len2
        end
    else
        for i = #self.info, 1, -1 do
            local info = self.info[i]
            local len = #t.word
            if t.index <= info.ss then
                info.ss = info.ss - 1
                info.es = info.es - 1
                info.sl = info.sl - len
                info.el = info.el - len
            end
        end
        self.info.text = t.word_new
        self.info.len_real = self.info.len_real - 1
        self.info.len_show = self.info.len_show - 1
    end
    return true
end

function ChatHelper:CopyContent(content)
    self:Empty()
    self.input_field.text = content
end