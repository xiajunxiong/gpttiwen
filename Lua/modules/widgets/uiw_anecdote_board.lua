AnecdoteBaseBoard = AnecdoteBaseBoard or DeclareMono("AnecdoteBaseBoard")
function AnecdoteBaseBoard:AnecdoteBaseBoard()
	self.view_type = nil
	self.close_func = nil
	self.anecdote_type = nil
end

function AnecdoteBaseBoard:OnCloseClick()
	--AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	if self.close_func then
		self.close_func()
	else
		self:DefaultCloseFunc()
	end
end

function AnecdoteBaseBoard:DefaultCloseFunc()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function AnecdoteBaseBoard:SetData(view_type,anecdote_type,close_func)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
	self.view_type = view_type
	self.close_func = close_func
	self.anecdote_type = anecdote_type
end