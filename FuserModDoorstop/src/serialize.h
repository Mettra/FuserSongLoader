#include "core_types.h"

#include <type_traits>
#include <functional>
#include <optional>

template<class ...Ts>
struct voider {
	using type = void;
};

struct DataBuffer {
	bool loading = true;
	size_t pos = 0;
	size_t size = 0;
	u8* buffer = nullptr;
	void *ctx_;
	std::function<void(size_t)> resize;

	bool watch_ = false;
	struct WatchedValue {
		u8 *data;
		size_t size;
		size_t buffer_pos;
	};
	std::vector<WatchedValue> watchedValues;

	std::vector<std::function<void(DataBuffer &)>> finalizeFunctions;

	struct DerivedBuffer {
		DataBuffer *base = nullptr;
		size_t offset = 0;
	};
	std::optional<DerivedBuffer> derivedBuffer;
	

	DataBuffer() {
		resize = [](size_t) {
			throw std::out_of_range("Cannot resize the buffer!");
		};
	}

	void watch(std::function<void()> fn) {
		if (derivedBuffer.has_value()) {
			derivedBuffer->base->watch_ = true;
		}
		else {
			watch_ = true;

		}

		fn();

		if (derivedBuffer.has_value()) {
			derivedBuffer->base->watch_ = false;
		}
		else {
			watch_ = false;
		}
	}

	void finalize() {
		for (auto &&w : watchedValues) {
			memcpy(buffer + w.buffer_pos, w.data, w.size);
		}

		for (auto &&f : finalizeFunctions) {
			f(*this);
		}
		watchedValues.clear();
	}

	DataBuffer setupFromHere() {
		DerivedBuffer dB;
		dB.base = this;
		dB.offset = pos;

		DataBuffer newBuffer = *this;
		newBuffer.buffer = nullptr;
		newBuffer.pos = 0;
		if (loading) {
			newBuffer.size = size - pos;
		}
		else {
			newBuffer.size = 0;
		}
		newBuffer.derivedBuffer = dB;

		return newBuffer;
	}

	void setupVector(std::vector<u8> &v) {
		buffer = v.data();
		size = v.size();
		resize = [&](size_t sz) {
			v.resize(sz);
			buffer = v.data();
			size = sz;
		};
	}

	template<typename T>
	T& ctx() {
		return *reinterpret_cast<T*>(ctx_);
	}


	void serialize(u8 *data, size_t data_size) {
		if (derivedBuffer.has_value()) {
			size_t prevPos = derivedBuffer->base->pos;
			
			derivedBuffer->base->pos = pos + derivedBuffer->offset;
			derivedBuffer->base->serialize(data, data_size);
			derivedBuffer->base->pos = prevPos;

			pos += data_size;
			if (!loading && pos > size) {
				size = pos;
			}

			return;
		}

		if (data_size > 1024) {
			__debugbreak();
		}

		if (loading && pos + data_size > size) {
			__debugbreak();
			return;
		}

		//constexpr u32 dbgpos = 4346;
		//if (!loading) {
		//	if (pos <= dbgpos && pos + data_size > dbgpos) {
		//		__debugbreak();
		//	}
		//}

		if (loading) {
			memcpy(data, buffer + pos, data_size);
		}
		else {
			//If we've seeked ahead, then write 0's until the new position
			if (pos > size) {
				u32 diff = (pos - size);
				resize(size + diff);
				memset(buffer + pos - diff, 0, diff);
			}

			//Ensure size
			if (pos + data_size > size) {
				resize(pos + data_size);
			}

			memcpy(buffer + pos, data, data_size);

			if (watch_) {
				WatchedValue v;
				v.buffer_pos = pos;
				v.data = data;
				v.size = data_size;
				watchedValues.emplace_back(v);
			}
		}

		pos += data_size;
	}

	template<class T, class = void>
	struct has_serialize : std::false_type {};

	template<class T>
	struct has_serialize<T, typename voider<decltype(std::declval<T>().serialize(std::declval<DataBuffer&>()))>::type> : std::true_type {};

	template<typename T>
	void serialize(T& data) {
		if constexpr (has_serialize<T>::value) {
			data.serialize(*this);
		}
		else if constexpr (std::is_fundamental_v<T>) {
			serialize((u8*)&data, sizeof(T));
		}
		else if constexpr (std::is_enum_v<T>) {
			serialize((u8*)&data, sizeof(std::underlying_type_t<T>));
		}
		else {
			static_assert(false, "Unsupported type to serialize!");
		}
	}

	template<typename T, size_t N>
	void serialize(T (&data)[N]) {
		for (size_t i = 0; i < N; ++i) {
			serialize(data[i]);
		}
	}

	template<typename T>
	void serialize(std::vector<T>& data) {
		u32 size = data.size();
		serialize(size);

		if (loading) {
			data.resize(size);
		}

		for (u32 i = 0; i < size; ++i) {
			serialize(data[i]);
		}
	}

	template<typename T>
	void serializeWithSize(std::vector<T>& data, size_t size) {
		if (loading) {
			data.resize(size);
		}

		for (u32 i = 0; i < size; ++i) {
			serialize(data[i]);
		}
	}

	void serialize(std::string& data) {
		

		if (loading) {
			u32 size = data.size();
			serialize(size);

			if (size != 0) {
				data.resize(size - 1);
				serialize((u8*)data.data(), size - 1);
				pos += 1;
			}
		}
		else {
			if (data.size() == 0) {
				u32 null = 0;
				serialize(null);
			}
			else {
				u32 size = data.size() + 1;
				serialize(size);
				serialize((u8*)data.data(), size);
			}
		}
	}
};