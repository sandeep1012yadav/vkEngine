#pragma once
#include "vkCore.h"

namespace vk
{
	enum class ObjectType
	{
		OT_GameObject,
		OT_FrameObject,
		OT_Mesh,
		OT_Camera,
		OT_Scene
	};
	class vkObject
	{
	protected:
		vkObject(const std::string& name, const ObjectType& type) : mName(name), mObjectType(type) {};
		~vkObject();
		std::string mName;
		ObjectType mObjectType;

	public:
		std::string GetName() { return mName; }
		void SetName(const std::string& name) { mName = name; }

		ObjectType GetObjectType() { return mObjectType; }
		void SetObjectType(const ObjectType& objectType) { mObjectType = objectType; }
	};
}
