#pragma once

#include <type_traits>
#include "core/fwd_delete.fwd.h"

namespace ot
{
	using nullptr_t = decltype(nullptr);
	
	template<typename T>
	constexpr std::remove_reference_t<T>&& as_movable(T&& t) noexcept
	{
		return static_cast<std::remove_reference_t<T>&&>(t);
	}

	template<typename T>
	constexpr T&& forward(std::remove_reference_t<T>& t) noexcept
	{
		return static_cast<T&&>(t);
	}

	template<typename T>
	constexpr T&& forward(std::remove_reference_t<T>&& t) noexcept
	{
		return static_cast<T&&>(t);
	}

	template<typename T>
	struct default_delete
	{
		constexpr default_delete() noexcept = default;
		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		constexpr default_delete(default_delete<U> const& d) noexcept { (void)d; }
		constexpr default_delete(fwd_delete<T> const& d) noexcept { (void)d; }

		constexpr operator fwd_delete<T>() noexcept { return {}; }

		void operator()(T* t) const noexcept
		{
			delete t;
		}
	};

	template<typename T>
	struct default_delete<T[]>
	{
		constexpr default_delete() noexcept = default;
		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U(*)[], T(*)[]>>>
		constexpr default_delete(default_delete<U[]> const& d) noexcept { }
		constexpr default_delete(fwd_delete<T[]> const& d) noexcept { }

		constexpr operator fwd_delete<T[]>() noexcept { return {}; }

		void operator()(T* t) const noexcept
		{
			delete[] t;
		}
	};

	// Pointer class representing unique ownership
	// Like std::unique_ptr, without having to pull an entire header
	template<typename T, typename Deleter=default_delete<T>>
	class uptr : Deleter
	{
		template<typename, typename> friend class uptr;

		Deleter& access_deleter() & noexcept { return static_cast<Deleter&>(*this); }
		Deleter&& access_deleter() && noexcept { return static_cast<Deleter&&>(*this); }

		T* ptr = nullptr;
	public:
		uptr() = default;
		explicit uptr(T* ptr) noexcept : ptr(ptr) {}
		uptr(T* ptr, Deleter deleter) noexcept : Deleter(deleter), ptr(ptr) {}
		uptr(uptr const&) = delete;
		uptr(uptr&& other) noexcept 
			: Deleter(as_movable(other).access_deleter())
			, ptr(other.ptr) 
		{ 
			other.ptr = nullptr; 
		}
		template<typename U, typename E>
		uptr(uptr<U, E>&& other) noexcept
			: Deleter(as_movable(other).access_deleter())
			, ptr(other.ptr)
		{
			other.ptr = nullptr;
		}
		constexpr uptr(nullptr_t) noexcept : ptr(nullptr) {}
		uptr& operator=(uptr const&) = delete;
		uptr& operator=(uptr&& other) noexcept
		{
			if (ptr != nullptr)
				access_deleter()(ptr);

			access_deleter() = as_movable(other).access_deleter();
			ptr = other.ptr;
			other.ptr = nullptr;
			
			return *this;
		}
		~uptr()
		{
			if (ptr != nullptr)
				access_deleter()(ptr);
		}

		void reset() noexcept
		{
			if (ptr != nullptr)
				access_deleter()(ptr);
			ptr = nullptr;
		}

		void reset(T* value) noexcept
		{
			if(ptr != nullptr)
				access_deleter()(ptr);
			ptr = value;
		}

		[[nodiscard]] T* release() noexcept
		{
			auto const old_ptr = ptr;
			ptr = nullptr;
			return old_ptr;
		}

		[[nodiscard]] T* get() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] Deleter get_deleter() const noexcept
		{
			return access_deleter();
		}

		[[nodiscard]] T& operator*() const
		{
			return *ptr;
		}

		[[nodiscard]] T* operator->() const noexcept
		{
			return ptr;
		}
	};

	template<typename T, typename RetT>
	class uptr<T, RetT(*)(T*)>
	{
		using Deleter = RetT(*)(T*);

		template<typename, typename> friend class uptr;

		Deleter& access_deleter() & noexcept { return deleter; }
		Deleter&& access_deleter() && noexcept { return static_cast<Deleter&&>(deleter); }

		T* ptr = nullptr;
		Deleter deleter = nullptr;

	public:
		uptr() = default;
		explicit uptr(T* ptr) noexcept : ptr(ptr) {}
		uptr(T* ptr, Deleter deleter) noexcept : ptr(ptr), deleter(deleter) {}
		uptr(uptr const&) = delete;
		uptr(uptr&& other) noexcept
			: ptr(other.ptr)
			, deleter(other.deleter)
		{
			other.ptr = nullptr;
		}
		template<typename U, typename E>
		uptr(uptr<U, E>&& other) noexcept
			: ptr(other.ptr)
			, deleter(as_movable(other).access_deleter())
		{
			other.ptr = nullptr;
		}
		constexpr uptr(nullptr_t) noexcept : ptr(nullptr) {}
		uptr& operator=(uptr const&) = delete;
		uptr& operator=(uptr&& other) noexcept
		{
			if (ptr != nullptr)
				access_deleter()(ptr);

			access_deleter() = as_movable(other).access_deleter();
			ptr = other.ptr;
			other.ptr = nullptr;

			return *this;
		}
		~uptr()
		{
			if (ptr != nullptr)
				access_deleter()(ptr);
		}

		void reset() noexcept
		{
			if (ptr != nullptr)
				access_deleter()(ptr);
			ptr = nullptr;
		}

		void reset(T* value) noexcept
		{
			if(ptr != nullptr)
				access_deleter()(ptr);
			ptr = value;
		}

		[[nodiscard]] T* release() noexcept
		{
			auto const old_ptr = ptr;
			ptr = nullptr;
			return old_ptr;
		}

		[[nodiscard]] T* get() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] Deleter get_deleter() const noexcept
		{
			return access_deleter();
		}

		[[nodiscard]] T& operator*() const
		{
			return *ptr;
		}

		[[nodiscard]] T* operator->() const noexcept
		{
			return ptr;
		}
	};

	template<typename T, typename D>
	[[nodiscard]] bool operator==(uptr<T, D> const& lhs, nullptr_t)
	{
		return lhs.get() == nullptr;
	}

	template<typename T, typename D>
	[[nodiscard]] bool operator!=(uptr<T, D> const& lhs, nullptr_t)
	{
		return lhs.get() != nullptr;
	}

	template<typename T>
	uptr<T> make_unique_default()
	{
		return uptr(new T);
	}

	template<typename T, typename... Args>
	uptr<T> make_unique(Args&&... args)
	{
		return uptr(new T(ot::forward<Args>(args)...));
	}

	template<typename T>
	using fwd_uptr = uptr<T, fwd_delete<T>>;
}
