#pragma once

namespace smp
{
	static std::string LoadStringResource(int resourceId)
	{
		auto puRes = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(resourceId), "TEXT");

		return { static_cast<const char*>(puRes->GetPointer()), puRes->GetSize() };
	}
}
