#pragma once

#include <memory>
#include <string>
#include <variant>

namespace Growl {

struct BaseError {
	virtual ~BaseError() = default;
	virtual std::string message() = 0;
};

using Error = std::unique_ptr<BaseError>;

template <typename T>
struct Result : protected std::variant<T, Error> {
	Result(T t) noexcept
		: std::variant<T, Error>{std::move(t)} {}

	Result(Error err) noexcept
		: std::variant<T, Error>{std::move(err)} {}

	bool hasError() const noexcept {
		return std::holds_alternative<Error>(*this);
	}

	bool hasResult() const noexcept {
		return !hasError();
	}

	Error& error() & noexcept {
		return std::get<Error>(*this);
	}

	const Error& error() const& noexcept {
		return std::get<Error>(*this);
	}

	Error&& error() && noexcept {
		return std::move(std::get<Error>(*this));
	}

	const Error&& error() const&& noexcept {
		return std::move(std::get<Error>(*this));
	}

	T& get() & noexcept {
		return std::get<T>(*this);
	}

	const T& get() const& noexcept {
		return std::get<T>(*this);
	}

	T&& get() && noexcept {
		return std::move(std::get<T>(*this));
	}

	const T&& get() const&& noexcept {
		return std::move(std::get<T>(*this));
	}
};

} // namespace Growl
