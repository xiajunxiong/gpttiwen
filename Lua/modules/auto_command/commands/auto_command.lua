
AutoCommand = AutoCommand or BaseClass()

function AutoCommand:__init()
    self.auto_state = AutoCommandState.None
    self.switch = false
end

function AutoCommand:Execute(finish_func)
    self.switch = true
    self.finish_func = finish_func
    self:DoExecute()
    return self.auto_state
end

function AutoCommand:DoExecute()
end

function AutoCommand:Stop(is_finish)
    self.switch = false
    self:DoStop()
    if is_finish then self.finish_func() end
end

function AutoCommand:DoStop()
end