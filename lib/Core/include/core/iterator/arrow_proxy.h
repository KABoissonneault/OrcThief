#pragma once

namespace ot
{
	template<typename ValueType>
	class arrow_proxy
	{
		ValueType value;
	public:
		arrow_proxy(ValueType v)
			: value(v)
		{

		}

		ValueType* operator->() const
		{
			return &value;
		}
	};
}