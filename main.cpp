#include <package/hpkg/HPKGDefs.h>
#include <package/hpkg/DataReader.h>
#include <package/hpkg/PackageReader.h>
#include <package/hpkg/PackageWriter.h>
#include <package/hpkg/StandardErrorOutput.h>

#include <Path.h>
#include <File.h>

#include <map>

#include <stdlib.h>
#include <stdio.h>

#include "dpkg_addon.h"
#include "DeltaPackageExtractorHandler.h"

using namespace BPackageKit::BHPKG;

DeltaPackageExtractor* CreateCopyPackageExtractor();

status_t ReadPackage(DeltaPackageExtractorHandler* handler, const char* pfn)
{
	status_t error;
	BStandardErrorOutput errorOtput;
	BPackageReader pakageReader(&errorOtput);
	handler->SetHeapReader(pakageReader.HeapReader());
	error = pakageReader.Init(pfn);
	if (error != B_OK)
		return error;
	
	return pakageReader.ParseContent(handler);
}

BMemoryIO* GetData(DeltaPackageEntryInfo* data, BAbstractBufferedDataReader* heapReader) {
	if(data->fDataEncodedInline) {
		printf("Data: Inline, %x bytes\n", data->fDataSize);
		return new BMemoryIO((const void*)data->fInlineData, data->fDataSize);
	} else {
		printf("Data: Heap, %x offest, %x bytes\n", data->fDataOffset, data->fDataSize);
		void* vData = malloc(data->fDataSize);
		heapReader->ReadData(data->fDataOffset, vData, data->fDataSize);
		return new BMemoryIO(vData, data->fDataSize);
	}
}

int main(int argc, const char** argv) {

	DeltaPackageExtractorHandler::gDeltaFileHandlers['COPY'] = CreateCopyPackageExtractor();

	if(argc < 3) {
		printf("Usage: %s path/to/a.dpkg path/to/file.hpkg\n", argv[0]);
		return 1;
	}

	status_t error;

	const char* pfn = argv[1];
	DeltaPackageExtractorHandler handler;

	ReadPackage(&handler, pfn);

	map<const char*, DeltaPackageEntryInfo*, cmp_str> ding = handler.PackageEntries();

	BPackageWriterParameters writerParameters;
	writerParameters.SetFlags(B_HPKG_WRITER_UPDATE_PACKAGE);

		
	DeltaPackageExtractorHandler nameHandler;
	
	ReadPackage(&nameHandler, argv[2]);
	
	BPackageWriter packageWriter(NULL);

	status_t result = packageWriter.Init(argv[2], &writerParameters);
	if (result != B_OK)
		return 1;

	typedef map<const char*, DeltaPackageEntryInfo*, cmp_str>::iterator iter_t;

	map<const char*, DeltaPackageEntryInfo*, cmp_str> datata = nameHandler.PackageEntries();
	
	for (iter_t it = ding.begin(); it != ding.end(); it++) {
		DeltaPackageExtractor* ex = DeltaPackageExtractorHandler::gDeltaFileHandlers[it->second->fHandlerID];
		if (ex != NULL) {
			char buffer[L_tmpnam];
			tmpnam(buffer);
			
			BFile file(buffer, B_CREATE_FILE | B_READ_WRITE);
			
			DeltaPackageEntryInfo* info = datata[it->second->fStringPath];
			
			BMemoryIO* origFile = GetData(info, nameHandler.HeapReader());
			BMemoryIO* deltaPackage = GetData(it->second, handler.HeapReader());
			ex->Extract(it->second, origFile, deltaPackage, &file);
			
			printf("Would run %s on %s into %s\n", ex->Name(), it->first, buffer);
		}
	}
	
	printf("%d\n", ding.size());

	return 0;
}
