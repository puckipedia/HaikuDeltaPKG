#ifndef _DPKG_ADDON
#define _DPKG_ADDON

struct DeltaPackageExtractor;

#include "DeltaPackageExtractorHandler.h"
#include <package/hpkg/PackageEntry.h>
#include <DataIO.h>

struct DeltaPackageExtractor {
	virtual const char* Name() const = 0;
	virtual uint32 Type() const = 0;
	virtual void Extract(DeltaPackageEntryInfo* info, BPositionIO* ActualFile,
					BPositionIO* DeltaPackage, BPositionIO* Result) = 0;
};

extern "C" DeltaPackageExtractor* CreateDPKGAddOn();

typedef DeltaPackageExtractor(*createaddon_t)();

#endif
