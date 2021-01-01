#include "uasset.h"

namespace asset_helper {
	PropertyValue createPropertyValue(const std::string &type, const bool useUnknown) {
		if (type == "BoolProperty") {
			return BoolProperty{};
		}
		else if (type == "Int8Property") {
			return PrimitiveProperty<i8>{};
		}
		else if (type == "Int16Property") {
			return PrimitiveProperty<i16>{};
		}
		else if (type == "IntProperty") {
			return PrimitiveProperty<i32>{};
		}
		else if (type == "Int64Property") {
			return PrimitiveProperty<i64>{};
		}
		else if (type == "UInt16Property") {
			return PrimitiveProperty<u16>{};
		}
		else if (type == "UInt32Property") {
			return PrimitiveProperty<u32>{};
		}
		else if (type == "UInt64Property") {
			return PrimitiveProperty<u64>{};
		}
		else if (type == "FloatProperty") {
			return PrimitiveProperty<float>{};
		}
		else if (type == "TextProperty") {
			return TextProperty{};
		}
		else if (type == "StrProperty") {
			return StringProperty{};
		}
		else if (type == "ObjectProperty") {
			return ObjectProperty{};
		}
		else if (type == "EnumProperty") {
			return EnumProperty{};
		}
		else if (type == "ByteProperty") {
			return ByteProperty{};
		}
		else if (type == "NameProperty") {
			return NameProperty{};
		}
		else if (type == "ArrayProperty") {
			return ArrayProperty{};
		}
		else if (type == "MapProperty") {
			return MapProperty{};
		}
		else if (type == "StructProperty") {
			return StructProperty{};
		}
		else if (type == "Guid") {
			return PrimitiveProperty<Guid>{};
		}
		else if (type == "SoftObjectProperty") {
			return SoftObjectProperty{};
		}
		else if (type == "DateTime") {
			return DateTime{};
		}
		else {
			if (useUnknown) {
				printf("Unknown type %s!\n", type.c_str());
				return UnknownProperty{};
			}
			else {
				return new IPropertyDataList();
			}
		}
	}

	std::string getTypeForValue(const PropertyValue &v) {
		if (std::holds_alternative<BoolProperty>(v)) {
			return "BoolProperty";
		}
		else if (std::holds_alternative<PrimitiveProperty<i8>>(v)) {
			return "Int8Property";
		}
		else if (std::holds_alternative<PrimitiveProperty<i16>>(v)) {
			return "Int16Property";
		}
		else if (std::holds_alternative<PrimitiveProperty<i32>>(v)) {
			return "IntProperty";
		}
		else if (std::holds_alternative<PrimitiveProperty<i64>>(v)) {
			return "Int64Property";
		}
		else if (std::holds_alternative<PrimitiveProperty<u16>>(v)) {
			return "UInt16Property";
		}
		else if (std::holds_alternative<PrimitiveProperty<u32>>(v)) {
			return "UInt32Property";
		}
		else if (std::holds_alternative<PrimitiveProperty<u64>>(v)) {
			return "UInt64Property";
		}
		else if (std::holds_alternative<PrimitiveProperty<float>>(v)) {
			return "FloatProperty";
		}
		else if (std::holds_alternative<TextProperty>(v)) {
			return "TextProperty";
		}
		else if (std::holds_alternative<StringProperty>(v)) {
			return "StrProperty";
		}
		else if (std::holds_alternative<ObjectProperty>(v)) {
			return "ObjectProperty";
		}
		else if (std::holds_alternative<EnumProperty>(v)) {
			return "EnumProperty";
		}
		else if (std::holds_alternative<ByteProperty>(v)) {
			return "ByteProperty";
		}
		else if (std::holds_alternative<ArrayProperty>(v)) {
			return "ArrayProperty";
		}
		else if (std::holds_alternative<MapProperty>(v)) {
			return "MapProperty";
		}
		else if (std::holds_alternative<StructProperty>(v)) {
			return "StructProperty";
		}
		else if (std::holds_alternative<PrimitiveProperty<Guid>>(v)) {
			return "Guid";
		}
		else if (std::holds_alternative<SoftObjectProperty>(v)) {
			return "SoftObjectProperty";
		}
		else if (std::holds_alternative<DateTime>(v)) {
			return "DateTime";
		}
		else if (std::holds_alternative<IPropertyDataList*>(v)) {
			printf("ERROR! This type is meant to be internal, never serialized out!");
			return "";
		}
		else {
			return "UnknownProperty";
		}
	}

	template<class T, class = void>
	struct has_custom_header : std::false_type {};

	template<class T>
	struct has_custom_header<T, typename voider<decltype(T::custom_header)>::type> : std::true_type {};

	void serialize(DataBuffer &buffer, i64 length, PropertyValue &value) {
		buffer.ctx<AssetCtx>().length = length;

		std::visit([&](auto &&v) {
			using T = std::decay_t<decltype(v)>;

			if constexpr (!has_custom_header<T>::value) {
				if (buffer.ctx<AssetCtx>().parseHeader) {
					u8 dumbheader = 0;
					buffer.serialize(dumbheader);
					buffer.ctx<AssetCtx>().headerSize += 1;
				}
			}

			if constexpr (DataBuffer::has_serialize<T>::value) {
				buffer.serialize(v);
			}
			else if constexpr (std::is_same_v<T, IPropertyDataList*>) {
				buffer.serialize(*v);
			}
			else {
				static_assert(false, "Unable to serialize!");
			}
		}, value);
	}

	template<class T, class = void>
	struct has_length : std::false_type {};

	template<class T>
	struct has_length<T, typename voider<decltype(T::needs_length)>::type> : std::true_type {};

	bool needsLength(const PropertyValue &value) {
		return std::visit([&](auto &&v) {
			using T = std::decay_t<decltype(v)>;
			return has_length<T>::value;
		}, value);
	}


	StringRef32 createNoneRef(DataBuffer &buffer) {
		if (buffer.ctx<AssetCtx>().parsingSaveFormat) {
			StringRef32 noneRef;
			noneRef.str = "None";
			return noneRef;
		}
		else {
			return buffer.ctx<AssetCtx>().header->findOrCreateName("None");
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ArrayProperty::~ArrayProperty() {
	for (auto &&ptr : values) {
		asset_helper::PropertyValue *value = (asset_helper::PropertyValue *)ptr;
		delete value;
	}
}

void ArrayProperty::serialize(DataBuffer &buffer) {
	bool parseHeader = buffer.ctx<AssetCtx>().parseHeader;
	if (parseHeader) {
		size_t headerStart = buffer.pos;

		buffer.serialize(arrayType);
		u8 null = 0;
		buffer.serialize(null);

		buffer.ctx<AssetCtx>().headerSize += (buffer.pos - headerStart);
	}

	if (buffer.loading) {
		i32 size;
		buffer.serialize(size);

		values.resize(size);
		for (i32 i = 0; i < size; ++i) {
			IPropertyValue *value = new IPropertyValue();
			std::string valueType = buffer.ctx<AssetCtx>().parsingSaveFormat ? arrayType.str : arrayType.getString(*buffer.ctx<AssetCtx>().header);
			value->v = asset_helper::createPropertyValue(valueType);

			buffer.ctx<AssetCtx>().parseHeader = false;
			asset_helper::serialize(buffer, 0, value->v);
			buffer.ctx<AssetCtx>().parseHeader = parseHeader;

			values[i] = value;
		}
	}
	else {
		i32 size = values.size();
		buffer.serialize(size);

		for (auto &&ptr : values) {
			asset_helper::PropertyValue *value = (asset_helper::PropertyValue *)ptr;

			buffer.ctx<AssetCtx>().parseHeader = false;
			asset_helper::serialize(buffer, 0, *value);
			buffer.ctx<AssetCtx>().parseHeader = parseHeader;
		}
	}
}

void StructProperty::serialize(DataBuffer &buffer) {
	bool parseHeader = buffer.ctx<AssetCtx>().parseHeader;
	if (parseHeader) {
		size_t headerStart = buffer.pos;

		buffer.serialize(type);
		buffer.serialize(guid);
		u8 null = 0;
		buffer.serialize(null);

		buffer.ctx<AssetCtx>().headerSize += (buffer.pos - headerStart);
	}

	if (buffer.loading) {
		auto len = buffer.ctx<AssetCtx>().length;
		auto currentPos = buffer.pos;

		do {
			IPropertyValue *value = new IPropertyValue();

			std::string typeStr = buffer.ctx<AssetCtx>().parsingSaveFormat ? type.str : type.getString(*buffer.ctx<AssetCtx>().header);
			value->v = asset_helper::createPropertyValue(typeStr, false);

			buffer.ctx<AssetCtx>().parseHeader = false;
			asset_helper::serialize(buffer, 0, value->v);
			buffer.ctx<AssetCtx>().parseHeader = parseHeader;

			values.emplace_back(value);
		} while ((buffer.pos - currentPos) < len);
	}
	else {
		for (auto &&value : values) {
			buffer.ctx<AssetCtx>().parseHeader = false;
			asset_helper::serialize(buffer, 0, value->v);
			buffer.ctx<AssetCtx>().parseHeader = parseHeader;
		}
	}
}


void MapProperty::serialize(DataBuffer &buffer) {
	bool parseHeader = buffer.ctx<AssetCtx>().parseHeader;
	if (buffer.ctx<AssetCtx>().parseHeader) {
		size_t headerStart = buffer.pos;

		buffer.serialize(keyType);
		buffer.serialize(valueType);

		u8 null = 0;
		buffer.serialize(null);

		buffer.ctx<AssetCtx>().headerSize += (buffer.pos - headerStart);
	}

	i32 usuallyZero = 0;
	buffer.serialize(usuallyZero);

	i32 size = map.size();
	buffer.serialize(size);

	if (buffer.loading) {
		map.resize(size);
		for (i32 i = 0; i < size; ++i) {
			MapPair pair;

			//Key
			{
				IPropertyValue *key = new IPropertyValue();
				std::string keyTypeStr = buffer.ctx<AssetCtx>().parsingSaveFormat ? keyType.str : keyType.getString(*buffer.ctx<AssetCtx>().header);
				key->v = asset_helper::createPropertyValue(keyTypeStr);

				buffer.ctx<AssetCtx>().parseHeader = false;
				asset_helper::serialize(buffer, 0, key->v);
				buffer.ctx<AssetCtx>().parseHeader = parseHeader;

				pair.key = key;
			}

			//Value
			{
				IPropertyValue *value = new IPropertyValue();
				std::string keyTypeStr = buffer.ctx<AssetCtx>().parsingSaveFormat ? valueType.str : valueType.getString(*buffer.ctx<AssetCtx>().header);
				value->v = asset_helper::createPropertyValue(keyTypeStr);

				buffer.ctx<AssetCtx>().parseHeader = false;
				asset_helper::serialize(buffer, 0, value->v);
				buffer.ctx<AssetCtx>().parseHeader = parseHeader;

				pair.value = value;
			}


			map[i] = pair;
		}
	}
	else {
		for (auto &&v : map) {
			buffer.ctx<AssetCtx>().parseHeader = false;
			asset_helper::serialize(buffer, 0, v.key->v);
			buffer.ctx<AssetCtx>().parseHeader = parseHeader;

			buffer.ctx<AssetCtx>().parseHeader = false;
			asset_helper::serialize(buffer, 0, v.value->v);
			buffer.ctx<AssetCtx>().parseHeader = parseHeader;
		}
	}
}


const std::string& StringRef32::getString(const AssetHeader &header) const {
	return header.getHeaderRef(ref);
}

const std::string& StringRef64::getString(const AssetHeader &header) const {
	return header.getHeaderRef(ref);
}