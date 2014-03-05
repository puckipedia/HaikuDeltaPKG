#include <package/hpkg/HPKGDefs.h>
#include <package/hpkg/DataReader.h>
#include <package/hpkg/PackageReader.h>
#include <package/hpkg/StandardErrorOutput.h>

#include <Path.h>

#include <map>

#include <stdio.h>

#include "dpkg_addon.h"
#include "DeltaPackageExtractorHandler.h"

using namespace BPackageKit::BHPKG;

DeltaPackageExtractor* CreateCopyPackageExtractor();

int main(int argc, const char** argv) {

	DeltaPackageExtractorHandler::gDeltaFileHandlers['COPY'] = CreateCopyPackageExtractor();

	if(argc < 2) {
		printf("Usage: %s path/to/a.dpkg\n", argv[0]);
		return 1;
	}

	const char* pfn = argv[1];
	
	BStandardErrorOutput errorOutput;
	BPackageReader packageReader(&errorOutput);
	status_t error = packageReader.Init(pfn);
	if (error != B_OK)
		return 1;

	// list
	DeltaPackageExtractorHandler handler;
	error = packageReader.ParseContent(&handler);
	if (error != B_OK)
		return 1;

	map<const char*, DeltaPackageEntryInfo*, cmp_str> ding = handler.PackageEntries();

	typedef map<const char*, DeltaPackageEntryInfo*, cmp_str>::iterator iter_t;
	for (iter_t it = ding.begin(); it != ding.end(); it++) {
		DeltaPackageExtractor* ex = DeltaPackageExtractorHandler::gDeltaFileHandlers[it->second->fHandlerID];
		if (ex != NULL) {
			printf("Would run %s on %s\n", ex->Name(), it->first);
		}
	}
	
	printf("%d\n", ding.size());

	return 0;
}
