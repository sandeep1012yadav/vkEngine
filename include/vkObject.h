#pragma once

namespace vk
{
	class vkObject
	{
	public:
		vkObject();
		~vkObject();

		virtual void Process(float fTimeElapsed) = 0;
		virtual void Render(float fTimeElapsed) = 0;

	private:

	};
}
