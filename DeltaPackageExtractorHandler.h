#ifndef DELTA_PACKAGE_EXTRACTOR
#define DELTA_PACKAGE_EXTRACTOR

#include <map>

#include <string.h>

#include <Path.h>
#include <String.h>

#include <package/hpkg/DataReader.h>
#include <package/hpkg/PackageAttributeValue.h>
#include <package/hpkg/PackageContentHandler.h>
#include <package/hpkg/PackageEntry.h>
#include <package/hpkg/PackageEntryAttribute.h>

struct DeltaPackageEntryInfo;

#include "dpkg_addon.h"
using namespace std;
using namespace BPackageKit::BHPKG;

struct DeltaPackageEntryInfo {
	int32 fHandlerID;
	uint64 fExtractedSize;
	BString fStringPath;

	uint64 fDataSize : 63;
	bool fDataEncodedInline : 1;

	union {
		uint64 fDataOffset;
		uint8 fInlineData[B_HPKG_MAX_INLINE_DATA_SIZE];
	};

	bool fHandled;
};

struct cmp_str {
	bool operator()(const char* a, const char* b) {
		return strcmp(a, b) < 0;
	}
};

struct DeltaPackageExtractorHandler : BPackageContentHandler {
	
	
						DeltaPackageExtractorHandler();
	status_t			HandleEntry(BPackageEntry* entry);
	status_t			HandleEntryAttribute(BPackageEntry* entry,
									BPackageEntryAttribute* attribute);
	status_t			HandleEntryDone(BPackageEntry* entry);

	status_t			HandlePackageAttribute(
									const BPackageInfoAttributeValue& value
									);

	void				HandleErrorOccurred();
	void				SetHeapReader(BAbstractBufferedDataReader* reader)
							{ fHeapReader = reader; }
	BAbstractBufferedDataReader* HeapReader() { return fHeapReader; }

	void*				GetData(BPackageData& data);
	
	map<const char*, DeltaPackageEntryInfo*, cmp_str> PackageEntries()
		{ return fPackageEntries; }

	static map<uint32, DeltaPackageExtractor*>			gDeltaFileHandlers;
private:
	BPath												fPath;
	map<const char*, DeltaPackageEntryInfo*, cmp_str>	fPackageEntries;
	BAbstractBufferedDataReader*						fHeapReader;
};

#endif
