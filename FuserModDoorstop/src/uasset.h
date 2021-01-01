#include "core_types.h"
#include "serialize.h"
#include "sha1.h"
#include "crc.h"

struct AssetHeader;

struct BaseCtx {
	bool useStringRef = true;
};

struct UnrealName {
	std::string name;
	i16 nonCasePreservingHash;
	i16 casePreservingHash;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(name);
		buffer.serialize(nonCasePreservingHash);
		buffer.serialize(casePreservingHash);
	}
};

struct Guid {
	char guid[16];

	void serialize(DataBuffer &buffer) {
		buffer.serialize(guid);
	}
};

struct Link {
	u64 base;
	u64 cls;
	i32 link;
	u64 property;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(base);
		buffer.serialize(cls);
		buffer.serialize(link);
		buffer.serialize(property);
	}
};


struct StringRef32 {
	i32 ref;
	std::string str;

	bool operator==(const StringRef32& rhs) {
		if (str.empty()) {
			return ref == rhs.ref;
		}
		else {
			return str == rhs.str;
		}
	}

	const std::string& getString(const AssetHeader &header) const;
	const std::string& getString(const AssetHeader *header) const {
		if (header) {
			return getString(*header);
		}
		else {
			return str;
		}
	}

	void serialize(DataBuffer &buffer) {
		if (buffer.ctx<BaseCtx>().useStringRef) {
			buffer.serialize(ref);
		}
		else {
			buffer.serialize(str);
		}
	}
};

struct StringRef64 {
	i64 ref;
	std::string str;

	StringRef64() {}
	StringRef64(StringRef32 r) : ref(r.ref), str(r.str) {}

	const std::string& getString(const AssetHeader &header) const;
	const std::string& getString(const AssetHeader *header) const {
		if (header) {
			return getString(*header);
		}
		else {
			return str;
		}
	}

	void serialize(DataBuffer &buffer) {
		if (buffer.ctx<BaseCtx>().useStringRef) {
			buffer.serialize(ref);
		}
		else {
			buffer.serialize(str);
		}
	}
};


struct Catagory {
	i32 classIdx;
	i32 superIdx;
	i32 templateIdx;
	i32 outerIdx;
	u64 objectName;
	i32 objectFlags;

	i64 lengthV;
	i64 startV;

	i32 forcedExport;
	i32 notForClient;
	i32 notForServer;
	
	Guid packageGuid;
	u32 packageFlags;

	i32 notAlwaysLoadedForEditorGame;
	i32 isAsset;

	i32 firstExportDependency;
	i32 serializationBeforeSerializationDependencies;
	i32 createBeforeSerializationDependencies;
	i32 serializationBeforeCreateDependencies;
	i32 createBeforeCreateDependencies;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(classIdx);
		buffer.serialize(superIdx);
		
		buffer.serialize(templateIdx);

		buffer.serialize(outerIdx);
		buffer.serialize(objectName);

		buffer.serialize(objectFlags);

		buffer.watch([&]() { buffer.serialize(lengthV); });
		buffer.watch([&]() { buffer.serialize(startV); });

		buffer.serialize(forcedExport);
		buffer.serialize(notForClient);
		buffer.serialize(notForServer);

		buffer.serialize(packageGuid);
		buffer.serialize(packageFlags);

		buffer.serialize(notAlwaysLoadedForEditorGame);
		buffer.serialize(isAsset);

		buffer.serialize(firstExportDependency);
		buffer.serialize(serializationBeforeSerializationDependencies);
		buffer.serialize(createBeforeSerializationDependencies);
		buffer.serialize(serializationBeforeCreateDependencies);
		buffer.serialize(createBeforeCreateDependencies);
	}
};

struct CatagoryRef {
	std::vector<i32> data;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(data);
	}
};

struct Version {
	u16 major;
	u16 minor;
	u16 patch;
	i32 changelist;
	std::string branch;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(major);
		buffer.serialize(minor);
		buffer.serialize(patch);
		buffer.serialize(changelist);
		buffer.serialize(branch);
	}
};

struct CustomVersion {
	Guid key;
	i32 version;
	void serialize(DataBuffer &buffer) {
		buffer.serialize(key);
		buffer.serialize(version);
	}
};

struct AssetHeader {
	u32 magic;
	i32 legacyFileVersion;
	i32 legacyUE3Version;
	i32 UE4FileVersion;
	i32 fileVersionLincenceeUE4;
	std::vector<CustomVersion> customVersions;
	i32 totalHeaderSize;
	std::string name;
	u32 packageFlags;
	i32 nameCount;
	i32 nameOffset;
	std::string localizationId;
	i32 gatherableTextDataCount;
	i32 exportsCount;
	i32 exportsOffset;
	i32 importCount;
	i32 importOffset;
	i32 dependenciesOffset;
	i32 softPackageReferencesCount;
	i32 softPackageReferencesOffset;

	i32 searchableNamesOffset;
	i32 thumbnailTableOffset;

	Guid assetGUID;

	struct Generation {
		i32 exportCount;
		i32 nameCount;

		void serialize(DataBuffer &buffer) {
			buffer.serialize(exportCount);
			buffer.serialize(nameCount);
		}
	};
	std::vector<Generation> generations;

	Version savedByVersion;
	Version compatibleWithVersion;
	u32 compressionFlags;

	struct CompressedChunk {
		i32 data[4];

		void serialize(DataBuffer &buffer) {
			buffer.serialize(data);
		}
	};
	std::vector<CompressedChunk> compressedChunks;

	u32 packageSource;

	std::vector<std::string> additionalPackagesToCook;

	i32 uexpDataOffset;
	i64 fileSize_minusFour;

	i32 worldTileInfoDataOffset;
	std::vector<i32> chunkIDs;
	i32 preloadDependencyCount;
	i32 preloadDependencyOffset;

	std::vector<UnrealName> names;
	std::vector<Link> links;
	std::vector<Catagory> catagories;
	std::vector<CatagoryRef> catagoryGroups;
	std::vector<std::string> section5Strings;

	u32 uexpDataCount;
	std::vector<i32> uexpData;

	std::vector<i32> preloadDependencies;

	u64 getLinkRef(i32 idx) const {
		if (idx < 0) {
			return links[-(idx + 1)].property;
		}
		else {
			return -idx;
		}
	}

	const std::string& getHeaderRef(i32 ref) const {
		static std::string BAD_STRING = "BAD";
		if (ref < 0) return BAD_STRING;
		if (ref > names.size()) BAD_STRING;
		return names[ref].name;
	}

	StringRef32 findOrCreateName(const std::string &str) {
		for (size_t i = 0; i < names.size(); ++i) {
			if (names[i].name == str) {
				StringRef32 r;
				r.ref = i;
				return r;
			}
		}

		auto idx = names.size();
		UnrealName name;
		name.name = str;
		names.emplace_back(name);

		StringRef32 r;
		r.ref = idx;
		return r;
	}

	StringRef32 findName(const std::string &str) {
		for (size_t i = 0; i < names.size(); ++i) {
			if (names[i].name == str) {
				StringRef32 r;
				r.ref = i;
				return r;
			}
		}

		StringRef32 r;
		r.ref = std::numeric_limits<i32>::max();
		return r;
	}

	void serialize(DataBuffer &buffer) {
		if (!buffer.loading) {
			nameCount = names.size();

			if (generations.size() > 0) {
				generations[0].exportCount = exportsCount;
				generations[0].nameCount = nameCount;
			}
		}

		size_t start = 0;

		buffer.serialize(magic);
		if (magic != 0x9E2A83C1) {
			__debugbreak();
			return;
		}

		buffer.serialize(legacyFileVersion);
		buffer.serialize(legacyUE3Version);
		buffer.serialize(UE4FileVersion);
		buffer.serialize(fileVersionLincenceeUE4);
		buffer.serialize(customVersions);
		buffer.watch([&]() { buffer.serialize(totalHeaderSize); });
		buffer.serialize(name);
		buffer.serialize(packageFlags);
		buffer.serialize(nameCount);
		buffer.watch([&]() { buffer.serialize(nameOffset); });
		buffer.serialize(localizationId);
		buffer.serialize(gatherableTextDataCount);
		
		buffer.serialize(exportsCount);
		buffer.watch([&]() { buffer.serialize(exportsOffset); });

		buffer.serialize(importCount);
		buffer.watch([&]() { buffer.serialize(importOffset); });
		
		buffer.watch([&]() { buffer.serialize(dependenciesOffset); });

		buffer.serialize(softPackageReferencesCount);
		buffer.watch([&]() { buffer.serialize(softPackageReferencesOffset); });

		buffer.serialize(searchableNamesOffset);
		buffer.serialize(thumbnailTableOffset);

		buffer.serialize(assetGUID);
		buffer.serialize(generations);

		buffer.serialize(savedByVersion);
		buffer.serialize(compatibleWithVersion);
		
		buffer.serialize(compressionFlags);
		buffer.serialize(compressedChunks);

		buffer.serialize(packageSource);
		buffer.serialize(additionalPackagesToCook);

		buffer.watch([&]() { buffer.serialize(uexpDataOffset); });
		buffer.watch([&]() { buffer.serialize(fileSize_minusFour); });

		buffer.serialize(worldTileInfoDataOffset);
		buffer.serialize(chunkIDs);
		buffer.serialize(preloadDependencyCount);
		buffer.watch([&]() { buffer.serialize(preloadDependencyOffset); });

		auto jumpOrSetOffset = [&buffer](auto &pos) {
			if (buffer.loading) {
				buffer.pos = pos;
			}
			else {
				pos = buffer.pos;
			}
		};

		jumpOrSetOffset(nameOffset);
		buffer.serializeWithSize(names, nameCount);

		jumpOrSetOffset(importOffset);
		buffer.serializeWithSize(links, importCount);

		jumpOrSetOffset(exportsOffset);
		buffer.serializeWithSize(catagories, exportsCount);

		jumpOrSetOffset(dependenciesOffset);
		buffer.serializeWithSize(catagoryGroups, exportsCount);

		if (softPackageReferencesOffset != 0) {
			jumpOrSetOffset(softPackageReferencesOffset);
			buffer.serializeWithSize(section5Strings, softPackageReferencesCount);
		}

		if (totalHeaderSize > 0 && exportsCount > 0) {
			jumpOrSetOffset(uexpDataOffset);
			buffer.serialize(uexpData);

			jumpOrSetOffset(preloadDependencyOffset);
			buffer.serializeWithSize(preloadDependencies, preloadDependencyCount);
		}

		if (!buffer.loading) totalHeaderSize = buffer.pos;
	}
};

///////////////////////////////////////////////////////////////

struct AssetCtx {
	BaseCtx baseCtx;
	AssetHeader *header = nullptr;
	i64 length = 0;
	bool parseHeader = true;
	u32 headerSize = 0;
	bool parsingSaveFormat = false;
};

template<typename T>
struct PrimitiveProperty {
	T data;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(data);
	}
};

struct TextProperty {
	i32 flag;
	i8 historyType;
	u64 extras;
	std::vector<std::string> strings;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(flag);
		buffer.serialize(historyType);

		if (historyType == -1) {
			i32 num = extras;
			buffer.serialize(num);
			extras = num;
			buffer.serializeWithSize(strings, num);
		}
		else if (historyType == 0) {
			buffer.serializeWithSize(strings, 3);
		}
		else if (historyType = 11) {
			buffer.serialize(extras);
			buffer.serializeWithSize(strings, 1);
		}
	}
};

struct StringProperty {
	std::string str;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(str);
	}
};

struct ObjectProperty {
	i32 linkVal;

	StringRef32 type;
	StringRef32 value;
	void serialize(DataBuffer &buffer) {
		buffer.serialize(linkVal);

		if (buffer.ctx<AssetCtx>().parsingSaveFormat) {
			buffer.serialize(type);
			buffer.serialize(value);
		}
	}
};

struct EnumProperty {
	static const bool custom_header = true;

	StringRef64 enumType;
	u8 blank;
	StringRef64 value;


	void serialize(DataBuffer &buffer) {
		if (buffer.ctx<AssetCtx>().parseHeader) {
			size_t headerStart = buffer.pos;

			buffer.serialize(enumType);
			buffer.serialize(blank);

			buffer.ctx<AssetCtx>().headerSize += (buffer.pos - headerStart);
		}

		buffer.serialize(value);
	}
};

struct NameProperty {
	StringRef32 name;
	u32 v;

	void serialize(DataBuffer &buffer) {
		if (!buffer.ctx<AssetCtx>().parsingSaveFormat) {
			buffer.serialize(name);
			buffer.serialize(v);
		}
		else {
			buffer.serialize(name);
		}
	}
};

struct IPropertyValue;

struct ArrayProperty {
	ArrayProperty() {}
	ArrayProperty(ArrayProperty &&rhs) {
		arrayType = std::move(rhs.arrayType);
		values = std::move(rhs.values);
		rhs.values.clear();
	}
	ArrayProperty& operator=(ArrayProperty&& rhs) {
		arrayType = std::move(rhs.arrayType);
		values = std::move(rhs.values);
		rhs.values.clear();
		return *this;
	}

	static const bool custom_header = true;
	StringRef64 arrayType;
	std::vector<IPropertyValue*> values;

	~ArrayProperty();
	void serialize(DataBuffer &buffer);
};

struct MapProperty {
	static const bool custom_header = true;
	StringRef64 keyType;
	StringRef64 valueType;

	struct MapPair {
		IPropertyValue* key;
		IPropertyValue* value;
	};
	std::vector<MapPair> map;

	void serialize(DataBuffer &buffer);
};

struct StructProperty {
	static const bool custom_header = true;
	static const bool needs_length = true;

	Guid guid;
	StringRef64 type;
	std::vector<IPropertyValue*> values;

	void serialize(DataBuffer &buffer);
	IPropertyValue *get(const std::string &name);
};

struct ByteProperty {
	static const bool custom_header = true;

	StringRef64 enumType;
	i32 byteType;
	i32 value;

	void serialize(DataBuffer &buffer) {
		if (buffer.ctx<AssetCtx>().parseHeader) {
			size_t headerStart = buffer.pos;

			buffer.serialize(enumType);
			u8 null;
			buffer.serialize(null);

			buffer.ctx<AssetCtx>().headerSize += (headerStart - buffer.pos);
		}

		if (buffer.ctx<AssetCtx>().length == 1) {
			u8 v = value;
			buffer.serialize(v);
			value = v;
		}
		else if (buffer.ctx<AssetCtx>().length == 8 || buffer.ctx<AssetCtx>().length == 0) {
			u64 v = value;
			buffer.serialize(v);
			value = v;
		}
	}
};

struct SoftObjectProperty {
	StringRef32 name;
	u64 value;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(name);

		if (buffer.ctx<AssetCtx>().parsingSaveFormat) {
			u32 smolValue = value;
			buffer.serialize(smolValue);
			value = smolValue;
		}
		else {
			buffer.serialize(value);
		}
	}
};

struct BoolProperty {
	static const bool custom_header = true;
	bool value;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(value);

		if (buffer.ctx<AssetCtx>().parseHeader) {
			u8 null = 0;
			buffer.serialize(null);
		}
	}
};

struct DateTime {
	u64 time;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(time);
	}
};

struct IPropertyDataList;

struct UnknownProperty {
	static const bool needs_length = true;

	std::vector<u8> data;

	void serialize(DataBuffer &buffer) {
		buffer.serializeWithSize(data, (size_t)buffer.ctx<AssetCtx>().length);
	}
};

namespace asset_helper {
	using PropertyValue = std::variant<UnknownProperty, BoolProperty, PrimitiveProperty<i8>, PrimitiveProperty<i16>, PrimitiveProperty<i32>, PrimitiveProperty<i64>, PrimitiveProperty<u16>, PrimitiveProperty<u32>, PrimitiveProperty<u64>, PrimitiveProperty<float>,
									   TextProperty, StringProperty, ObjectProperty, EnumProperty, ByteProperty, NameProperty, ArrayProperty, MapProperty, StructProperty, PrimitiveProperty<Guid>, SoftObjectProperty, IPropertyDataList*, DateTime>;

	PropertyValue createPropertyValue(const std::string &type, const bool useUnknown = true);
	std::string getTypeForValue(const PropertyValue &v);

	void serialize(DataBuffer &buffer, i64 length, PropertyValue &value);

	bool needsLength(const PropertyValue &value);

	StringRef32 createNoneRef(DataBuffer &buffer);
}

struct IPropertyValue {
	asset_helper::PropertyValue v;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PropertyData {
	StringRef32 nameRef;
	i32 widgetData = 0;
	StringRef64 typeRef;
	i64 length = 0;
	asset_helper::PropertyValue value;
	bool isNone = false;

	void serialize(DataBuffer &buffer) {
		auto headerPtr = buffer.ctx<AssetCtx>().header;

		buffer.serialize(nameRef);

		if (!buffer.ctx<AssetCtx>().parsingSaveFormat) {
			buffer.serialize(widgetData);

			if (nameRef.getString(*headerPtr) == "None" || nameRef.ref == 0) {
				isNone = true;
				return;
			}
		}
		else {
			if (nameRef.str == "None") {
				isNone = true;
				return;
			}
		}

		buffer.serialize(typeRef);
		buffer.serialize(length);


		if (buffer.loading) {
			std::string type;

			if (!buffer.ctx<AssetCtx>().parsingSaveFormat) {
				type = nameRef.getString(*headerPtr);
				if (typeRef.ref > 0) {
					type = typeRef.getString(*headerPtr);
				}
			}
			else {
				type = typeRef.str;
			}

			value = asset_helper::createPropertyValue(type);
		}

		size_t beforeProp = buffer.pos;
		auto prevSize = buffer.ctx<AssetCtx>().headerSize;

		buffer.ctx<AssetCtx>().headerSize = 0;
		asset_helper::serialize(buffer, length, value);

		if (!buffer.loading && asset_helper::needsLength(value)) {
			size_t finalPos = buffer.pos;
			length = (finalPos - beforeProp) - buffer.ctx<AssetCtx>().headerSize;
			buffer.pos = beforeProp - sizeof(length);
			buffer.serialize(length);
			buffer.pos = finalPos;

		}

		buffer.ctx<AssetCtx>().headerSize = prevSize;
	}
};

struct IPropertyDataList {
	std::vector<PropertyData> properties;

	PropertyData* get(StringRef32 name) {
		for (auto &&p : properties) {
			if (p.nameRef == name) {
				return &p;
			}
		}

		return nullptr;
	}

	void serialize(DataBuffer &buffer) {
		bool parseHeader = buffer.ctx<AssetCtx>().parseHeader;
		buffer.ctx<AssetCtx>().parseHeader = true;

		if (buffer.loading) {
			bool keepParsing = true;
			while(buffer.pos < buffer.size - 4) {
				PropertyData data;
				buffer.serialize(data);

				if (!data.isNone) {
					properties.emplace_back(std::move(data));
				}
				else {
					break;
				}
			}
		}
		else {
			for (auto &&p : properties) {
				buffer.serialize(p);
			}

			StringRef32 NoneRef = asset_helper::createNoneRef(buffer);
			buffer.serialize(NoneRef);

			i32 null = 0;
			buffer.serialize(null);
		}

		buffer.ctx<AssetCtx>().parseHeader = parseHeader;
	}
};

struct NormalCategory {
	IPropertyDataList data;

	void serialize(DataBuffer &buffer) {
		buffer.serialize(data);
	}
};

struct DataTableCategory {
	NormalCategory base;
	StringRef32 dataType;

	struct Entry {
		StringRef32 rowName;
		i32 duplicateId;
		StructProperty value;

		void serialize(DataBuffer &buffer) {
			buffer.serialize(rowName);
			buffer.serialize(duplicateId);
			buffer.serialize(value);
		}
	};

	std::vector<Entry> entries;

	void serialize(DataBuffer &buffer) {
		auto &&header = *buffer.ctx<AssetCtx>().header;
		buffer.serialize(base);

		
		if(buffer.loading) {
			for (auto &&p : base.data.properties) {
				if (p.nameRef.getString(header) == "RowStruct") {
					if (auto objPtr = std::get_if<ObjectProperty>(&p.value)) {
						dataType.ref = header.getLinkRef(objPtr->linkVal);
						break;
					}
				}
			}
		}

		i32 null = 0;
		buffer.serialize(null);

		i32 size = entries.size();
		buffer.serialize(size);

		bool parseHeader = buffer.ctx<AssetCtx>().parseHeader;
		buffer.ctx<AssetCtx>().parseHeader = false;
		buffer.ctx<AssetCtx>().length = 0;
		if (buffer.loading) {
			for (i32 i = 0; i < size; ++i) {
				Entry e;
				e.value.type.ref = dataType.ref;

				e.serialize(buffer);
				entries.emplace_back(std::move(e));
			}
		}
		else {
			buffer.serializeWithSize(entries, size);
		}
		buffer.ctx<AssetCtx>().parseHeader = parseHeader;
	}
};

struct AssetData {
	struct CatagoryValue {
		using CatagoryVariant = std::variant<NormalCategory, DataTableCategory>;

		CatagoryVariant value;
		std::vector<u8> extraData;
	};
	std::vector<CatagoryValue> catagoryValues;

	void serialize(DataBuffer &buffer) {
		auto &&header = *buffer.ctx<AssetCtx>().header;

		if (buffer.loading) {
			size_t catIdx = 0;
			for(auto &&c : header.catagories) {
				DataBuffer b = buffer.setupFromHere();
				b.size = c.lengthV;

				CatagoryValue v;

				std::string name = header.getHeaderRef(header.getLinkRef(c.classIdx));
				if (name == "DataTable") {
					DataTableCategory dataCat;
					b.serialize(dataCat);
					v.value = std::move(dataCat);
				}
				else {
					NormalCategory normalCat;
					b.serialize(normalCat);
					v.value = std::move(normalCat);
				}

				i32 nextStart = buffer.size - buffer.pos - 4;
				if (catIdx + 1 < header.catagories.size()) {
					nextStart = header.catagories[catIdx + 1].startV;
				}

				i32 extraLen = nextStart - b.pos;
				b.serializeWithSize(v.extraData, extraLen);

				catagoryValues.emplace_back(std::move(v));
				++catIdx;

				buffer.pos = b.pos + b.derivedBuffer->offset;
			}
		}
		else {
			size_t idx = 0;
			for (auto &&c : catagoryValues) {
				
				size_t start = buffer.pos;

				DataBuffer b = buffer.setupFromHere();
				std::visit([&](auto &&v) {
					b.serialize(v);
				}, c.value);

				header.catagories[idx].startV = start;
				header.catagories[idx].lengthV = b.size;

				buffer.pos = b.pos + b.derivedBuffer->offset;
				buffer.serializeWithSize(c.extraData, c.extraData.size());
				++idx;
			}

			i32 footer = 0x9E2A83C1;
			buffer.serialize(footer);
		}
	}
};

struct Asset {
	AssetHeader header;
	AssetData data;

	void serialize(DataBuffer &buffer) {
		AssetCtx ctx;
		ctx.parseHeader = true;
		ctx.header = &header;
		buffer.ctx_ = &ctx;

		buffer.serialize(header);
		buffer.serialize(data);

		header.fileSize_minusFour = buffer.size - 4;
	}
};




struct SaveFile {
	char start[48];
	std::string structName;
	IPropertyDataList properties;

	void serialize(DataBuffer &buffer) {
		AssetCtx ctx;
		ctx.baseCtx.useStringRef = false;
		ctx.parsingSaveFormat = true;

		buffer.ctx_ = &ctx;
		buffer.serialize(start);
		buffer.serialize(structName);
		buffer.serialize(properties);
	}
};


struct SHAHash {
	u8 data[20];
	void serialize(DataBuffer &buffer) {
		buffer.watch([&]() { buffer.serialize(data); });
	}
};


struct FinalizeHash { 
	size_t start;
	size_t size;
	SHAHash *hash;

	void operator()(DataBuffer &b) {
		SHA1 computedHash;
		computedHash.reset();
		computedHash.update(b.buffer + start, size);
		computedHash.finalize();

		memcpy(hash->data, computedHash.digest, sizeof(computedHash.digest));

		auto&& myData = hash->data;
		bool found = false;
		for (auto &&w : b.watchedValues) {
			if (w.data == (u8*)myData) {
				memcpy(b.buffer + w.buffer_pos, myData, sizeof(myData));
				found = true;
			}
		}

		if (!found) {
			__debugbreak();
		}
	}
};
//

enum class EPakVersion : u32
{
	INITIAL = 1,
	NO_TIMESTAMPS = 2,
	COMPRESSION_ENCRYPTION = 3,         // UE4.13+
	INDEX_ENCRYPTION = 4,               // UE4.17+ - encrypts only pak file index data leaving file content as is
	RELATIVE_CHUNK_OFFSETS = 5,         // UE4.20+
	DELETE_RECORDS = 6,                 // UE4.21+ - this constant is not used in UE4 code
	ENCRYPTION_KEY_GUID = 7,            // ... allows to use multiple encryption keys over the single project
	FNAME_BASED_COMPRESSION_METHOD = 8, // UE4.22+ - use string instead of enum for compression method
	FROZEN_INDEX = 9,
	PATH_HASH_INDEX = 10,
	FNV64BUGFIX = 11,


	LAST,
	INVALID,
	LATEST = LAST - 1
};

struct PakFile {
	struct Info {
		static const u32 OFFSET = 221;

		Guid guid;
		bool isEncrypted;
		u32 magic;
		EPakVersion version;
		i64 indexOffset;
		i64 indexSize;
		SHAHash hash;
		bool isFrozen = false;
		char compressionName[32];

		void serialize(DataBuffer &buffer) {
			size_t start = buffer.pos;

			if (buffer.loading) {
				buffer.pos = buffer.size - OFFSET;
			}

			buffer.serialize(guid);
			buffer.serialize(isEncrypted);
			buffer.serialize(magic);
			if (magic != 0x5A6F12E1) {
				return;
			}

			buffer.serialize(version);
			buffer.watch([&]() { buffer.serialize(indexOffset); });
			buffer.watch([&]() { buffer.serialize(indexSize); });
			buffer.watch([&]() { buffer.serialize(hash); });

			if (version == EPakVersion::FROZEN_INDEX) {
				buffer.serialize(isFrozen);
			}

			buffer.serialize(compressionName);

			if (!buffer.loading) {
				std::vector<u8> nullData;
				nullData.resize(OFFSET - (buffer.pos - start));
				buffer.serialize(nullData.data(), nullData.size());
			}
		}
	};

	struct PakEntry {
		std::string name;

		struct EntryData {
			i64 offset;
			i64 size;
			i64 uncompressedSize;
			i32 compressionMethodIdx;
			SHAHash hash;

			u8 flags;
			u32 compressionBlockSize;
			

			// Flags for serialization (volatile)
			bool inFilePrefix = false;
			//

			void serialize(DataBuffer &buffer) {
				if (inFilePrefix) {
					i64 null = 0;
					buffer.serialize(null);
				}
				else {
					buffer.watch([&]() { buffer.serialize(offset); });
				}

				buffer.watch([&]() { buffer.serialize(size); });
				buffer.watch([&]() { buffer.serialize(uncompressedSize); });
				buffer.serialize(compressionMethodIdx);
				buffer.watch([&]() { buffer.serialize(hash); });
				if (compressionMethodIdx != 0) {

				}
				buffer.serialize(flags);
				buffer.serialize(compressionBlockSize);
			}
		};
		EntryData entryData;

		struct PakAssetData {
			AssetHeader *header;
			AssetData data;
			size_t size;

			void serialize(DataBuffer &buffer) {
				AssetCtx ctx;
				ctx.header = header;
				buffer.ctx_ = &ctx;
				buffer.serialize(data);

				header->fileSize_minusFour = buffer.size + header->totalHeaderSize - 4;
				size = buffer.size;

				for (auto &&c : header->catagories) {
					c.startV += header->totalHeaderSize;
				}
			}
		};

		std::variant<AssetHeader, PakAssetData> data;
		

		void serialize(DataBuffer &buffer) {
			buffer.serialize(name);

			size_t start = buffer.pos;
			buffer.serialize(entryData);
			u32 structOffset = buffer.pos - start;

			if (buffer.loading) {
				size_t currentPos = buffer.pos;

				if (name.find(".uasset") != std::string::npos) {
					DataBuffer assetBuffer;
					assetBuffer.buffer = buffer.buffer + entryData.offset + structOffset;
					assetBuffer.size = entryData.uncompressedSize;

					AssetHeader header;
					assetBuffer.serialize(header);
					data = header;
				}
				else if (name.find(".uexp") != std::string::npos) {
					auto searchStr = name.substr(0, name.size() - 5) + ".uasset";

					AssetHeader *foundHeader = nullptr;
					for (auto &&e : buffer.ctx<PakFile>().entries) {
						if (e.name == searchStr) {
							foundHeader = &std::get<AssetHeader>(e.data);
						}
					}

					if (foundHeader) {
						PakAssetData pakData;
						pakData.header = foundHeader;

						DataBuffer assetBuffer;
						assetBuffer.buffer = buffer.buffer + entryData.offset + structOffset;
						assetBuffer.size = entryData.uncompressedSize;
						assetBuffer.serialize(pakData);

						data = std::move(pakData);
					}
				}

				buffer.pos = currentPos;
			}
		}
	};


	Info info_footer;
	std::string mountPoint;
	std::vector<PakEntry> entries;

	void serialize(DataBuffer &buffer) {
		buffer.ctx_ = this;

		if (buffer.loading) {
			buffer.serialize(info_footer);
			buffer.pos = info_footer.indexOffset;

			buffer.serialize(mountPoint);
			buffer.serialize(entries);
		}
		else {
			for (auto &&e : entries) {
				e.entryData.offset = buffer.pos;

				e.entryData.inFilePrefix = true;
				buffer.serialize(e.entryData);
				e.entryData.inFilePrefix = false;

				std::visit([&](auto &&d) {
					DataBuffer b = buffer.setupFromHere();
					b.serialize(d);
					buffer.pos = b.pos + b.derivedBuffer->offset;

					FinalizeHash fh;
					fh.start = b.derivedBuffer->offset;
					fh.size = b.size;
					fh.hash = &e.entryData.hash;
					buffer.finalizeFunctions.emplace_back(std::move(fh));

					e.entryData.size = b.size;
					e.entryData.uncompressedSize = b.size;
				}, e.data);
			}

			info_footer.indexOffset = buffer.pos;
			buffer.serialize(mountPoint);
			buffer.serialize(entries);

			info_footer.indexSize = buffer.pos - info_footer.indexOffset;

			FinalizeHash fh;
			fh.start = info_footer.indexOffset;
			fh.size = info_footer.indexSize;
			fh.hash = &info_footer.hash;
			buffer.finalizeFunctions.emplace_back(std::move(fh));

			buffer.serialize(info_footer);
		}
	}
};

struct PakSigFile {
	u32 magic = 0x73832DAA;
	u32 version = 1;
	std::vector<u8> encrypted_total_hash;
	std::vector<u32> chunks;

	const unsigned char known_good_hash[512] = {
		0x98, 0xFE, 0xD5, 0x13, 0x09, 0xE7, 0xB9, 0x85, 0xB1, 0x93, 0x7F, 0x81, 0xC9, 0x20, 0x5F, 0x81,
		0x79, 0x9B, 0x3B, 0x2E, 0x05, 0x41, 0x77, 0x84, 0x52, 0xB0, 0xD2, 0xE7, 0x13, 0x77, 0x02, 0x9D,
		0x59, 0x23, 0xD3, 0x2F, 0x08, 0x06, 0x90, 0xEC, 0x16, 0xC1, 0x0B, 0x10, 0x9C, 0xC3, 0x1A, 0xEE,
		0x09, 0x32, 0x76, 0xEA, 0xB8, 0xE7, 0xAF, 0x9F, 0x19, 0xD0, 0x00, 0xA6, 0x36, 0x57, 0x7D, 0x87,
		0x71, 0x92, 0x86, 0xD5, 0x24, 0xE0, 0x22, 0x01, 0xED, 0xE5, 0xB6, 0xB3, 0x37, 0xBD, 0x58, 0x58,
		0xA5, 0xB7, 0x6F, 0x21, 0x60, 0xBD, 0x24, 0x71, 0x86, 0xB8, 0x6B, 0x92, 0xB8, 0x03, 0x27, 0x0C,
		0x70, 0xF0, 0x06, 0x99, 0x5D, 0x10, 0x23, 0xBE, 0x0B, 0x50, 0x07, 0x86, 0xAD, 0x18, 0x88, 0x12,
		0x49, 0xE1, 0x6A, 0xA6, 0xEC, 0x1D, 0x55, 0xAB, 0x1A, 0xC2, 0x32, 0x75, 0x54, 0x10, 0x7C, 0x1A,
		0x19, 0xE1, 0x29, 0xF4, 0xC3, 0x94, 0x0E, 0xB7, 0x8D, 0xA4, 0x0B, 0xC6, 0x89, 0xD9, 0x52, 0x83,
		0x0B, 0x94, 0xB8, 0x42, 0x5B, 0x0D, 0x14, 0xB2, 0x08, 0x69, 0x20, 0xA5, 0x09, 0x31, 0x83, 0x7D,
		0xBD, 0x86, 0xBC, 0x0B, 0x35, 0x3D, 0x57, 0x03, 0xBE, 0xAA, 0xEC, 0x26, 0xEE, 0xC0, 0x78, 0x3C,
		0x38, 0x99, 0x24, 0x7B, 0xCA, 0xC5, 0xA3, 0xBC, 0x65, 0x5A, 0x9B, 0x0E, 0x2C, 0x99, 0x30, 0x8B,
		0x8B, 0xD2, 0x19, 0xFD, 0x56, 0xD9, 0x6C, 0xF0, 0xF8, 0xFE, 0xB4, 0xF8, 0x7E, 0xD6, 0xBC, 0x6B,
		0xA2, 0xD3, 0xEF, 0x93, 0x3F, 0x88, 0x19, 0x73, 0xC1, 0x02, 0xD5, 0xF6, 0xCE, 0xBD, 0x7D, 0x2B,
		0x0E, 0x75, 0xF7, 0xB2, 0x09, 0x78, 0x1C, 0x1C, 0xD7, 0x32, 0x01, 0x8A, 0xE8, 0x16, 0x58, 0x48,
		0x94, 0x3A, 0x7B, 0x2C, 0x3F, 0x87, 0x3C, 0xC0, 0x4B, 0x5E, 0xE4, 0xE8, 0xB0, 0x6E, 0x1E, 0x44,
		0x5A, 0x1A, 0xBF, 0x72, 0x44, 0x27, 0x7A, 0x27, 0x49, 0xF2, 0x44, 0xDC, 0xBB, 0x87, 0x16, 0xFF,
		0xAF, 0xCD, 0x63, 0xD1, 0xAF, 0x48, 0xFF, 0x07, 0x80, 0x98, 0xEF, 0x22, 0x7D, 0xEE, 0x22, 0x5F,
		0xCB, 0x70, 0x51, 0x8B, 0x76, 0x06, 0x53, 0x94, 0xAE, 0x76, 0x32, 0x0A, 0x82, 0x6F, 0xB1, 0x7D,
		0xB2, 0x97, 0x06, 0x54, 0x9C, 0x63, 0x4A, 0x02, 0xD0, 0xBA, 0xD5, 0x59, 0x2C, 0x01, 0x9A, 0x16,
		0xA0, 0x0E, 0x57, 0x3F, 0xAF, 0xF4, 0xBD, 0xBE, 0x75, 0x07, 0x3A, 0x76, 0xD3, 0x38, 0x9A, 0x26,
		0xC6, 0x97, 0xBA, 0xCE, 0x23, 0x41, 0xC6, 0x8E, 0xD8, 0x0C, 0x25, 0xE0, 0x1E, 0xA1, 0x1A, 0x42,
		0x62, 0x50, 0xC2, 0x4D, 0xF2, 0x79, 0xB8, 0x32, 0xB9, 0xBD, 0x30, 0x9F, 0xE2, 0xFF, 0x62, 0xA6,
		0x72, 0x08, 0xDF, 0x44, 0xF8, 0xC2, 0xFA, 0x9B, 0xA0, 0xE9, 0x6B, 0x69, 0x80, 0xEF, 0x44, 0x91,
		0xBC, 0x8B, 0x94, 0x48, 0xD3, 0xB0, 0x56, 0xC7, 0x4D, 0x54, 0xEA, 0xF9, 0x32, 0x61, 0x4D, 0x9A,
		0x5B, 0x5D, 0x7F, 0x9D, 0x98, 0xA0, 0x9C, 0x9A, 0x6D, 0x11, 0x01, 0xBF, 0xA6, 0xEC, 0xD9, 0x7E,
		0x4E, 0x71, 0x6A, 0xC7, 0x32, 0xD5, 0x10, 0x26, 0x58, 0xC2, 0x3A, 0xBD, 0xA9, 0x4D, 0x78, 0x20,
		0x3F, 0xFE, 0xDB, 0x6E, 0xEA, 0x06, 0xAF, 0xA6, 0xAA, 0xAF, 0x56, 0xC6, 0x95, 0x3A, 0x0E, 0x89,
		0xA3, 0x52, 0x90, 0x09, 0xEC, 0x63, 0xDA, 0x75, 0x20, 0x65, 0xAF, 0xF9, 0xBC, 0x6C, 0xD9, 0x5F,
		0x00, 0xEE, 0x22, 0x2F, 0x55, 0x4C, 0x9C, 0x2E, 0xFE, 0x6A, 0x1F, 0x3D, 0xCC, 0xCF, 0x24, 0xD5,
		0x18, 0x25, 0x9B, 0x81, 0x05, 0x25, 0xAD, 0x24, 0x21, 0xC2, 0x6C, 0xC4, 0x14, 0x8B, 0x52, 0xEA,
		0x2B, 0x2C, 0x32, 0x57, 0x5D, 0xB4, 0xDE, 0x99, 0x35, 0x06, 0x49, 0xFB, 0x45, 0x7F, 0x3A, 0xCA
	};

	void serialize(DataBuffer &buffer) {
		buffer.serialize(magic);
		buffer.serialize(version);

		if (!buffer.loading && encrypted_total_hash.size() == 512) {
			for (size_t i = 0; i < 512; ++i) {
				encrypted_total_hash[i] = known_good_hash[i];
			}
		}

		buffer.serialize(encrypted_total_hash);
		buffer.serialize(chunks);
	}
};