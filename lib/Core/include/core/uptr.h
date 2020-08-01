#pragma once

#include <utility>

namespace ot
{
	using nullptr_t = decltype(nullptr);

	template<typename T>
	struct default_delete
	{
		void operator()(T* t) const noexcept
		{
			delete t;
		}
	};

	template<typename T>
	struct default_delete<T[]>
	{
		void operator()(T* t) const noexcept
		{
			delete[] t;
		}
	};

	// Struct used for forward declaring a template
	template<typename T>
	struct fwd_delete
	{
		void operator()(T* p) const noexcept;
	};

	template<typename T>
	struct fwd_delete<T[]>
	{
		void operator()(T* p) const noexcept;
	};

	// Pointer class representing unique ownership
	// Like std::unique_ptr, without having to pull an entire header
	template<typename T, typename Deleter=default_delete<T>>
	class uptr : Deleter
	{
		Deleter& access_deleter() & noexcept { return static_cast<Deleter&>(*this); }
		Deleter&& access_deleter() && noexcept { return static_cast<Deleter&&>(*this); }

		T* ptr = nullptr;
	public:
		uptr() = default;
		explicit uptr(T* ptr) noexcept : ptr(ptr) {}
		uptr(T* ptr, Deleter deleter) noexcept : Deleter(deleter), ptr(ptr) {}
		uptr(uptr const&) = delete;
		uptr(uptr&& other) noexcept : Deleter(std::move(other).access_deleter()), ptr(other.ptr) { other.ptr = nullptr; }
		uptr& operator=(uptr const&) = delete;
		uptr& operator=(uptr&& other) noexcept
		{
			access_deleter()(ptr);

			access_deleter() = std::move(other).access_deleter();
			ptr = other.ptr;
			other.ptr = nullptr;
			
			return *this;
		}
		~uptr()
		{
			access_deleter()(ptr);
		}

		void reset() noexcept
		{
			access_deleter()(ptr);
			ptr = nullptr;
		}

		void reset(T* value) noexcept
		{
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

	template<typename T>
	[[nodiscard]] bool operator==(uptr<T> const& lhs, nullptr_t)
	{
		return lhs.get() == nullptr;
	}

	template<typename T>
	[[nodiscard]] bool operator!=(uptr<T> const& lhs, nullptr_t)
	{
		return lhs.get() != nullptr;
	}

	template<typename T>
	uptr<T> make_unique_default()
	{
		return uptr(new T);
	}

	template<typename T, typename... Args>
	uptr<T> make_unique(Args... args)
	{
		return uptr(new T(std::forward<Args>(args)...));
	}
}