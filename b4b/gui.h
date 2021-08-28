namespace gui
{
	bool is_enabled = true;

	void initalize()
	{
		if (GetAsyncKeyState(VK_INSERT))
		{
			is_enabled = !is_enabled;
		}

		if (is_enabled)
		{

		}
	}
}